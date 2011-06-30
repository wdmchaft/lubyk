--[[------------------------------------------------------

  Basic tests for lubyk GUI
  -------------------------

  This tests multi-threading and event posting / triggering.
  We cannot trigger these tests with other tests because Qt
  does not like beeing started and stopped multiple times.

--]]------------------------------------------------------
require 'lubyk'

local should = test.Suite('mimas.Widget')

--==================== Widget sub-class example =======

-- This is how you create a sub-class of mimas.Widget
local lib = mimas.WidgetClass('test.MyWidget')
MyWidget  = lib

function lib:init(name)
  self.name = name
end

-- custom paint
function lib:paint(p, w, h)
  local box_padding = 5
  local hpen_width = 2 -- half pen width
  local bp = hpen_width + box_padding -- full box padding
  local arc_radius = 15
  local text_padding = 20
  local pen = mimas.Pen(hpen_width * 2, mimas.Color(0.2, 0.8, 0.8, 0.8))
  pen:setStyle(mimas.DashLine)
  p:setBrush(mimas.Color(0.5, 0.5, 0.5))
  p:setPen(pen)
  p:drawRoundedRect(bp, bp, w + arc_radius + bp, h - 2 * bp, arc_radius)
  p:drawText(text_padding, text_padding, w - 2*text_padding, h - 2*text_padding, mimas.AlignCenter, self.name)
end

--=====================================================

function should.add_widgets_to_window(t)
  -- we use the test env to protect from gc
  t.win = mimas.Window()
  t.lay = mimas.VBoxLayout(t.win)
  t.wid1 = MyWidget('Hello')
  t.lay:addWidget(t.wid1)
  t.wid2 = MyWidget('Lubyk')
  t.lay:addWidget(t.wid2)
  t.win:move(100, 100)
  t.win:resize(400, 400)
  t.win:show()

  t.thread = lk.Thread(function()
    sleep(400)
    t.win:close()
    assertTrue(true)
  end)
end

local function opName(op)
  if op == mimas.MousePress then
    return 'MousePress'
  elseif op == mimas.MouseRelease then
    return 'MouseRelease'
  elseif op == mimas.DoubleClick then
    return 'DoubleClick'
  else
    return '???'
  end
end

local function modName(mod)
  if mod == mimas.NoModifier then
    return 'NoModifier'
  elseif mod == mimas.ShiftModifier then
    return 'ShiftModifier'
  elseif mod == mimas.ControlModifier then
    return 'ControlModifier'
  elseif mod == mimas.AltModifier then
    return 'AltModifier'
  elseif mod == mimas.MetaModifier then
    return 'MetaModifier'
  elseif mod == mimas.KeypadModifier then
    return 'KeypadModifier'
  elseif mod == mimas.GroupSwitchModifier then
    return 'GroupSwitchModifier'
  else
    return '???'
  end
end

function should.respond_to_click(t)
  -- we use the test env to protect from gc
  t.win = mimas.Window()
  function t.win.click(x, y, op, btn, mod)
    t.label:setText(
      string.format('x:%i y:%i op:%s btn:%i mod:%s',
        x,
        y,
        opName(op),
        btn,
        modName(mod)
      )
    )
  end


  t.lay = mimas.VBoxLayout(t.win)
  t.label = mimas.Label('Click on window (not on me)')
  t.lay:addWidget(t.label)
  t.win:move(100, 300)
  t.win:resize(400, 400)
  t.win:show()

  t.thread = lk.Thread(function()
    sleep(14000)
    t.win:close()
    assertTrue(true)
  end)
end
-- other tests for Widget are in window_test


test.all()