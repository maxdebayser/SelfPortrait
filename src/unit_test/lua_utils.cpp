#include "lua_utils.h"
#include <stdexcept>
#include "str_conversion.h"
#include <cxxtest/TestSuite.h>

namespace LuaUtils {
	int atPanicThrow(lua_State* L) {
		throw std::runtime_error(fmt_str("error during execution of Lua code: %1\n", lua_tostring(L, -1)));
		return 0;
	}

	int ts_fail(lua_State* L) {
		TS_FAIL(luaL_checkstring(L, -1));
		return 0;
	}
}
