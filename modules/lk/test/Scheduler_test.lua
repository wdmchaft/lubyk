--[[------------------------------------------------------

  lk.Scheduler
  ------------

  Manages a main loop or passes this loop to the GUI if
  one is running.

--]]------------------------------------------------------
require 'lubyk'

local should = test.Suite('lk.Scheduler')
local sched_bak

function should.setup()
  sched_bak = sched
end

function should.teardown()
  sched = sched_bak
  lk.Thread.setScheduler(sched)
end

function should.autoload()
  assertType('table', lk.Scheduler)
end

function should.createThread(t)
  local sched = lk.Scheduler()
  lk.Thread.setScheduler(sched)
  local function foo() 
    t.continue = true
  end
  local thread = lk.Thread(foo)
  assertEqual(thread.wrap, sched.at_next)
  assertEqual(0, thread.wrap.at)
  coroutine.resume(thread.co)
  assertTrue(t.continue)
end

function should.run(t)
  local sched = lk.Scheduler()
  lk.Thread.setScheduler(sched)
  local x = lk.Thread(function()
    t.continue = true
  end)
  sched:run()
  assertTrue(t.continue)
end

function should.runWithSocket(t)
  local sched = lk.Scheduler()
  lk.Thread.setScheduler(sched)
  local port
  local srv = lk.Socket(function(self)
    --print('[1] bind & listen')
    port = self:bind('127.0.0.1', 0)
    self:listen()
    --print('[2] accept')
    self:accept(function(cli)
      --print('[7] remote = ', cli.sock_fd)
      --print('[8] srv recvLine')
      local data = cli:recvLine()
      --print('[9] srv received')
      --print(string.format("'%s'", data))
      assertEqual('Hello Lubyk!', data)
      t.continue = true
    end)
  end)

  -- client thread
  local cli = lk.Socket(function(self)
    --print('[3] create cli')
    --print('[4] cli connect')
    self:connect('127.0.0.1', port)
    --print('[5] cli send')
    self:send('Hello Lubyk!\n')
    --print('[6] cli done')
  end)
  
  -- runs until socket thread ends
  sched:run()
  assertTrue(t.continue)
end

function should.runWithZmqSocket(t)
  -- This test also ensures that created threads are run in creation order
  -- (server first, then client).
  local sched = lk.Scheduler()
  lk.Thread.setScheduler(sched)
  local port
  local srv = zmq.Push()
  local thread = lk.Thread(function()
    srv:send('Hello Lubyk!')
  end)

  local cli
  cli = zmq.SimplePull(srv:port(), function(...)
    local data = ...
    assertEqual('Hello Lubyk!', data)
    t.continue = true
    cli:quit()
  end)
  
  -- runs until socket thread ends
  sched:run()
  assertTrue(t.continue)
end


function should.runWithManyZmqSockets(t)
  -- Ensure that many REQ or REP messages are read during a single select loop.
  -- simple echo server
  local srv = zmq.SimpleRep(function(...)
    --print('REP', ...)
    return ...
  end)
  local port = srv:port()

  local NB = 40
  local cli = {}
  for i=1,NB do
    local req = zmq.Req()
    --print('[1]', i)
    req:connect(string.format('tcp://localhost:%i', port))
    --print('[2]', i)
    req:send('test', i)
    --print('[3]', i)
    cli[i] = {
      thread = lk.Thread(function()
        -- inserts req in select with waitRead
        --print('[4]', i)
        local msg, nb = req:recv()
        --print('[5]', i)
        assertEqual('test', msg)
        assertEqual(i, nb)
        cli[i] = 'DONE'
        collectgarbage()
      end)
    }
  end
  -- We yield so that all the req threads get started
  sleep(10)


  local all_ok
  t:timeout(function()
    all_ok = true
    for i=1,NB do
      if cli[i] ~= 'DONE' then
        all_ok = false
        break
      end
    end
    return all_ok
  end)
  assertTrue(all_ok)
end

function should.runWithManySockets(t)
  --win = mimas.Window()
  -- Ensure that many REQ or REP messages are read during a single select loop.
  -- simple echo server
  --
  local port
  local srv = lk.Socket()
  port = srv:bind('127.0.0.1', 0)
  srv:listen()
  srv.thread = lk.Thread(function()
    while srv:shouldRun() do
      srv:accept(function(cli)
        --print(string.format("%i accepted", cli:fd()))
        local data = cli:recvLine()
        --print(string.format("%i SEND %s", cli:fd(), data))
        cli:send(data .. ' OK\n')
        sleep(100) -- Or sent data is not received. FIXME !!
        cli:close()
      end)
    end
  end)

  local NB = 13
  local cli = {}
  for i=1,NB do
    -- client thread
    local req = lk.Socket()
    --print('[1]', i)
    req:connect('127.0.0.1', port)
    --print('[2]', i)
    req:send(string.format('Hello %i\n', i))
    cli[i] = {
      thread = lk.Thread(function()
        -- inserts req in select with waitRead
        local msg = req:recv('*l')
        assertEqual(string.format('Hello %i OK', i), msg)
        cli[i] = 'DONE'
        collectgarbage('collect')
      end)
    }
  end

  local all_ok
  t:timeout(function()
    all_ok = true
    for i=1,NB do
      if cli[i] ~= 'DONE' then
        return false
      end
    end
    return true
  end)
  assertTrue(all_ok)
end

function should.try(t)
  local count = 0
  t.test = sched:try(function()
    count = count + 1
    assert(false, 'this is an error')
  end, function()
    t.continue = true
  end, 10)

  t:timeout(2000, function()
    return t.continue
  end)
  assertEqual(4, count)
end

test.all()
