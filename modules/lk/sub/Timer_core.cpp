#include "lk/Timer.h"

#include "lua_cpp_helper.h"


using namespace lk;


/* ============================ Constructors     ====================== */

/** lk::Timer::Timer(lubyk::Worker *worker, float interval)
 * include/lk/Timer.h:48
 */
static int Timer_Timer(lua_State *L) {
  try {
    lubyk::Worker *worker = *((lubyk::Worker **)luaL_checkudata(L, 1, "lubyk.Worker"));
    float interval = luaL_checknumber(L, 2);
    Timer * retval__ = new Timer(worker, interval);
    lua_pushclass<Timer>(L, retval__, "lk.Timer");
    return 1;
  } catch (std::exception &e) {
    return luaL_error(L, "lk.Timer.Timer: %s", e.what());
  } catch (...) {
    return luaL_error(L, "lk.Timer.Timer: Unknown exception");
  }
}


/* ============================ Destructor       ====================== */

static int Timer_destructor(lua_State *L) {
  Timer **userdata = (Timer**)luaL_checkudata(L, 1, "lk.Timer");

  
  if (*userdata) delete *userdata;
  
  *userdata = NULL;
  return 0;
}



/* ============================ tostring         ====================== */

static int Timer__tostring(lua_State *L) {
  Timer **userdata = (Timer**)luaL_checkudata(L, 1, "lk.Timer");
  
  
  lua_pushfstring(L, "<lk.Timer: %p %li>", *userdata, (*userdata)->interval());
  
  return 1;
}

/* ============================ Member Methods   ====================== */


/** time_t lk::Timer::interval()
 * include/lk/Timer.h:68
 */
static int Timer_interval(lua_State *L) {
  try {
    Timer *self__ = *((Timer**)luaL_checkudata(L, 1, "lk.Timer"));
    time_t  retval__ = self__->interval();
    lua_pushnumber(L, retval__);
    return 1;
  } catch (std::exception &e) {
    return luaL_error(L, "lk.Timer.interval: %s", e.what());
  } catch (...) {
    return luaL_error(L, "lk.Timer.interval: Unknown exception");
  }
}


/** void lk::Timer::join()
 * include/lk/Timer.h:63
 */
static int Timer_join(lua_State *L) {
  try {
    Timer *self__ = *((Timer**)luaL_checkudata(L, 1, "lk.Timer"));
    self__->join();
    return 0;
  } catch (std::exception &e) {
    return luaL_error(L, "lk.Timer.join: %s", e.what());
  } catch (...) {
    return luaL_error(L, "lk.Timer.join: Unknown exception");
  }
}


/** void lk::Timer::set_callback(lua_State *L)
 * include/lk/Timer.h:72
 */
static int Timer_set_callback(lua_State *L) {
  try {
    Timer *self__ = *((Timer**)luaL_checkudata(L, 1, "lk.Timer"));
    
    self__->set_callback(L);
    return 0;
  } catch (std::exception &e) {
    return luaL_error(L, "lk.Timer.set_callback: %s", e.what());
  } catch (...) {
    return luaL_error(L, "lk.Timer.set_callback: Unknown exception");
  }
}


/** void lk::Timer::start()
 * include/lk/Timer.h:58
 */
static int Timer_start(lua_State *L) {
  try {
    Timer *self__ = *((Timer**)luaL_checkudata(L, 1, "lk.Timer"));
    self__->start();
    return 0;
  } catch (std::exception &e) {
    return luaL_error(L, "lk.Timer.start: %s", e.what());
  } catch (...) {
    return luaL_error(L, "lk.Timer.start: Unknown exception");
  }
}


/** void lk::Timer::stop()
 * include/lk/Timer.h:54
 */
static int Timer_stop(lua_State *L) {
  try {
    Timer *self__ = *((Timer**)luaL_checkudata(L, 1, "lk.Timer"));
    self__->stop();
    return 0;
  } catch (std::exception &e) {
    return luaL_error(L, "lk.Timer.stop: %s", e.what());
  } catch (...) {
    return luaL_error(L, "lk.Timer.stop: Unknown exception");
  }
}




/* ============================ Lua Registration ====================== */

static const struct luaL_Reg Timer_member_methods[] = {
  {"interval"          , Timer_interval},
  {"join"              , Timer_join},
  {"set_callback"      , Timer_set_callback},
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
