--[[------------------------------------------------------

  lk.Scheduler
  ------------

  Main event loop management. When we use a graphical
  interface, the scheduler automatically passes the
  loop to the GUI.

--]]------------------------------------------------------
require 'zmq'
local lib     = {type = 'lk.Scheduler'}
lib.__index   = lib
lk.Scheduler  = lib
local private = {}
local elapsed = elapsed

setmetatable(lib, {
  __call = function(lib, ...)
    return lib.new(...)
  end
})

function lib.new()
  local self = {
    now        = 0,
    -- This is used by widgets to detect volontary close vs
    -- quit close.
    should_run = true,
    -- Ordered linked list of events by event time.
    at_next    = nil,
    -- List of threads that have added their filedescriptors to
    -- select.
    fd_count      = 0,
    idx_to_thread = {},
    -- These are plain lua functions that will be called when
    -- quitting.
    finalizers    = {},
    -- Using zmq for polling
    poller        = zmq.Poller(),
  }
  return setmetatable(self, lib)
end

--- Sleeps a given number of milliseconds starting from the
-- current time (not the logical time). This should not be used as a
-- timer because it will drift.
function lib:sleep(delay)
  if delay == 0 then
    coroutine.yield('wait', 0)
  else
    coroutine.yield('wait', elapsed() + delay)
  end
end

function sleep(delay)
  sched:sleep(delay)
end

function lib:quit()
  self.should_run = false
  return true
end

--- Make it easy to rewrite logger.
function lib:log(typ, msg)
  print(typ, msg)
end

--- Waits a given number of milliseconds starting from the
-- logical time. This should be used as a timer because it will not
-- drift.
function lib:wait(delay)
  if delay == 0 then
    coroutine.yield('wait', 0)
  else
    coroutine.yield('wait', self.now + delay)
  end
end

function lib:waitRead(fd)
  coroutine.yield('read', fd)
end

function lib:waitWrite(fd)
  coroutine.yield('write', fd)
end

function lib:abort()
  coroutine.yield('abort')
end

function lib:run(func)
  if func then
    self.main = lk.Thread(func)
  end

  if not rawget(_G, 'mimas') then
    -- without mimas
    self:loop()
  end

  if self.should_run or self.restart_with_mimas then
    self.restart_with_mimas = nil
    -- restarting
    self.should_run = true
    -- now we are with mimas
    self.mimas = true
    private.usePoller(self, mimas.Poller())
    app:exec()
  end
  private.finalize(self)
end

function lib:loop()
  local poller = self.poller
  local idx_to_thread = self.idx_to_thread


  --=============================================== MAIN EVENT LOOP
  while self.should_run do
    local now = elapsed()
    local now_list = {}
    local timeout = -1
    local next_thread = self.at_next
    while next_thread and next_thread.at < now do
      -- collect
      table.insert(now_list, next_thread)
      -- get next
      next_thread = next_thread.at_next
    end
    -- remove from at_list
    self.at_next = next_thread

    for i, thread in ipairs(now_list) do
      -- We run them all now so that we give time for 'select' in
      -- case they keep on adding new events in at_next list.
      private.runThread(self, thread)
    end

    -- the running thread might have added new elements
    next_thread = self.at_next
    now = elapsed()

    if next_thread then
      if next_thread.at < now then
        timeout = 0
      else
        timeout = next_thread.at - now
      end
    end

    if self.fd_count == 0 and timeout == -1 and not self.mimas then
      -- done
      self.should_run = false
    elseif self.should_run then
      if not poller:poll(timeout) then
        -- interrupted
        self.should_run = false
        break
      end
      -- First collect events so that running the threads (and
      -- possibly adding new file descriptors) does not alter
      -- the list.
      local events = poller:events()
      if events then
        for _, ev_idx in ipairs(events) do
          local thread = idx_to_thread[ev_idx]
          if thread then
            private.runThread(self, thread)
          else
            error(string.format("Unknown thread idx '%i' in poller", ev_idx))
          end
        end
      end
    end
    if self.garbage then
      private.cleanup(self)
    end
  end
end

function lib:scheduleAt(at, thread)
  -- We use the wrapper internally so that we do not prevent
  -- garbage collection.
  thread.at = at
  -- sorted insert (next event first)
  local prev = self
  local ne = prev.at_next
  local previous_at
  if ne then
    previous_at = ne.at
  end
  while true do
    if not ne then
      prev.at_next = thread
      thread.at_next = nil
      break
    elseif at < ne.at then
      prev.at_next   = thread
      thread.at_next = ne
      break
    else
      prev = ne
    end
    ne = prev.at_next
  end
  if self.mimas and self.at_next.at ~= previous_at then
    -- We need to reschedule because this method can be called from a GUI
    -- callback and we won't get back to the loop to do this.
    self.poller:resumeAt(self.at_next.at)
  end
end

--- Tries to run 'func' count times (retry every 2^n seconds) and calls
-- 'failed' function on failure.
function lib:try(count, func, failed, wait)
  if type(count) == 'function' then
    func, failed, wait = count, func, failed
    -- default count
    count = 4
  end
  local wait = wait or 1000
  return lk.Thread(function()
    local res, err
    for i=1,count do
      res, err = self:pcall(func)
      if res then
        return
      else
        sleep(wait * 2^i)
      end
    end
    failed(err)
  end)
end

