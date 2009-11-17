#ifndef OSCIT_TEST_MOCK_OBJECT_PROXY_LOGGER_H_
#define OSCIT_TEST_MOCK_OBJECT_PROXY_LOGGER_H_

#include "oscit/object.h"
#include "mock/logger.h"

class ObjectProxyLogger : public ObjectProxy, protected MockLogger {
public:
  ObjectProxyLogger(const char *name, const Value &type, std::ostringstream *stream) :
                ObjectProxy(name, type),
                MockLogger(name, stream) {}

  ObjectProxyLogger(const std::string &name, const Value &type) :
                ObjectProxy(name, type),
                MockLogger(name.c_str(), NULL) {}

  virtual const Value trigger(const Value &val) {
    log("trigger", val, value_);
    return value_;
  }

  /** This method should be implemented in order to give a visual feedback to
   * users that the value has changed (GUI object notification).
   */
  virtual void value_changed() {
    log("value_changed", value_);
  }

  RootProxy *root_proxy() {
    return root_proxy_;
  }

  void set_stream(std::ostream *stream) {
    MockLogger::set_stream(stream);
  }

};


#endif // OSCIT_TEST_MOCK_OBJECT_PROXY_LOGGER_H_