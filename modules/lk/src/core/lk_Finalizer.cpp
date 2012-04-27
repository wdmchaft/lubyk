/**
 *
 * MACHINE GENERATED FILE. DO NOT EDIT.
 *
 * Bindings for class Finalizer
 *
 * This file has been generated by dub 2.1.~.
 */
#include "dub/dub.h"
#include "lk/Finalizer.h"

using namespace lk;

/** lk::Finalizer::Finalizer()
 * include/lk/Finalizer.h:45
 */
static int Finalizer_Finalizer(lua_State *L) {
  try {
    Finalizer *retval__ = new Finalizer();
    retval__->pushobject(L, retval__, "lk.Finalizer", true);
    return 1;
  } catch (std::exception &e) {
    lua_pushfstring(L, "Finalizer: %s", e.what());
  } catch (...) {
    lua_pushfstring(L, "Finalizer: Unknown exception");
  }
  return dub_error(L);
}

/** virtual lk::Finalizer::~Finalizer()
 * include/lk/Finalizer.h:47
 */
static int Finalizer__Finalizer(lua_State *L) {
  try {
    DubUserdata *userdata = ((DubUserdata*)dub_checksdata_d(L, 1, "lk.Finalizer"));
    if (userdata->gc) {
      Finalizer *self = (Finalizer *)userdata->ptr;
      self->finalize();
    }
    userdata->gc = false;
    return 0;
  } catch (std::exception &e) {
    lua_pushfstring(L, "~Finalizer: %s", e.what());
  } catch (...) {
    lua_pushfstring(L, "~Finalizer: Unknown exception");
  }
  return dub_error(L);
}



// --=============================================== __tostring
static int Finalizer___tostring(lua_State *L) {
  Finalizer *self = *((Finalizer **)dub_checksdata_n(L, 1, "lk.Finalizer"));
  lua_pushfstring(L, "lk.Finalizer: %p", self);
  
  return 1;
}

// --=============================================== METHODS

static const struct luaL_Reg Finalizer_member_methods[] = {
  { "new"          , Finalizer_Finalizer  },
  { "__gc"         , Finalizer__Finalizer },
  { "__tostring"   , Finalizer___tostring },
  { "deleted"      , dub_isDeleted        },
  { NULL, NULL},
};


extern "C" int luaopen_lk_Finalizer(lua_State *L)
{
  // Create the metatable which will contain all the member methods
  luaL_newmetatable(L, "lk.Finalizer");
  // <mt>

  // register member methods
  luaL_register(L, NULL, Finalizer_member_methods);
  // save meta-table in lk
  dub_register(L, "lk", "Finalizer_core");
  // <mt>
  lua_pop(L, 1);
  return 0;
}
