#ifndef LCOMPAT_H
#define LCOMPAT_H

#include "lua.h"
#include "lauxlib.h"

/* Pseudo-indices */
#define LUA_GLOBALSINDEX (-10002)
#define LUA_ENVIRONINDEX (-10001)

/* Global table access */
#define lua_settable(L, idx) \
    ((idx) == LUA_GLOBALSINDEX ? (lua_pushglobaltable(L), lua_insert(L, -3), lua_settable(L, -3), lua_remove(L, -1)) : lua_settable(L, (idx)))

#define lua_gettable(L, idx) \
    ((idx) == LUA_GLOBALSINDEX ? (lua_pushglobaltable(L), lua_gettable(L, -1)) : lua_gettable(L, (idx)))

#define lua_getfield_original lua_getfield
#define lua_setfield_original lua_setfield
        
static inline void lua_getfield_wrapper(lua_State *L, int idx, const char *k) {
    if (idx == LUA_GLOBALSINDEX) {
        lua_rawgeti(L, LUA_REGISTRYINDEX, LUA_RIDX_GLOBALS);
        lua_getfield(L, -1, k);
        lua_remove(L, -2);
    } else {
        lua_getfield_original(L, idx, k);
    }
}

static inline void lua_setfield_wrapper(lua_State *L, int idx, const char *k) {
    if (idx == LUA_GLOBALSINDEX) {
        lua_rawgeti(L, LUA_REGISTRYINDEX, LUA_RIDX_GLOBALS);
        lua_pushvalue(L, -2);
        lua_setfield(L, -2, k);
        lua_pop(L, 2);
    } else {
        lua_setfield_original(L, idx, k);
    }
}

#define lua_getfield lua_getfield_wrapper
#define lua_setfield lua_setfield_wrapper
 
#define lua_rawset(L, idx) \
    ((idx) == LUA_GLOBALSINDEX ? (lua_pushglobaltable(L), lua_insert(L, -3), lua_rawset(L, -3), lua_remove(L, -1)) : lua_rawset(L, (idx)))

#define lua_rawget(L, idx) \
    ((idx) == LUA_GLOBALSINDEX ? (lua_pushglobaltable(L), lua_rawget(L, -1)) : lua_rawget(L, (idx)))

#define lua_setglobal(L,name) lcompat_setglobal(L,name)
#define lua_getglobal(L,name) lcompat_getglobal(L,name)

/* Environment handling */
#define lua_getfenv(L, i)    lcompat_getfenv(L, (i))
#define lua_setfenv(L, i)    lcompat_setfenv(L, (i))

/* Deprecated function replacements */
#define lua_open             luaL_newstate
#define lua_newtable(L)      lua_createtable(L, 0, 0)
#define lua_strlen(L,i)      lua_rawlen(L, (i))
#define lua_objlen(L,i)      lua_rawlen(L, (i))
#define lua_equal(L,a,b)     lua_compare(L,a,b,LUA_OPEQ)
#define lua_lessthan(L,a,b)  lua_compare(L,a,b,LUA_OPLT)
#define lua_require(L, name, func) \
  (luaL_requiref(L, name, func, 1), lua_pop(L, 1))

/* Integer compatibility */
#define luaL_checkint(L,n)   ((int)luaL_checkinteger(L, (n)))
#define luaL_optint(L,n,d)   ((int)luaL_optinteger(L, (n), (d)))
#define luaL_checklong(L,n)  ((long)luaL_checkinteger(L, (n)))
#define luaL_optlong(L,n,d)  ((long)luaL_optinteger(L, (n), (d)))

#define lua_number2int(i,d)     ((i)=(int)(d))
#define lua_number2integer(i,d) ((i)=(lua_Integer)(d))

/* Error handling */
#define luaL_typerror(L,n,t) \
    luaL_error(L, "bad argument #%d (%s expected, got %s)", (n), (t), luaL_typename(L, (n)))

/* Table length (Lua 5.1 compatibility) */
#define luaL_getn(L,i)       ((int)lua_rawlen(L, (i)))
#define luaL_setn(L,i,j)     ((void)0)

/* Protected calls */
#define lua_cpcall(L,f,u)    lcompat_cpcall(L, f, u)

/* Library registration */
#define luaL_register(L, libname, l) \
  (luaL_newlib(L,l), lua_pushvalue(L, -1), lua_setglobal(L, libname))

#define luaL_openlib(L, n, l, nu) \
  ( (n) ? (luaL_newlib(L, l), lua_pushvalue(L, -1), lua_setglobal(L, n)) : \
          (luaL_setfuncs(L, l, nu)) )

/* Compatibility API */
LUA_API int lcompat_getfenv(lua_State *L, int idx);
LUA_API int lcompat_setfenv(lua_State *L, int idx);
LUA_API int lcompat_cpcall(lua_State *L, lua_CFunction func, void *ud);
void lcompat_pushvalue_at_globalsindex(lua_State *L);
void lcompat_setglobal(lua_State *L, const char *name);
void lcompat_getglobal(lua_State *L, const char *name);
void lcompat_pushglobals(lua_State *L);

/* Main initialization */
LUA_API int luaopen_compat(lua_State *L);

#endif /* LCOMPAT_H */
