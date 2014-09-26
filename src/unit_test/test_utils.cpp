/*
** SelfPortrait API
** See Copyright Notice in lua_utils.h
*/

#include "test_utils.h"
#include <stdexcept>
#include "str_conversion.h"
#include <cxxtest/TestSuite.h>
//#include <dlfcn.h>
using namespace std;

namespace LuaUtils {

    LuaStateHolder::LuaStateHolder(lua_State* L, const string& addLuaPath, const string& addCPath)
        : m_L(L)
    {
        lua_atpanic(m_L, atPanicThrow);
        luaL_openlibs(m_L);
        lua_pushcfunction(m_L, ts_fail);
        lua_setglobal(L, "TS_FAIL");
        lua_pushcfunction(m_L, ts_trace);
        lua_setglobal(L, "TS_TRACE");
        lua_pushcfunction(m_L, ts_warn);
        lua_setglobal(L, "TS_WARN");

        lua_getglobal(L, "package");
        luaL_checktype(L, 1, LUA_TTABLE);

        // for cxxtest.lua
        lua_getfield(L, 1, "path");
        lua_pushstring(L, ";");
        lua_pushstring(L, srcpath().c_str());
        lua_pushstring(L, "/?.lua");
        lua_concat(L, 4);
        lua_setfield(L, 1, "path");

        if (!addLuaPath.empty()) {
            lua_getfield(L, 1, "path");
            lua_pushstring(L, ";");
            lua_pushstring(L, addLuaPath.c_str());
            lua_concat(L, 3);
            lua_setfield(L, 1, "path");
        }

        // for libluacppreflect
        lua_getfield(L, 1, "cpath");
        lua_pushstring(L, ";");
        lua_pushstring(L, binpath().c_str());
        lua_pushstring(L, "/../lua_module/?.so");
        lua_pushstring(L, ";");
        lua_pushstring(L, binpath().c_str());
        lua_pushstring(L, "/../lua_module/?.dll"); // don't want to mess with platform DEFINEs
        lua_concat(L, 7);
        lua_setfield(L, 1, "cpath");

        if (!addCPath.empty()) {
            lua_getfield(L, 1, "cpath");
            lua_pushstring(L, ";");
            lua_pushstring(L, addCPath.c_str());
            lua_concat(L, 3);
            lua_setfield(L, 1, "cpath");
        }

        // Uncomment this to prevent the unloading of the library
        // When the library is unloaded valgrind cannot translate function addresses to names
        // in the stacktrace
        //std::string library_path = fmt_str("%1/../lua_module/libluaselfportrait.so", binpath());
        //void* handle = dlopen(library_path.c_str(), RTLD_NOW);

    }

    LuaStateHolder::LuaStateHolder(const string& addLuaPath, const string& addCPath)
        : LuaStateHolder(luaL_newstate(), addLuaPath, addCPath) {}

    int atPanicThrow(lua_State* L) {
        throw std::runtime_error(fmt_str("Error during execution of Lua code:\n%1", lua_tostring(L, -1)));
        return 0;
    }



    int ts_fail(lua_State* L) {

        const int nargs = lua_gettop(L);

        if (nargs == 3) {
            CxxTest::TestTracker::tracker().failedTest( luaL_checkstring(L, -3), luaL_checkint(L, -2), luaL_checkstring(L, -1) );
        } else if (nargs >= 1) {
            lua_Debug debug;
            lua_getstack(L, 1, &debug);
            lua_getinfo(L, "Sl", &debug);
            CxxTest::TestTracker::tracker().failedTest( debug.short_src, debug.currentline, luaL_checkstring(L, -1) );
        } else {
            luaL_error(L, "TS_FAIL called with an illegal number of arguments");
        }
        return 0;
    }

    int ts_trace(lua_State* L) {

        const int nargs = lua_gettop(L);

        if (nargs == 3) {
            CxxTest::TestTracker::tracker().trace(luaL_checkstring(L, -3), luaL_checkint(L, -2), luaL_checkstring(L, -1) );
        } else if (nargs >= 1) {
            lua_Debug debug;
            lua_getstack(L, 1, &debug);
            lua_getinfo(L, "Sl", &debug);
            CxxTest::TestTracker::tracker().trace( debug.short_src, debug.currentline, luaL_checkstring(L, -1) );
        } else {
            luaL_error(L, "TS_TRACE called with an illegal number of arguments");
        }
        return 0;
    }

    int ts_warn(lua_State* L) {

        const int nargs = lua_gettop(L);

        if (nargs == 3) {
            CxxTest::TestTracker::tracker().warning(luaL_checkstring(L, -3), luaL_checkint(L, -2), luaL_checkstring(L, -1) );
        } else if (nargs >= 1) {
            lua_Debug debug;
            lua_getstack(L, 1, &debug);
            lua_getinfo(L, "Sl", &debug);
            CxxTest::TestTracker::tracker().warning( debug.short_src, debug.currentline, luaL_checkstring(L, -1) );
        } else {
            luaL_error(L, "TS_WARN called with an illegal number of arguments");
        }
        return 0;
    }
}
