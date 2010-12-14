/*
  ==============================================================================

   This file is part of the RUBYK project (http://rubyk.org)
   Copyright (c) 2007-2010 by Gaspard Bucher - Buma (http://teti.ch).

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

#include "test_helper.h"

class NodeTest : public TestHelper {
public:
  void test_outlet_for_value( void ) {
    Root base;
    Real value = 0;
    DummyNode *sender = base.adopt(new DummyNode(&value));
    Object *out  = sender->adopt(new Object(Rubyk::NODE_OUT_KEY));
    Outlet *ping = out->adopt(new Outlet(sender, "ping", Oscit::real_io("Receive real values.")));

    Outlet *outlet = sender->outlet_for_value(1, Value(1.0));
    assert_equal(ping, outlet);
    outlet = sender->outlet_for_value(1, gNilValue);
    assert_equal(ping, outlet);
    outlet = sender->outlet_for_value(1, Value("hello"));
    assert_equal((Outlet*)NULL, outlet);
  }

  void test_view( void ) {
    Root base;
    Real value = 0;
    DummyNode *node = base.adopt(new DummyNode(&value));
    assert_equal("null", node->attributes()[Oscit::VIEW].to_json());
    node->init();
    assert_equal("{\"widget\":\"Node\", \"hue\":203, \"width\":60, \"height\":20}", node->attributes()[Oscit::VIEW].to_json());
  }

  void test_to_hash( void ) {
    Root base;
    Real value = 0;
    DummyNode *node = base.adopt(new DummyNode(&value));
    node->init();
    Object *out  = node->adopt(new Object(Rubyk::NODE_OUT_KEY));
    out->adopt(new Outlet(node, "ping", Oscit::real_io("Sends real values.")));
    Value hash(node->to_hash());
    assert_equal("\"Basic Node\"", hash[Oscit::INFO].to_json());
    assert_equal("{\"name\":\"string\", \"signature\":\"s\"}", hash[Oscit::TYPE].to_json());
    assert_equal("\"Node\"", hash[Oscit::VIEW][Oscit::WIDGET].to_json());
    assert_equal("{\"@info\":\"Sends real values.\", \"@type\":{\"name\":\"real\", \"signature\":\"f\"}}", hash[Rubyk::NODE_OUT_KEY]["ping"].to_json());
  }
};