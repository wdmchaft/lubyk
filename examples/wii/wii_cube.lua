--[[------------------------------------------------------

  Use the wii.Remote to control OpenGL
  ------------------------------------

  Start the script and then press buttons '1' and '2' on
  the Remote to connect. The cube rotates depending on the
  data from the x, y and z axis. Press '1' or '2' to
  change the color, press '-' or '+' to change
  transparency. Press 'A' to start recording, 'A' again to
  stop and playback recorded data in a loop.

  Commands summary:

  1+2: discovery
  1: red
  2: blue
  +: opaque
  -: transparent
  A: record - playback - stop - record ...
  Home: quit

--]]------------------------------------------------------
require 'rubyk'

app = mimas.Application()

x, y, z = 0, 0, 0
alpha = 0.5
color = 'red'

win = mimas.GLWindow()
function win.initializeGL()
  gl.Enable("POINT_SMOOTH")
  gl.Enable("SMOOTH")
  gl.Enable("BLEND")                                -- Enable alpha blending
  gl.BlendFunc("SRC_ALPHA", "ONE_MINUS_SRC_ALPHA")  -- Set blend function

  gl.ClearDepth(1.0)
  gl.DepthFunc("LEQUAL");

  -- glEnable(GL_CULL_FACE);
  -- glEnable(GL_DEPTH_TEST);

  gl.Hint("PERSPECTIVE_CORRECTION_HINT", "NICEST") -- Really nice perspective
  gl.ClearColor(0.2,0.2,0.2,0.5)
end

function win.resizeGL(width, height)
  gl.Enable("BLEND")
  gl.Disable("DEPTH_TEST")
  gl.BlendFunc("SRC_ALPHA", "ONE_MINUS_SRC_ALPHA")

  gl.Enable("LINE_SMOOTH")
  -- Select the projection matrix
  gl.MatrixMode("PROJECTION")
  -- reset
  gl.LoadIdentity()
  -- Calculate the aspect ratio of the view
  gl.Perspective(
    45,               -- Field of view angle
    width / height,   -- Aspect ration
    1,                -- zNear
    100               -- zFar
  )
end

function win.paintGL()
  gl.Clear( "COLOR_BUFFER_BIT, DEPTH_BUFFER_BIT")
  gl.MatrixMode("MODELVIEW")
  gl.LoadIdentity()

  gl.Translate(0.0, 0.0, -6.0)

  gl.Rotate(x * 360, 1.0, 0.0, 0.0)
  gl.Rotate(y * 360, 0.0, 1.0, 0.0)
  gl.Rotate(z * 360, 0.0, 0.0, 1.0)


  gl.Color(0.5,0.5,0.5,0.8)
  gl.LineWidth(1.0)
  glut.WireCube(2.6)

  if color == 'red' then
    gl.Color(0.8,0.2,0.2,alpha)
  else
    gl.Color(0.2,0.2,0.8,alpha)
  end
  glut.SolidCube(2.6)
end
win:show()

-- in memory database stream recorder
stream = db.Stream()
stream.loop = true

function process_acceleration(lx, ly, lz)
  x = x + (lx - x) * 0.1
  y = y + (ly - y) * 0.1
  z = z + (lz - z) * 0.1
  win:updateGL()
end

function stream.playback(row)
  if row.b then
    process_button(row.b)
  else
    process_acceleration(row.x, row.y, row.z)
  end
end

function process_button(btn)
  if btn == 'Remote.1' then
    color = 'red'
  elseif btn == 'Remote.2' then
    color = 'blue'
  elseif btn == 'Remote.-' then
    alpha = alpha - 0.1
    if alpha < 0.1 then
      alpha = 0.1
    end
  elseif btn == 'Remote.+' then
    alpha = alpha + 0.1
    if alpha > 1.0 then
      alpha = 1.0
    end
  elseif btn == 'Remote.H' then
    app:quit()
  end
end

browser = wii.Browser(function(found_wii)
  wiimote = found_wii
  function wiimote.acceleration(device, lx, ly, lz)
    stream:rec{x = lx, y = ly, z = lz}
    if not stream.playing then
      process_acceleration(lx, ly, lz)
    end
  end

  function wiimote.button(btn, on)
    if on then
      if btn == 'Remote.A' then
        if stream.recording then
          print("Stop recording and start playback.")
          stream:rec_stop()
          stream:play()
        elseif stream.playing then
          print("Playback stop.")
          stream:stop()
        else
          print("Started recording...")
          stream:rec_start()
        end
      elseif not stream.playing then
        stream:rec{b=btn}
        process_button(btn)
      end
    end
    win:updateGL()
  end
end)

app:exec()
