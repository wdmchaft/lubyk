--[[------------------------------------------------------

  lk.Patch test
  -------------

  ...

--]]------------------------------------------------------
require 'lubyk'

local should = test.Suite('lk.Patch')

--local should2 = test.Suite('lk.Patch')
--local should = {}

function should.loadCode()
  assertTrue(lk.Patch)
end

local function makePatch()
  return lk.Patch(fixture.path('project/simple/_patch.yml'))
end

function should.createEmptyPatch()
  local patch = lk.Patch()
  assertEqual('lk.Patch', patch.type)
end

function should.createPatchWithNewFilename()
  local patch = lk.Patch(fixture.path('foo.yml'))
  assertEqual('lk.Patch', patch.type)
  assertFalse(patch.inline)
end

function should.createLiteralPatch()
  local patch = lk.Patch [[
nodes:
  add:
    code: |
      inlet('val1', 'First value [number].')
      inlet('val2', 'Second value [number].')
      sum = outlet('sum', 'Sends sum of first and second values [number].')

      val1 = val1 or 0
      val2 = val2 or 0

      function inlet.val1(v)
        val1 = v
        sum(val1 + val2)
      end

      function inlet.val2(v)
        val2 = v
        sum(val1 + val2)
      end
    x: 70
    y: 95
    links:
      sum:
        store/in/value: true
    params:
      val1: 0
      val2: 5

  store:
    x: 70
    y: 135
]]
  assertTrue(patch.inline)
  assertMatch('Patch_test.lua', patch.filepath)
end

function should.createPatchWithInlineCode()
  local patch = lk.Patch(fixture.path('inline.yml'))
  assertEqual('lk.Patch', patch.type)
  assertEqual(patch.nodes.store, patch:get('/inline/store'))
  assertEqual(patch.nodes.add,   patch:get('/inline/add'))
end

function should.addNodes()
  local patch = makePatch()
  assertType('table', patch.nodes.add)
  assertType('table', patch.nodes.store)
end

function should.dumpNodes()
  local patch = makePatch()
  local dump  = patch:dump()
  assertType('table', dump.nodes.add)
  assertType('table', dump.nodes.store)
end

function should.partialDump()
  local patch = makePatch()
  local dump  = patch:partialDump{nodes={add={x=true}}}
  assertValueEqual({nodes={add={x=70}}}, dump)
end

function should.setDefaultValues()
  local nodes = makePatch().nodes
  assertEqual(5, nodes.add.env.val2)
end

function should.getElementsFromUrl()
  local patch = makePatch()
  assertEqual(patch.nodes.store,               patch:get('/simple/store'))
  assertEqual(patch.nodes.add,                 patch:get('/simple/add'))
  assertEqual(patch.nodes.add.inlets.val1,     patch:get('/simple/add/in/val1'))
  assertEqual(patch.nodes.add.inlets.val2,     patch:get('/simple/add/in/val2'))
  assertEqual(patch.nodes.add.outlets.sum,     patch:get('/simple/add/out/sum'))
  assertEqual(patch.nodes.store.inlets.value,  patch:get('/simple/store/in/value'))
  assertEqual(patch.nodes.store.outlets.value, patch:get('/simple/store/out/value'))
end

function should.getElementsFromPath()
  local patch = makePatch()
  assertEqual(patch.nodes.store,               patch:findByPath('store'))
  assertEqual(patch.nodes.add,                 patch:findByPath('add'))
  assertEqual(patch.nodes.add.inlets.val1,     patch:findByPath('add/in/val1'))
  assertEqual(patch.nodes.add.inlets.val2,     patch:findByPath('add/in/val2'))
  assertEqual(patch.nodes.add.outlets.sum,     patch:findByPath('add/out/sum'))
  assertEqual(patch.nodes.store.inlets.value,  patch:findByPath('store/in/value'))
  assertEqual(patch.nodes.store.outlets.value, patch:findByPath('store/out/value'))
end

function should.assertElementTypeFromUrl()
  local patch = makePatch()
  local obj, err = patch:get('/simple/store', lk.Node)
  assertTrue(obj)
  obj, err = patch:get('/simple/store', lk.Inlet)
  assertMatch('expected lk.Inlet, found lk.Node', err)
  obj, err = patch:get('/simple/add/in/val1', lk.Inlet)
  assertTrue(obj)
  obj, err = patch:get('/simple/add/in/val1', lk.Outlet)
  assertMatch('expected lk.Outlet, found lk.Inlet', err)
end

function should.connectNodes()
  local nodes = makePatch().nodes
  nodes.add.inlets.val1.receive(15)
  assertEqual(20, nodes.store.env.value)
end

function should.findProcess()
  local patch = makePatch()
  patch.process_watch = {pending_processes = {}}
  local other = patch:findProcess('foobar')
  assertEqual('lk.RemoteProcess', other.type)
end

test.all()