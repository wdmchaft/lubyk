#include "test_helper.h"

class LuaTest : public CxxTest::TestSuite, public ParseTest
{
public:

  void test_return( void ) 
  { assert_print("n=Lua(\"return 77\")\nn=>p\np=Print()\nn.bang\n","77\n"); }

  void test_create_load( void ) 
  { assert_print("n=Lua(load:\"test/fixtures/test.lua\")\nn.script\n", "function rubyk_n(sig)\nreturn sig+12\nend\n"); }

  void test_load( void ) 
  { assert_print("n=Lua()\nn.load(\"test/fixtures/test.lua\")\nn.script\n", "function rubyk_n(sig)\nreturn sig+12\nend\n"); }

};