#include "test_utils.h"
#include <stdexcept>
#include "str_conversion.h"
#include <cxxtest/TestSuite.h>


namespace LuaUtils {
	int atPanicThrow(lua_State* L) {
		throw std::runtime_error(fmt_str("error during execution of Lua code: %1\n", lua_tostring(L, -1)));
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
