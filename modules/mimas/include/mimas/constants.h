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
#ifndef LUBYK_INCLUDE_MIMAS_CONSTANTS_H_
#define LUBYK_INCLUDE_MIMAS_CONSTANTS_H_

#include "mimas/mimas.h"

#include "lubyk.h"

#include <string>

namespace mimas {

/** List of Brush types.
 */
enum BrushStyles {
  NoBrush                 = Qt::NoBrush,
  SolidPattern            = Qt::SolidPattern,
  //Dense1Pattern           = Qt::Dense1Pattern
  //Dense2Pattern           = Qt::Dense2Pattern
  //Dense3Pattern           = Qt::Dense3Pattern
  //Dense4Pattern           = Qt::Dense4Pattern
  //Dense5Pattern           = Qt::Dense5Pattern
  //Dense6Pattern           = Qt::Dense6Pattern
  //Dense7Pattern           = Qt::Dense7Pattern
  //HorPattern              = Qt::HorPattern
  //VerPattern              = Qt::VerPattern
  //CrossPattern            = Qt::CrossPattern
  //BDiagPattern            = Qt::BDiagPattern
  //FDiagPattern            = Qt::FDiagPattern
  //DiagCrossPattern        = Qt::DiagCrossPattern
  LinearGradientPattern   = Qt::LinearGradientPattern,
  ConicalGradientPattern  = Qt::ConicalGradientPattern,
  RadialGradientPattern   = Qt::RadialGradientPattern,
  TexturePattern          = Qt::TexturePattern,
};

enum TextFlags {
  AlignLeft                 = Qt::AlignLeft,
  AlignRight                = Qt::AlignRight,
  AlignHCenter              = Qt::AlignHCenter,
  AlignJustify              = Qt::AlignJustify,
  AlignTop                  = Qt::AlignTop,
  AlignBottom               = Qt::AlignBottom,
  AlignVCenter              = Qt::AlignVCenter,
  AlignCenter               = Qt::AlignCenter,
  TextDontClip              = Qt::TextDontClip,
  TextSingleLine            = Qt::TextSingleLine,
  TextExpandTabs            = Qt::TextExpandTabs,
  TextShowMnemonic          = Qt::TextShowMnemonic,
  TextWordWrap              = Qt::TextWordWrap,
  TextIncludeTrailingSpaces = Qt::TextIncludeTrailingSpaces,
};

} // mimas

#endif // LUBYK_INCLUDE_MIMAS_CONSTANTS_H_