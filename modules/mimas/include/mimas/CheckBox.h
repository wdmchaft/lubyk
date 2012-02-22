/*
  ==============================================================================

   This file is part of the LUBYK project (http://lubyk.org)
   Copyright (c) 2007-2011 by Gaspard Bucher (http://teti.ch).

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
#ifndef LUBYK_INCLUDE_MIMAS_CHECK_BOX_H_
#define LUBYK_INCLUDE_MIMAS_CHECK_BOX_H_

#include "lubyk.h"
using namespace lubyk;

#include "mimas/mimas.h"
#include "mimas/constants.h"
#include "mimas/Widget.h"

#include <QtGui/QCheckBox>
#include <QtGui/QMouseEvent>

#include <iostream>

namespace mimas {

/** CheckBox widget.
 *
 * @dub lib_name:'CheckBox_core'
 *      destructor: 'luaDestroy'
 *      super: 'QCheckBox'
 */
class CheckBox: public QCheckBox, public ThreadedLuaObject {
  Q_OBJECT
  Q_PROPERTY(QString class READ cssClass)
  Q_PROPERTY(float hue READ hue WRITE setHue)

public:
  CheckBox(const char *title = NULL, QWidget *parent = NULL)
      : QCheckBox(title, parent) {
    QObject::connect(this, SIGNAL(clicked(bool)),
                     this, SLOT(clickedSlot(bool)));
    MIMAS_DEBUG_CC
  }

  ~CheckBox() {
    MIMAS_DEBUG_GC
  }

  QString cssClass() const {
    return QString("checkbox");
  }

  QSize size_hint_;

  void setHue(float hue) {
    hue_ = hue;
    update();
  }

  void setText(const char *txt) {
    QCheckBox::setText(QString(txt));
  }

  float hue() {
    return hue_;
  }

private slots:
  void clickedSlot(bool checked) {
    lua_State *L = lua_;

    if (!pushLuaCallback("click")) return;
    lua_pushboolean(L, checked);
    // <func> <self> <checked>
    int status = lua_pcall(L, 2, 0, 0);

    if (status) {
      // cannot raise an error here...
      fprintf(stderr, "Error in 'click' callback: %s\n", lua_tostring(L, -1));
    }
  }
private:
  float hue_;
};

} // mimas
#endif // LUBYK_INCLUDE_MIMAS_CHECK_BOX_H_