-- This is called by lk.Thread when the thread is garbage collected.
-- We will cleanup any filedescriptor in the main thread (not now or
-- we crash because we call C from the garbage collector).
function lib:remove(thread)
  if thread.idx then
    self.idx_to_thread[thread.idx] = nil
  end
  if thread.fd then
    if not self.garbage then
      self.garbage = {thread}
    else
      table.insert(self.garbage, thread)
    end
  end
  self:removeFd(thread)
  thread.t.t = nil
end

function lib:removeFd(thread)
  local fd = thread.fd
  if thread.idx then
    self.idx_to_thread[thread.idx] = nil
  end
  if fd then
    self.poller:remove(thread.idx)
    thread.fd  = nil
    self.fd_count = self.fd_count - 1
  end
  thread.idx = nil
end

function lib:pcall(f, errorHandler)
  errorHandler = errorHandler or private.errorHandler
  local co = coroutine.create(f)
  while true do
    local status, a, b = coroutine.resume(co)
    if coroutine.status(co) == 'suspended' then
      coroutine.yield(a, b)   -- suspend across `mypcall'
    elseif not status then
      -- error
      return false, errorHandler(a, co)
    else
      -- ended normally
      return status, a, b   -- error or normal return
    end
  end
end

--=============================================== PRIVATE

local zmq_POLLIN, zmq_POLLOUT = zmq.POLLIN, zmq.POLLOUT
local zmq_const = {read = zmq_POLLIN, write = zmq_POLLOUT}

function private:usePoller(new_poller)
  self.poller = new_poller
  local idx_to_thread = self.idx_to_thread
  self.idx_to_thread = {}
  -- add all filedescriptors
  for idx, thread in pairs(idx_to_thread) do
    thread.idx = new_poller:add(thread.fd, thread.event)
    self.idx_to_thread[thread.idx] = thread
  end
end

-- FIXME: If fd == zmq.Socket, we never gc because 'self' is in
-- the zmq socket thread (even if we do not use it).
--
-- What objects really need their own thread (+userdata env) ?
--   * Mimas widgets
--   * Nothing else

function private:runThread(thread)
  -- get lk.Thread from wrap
  local t = thread.t.t
  if not t or not t.co then
    -- has been killed or gc
    if thread.fd then
      self:removeFd(thread)
      thread.fd = nil
    end
    -- let it repose in peace
    if t then
      t:finalize(self)
    end
    return
  end
  if thread.at and thread.at > 0 then
    -- logical time
    sched.now = thread.at
    -- This does not work. Think better and make something that works.
    -- if elapsed() > thread.at + 2 then
    --   -- 2 == jitter between realtime and logical time.
    --   print("OUT OF REAL TIME", elapsed() - thread.at)
    -- end
  else
    sched.now = elapsed()
  end
  thread.at = nil
  local ok, a, b = coroutine.resume(t.co)
  local event = zmq_const[a]

  if not ok then
    -- a = error (unhandled errors is bad)
    print('ERROR', a, t.co, debug.traceback(t.co))
    if t.restart then
      -- This is a safety net to avoid dead sockets.
      t.co = coroutine.create(t.func)
    else
      if thread.fd then
        self:removeFd(thread)
      end
      -- let it repose in peace
      thread.fd = nil
      t.co = nil
      t:finalize(self)
    end
  elseif event then
    if thread.fd then
      if thread.fd == b then
        -- same
        self.poller:modify(thread.idx, event)
      else
        -- changed fd
        self.poller:modify(thread.idx, event, b)
      end
    else
      -- add fd
      thread.fd = b
      self.fd_count = self.fd_count + 1
      -- keep zmq.Socket object to avoid gc
      thread.idx = self.poller:add(b, event)
      -- if process then
      --   print(process.name, 'ADD', thread.idx, b, event)
      -- elseif morph then
      --   print('morph', 'ADD', thread.idx, b, event)
      -- else
      --   print('???', 'ADD', thread.idx, b, event)
      -- end
      self.idx_to_thread[thread.idx] = thread
    end
    -- We need this information in case we change poller.
    thread.event = event
  elseif a == 'join' then
    if thread.fd then
      self:removeFd(thread)
    end
    b:addJoin(thread)
  elseif a == 'wait' then
    if thread.fd then
      self:removeFd(thread)
    end
    -- b = at
    -- a value of 0 == execute again as soon as possible
    self:scheduleAt(b, thread)
  elseif not t.co or coroutine.status(t.co) == 'dead' or a == 'abort' then
    if thread.fd then
      self:removeFd(thread)
    end
    -- let it repose in peace
    thread.fd = nil
    t.co = nil
    t:finalize(self)
  elseif a == 'mimas' then
    self.should_run = false
    self.restart_with_mimas = true
    -- restart as soon as we have restarted mimas scheduler
    self:scheduleAt(0, thread)
  else
    if thread.fd then
      self:removeFd(thread)
    end
    print('BAD YIELD:', a, b)
  end
end


function private:finalize()
  for _, func in ipairs(self.finalizers) do
    -- if one fails, should not crash all finalizers...
    pcall(func)
  end
end


-- FIXME: traceback not working good enough
function private.errorHandler(err, co)
  if true then
    return err .. '\n' .. debug.traceback(co)
  end
  local tb = lk.split(debug.traceback(co), '\n')
  local max_i = 5
  local message = err
  for i = 4,#tb do
    if string.find(tb[i], 'lubyk/lib/test.lua') then
      max_i = i - 2
      break
    end
  end
  for i = 4,max_i do
    message = message .. '\n' .. tb[i]
  end
  return message
end

--- After each iteration, cleanup any dead thread.
function private:cleanup()
  for _, thread in ipairs(self.garbage) do
    self:removeFd(thread)
  end
  self.garbage = nil
end
