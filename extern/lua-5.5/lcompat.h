/*
** $Id: lcompat.h $
** Lua 5.1 compatibility bindings header for Lua 5.5
** See Copyright Notice in lua.h
*/

#ifndef LCOMPAT_H
#define LCOMPAT_H

#include <math.h>

#include "lua.h"
#include "lauxlib.h"

/*==============================================================================
  Pseudo-indices (Lua 5.1 compatibility)
==============================================================================*/

#define LUA_GLOBALSINDEX (-10002)
#define LUA_ENVIRONINDEX (-10001)

/*==============================================================================
  Global table access wrappers
==============================================================================*/

/* Macro version for lua_settable */
#define lua_settable(L, idx) \
  ((idx) == LUA_GLOBALSINDEX ? (lua_pushglobaltable(L), lua_insert(L, -3), \
    lua_settable(L, -3), lua_remove(L, -1)) : lua_settable(L, (idx)))

/* Macro version for lua_gettable */
#define lua_gettable(L, idx) \
  ((idx) == LUA_GLOBALSINDEX ? (lua_pushglobaltable(L), lua_gettable(L, -1)) \
    : lua_gettable(L, (idx)))

/* Save original functions */
#define lua_getfield_original lua_getfield
#define lua_setfield_original lua_setfield

/* Inline wrapper for lua_getfield */
static inline void lua_getfield_wrapper(lua_State *L, int idx, const char *k) {
  if (idx == LUA_GLOBALSINDEX) {
    lua_pushglobaltable(L);
    lua_getfield_original(L, -1, k);
    lua_remove(L, -2);  /* remove global table */
  } else {
    lua_getfield_original(L, idx, k);
  }
}

/* Inline wrapper for lua_setfield */
static inline void lua_setfield_wrapper(lua_State *L, int idx, const char *k) {
  if (idx == LUA_GLOBALSINDEX) {
    lua_pushglobaltable(L);
    lua_pushvalue(L, -2);  /* duplicate value */
    lua_setfield_original(L, -2, k);
    lua_pop(L, 2);  /* pop global table and value */
  } else {
    lua_setfield_original(L, idx, k);
  }
}

/* Override the standard functions */
#define lua_getfield lua_getfield_wrapper
#define lua_setfield lua_setfield_wrapper

/* Raw versions of table access */
#define lua_rawset(L, idx) \
  ((idx) == LUA_GLOBALSINDEX ? (lua_pushglobaltable(L), lua_insert(L, -3), \
    lua_rawset(L, -3), lua_remove(L, -1)) : lua_rawset(L, (idx)))

#define lua_rawget(L, idx) \
  ((idx) == LUA_GLOBALSINDEX ? (lua_pushglobaltable(L), lua_rawget(L, -1)) \
    : lua_rawget(L, (idx)))

/* Global variable access */
#define lua_setglobal(L, name) lcompat_setglobal(L, name)
#define lua_getglobal(L, name) lcompat_getglobal(L, name)

/*==============================================================================
  Environment handling (Lua 5.1 compatibility)
==============================================================================*/

#define lua_getfenv(L, i) lcompat_getfenv(L, (i))
#define lua_setfenv(L, i) lcompat_setfenv(L, (i))

/*==============================================================================
  Deprecated function replacements
==============================================================================*/

#define lua_open       luaL_newstate
#define lua_newtable(L)    lua_createtable(L, 0, 0)
#define lua_strlen(L,i)    lua_rawlen(L, (i))
#define lua_objlen(L,i)    lua_rawlen(L, (i))
#define lua_equal(L,a,b)   lua_compare(L, (a), (b), LUA_OPEQ)
#define lua_lessthan(L,a,b)  lua_compare(L, (a), (b), LUA_OPLT)
#define lua_require(L, name, func) \
  (luaL_requiref(L, (name), (func), 1), lua_pop(L, 1))

/*==============================================================================
  Integer compatibility
==============================================================================*/


#define luaL_checkint(L,n)   ((int)luaL_checknumber(L, (n)))
#define luaL_optint(L,n,d)   ((int)luaL_optnumber(L, (n), (d)))
#define luaL_checklong(L,n)  ((long)luaL_checknumber(L, (n)))
#define luaL_optlong(L,n,d)  ((long)luaL_optnumber(L, (n), (d)))

#define lua_number2int(i,d)   ((i) = (int)(d))
#define lua_number2integer(i,d) ((i) = (lua_Integer)(d))

inline void lcompat_pushnumber(lua_State *L, lua_Number num) {
  if (floor(num) == num && num >= LUA_MININTEGER && num <= LUA_MAXINTEGER)
    lua_pushinteger(L, (lua_Integer)num);
  else
    lua_pushnumber(L, num);
}

#define lua_pushnumber(L,n) lcompat_pushnumber(L,n)

/*==============================================================================
  Error handling
==============================================================================*/

#define luaL_typerror(L,n,t) \
  luaL_error(L, "bad argument #%d (%s expected, got %s)", \
    (n), (t), luaL_typename(L, (n)))

/*==============================================================================
  Table length (Lua 5.1 compatibility)
==============================================================================*/

#define luaL_getn(L,i)     ((int)lua_rawlen(L, (i)))
#define luaL_setn(L,i,j)   ((void)0)  /* no-op in Lua 5.2+ */

/*==============================================================================
  Protected calls
==============================================================================*/

#define lua_cpcall(L,f,u)  lcompat_cpcall(L, (f), (u))

/*==============================================================================
  Library registration
==============================================================================*/

#define luaL_register(L, libname, l) \
  (luaL_newlib(L, l), lua_pushvalue(L, -1), lua_setglobal(L, libname))

#define luaL_openlib(L, n, l, nu) \
  ((n) ? (luaL_newlib(L, l), lua_pushvalue(L, -1), lua_setglobal(L, n)) : \
    (luaL_setfuncs(L, l, nu)))

/*==============================================================================
  Compatibility API declarations
==============================================================================*/

LUA_API int lcompat_getfenv(lua_State *L, int idx);
LUA_API int lcompat_setfenv(lua_State *L, int idx);
LUA_API int lcompat_cpcall(lua_State *L, lua_CFunction func, void *ud);
LUA_API void lcompat_pushvalue_at_globalsindex(lua_State *L);
LUA_API void lcompat_setglobal(lua_State *L, const char *name);
LUA_API int lcompat_getglobal(lua_State *L, const char *name);
LUA_API void lcompat_pushglobals(lua_State *L);

/* Main initialization */
LUA_API int luaopen_compat(lua_State *L);

#endif /* LCOMPAT_H */
