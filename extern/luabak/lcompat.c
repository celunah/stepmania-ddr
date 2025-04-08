/*
** Lua 5.1 compatibility bindings for Lua 5.5
** See Copyright Notice in lua.h
*/

#include <math.h>
#include <string.h>

#include "lua.h"
#include "lauxlib.h"
#include "lcompat.h"

static int lcompat_to_real_index(lua_State *L);                             
static int lcompat_absindex(lua_State *L);

/* Math library functions removed in 5.2+ */
static int math_log10(lua_State *L) {
    lua_pushnumber(L, log10(luaL_checknumber(L, 1)));
    return 1;
}

static int math_frexp(lua_State *L) {
    int e;
    lua_pushnumber(L, frexp(luaL_checknumber(L, 1), &e));
    lua_pushinteger(L, e);
    return 2;
}

static int math_ldexp(lua_State *L) {
    lua_Integer exp = luaL_checkinteger(L, 2);
    if (exp < INT_MIN || exp > INT_MAX) {
    	return luaL_error(L, "exponent out of integer range");
    }
    lua_pushnumber(L, ldexp(luaL_checknumber(L, 1), (int)exp));
    return 1;
}

/* Table library functions renamed or removed */
static int table_maxn(lua_State *L) {
    lua_Number max = 0;
    luaL_checktype(L, 1, LUA_TTABLE);
    lua_pushnil(L);  /* first key */
    while (lua_next(L, 1) != 0) {
        lua_pop(L, 1);  /* remove value */
        if (lua_type(L, -1) == LUA_TNUMBER) {
            lua_Number v = lua_tonumber(L, -1);
            if (v > max) max = v;
        }
    }
    lua_pushnumber(L, max);
    return 1;
}

static int table_foreach(lua_State *L) {
    luaL_checktype(L, 1, LUA_TTABLE);
    luaL_checktype(L, 2, LUA_TFUNCTION);
    lua_pushnil(L);  /* first key */
    while (lua_next(L, 1) != 0) {
        lua_pushvalue(L, 2);  /* function */
        lua_pushvalue(L, -3);  /* key */
        lua_pushvalue(L, -3);  /* value */
        lua_call(L, 2, 1);
        if (!lua_isnil(L, -1))
            return 1;
        lua_pop(L, 2);  /* remove value and result */
    }
    return 0;
}

static int table_foreachi(lua_State *L) {
    int i;
    lua_Unsigned n = lua_rawlen(L, 1);
    luaL_checktype(L, 1, LUA_TTABLE);
    luaL_checktype(L, 2, LUA_TFUNCTION);
    if (n > INT_MAX)
    	return luaL_error(L, "table too large");

    for (i = 1; i <= (int)n; i++) {
        lua_pushvalue(L, 2);  /* function */
        lua_pushinteger(L, i);  /* key */
        lua_rawgeti(L, 1, i);  /* value */
        lua_call(L, 2, 1);
        if (!lua_isnil(L, -1))
            return 1;
        lua_pop(L, 1);  /* remove result */
    }
    return 0;
}

static int table_getn(lua_State *L) {
    luaL_checktype(L, 1, LUA_TTABLE);
    lua_pushinteger(L, (lua_Integer)lua_rawlen(L, 1));
    return 1;
}

/* String library functions renamed or removed */
static int string_gfind(lua_State *L) {
    luaL_checkstring(L, 1);
    luaL_checkstring(L, 2);
    lua_settop(L, 2);
    lua_getglobal(L, "string");
    lua_getfield_original(L, -1, "gmatch");  /* get string.gmatch */
    lua_remove(L, -2);  /* remove string table */
    lua_pushvalue(L, 1);  /* string */
    lua_pushvalue(L, 2);  /* pattern */
    lua_call(L, 2, 1);  /* call string.gmatch */
    return 1;
}

/* Unpack - removed in 5.2+ and moved to table.unpack */
static int global_unpack(lua_State *L) {
    lua_getglobal(L, "table");
    lua_getfield_original(L, -1, "unpack");  /* get table.unpack */
    lua_remove(L, -2);  /* remove table */
    lua_insert(L, 1);  /* place function at the bottom */
    lua_call(L, lua_gettop(L) - 1, LUA_MULTRET);
    return lua_gettop(L);
}

