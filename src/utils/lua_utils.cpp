#include "lua_utils.h"

namespace LuaUtils {


	void stackDump(lua_State *L)
	{
		int top = lua_gettop(L);
		for (int i = 1; i <= top; i++) {
			int t = lua_type(L, i);
			switch (t) {
				case LUA_TSTRING:
					printf("%d: lua_string: %s\n", i, lua_tostring(L, i));
					break;
				case LUA_TBOOLEAN:
					printf("%d: lua_boolean: %d\n", i, lua_toboolean(L, i));
					break;
				case LUA_TNUMBER:
					printf("%d: lua_number: %f\n", i, lua_tonumber(L, i));
					break;
				case LUA_TNIL:
					printf("%d: lua_nil\n", i);
					break;
				case LUA_TTABLE:
					printf("%d: lua_table\n", i);
					break;
				case LUA_TFUNCTION:
					printf("%d: lua_function\n", i);
					break;
				case LUA_TUSERDATA:
					printf("%d: lua_user_data\n", i);
					break;
				case LUA_TTHREAD:
					printf("%d: lua_thread\n", i);
					break;
				default:
					printf("%d: unkonwn type\n", i);
					break;
			}
		}
	}

}
