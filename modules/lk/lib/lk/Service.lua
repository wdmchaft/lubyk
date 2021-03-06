--[[------------------------------------------------------

  lk.Service
  ----------

  The Service is a zmq.REP (reply) servier announced with
  mDNS and a zmq.PUB whose port can be queried with the
  lubyk.sub_port_url query.

--]]------------------------------------------------------

local lib = {type='lk.Service'}
lib.__index = lib
lk.Service  = lib

setmetatable(lib, {
  __call = function(lib, ...)
    return lib.new(...)
  end
})

function lib.new(name, opts)
  if type(opts) == 'function' then
    opts = {callback = opts}
  elseif not opts then
    opts = {}
  end
  local service_type, callback, type_name = opts.service_type, opts.callback, opts.type
  type_name = type_name or lib.type
  service_type = service_type or lubyk.service_type
  if not callback then
    -- dummy
    callback = function()
      return nil
    end
  end
  local info = opts.info or {}
  info.type = type_name
  local self = {name = name, callback = callback, info = (opts.info or {})}

  --======================================= PUB server
  self.pub = zmq.Pub()
  self.info.pub = self.pub:port()

  --======================================= REP server (sync)
  -- FIXME: REMOVE (risk of hanging)
  self.rep = zmq.SimpleRep(callback)

  --======================================= PULL server (async)
  self.pull = zmq.SimplePull(callback)

  self.info.pull = self.pull:port()

  --======================================= announce REP server
  self.registration = mdns.Registration(service_type, name, self.rep:port(), self.info, opts.registration_callback)
  -- TODO: act on name change ....

  return setmetatable(self, lib)
end

function lib:notify(...)
  self.pub:send(...)
end

function lib:join(...)
  self.rep:join(...)
  self.pull:join(...)
end

function lib:kill(...)
  self.registration:kill()
  self.registration = nil
  self.rep:kill(...)
  self.pull:kill(...)
end

function lib:quit(...)
  self.rep:quit(...)
  self.pull:quit(...)
end

