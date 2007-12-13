#include "test_helper.h"

/** Test Turing class.... not the 'Turing test' ;-)  */
class TuringTest : public CxxTest::TestSuite, public ParseTest
{
public:
  /*
    x = 10
    y = 4
    
    a  x:15 -> b
    a  y:53 -> b
    b  ----> b
    
    tokens
       0: x = 10
       1: y = 4

    goto    -   x   y
       a:   0   1   1
       b:   1   -   -

    send    -   x   y
       a:   /  15  53
       b:   /   -   -

  */
  
  void test_compile( void ) 
  { assert_result("n=Turing(\"x = 10\ny=4\na  x:15 -> b\na y:53 -> b\nb  ----> b\")\nn.tables\n",
                  "#<Turing:n 2, 2>\ntokens\n   0: x = 10\n   1: y = 4\n\ngoto    -   x   y\n   a:   0   1   1\n   b:   1   -   -\n\nsend    -   x   y\n   a:   /  15  53\n   b:   /   -   -\n"); }
  
  /*

    a  10:15 -> b
    a  4:53 -> b
    b  ----> b
    
    tokens
      0 : 10
      1 : 4

    goto    -  10   4
       a:   0   1   1
       b:   1   -   -

    send    -  10   4
       a:   /  15  53
       b:   /   -   -
    
  */
  void test_compile_val_tokens( void )
  { assert_result("n=Turing(\"a  10:15 -> b\na 4:53 -> b\nb  ----> b\")\nn.tables\n",
                  "#<Turing:n 2, 2>\ntokens\n  0 : 10\n  1 : 4\n\ngoto    -  10   4\n   a:   0   1   1\n   b:   1   -   -\n\nsend    -  10   4\n   a:   /  15  53\n   b:   /   -   -\n"); }
  /*
    a  x  -> b { X }
    a  'y'-> b { Y }
    b  ----> b {   }
    
    goto ======
       . x y
    a  0 1 1
    b  1 - -

    send ======
       . x y
    a  / X Y
    b  / - -
  */
  
  void test_comment( void ) 
  { assert_result("n=Turing(\"a  x  -> b { X } # this is a comment\n=begin\nmulti\nline\ncomment\n=end\na  'y'-> b { Y }\nb  ----> b {   }\")\nn.tables\n",
                  "#<Turing:n -->\ngoto\n 0 1 1\n 1 - -\nsend\n / 88 89\n / - -\n"); }
  /*    
    'x' = 120
    'y' = 121

    'X' = 88
    'Y' = 89

    test sequence is:
    x => b (X)
    ? => a (A)
    y => b (Y)
  */
  
  void test_sequence( void )
  {
    setup_with_print("n=Turing(\"a  x  -> b { X } # this is a comment\n=begin\nmulti\nline\ncomment\n=end\na  'y'-> b { Y }\nb  ----> a { A }\nb  -x--> b {   }\")\n");
    assert_bang("n.input(120)\n",  "88\n");
    assert_bang("n.input(5644)\n", "65\n");
    assert_bang("n.input(121)\n",  "89\n");
  }
  
};