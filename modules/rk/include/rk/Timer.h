#include "rubyk.h"
#include "rubyk/timer.h"
#include "rubyk/worker.h"

namespace rk {
/** Calls a lua function back at regular intervals. If the called function returns
 * a number, the number sets the new interval (0 = stop).
 * @dub string_format:'%%li'
 *      string_args:'(*userdata)->interval()'
 *      lib_name:'Timer_core'
 */
class Timer : public rubyk::LuaCallback
{
public:
  Timer(rubyk::Worker *worker, float interval, int lua_func_idx)
    : rubyk::LuaCallback(worker, lua_func_idx),
      timer_(this, interval) {}

  ~Timer() {}

  void stop() {
    timer_.stop();
  }

  void start() {
    timer_.start();
  }

  void join() {
    rubyk::ScopedUnlock unlock(worker_);
    timer_.join();
  }

  time_t interval() {
    return timer_.interval();
  }
private:
  void bang() {
    lua_State *L = worker_->lua_;
    // find function and call
    rubyk::ScopedLock lock(worker_);

    push_lua_callback();

    int status = lua_pcall(worker_->lua_, 0, 1, 0);
    if (status) {
      printf("Error triggering timer: %s\n", lua_tostring(L, -1));
    }

    if (lua_type(L, -1) == LUA_TNUMBER) {
      double interval = lua_tonumber(L, -1);
      if (interval == 0) {
        timer_.stop_from_loop();
      } else if (interval < 0) {
        luaL_error(L, "Timer interval must be a positive value (got %f)\n", interval);
      }
      timer_.set_interval_from_loop(interval);
    }

    // clear stack
    lua_settop(worker_->lua_, 0);
  }

  rubyk::Timer<rk::Timer, &rk::Timer::bang> timer_;
};

} // rk