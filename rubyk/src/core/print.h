#ifndef _PRINT_H_
#define _PRINT_H_
#include "node.h"
#include <ostream>

class Print : public Node
{
public:
  TYPED("Object.Node.Print")
  
  Print() : output_(&std::cout), prefix_("") {}
  
  const Value init() {
    if (prefix_ == "") prefix_ = name_;
    return gNilValue;
  }
  
  // [1], {2}
  const Value print(const Value &val) {
    if (val.is_nil()) {
      *output_ << prefix_ << ": Bang!\n";
    } else {
      *output_ << prefix_ << ": " << val.lazy_json() << std::endl;
    }
    return val;
  }
  
  // [2], {1}
  const Value prefix(const Value &val) {
    if (val.is_string()) {
      prefix_ = val.str();
    }
    return Value(prefix_);
  }
  
  virtual void inspect(Value *hash) const {
    hash->set("prefix", prefix_);
  }
  
  void set_output(std::ostream *output) { output_ = output; }
  
  std::ostream * output_;
private:
  std::string   prefix_;
};

#endif // _PRINT_H_