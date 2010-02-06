/*
  ==============================================================================

   This file is part of the RUBYK project (http://rubyk.org)
   Copyright (c) 2007-2009 by Gaspard Bucher - Buma (http://teti.ch).

  ------------------------------------------------------------------------------

   Permission is hereby granted, free of charge, to any person obtaining a copy
   of this software and associated documentation files (the "Software"), to deal
   in the Software without restriction, including without limitation the rights
   to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
   copies of the Software, and to permit persons to whom the Software is
   furnished to do so, subject to the following conditions:

   The above copyright notice and this permission notice shall be included in
   all copies or substantial portions of the Software.

   THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
   IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
   FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
   AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
   LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
   OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
   THE SOFTWARE.

  ==============================================================================
*/
#include "rubyk.h"
#include "gl_window.h"

class GLWindowNode : public GLWindow {
  Real red_;
public:
  const Value start() {
    return open(gNilValue);
  }

  const Value close(const Value &val) {
    close_window();
    return gNilValue;
  }

  const Value open(const Value &val) {
    open_window(50, 50, 400, 200);
    return gNilValue;
  }

  virtual void resized(int width, int height) {
    // Reset current viewport
    glViewport( 0, 0, width, height );
    glMatrixMode( GL_PROJECTION );   // Select the projection matrix
    glLoadIdentity();                // and reset it
    // Calculate the aspect ratio of the view
    gluPerspective( 45.0f, (float)width / height, 0.1f, 100.0f );
    //gluOrtho2D(0, width, 0, height);
    glMatrixMode( GL_MODELVIEW );    // Select the modelview matrix
  }

  void draw() {
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
    glLoadIdentity();   // Reset the current modelview matrix
    send(gNilValue);
  }
};

extern "C" void init(Planet &planet) {
  CLASS_NAMED(GLWindowNode, "GLWindow", "OpenGL window", "no options yet")
  OUTLET(NoteOut, draw, NilIO("Bang to execute OpenGL."))
  METHOD(GLWindowNode, open,  BangIO("Open a window."))
  METHOD(GLWindowNode, close, BangIO("Close opened window."))
}