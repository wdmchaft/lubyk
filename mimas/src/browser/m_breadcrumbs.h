/*
  ==============================================================================

   This file is part of the MIMAS project (http://rubyk.org/mimas)
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

#ifndef MIMAS_SRC_DEVICE_BROWSER_M_BREADCRUMBS_H_
#define MIMAS_SRC_DEVICE_BROWSER_M_BREADCRUMBS_H_
#include "m_browser.h"

#include <list>

class MimasWindowContent;
class MBreadcrumbsItem;

class MBreadcrumbs : public Component {
public:

  MBreadcrumbs(MBrowser *browser)
    : mimas_(browser->mimas()),
      browser_(browser) {}

  MBreadcrumbs(MBrowser *browser, const Url &url)
    : browser_(browser) {
    set_url(url);
    setInterceptsMouseClicks(false, true); // pass through and test child components
  }

  /** Url for which we want to display the path hierarchy.
   */
  void set_url(const Url &url);

  // ========== Component callbacks
  virtual void resized();

  virtual void paint(Graphics &g);

  virtual void mouseDown(const MouseEvent &e) {
    std::cout << "hit breadcrumbs!\n";
  }

private:
  MimasWindowContent *mimas_;
  MBrowser *browser_;


  /** Currently displayed url.
   */
  Url url_;

  /** Delete all MBreadcrumbsItem in the path list.
   */
  void clear();

  /** List of paths leading to the currently loaded path. For example
   * if the path is "/one/two/three", this will display:
   * "/" > "one" > "two" > "three"
   */
  std::list<MBreadcrumbsItem *> paths_;
};


#endif MIMAS_SRC_DEVICE_BROWSER_M_BREADCRUMBS_H_