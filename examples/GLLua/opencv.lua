--[[  Run with (in build directory)
lib("lib")
win = GLWindow()
cv = GLLua("../examples/GLLua/opencv.lua")
win => cv
video = VideoIn()
video => video~cv
m = Metro(3000)
m => win
--]]

res   = res   or cv.Mat()
small = small or cv.Mat()
blur  = blur  or cv.Mat()
size    = cv.Size(100,100)
blur_k  = cv.Size(10,10)

n = n or 0
x = x or 0
y = y or 0
z = z or 0

function video(frame)
  cv.resize(frame, res, size, 0, 0, cv.INTER_LINEAR)
  --cv.blur(small, res, blur_k)
  --N = 7
  --lowThresh = 0
  --highThresh = 100
  --aperature_size = N
	--cv.Canny(frame, small, lowThresh*N*N, highThresh*N*N, aperature_size );
  --n = 12.972683264223
  n = n + math.pi / 300
  x = math.cos(n / 0.9) * 360 / math.pi
  y = math.sin(n / 0.7) * 360 / math.pi
  z = math.sin(n) * 360 / math.pi
end
inlet('video', MatrixIO('send me images'))

function draw()
  gl.Clear( "COLOR_BUFFER_BIT, DEPTH_BUFFER_BIT")
  gl.ClearDepth(1.0)
  gl.MatrixMode("MODELVIEW")
  gl.LoadIdentity()

  gl.Translate(0.0, 0.0, -6.0)

  gl.Rotate(x, 1.0, 0.0, 0.0)
  gl.Rotate(y, 0.0, 1.0, 0.0)
  gl.Rotate(z, 0.0, 0.0, 1.0)


  gl.Color(0.5,0.5,0.0,0.3)
  gl.LineWidth(1.0)
  glut.WireCube(2.6)

  gl.Color(0.5,0.5,0.0, 0.3)
  glut.SolidCube(2.6)

  gl.PushMatrix()
    gl.Translate(0.0, 0.0, 1.9)
    rk.draw_matrix(res, 1.3, 1.3, -1.3, -1.3, 0.3)
  gl.PopMatrix()

end

function resize(width, height)
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
    45.0,             -- Field of view angle
    width / height,   -- Aspect ration
    1.0,              -- zNear
    100.0             -- zFar
  )
  view.width  = width
  view.height = height
end
