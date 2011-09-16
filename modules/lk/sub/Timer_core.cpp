#include "lk/Timer.h"

#include "lua_cpp_helper.h"





using namespace lk;


/* ============================ Constructors     ====================== */

/** lk::Timer::Timer(float interval)
 * include/lk/Timer.h:47
 */
static int Timer_Timer(lua_State *L) {
  try {
    float interval = dubL_checknumber(L, 1);
    Timer * retval__ = new Timer(interval);
    // The class inherits from 'LuaObject', use luaInit instead of lua_pushclass.
    return retval__->luaInit(L, retval__, "lk.Timer");
  } catch (std::exception &e) {
    lua_pushfstring(L, "Timer: %s", e.what());
  } catch (...) {
    lua_pushfstring(L, "Timer: Unknown exception");
  }
  return lua_error(L);
}



/* ============================ Destructor       ====================== */

static int Timer_destructor(lua_State *L) {
  Timer **userdata = (Timer**)dubL_checksdata_n(L, 1, "lk.Timer");

  
  if (*userdata) (*userdata)->luaDestroy();
  
  *userdata = NULL;
  return 0;
}



/* ============================ tostring         ====================== */

static int Timer__tostring(lua_State *L) {
  Timer **userdata = (Timer**)dubL_checksdata_n(L, 1, "lk.Timer");
  
  
  lua_pushfstring(L, "<lk.Timer: %p %f (%s)>", *userdata, (float)(*userdata)->interval(), (float)(*userdata)->isRunning() ? "ON" : "OFF");
  
  return 1;
}

/* ============================ Member Methods   ====================== */


/** time_t lk::Timer::interval()
 * include/lk/Timer.h:70
 */
static int Timer_interval(lua_State *L) {
  try {
    Timer *self = *((Timer**)dubL_checksdata(L, 1, "lk.Timer"));
    time_t  retval__ = self->interval();
    lua_pushnumber(L, retval__);
    return 1;
  } catch (std::exception &e) {
    lua_pushfstring(L, "interval: %s", e.what());
  } catch (...) {
    lua_pushfstring(L, "interval: Unknown exception");
  }
  return lua_error(L);
}



/** bool lk::Timer::isRunning()
 * include/lk/Timer.h:66
 */
static int Timer_isRunning(lua_State *L) {
  try {
    Timer *self = *((Timer**)dubL_checksdata(L, 1, "lk.Timer"));
    bool  retval__ = self->isRunning();
    lua_pushboolean(L, retval__);
    return 1;
  } catch (std::exception &e) {
    lua_pushfstring(L, "isRunning: %s", e.what());
  } catch (...) {
    lua_pushfstring(L, "isRunning: Unknown exception");
  }
  return lua_error(L);
}



/** void lk::Timer::join()
 * include/lk/Timer.h:61
 */
static int Timer_join(lua_State *L) {
  try {
    Timer *self = *((Timer**)dubL_checksdata(L, 1, "lk.Timer"));
    self->join();
    return 0;
  } catch (std::exception &e) {
    lua_pushfstring(L, "join: %s", e.what());
  } catch (...) {
    lua_pushfstring(L, "join: Unknown exception");
  }
  return lua_error(L);
}



/** void lk::Timer::setInterval(float interval)
 * include/lk/Timer.h:74
 */
static int Timer_setInterval(lua_State *L) {
  try {
    Timer *self = *((Timer**)dubL_checksdata(L, 1, "lk.Timer"));
    float interval = dubL_checknumber(L, 2);
    self->setInterval(interval);
    return 0;
  } catch (std::exception &e) {
    lua_pushfstring(L, "setInterval: %s", e.what());
  } catch (...) {
    lua_pushfstring(L, "setInterval: Unknown exception");
  }
  return lua_error(L);
}



/** void lk::Timer::start(bool trigger_on_start=true)
 * include/lk/Timer.h:57
 */
static int Timer_start(lua_State *L) {
  try {
    Timer *self = *((Timer**)dubL_checksdata(L, 1, "lk.Timer"));
    int top__ = lua_gettop(L);
    if (top__ < 2) {
      self->start();
    } else {
      bool trigger_on_start = lua_toboolean(L, 2);
      self->start(trigger_on_start);
    }
    return 0;
  } catch (std::exception &e) {
    lua_pushfstring(L, "start: %s", e.what());
  } catch (...) {
    lua_pushfstring(L, "start: Unknown exception");
  }
  return lua_error(L);
}



/** void lk::Timer::stop()
 * include/lk/Timer.h:53
 */
static int Timer_stop(lua_State *L) {
  try {
    Timer *self = *((Timer**)dubL_checksdata(L, 1, "lk.Timer"));
    self->stop();
    return 0;
  } catch (std::exception &e) {
    lua_pushfstring(L, "stop: %s", e.what());
  } catch (...) {
    lua_pushfstring(L, "stop: Unknown exception");
  }
  return lua_error(L);
}





/* ============================ Lua Registration ====================== */

static const struct luaL_Reg Timer_member_methods[] = {
  {"interval"          , Timer_interval},
  {"isRunning"         , Timer_isRunning},
  {"join"              , Timer_join},
  {"setInterval"       , Timer_setInterval},
  {"start"             , Timer_start},
  {"stop"              , Timer_stop},
  {"__tostring"        , Timer__tostring},
  {"__gc"              , Timer_destructor},
  {NULL, NULL},
};

static const struct luaL_Reg Timer_namespace_methods[] = {
  {"Timer"             , Timer_Timer},
  {NULL, NULL},
};



#ifdef DUB_LUA_NO_OPEN
int luaload_lk_Timer_core(lua_State *L) {
#else
extern "C" int luaopen_lk_Timer_core(lua_State *L) {
#endif
  // Create the metatable which will contain all the member methods
  luaL_newmetatable(L, "lk.Timer");

  // metatable.__index = metatable (find methods in the table itself)
  lua_pushvalue(L, -1);
  lua_setfield(L, -2, "__index");

  // register member methods
  luaL_register(L, NULL, Timer_member_methods);
  // save meta-table in lk.Timer_
  register_mt(L, "lk", "Timer");

  // register class methods in a global namespace table
  luaL_register(L, "lk", Timer_namespace_methods);


	return 1;
}