/* Module system functions changed in 5.2+ */
static int compat_module(lua_State *L) {
    const char *modname = luaL_checkstring(L, 1);
    int loaded = 0;
    
    /* Check if module already exists in package.loaded */
    lua_getglobal(L, "package");
    lua_getfield_original(L, -1, "loaded");
    lua_getfield_original(L, -1, modname);
    loaded = !lua_isnil(L, -1);
    lua_pop(L, 3);  /* pop module value, package.loaded, package */
    
    /* Create or get _G[modname] */
    lua_getglobal(L, "_G");
    lua_getfield_original(L, -1, modname);
    if (lua_isnil(L, -1)) {  /* module does not exist? */
        lua_pop(L, 1);  /* remove nil */
        lua_newtable(L);  /* create module table */
        lua_pushvalue(L, -1);
        lua_setfield_original(L, -3, modname);  /* _G[modname] = module */
    }
    
    /* Set module environment */
    if (lua_getmetatable(L, -1) == 0) {
        lua_newtable(L);  /* create metatable */
        lua_pushvalue(L, -3);  /* _G */
        lua_setfield_original(L, -2, "__index");  /* mt.__index = _G */
        lua_setmetatable(L, -2);  /* setmetatable(module, mt) */
    }
    else {
        lua_pop(L, 1);  /* pop metatable */
    }
    
    /* Register module */
    if (!loaded) {
        lua_getglobal(L, "package");
        lua_getfield_original(L, -1, "loaded");
        lua_pushvalue(L, -3);  /* module table */
        lua_setfield_original(L, -2, modname);
        lua_pop(L, 2);  /* pop package.loaded, package */
    }
    
    /* Set it as environment for the functions */
    if (lua_gettop(L) > 2) {  /* extra arguments? */
        int i;
        for (i = 2; i <= lua_gettop(L); i++) {
            if (lua_isfunction(L, i)) {
                const char *name = lua_getupvalue(L, i, 1);  /* Get name of 1st upvalue */
                int nups = (name != NULL);  /* nups=1 if upvalue exists, 0 otherwise */
                if (nups == 1 && lua_isnil(L, -1)) {  /* is it _ENV? */
                    lua_pop(L, 1);  /* pop nil */
                    lua_pushvalue(L, -1);  /* module table */
                    lua_setupvalue(L, i, 1);  /* set _ENV */
                }
                else {
                    lua_pop(L, nups);
                }
            }
        }
    }
    
    return 1;  /* return module table */
}

/* setfenv/getfenv - removed in 5.2+ */
static int compat_setfenv(lua_State *L) {
    luaL_checktype(L, 2, LUA_TTABLE);
    lua_settop(L, 2);
    
    if (lua_isfunction(L, 1)) {
        /* functions with upvalues */
        if (lua_setupvalue(L, 1, 1) == NULL) {
            lua_pushstring(L, "unable to set environment");
            return lua_error(L);
        }
    }
    else if (lua_isnumber(L, 1)) {
        /* stack level */
        int level = (int)lua_tointeger(L, 1);
        if (level == 0) {
            /* change running thread */
            lua_pushthread(L);
            lua_pushvalue(L, 2);
            lua_rawset(L, LUA_REGISTRYINDEX);
        }
        else {
            /* non-0 level - find the calling function */
            lua_Debug ar;
            if (lua_getstack(L, level, &ar) == 0 ||
                lua_getinfo(L, "f", &ar) == 0 ||
                lua_iscfunction(L, -1)) {
                lua_pushstring(L, "invalid level");
                return lua_error(L);
            }
            lua_pushvalue(L, 2);
            lua_setupvalue(L, -2, 1);
            lua_pop(L, 1);  /* remove function */
        }
    }
    else {
        lua_pushstring(L, "invalid argument #1");
        return lua_error(L);
    }
    
    lua_pushvalue(L, 1);  /* return first argument */
    return 1;
}

