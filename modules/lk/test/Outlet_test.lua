--[[------------------------------------------------------

  lk.Outlet test
  --------------

  ...

--]]------------------------------------------------------
require 'lubyk'

local should = test.Suite('lk.Outlet')

local function mockNode()
  return {
    inlets         = {},
    sorted_inlets  = {},
    pending_inlets = {},
    outlets        = {},
    sorted_outlets = {},
    name           = 'foo'
  }
end

function should.createOutlet()
  local node = mockNode()
  local bang = lk.Outlet(node, 'bang', 'Bangs on every beat.')
  assertType('function', bang)
  assertEqual('bang', node.outlets.bang.name)
  assertEqual(bang, node.outlets.bang.send)
  assertEqual('Bangs on every beat.', node.outlets.bang.info)
end

function should.dump()
  local node = mockNode()
  local bang = lk.Outlet(node, 'bang', 'Bangs on every beat.')
  local beep = lk.Inlet(node, 'beep', 'Receives bangs.')
  node.outlets.bang:connect(node.inlets.beep)
  local dump = node.outlets.bang:dump()
  assertEqual('bang', dump.name)
  assertEqual('Bangs on every beat.', dump.info)
  assertType('table', dump.links)
  assertTrue(dump.links['foo/in/beep'])
end

function should.connectToInlets()
  local node = mockNode()
  local bang = lk.Outlet(node, 'bang', 'Send bang.')
  local beep = lk.Inlet(node, 'beep', 'Receives bangs.')
  assertPass(function()
    node.outlets.bang:connect(node.inlets.beep)
  end)
  local t = 0
  function beep.receive(value)
    t = value
  end
  bang(120)
  assertEqual(120, t)
end


test.all()