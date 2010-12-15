#include "rubyk.h"
#include "rubyk/timer.h"
#include "rubyk/worker.h"

namespace rk {

class Timer
{
public:
  Timer(rubyk::Worker *worker, float interval, int lua_func_idx) :
    worker_(worker), timer_(this, interval), func_idx_(lua_func_idx) {}

  ~Timer() {
    // release function
    luaL_unref(worker_->lua_, LUA_REGISTRYINDEX, func_idx_);
  }

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
    // push LUA_REGISTRYINDEX on top
    lua_rawgeti(L, LUA_REGISTRYINDEX, func_idx_);
    int status = lua_pcall(L, 0, 1, 0);

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
private:
  rubyk::Worker *worker_;
  rubyk::Timer<rk::Timer, &rk::Timer::bang> timer_;
  int func_idx_;
};

} // rk