static int compat_getfenv(lua_State *L) {
    int narg = lua_gettop(L);
    
    if (narg == 0) {
        /* no arguments, get current thread's env */
        lua_pushthread(L);
        lua_rawget(L, LUA_REGISTRYINDEX);
        if (lua_isnil(L, -1)) {
            /* if no environment set, use global */
            lua_pop(L, 1);
            lua_pushglobaltable(L);
        }
    }
    else if (lua_isfunction(L, 1)) {
        /* function argument, get its _ENV upvalue */
        const char *name = lua_getupvalue(L, 1, 1);
        if (name == NULL || strcmp(name, "_ENV") != 0) {
            /* no _ENV upvalue, return global table */
            lua_pop(L, 1);  /* pop nil or wrong upvalue */
            lua_pushglobaltable(L);
        }
    }
    else if (lua_isnumber(L, 1)) {
        /* stack level argument */
        int level = (int)lua_tointeger(L, 1);
        lua_Debug ar;
        if (level == 0) {
            /* get current thread's env */
            lua_pushthread(L);
            lua_rawget(L, LUA_REGISTRYINDEX);
            if (lua_isnil(L, -1)) {
                lua_pop(L, 1);
                lua_pushglobaltable(L);
            }
        }
        else if (lua_getstack(L, level, &ar) != 0 &&
                 lua_getinfo(L, "f", &ar) != 0 &&
                 !lua_iscfunction(L, -1)) {
            /* found Lua function at requested level */
            const char *name = lua_getupvalue(L, -1, 1);
            if (name == NULL || strcmp(name, "_ENV") != 0) {
                /* no _ENV, return global */
                lua_pop(L, 2);  /* pop nil/upvalue and function */
                lua_pushglobaltable(L);
            }
            else {
                /* remove function from stack */
                lua_remove(L, -2);
            }
        }
        else {
            /* level out of range or C function */
            lua_pushglobaltable(L);
        }
    }
    else {
        /* invalid argument, get global env */
        lua_pushglobaltable(L);
    }
    
    return 1;
}

/* loadstring renamed to load in 5.2+ */
static int compat_loadstring(lua_State *L) {
    size_t l;
    const char *s = luaL_checklstring(L, 1, &l);
    const char *chunkname = luaL_optstring(L, 2, s);
    
    lua_getglobal(L, "load");  /* get load function */
    lua_pushlstring(L, s, l);  /* push source string */
    lua_pushstring(L, chunkname);  /* push chunk name */
    lua_call(L, 2, LUA_MULTRET);  /* call load(s, chunkname) */
    
    return lua_gettop(L) - 2;  /* return results from load */
}

static int register_globals_helpers(lua_State *L) {                          
    /* Store functions in the compat table */                                
    lua_pushcfunction(L, lcompat_absindex);                                  
    lua_setfield_original(L, -2, "absindex");                                         
                                                                            
    lua_pushcfunction(L, lcompat_to_real_index);                             
    lua_setfield_original(L, -2, "to_real_index");                                    
                                                                             
    return 0;                                                                
}

