--[[------------------------------------------------------

  zmq.Subscriber
  --------------

  Lets you subscribe to publishers (zmq.Pub).

  WARNING: You cannot connect to a new publisher while
  receiving. To do this, you should use another Publisher
  using 'inproc' and subscribe to this publisher before
  entering the recv() loop (see lk.Service)

--]]------------------------------------------------------
require 'zmq'
require 'zmq.Socket'

function zmq.Sub(filter, func)
  if not func then
    func   = filter
    filter = nil
  end
  local instance = zmq.Socket(zmq.SUB, func)
  -- will not start until we sleep or unlock in another way
  -- so it is safe to setsockopt now
  
  if filter then
    instance:setsockopt(zmq.SUBSCRIBE, filter)
  else
    instance:setsockopt(zmq.SUBSCRIBE) -- filter none = get all
  end
  return instance
end
