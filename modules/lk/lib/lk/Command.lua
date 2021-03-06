--[[------------------------------------------------------

  lk.Command
  ----------

  The Command executes single commands or opens a console
  to interact with running lubyk morph servers and process.
  This is like a simplified (or scriptable) GUI.

--]]------------------------------------------------------
local lib     = {type='lk.Command'}
lib.__index   = lib
lk.Command    = lib
local private = {}

--private
local getTarget

setmetatable(lib, {
  -- new method
 __call = function(lib, url, ...)

  local self = {
    processes = {},
    -- private flag to find more processes
    find_more = true,
  }
  setmetatable(self, lib)

  if url then
    url = private.specialCommand(self, url, ...)
    if not url then
      -- done
    else
      local target, cmd = getTarget(url)
      self.target = target
      self.cmd = {cmd, ...}
    end
  end

  self.thread = lk.Thread(function()
    if self.find_more then
      self.process_watch = lk.ProcessWatch():addDelegate(self)
      while self.find_more do
        self.find_more = nil
        if self.abort then
          break
        else
          sleep(1000) -- let the processes resolve
        end
      end
    end

    self.thread = nil
  end)

  return self
end})

function getTarget(url)
  return string.match(url, '^/([^/]*)(.*)$')
end

-- TODO: Transform this to a command line that we could use to do everything that
-- we can do with the GUI.
--
-- For now simply enable the "-e" (exec) mode with the command to halt all local processes related to
-- lubyk:
-- > lubyk --zone default '/*/lk/quit'
-- Which is the same as
-- > lubyk --zone default quit
-- Or (by using .lubykrc for default zone)
-- > lubyk quit
--
-- To stop only process "a"
-- > lubyk --zone default /a/lk/quit
-- > lubyk --zone default quit a
--
-- Command format:
-- /[URL] {arg, ...}
--
-- CMD {target (default=*)} {arg, ...}
-- ==
-- /target/lk/CMD {arg, ...}
function lib:call(url, ...)
  local target_name, cmd = getTarget(url)
  if target_name == '*' then
    for _, process in pairs(self.processes) do
      private.runCmd(self, process, cmd, ...)
    end
  elseif target_name then
    local process = self.processes[target_name]
    if process then
      private.runCmd(self, process, cmd, ...)
    else
      print(string.format("Process '%s' not found.", target_name))
    end
  else
    print(string.format("Bad url '%s' (could not find process name).", url))
  end
end

--================================================== lk.ProcessWatch delegate

function lib:processConnected(process)
  self.find_more = true
  self.processes[process.name] = process
  if process.name == self.target or self.target == '*' then
    private.runCmd(self, process, unpack(self.cmd))
    self.abort = self.target == process.name
  end
end

function lib:processDisconnected(process)
  self.processes[process.name] = nil
end

--=============================================== PRIVATE

function private:specialCommand(cmd, ...)
  if cmd == '--test' then
    local all = test.all
    test.all = function() end -- dummy
    test.loadAll(...)
    all()
    return nil
  elseif cmd == '--debug' then
    remdebug.controller()
  elseif cmd == '--testInstall' then
    app = mimas.Application()
    dlg = editor.SimpleDialog {
      message = 'Lubyk works!',
--      cancel = 'Test more',
      ok = 'Ok',
    }
    function dlg.ok()
      dlg:close()
      self.find_more = false
    end
--    function dlg.cancel()
--      self:specialCommand('--test')
--    end
    dlg:resize(200,200)
    dlg:move(10, 10)
    dlg:show()
    app:exec()
    return nil
  elseif cmd == 'halt' then
    return '/*/lk/quit'
  elseif cmd == 'start' then
    morph = lk.DavMorph(...)
    run()
    return nil
  else
    return cmd
  end
end

function private:runCmd(process, cmd, ...)
  print(process.req:send(cmd, ...))
end