/* Register all compatibility functions */
int luaopen_compat(lua_State *L) {
    /* Get package.loaded table to avoid loading twice */
    int compat_table;
    lua_getglobal(L, "package");
    lua_getfield_original(L, -1, "loaded");
    lua_getfield_original(L, -1, "compat");
    
    if (!lua_isnil(L, -1)) {
        return 1;  /* Already loaded */
    }
    lua_pop(L, 3);  /* Remove package.loaded.compat, package.loaded, package */
    
    /* Create compat table */
    lua_newtable(L);
    compat_table = lua_gettop(L);
    
    /* Math library compatibility */
    lua_getglobal(L, "math");
    if (lua_istable(L, -1)) {
        lua_pushcfunction(L, math_log10);
        lua_setfield_original(L, -2, "log10");
        
        lua_pushcfunction(L, math_frexp);
        lua_setfield_original(L, -2, "frexp");
        
        lua_pushcfunction(L, math_ldexp);
        lua_setfield_original(L, -2, "ldexp");
    }
    lua_pop(L, 1);  /* pop math table */
    
    /* Table library compatibility */
    lua_getglobal(L, "table");
    if (lua_istable(L, -1)) {
        lua_pushcfunction(L, table_maxn);
        lua_setfield_original(L, -2, "maxn");
        
        lua_pushcfunction(L, table_foreach);
        lua_setfield_original(L, -2, "foreach");
        
        lua_pushcfunction(L, table_foreachi);
        lua_setfield_original(L, -2, "foreachi");
        
        lua_pushcfunction(L, table_getn);
        lua_setfield_original(L, -2, "getn");
    }
    lua_pop(L, 1);  /* pop table table */
    
    /* String library compatibility */
    lua_getglobal(L, "string");
    if (lua_istable(L, -1)) {
        lua_pushcfunction(L, string_gfind);
        lua_setfield_original(L, -2, "gfind");
    }
    lua_pop(L, 1);  /* pop string table */
    
    /* Global functions */
    lua_pushcfunction(L, global_unpack);
    lua_setglobal(L, "unpack");
    
    lua_pushcfunction(L, compat_module);
    lua_setglobal(L, "module");
    
    lua_pushcfunction(L, compat_setfenv);
    lua_setglobal(L, "setfenv");
    
    lua_pushcfunction(L, compat_getfenv);
    lua_setglobal(L, "getfenv");
    
    lua_pushcfunction(L, compat_loadstring);
    lua_setglobal(L, "loadstring");
    
    /* Register the compatibility module */
    lua_getglobal(L, "package");
    lua_getfield_original(L, -1, "loaded");
    lua_pushvalue(L, compat_table);
    lua_setfield_original(L, -2, "compat");
    lua_pop(L, 2);  /* pop package.loaded and package */

    register_globals_helpers(L);
    
    return 1;
}

/* Convert any index to absolute index */
static int lcompat_absindex(lua_State *L) {
    lua_Integer idx = luaL_checkinteger(L, 1);  // Get index from Lua
    if (idx < INT_MIN || idx > INT_MAX)
        return luaL_error(L, "index out of integer range");
    if (idx > 0 || idx <= LUA_REGISTRYINDEX) {
        lua_pushinteger(L, idx);
    } else {
        lua_pushinteger(L, lua_gettop(L) + idx + 1);
    }
    return 1;  // Return one value
}

/* Convert 5.1 pseudo-indices to 5.4 equivalents */
static int lcompat_to_real_index(lua_State *L) {
    lua_Integer idx = luaL_checkinteger(L, 1);  // Get index from Lua
    if (idx < INT_MIN || idx > INT_MAX)
        return luaL_error(L, "index out of integer range");
    
    if (idx == LUA_GLOBALSINDEX) {
        lua_pushglobaltable(L);
        return 1;
    }
    else if (idx == LUA_ENVIRONINDEX) {
        lua_Debug ar;
        if (lua_getstack(L, 0, &ar) && lua_getinfo(L, "f", &ar)) {
            const char *name = lua_getupvalue(L, -1, 1);
            if (name != NULL && strcmp(name, "_ENV") == 0) {
                lua_remove(L, -2);  // remove function
                return 1;
            }
            lua_pop(L, 2);  // pop function and upvalue
        }
        lua_pushglobaltable(L);
        return 1;
    }
    
    lua_pushinteger(L, idx);
    return 1;
}

/* Push global table on stack (equivalent to pushing value at LUA_GLOBALSINDEX) */
void lcompat_pushvalue_at_globalsindex(lua_State *L) {
    lua_pushglobaltable(L);
}

/* Push global table on stack */
void lcompat_pushglobals(lua_State *L) {
    lua_pushglobaltable(L);
}

/* Enhanced setglobal that handles 5.1-style global table access */
void lcompat_setglobal(lua_State *L, const char *name) {
    lua_pushglobaltable(L);       /* get globals table */
    lua_pushvalue(L, -2);         /* copy value */
    lua_setfield_original(L, -2, name);    /* globals[name] = value */
    lua_pop(L, 2);                /* pop value and globals */
}

/* Enhanced getglobal that handles 5.1-style global table access */
void lcompat_getglobal(lua_State *L, const char *name) {
    lua_pushglobaltable(L);       /* get globals table */
    lua_getfield_original(L, -1, name);    /* push globals[name] */
    lua_remove(L, -2);            /* remove globals table */
}

