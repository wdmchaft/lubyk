--[[------------------------------------------------------

  rk.Service
  ----------

  The Service is a zmq.REP (reply) servier announced with
  mDNS and a zmq.PUB whose port can be queried with the
  rubyk.sub_port_url query.

--]]------------------------------------------------------

local lib = {}
lib.__index = lib
rk.Service  = lib

setmetatable(lib, {
  -- new method
 __call = function(table, name, service_type, callback, port)
  if not callback then
    callback = service_type
    service_type = rubyk.service_type
  end
  if not callback then
    -- dummy
    callback = function()
      return nil
    end
  end
  local instance = {name = name, browser = rk.ServiceBrowser(service_type), callback = callback}

  --======================================= PUB server
  instance.pub = zmq.Pub()

  --======================================= REP server (sync)
  instance.rep = zmq.SimpleRep(function(...)
    -- we do not pass callback directly so that we can update the function with instance.callback=..
    if ... == rubyk.info_url then
      -- rubyk special commands
      -- publish and pull ports
      return {pub = instance.pub:port(), pull = instance.pull:port()}
    else
      -- handle requests here
      instance.callback(...)
    end
  end)

  --======================================= PULL server (async)
  instance.pull = zmq.SimplePull(function(...)
    -- handle requests here
    instance.callback(...)
  end)

  --======================================= announce REP server
  instance.registration = mdns.Registration(service_type, name, instance.rep:port())

  setmetatable(instance, lib)
  return instance
end})

function lib:notify(...)
  self.pub:send(...)
end

function lib:join(...)
  self.rep:join(...)
  self.pull:join(...)
end

function lib:kill(...)
  self.rep:kill(...)
  self.pull:kill(...)
end

function lib:quit(...)
  self.rep:quit(...)
  self.pull:quit(...)
end
