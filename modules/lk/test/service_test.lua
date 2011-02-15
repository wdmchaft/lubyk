--[[------------------------------------------------------

  lk.Service
  -----------

  zmq + mDNS

--]]------------------------------------------------------
require 'lubyk'

local should = test.Suite('lk.Client/Service')

function should.connect_when_remote_appears()
  local continue = false
  local received_count = 0
  local function receive_message(message)
    received_count = received_count + 1
    if message == 'message from Mars' then
      continue = true
    end
  end

  -- create a client that checks for 'message from Mars'
  local venus = lk.Client(receive_message)

  -- subscribe to messages from 'Mars'
  -- connection goes from subscriber to publisher and can be initiated before
  -- Mars is even created.
  venus:subscribe('Mars')

  -- create a service called 'Mars' with default service type '_lubyk._tcp'
  -- and that replies to all messages with nil.
  local mars = lk.Service('Mars')

  -- connected becomes true when 'Mars' appears on the network
  while not venus:connected() do
    sleep(10) -- make sure everything is ready before sending
  end

  mars:notify('One')
  mars:notify('message from Mars')

  while not continue do
    sleep(10)
  end

  assert_equal(2, received_count)
end

test.all()