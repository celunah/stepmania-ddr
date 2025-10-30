set(LUA_SRC
    "lua-5.5/lapi.c"
    "lua-5.5/lauxlib.c"
    "lua-5.5/lbaselib.c"
    "lua-5.5/lcode.c"
    "lua-5.5/lcompat.c"
    "lua-5.5/lctype.c"
    "lua-5.5/ldblib.c"
    "lua-5.5/ldebug.c"
    "lua-5.5/ldo.c"
    "lua-5.5/ldump.c"
    "lua-5.5/lfunc.c"
    "lua-5.5/lgc.c"
    "lua-5.5/linit.c"
    "lua-5.5/liolib.c"
    "lua-5.5/llex.c"
    "lua-5.5/lmathlib.c"
    "lua-5.5/lmem.c"
    "lua-5.5/loadlib.c"
    "lua-5.5/lobject.c"
    "lua-5.5/lopcodes.c"
    "lua-5.5/loslib.c"
    "lua-5.5/lparser.c"
    "lua-5.5/lstate.c"
    "lua-5.5/lstring.c"
    "lua-5.5/lstrlib.c"
    "lua-5.5/ltable.c"
    "lua-5.5/ltablib.c"
    "lua-5.5/ltm.c"
    "lua-5.5/lundump.c"
    "lua-5.5/lvm.c"
    "lua-5.5/lzio.c")

set(LUA_HPP
    "lua-5.5/lapi.h"
    "lua-5.5/lauxlib.h"
    "lua-5.5/lcode.h"
    "lua-5.5/lcompat.h"
    "lua-5.5/lctype.h"
    "lua-5.5/ldebug.h"
    "lua-5.5/lfunc.h"
    "lua-5.5/lgc.h"
    "lua-5.5/llex.h"
    "lua-5.5/llimits.h"
    "lua-5.5/lmem.h"
    "lua-5.5/lobject.h"
    "lua-5.5/lopcodes.h"
    "lua-5.5/lparser.h"
    "lua-5.5/lstate.h"
    "lua-5.5/lstring.h"
    "lua-5.5/ltable.h"
    "lua-5.5/ltm.h"
    "lua-5.5/lua.h"
    "lua-5.5/luaconf.h"
    "lua-5.5/lualib.h"
    "lua-5.5/lundump.h"
    "lua-5.5/lvm.h"
    "lua-5.5/lzio.h")

source_group("" FILES ${LUA_SRC})
source_group("" FILES ${LUA_HPP})

add_library("lua-5.5" STATIC ${LUA_SRC} ${LUA_HPP})

set_property(TARGET "lua-5.5" PROPERTY FOLDER "External Libraries")

# include_directories(src)

if(MSVC)
  sm_add_compile_definition("lua-5.5" _CRT_SECURE_NO_WARNINGS)
  set_source_files_properties(${LUA_SRC} PROPERTIES LANGUAGE CXX)
endif(MSVC)

disable_project_warnings("lua-5.5")
