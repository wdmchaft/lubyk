--[[------------------------------------------------------

  lk.Node test
  ------------

  ...

--]]------------------------------------------------------
require 'lubyk'

local should = test.Suite('lk.Node')

local function make_patch()
  return lk.Patch(fixture.path('foo.yml'))
end

local function make_node(patch, name)
  name = name or 'foo'
  return lk.Node(patch or make_patch(), name, [[
    inlet('raw', 'Receive raw value [float].')
    scaled = outlet('scaled', 'Sends scaled value [float].')
    scale = scale or 1

    function inlet.raw(x)
      received = x
      scaled(x * scale)
    end

    function assert_received(x)
      assert_equal(x, received)
    end
  ]])
end

function should.run_code()
  local node = make_node()
  assert_equal(1, node.env.scale)
end

function should.merge_params_in_env()
  local node = make_node()
  node:set_params{scale=2.0}
  assert_equal(2.0, node.env.scale)
end

function should.set()
  local node = make_node()
  node:set{params={scale=2.0},x=10,y=30}
  assert_equal(2.0, node.env.scale)
  assert_equal(10, node.x)
  assert_equal(30, node.y)
end

function should.declare_inlets()
  local node = make_node()
  assert_equal('raw', node.inlets.raw.name)
end

function should.declare_outlets()
  local node = make_node()
  assert_equal('scaled', node.outlets.scaled.name)
end

function should.read_global_env()
  local node = lk.Node(make_patch(), 'foo', [[
    function test(x)
      assert_equal(x, node_test_global)
    end

    function set(x)
      node_test_global = x
    end
  ]])
  node_test_global = 3
  -- new global created on first read
  node.env.test(3)
  -- a copy exists in env, does not read this anymore
  -- (performance).
  env_test_global = 4
  node.env.test(3)
  -- change does not propagage outside
end

function should.not_write_global_env()
  local node = lk.Node(make_patch(), 'foo', [[
    function test(x)
      assert_equal(x, node_test_global)
    end

    function set(x)
      node_test_global = x
    end
  ]])
  node_test_global = 1
  node.env.set(3)
  node.env.test(3)
  -- change does not propagage outside
  assert_equal(1, node_test_global)
end

function should.connect_slots()
  local a = make_node()
  local b = make_node()
  a.outlets.scaled:connect(b.inlets.raw)
  a:set_params{scale=3}
  a.inlets.raw.receive(5)
  b.env.assert_received(15)
end

function should.connect_slots_on_set()
  local p = make_patch()
  local a = make_node(p)
  -- pending links
  a:set{params={scale=3}, links={scaled='b/in/raw'}}
  -- should resolve links now
  local b = make_node(p, 'b')
  a.inlets.raw.receive(5)
  b.env.assert_received(15)
end

test.all()