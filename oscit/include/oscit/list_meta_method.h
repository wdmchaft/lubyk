#ifndef OSCIT_INCLUDE_OSCIT_LIST_META_METHOD_H_
#define OSCIT_INCLUDE_OSCIT_LIST_META_METHOD_H_
#include "oscit/root.h"

namespace oscit {

class ListMetaMethod : public Object
{
public:  
  /** Class signature. */
  TYPED("Object.ListMetaMethod")
  
  ListMetaMethod(const char *name) : Object(name, StringIO("url", "List all children under the given url.")) {}

  virtual const Value trigger(const Value &url) {
    Value error;
    Object * target = root_->find_or_build_object_at(url.c_str(), &error);
    
    return target ? target->list() : error;
  }
};

} // oscit

#endif // OSCIT_INCLUDE_OSCIT_LIST_META_METHOD_H_
