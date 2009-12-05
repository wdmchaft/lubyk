#ifndef OSCIT_INCLUDE_OSCIT_CALLBACK_LIST_H_
#define OSCIT_INCLUDE_OSCIT_CALLBACK_LIST_H_
#include <list>

#include "oscit/callback.h"

namespace oscit {

/** Holds a list of callbacks. The list is responsible for staying
 * sane (no callbacks to dangling pointers) by registering its own
 * callback in the original callback target to be notified on destroy.
 */
class CallbackList {
public:
  typedef std::list<Callback*>::iterator Iterator;

  virtual ~CallbackList() {
    Iterator it, end = callbacks_.end();
    for(it = callbacks_.begin(); it != end; ++it) {
      delete *it;
    }
  }

  /** Adopt a new callback.
   *  The notifier becomes responsible for the callback deletion.
   */
  void adopt_callback(Callback *callback) {
    callbacks_.push_back(callback);
    callback->set_list(this);
  }

  /** Remove a specific callback from the list.
   */
  void remove_callback(Callback *callback) {
    Iterator it = callbacks_.begin(), end = callbacks_.end();
    while (it != end) {
      if (*it == callback) {
        delete *it;
        it = callbacks_.erase(it);
      } else {
        ++it;
      }
    }
  }

  // /** Remove a specific callback from the list.
  //  */
  // void remove_callbacks_to(CallbackObserver *observer) {
  //   Iterator it = callbacks_.begin(), end = callbacks_.end();
  //   while (it != end) {
  //     if ((*it)->observer() == observer) {
  //       delete *it;
  //       it = callbacks_.erase(it);
  //     } else {
  //       ++it;
  //     }
  //   }
  // }

  /** Trigger all callbacks in the list.
   */
  void trigger_all() {
    Iterator it, end = callbacks_.end();
    for(it = callbacks_.begin(); it != end; ++it) {
      (*it)->trigger();
    }
  }
  /** Find the callback identified by the given key,
   *  trigger it and remove from list.
   */
  // void trigger_and_remove(void *var, void *data) {
  //   std::list<Callback*>::iterator it, end;
  //   end = callbacks_.end();
  //   for (it = callbacks_.begin(); it != end; ++it) {
  //     if ((*it)->key_match(var, data)) {
  //       (*it)->trigger();
  //       delete *it;
  //       it = callbacks_.erase(it);
  //     }
  //   }
  // }

protected:
  std::list<Callback*> callbacks_;
};


} // oscit

#endif // OSCIT_INCLUDE_OSCIT_CALLBACK_LIST_H_