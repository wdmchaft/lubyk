/**
 *
 * MACHINE GENERATED FILE. DO NOT EDIT.
 *
 * Bindings for class Context
 *
 * This file has been generated by dub 2.1.~.
 */
#include "dub/dub.h"
#include "zmq/Context.h"

using namespace zmq;

/** zmq::Context::Context()
 * include/zmq/Context.h:59
 */
static int Context_Context(lua_State *L) {
  try {
    Context *retval__ = new Context();
    dub_pushudata(L, retval__, "zmq.Context", true);
    return 1;
  } catch (std::exception &e) {
    lua_pushfstring(L, "Context: %s", e.what());
  } catch (...) {
    lua_pushfstring(L, "Context: Unknown exception");
  }
  return dub_error(L);
}

/** zmq::Context::~Context()
 * include/zmq/Context.h:61
 */
static int Context__Context(lua_State *L) {
  try {
    DubUserdata *userdata = ((DubUserdata*)dub_checksdata_d(L, 1, "zmq.Context"));
    if (userdata->gc) {
      Context *self = (Context *)userdata->ptr;
      delete self;
    }
    userdata->gc = false;
    return 0;
  } catch (std::exception &e) {
    lua_pushfstring(L, "~Context: %s", e.what());
  } catch (...) {
    lua_pushfstring(L, "~Context: Unknown exception");
  }
  return dub_error(L);
}

/** size_t zmq::Context::count()
 * include/zmq/Context.h:65
 */
static int Context_count(lua_State *L) {
  try {
    Context *self = *((Context **)dub_checksdata(L, 1, "zmq.Context"));
    lua_pushnumber(L, self->count());
    return 1;
  } catch (std::exception &e) {
    lua_pushfstring(L, "count: %s", e.what());
  } catch (...) {
    lua_pushfstring(L, "count: Unknown exception");
  }
  return dub_error(L);
}



// --=============================================== __tostring
static int Context___tostring(lua_State *L) {
  Context *self = *((Context **)dub_checksdata_n(L, 1, "zmq.Context"));
  lua_pushfstring(L, "zmq.Context: %p (%f)", self, self-> count());
  
  return 1;
}

// --=============================================== METHODS

static const struct luaL_Reg Context_member_methods[] = {
  { "new"          , Context_Context      },
  { "__gc"         , Context__Context     },
  { "count"        , Context_count        },
  { "__tostring"   , Context___tostring   },
  { NULL, NULL},
};


extern "C" int luaopen_zmq_Context(lua_State *L)
{
  // Create the metatable which will contain all the member methods
  luaL_newmetatable(L, "zmq.Context");
  // <mt>

  // register member methods
  luaL_register(L, NULL, Context_member_methods);
  // save meta-table in zmq
  dub_register(L, "zmq", "Context_core");
  // <mt>
  lua_pop(L, 1);
  return 0;
}