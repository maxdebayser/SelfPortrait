/*
** SelfPortrait API
** See Copyright Notice in lua_utils.h
*/
#include "lua_utils.h"
#include <stdexcept>
#include "str_conversion.h"
using namespace std;

namespace {
	void handleUserdata(lua_State* L, int index) {

		void* p = lua_touserdata(L, index);

		if (p != nullptr) {
			if (lua_getmetatable(L, index)) {
				lua_pushvalue(L, LUA_REGISTRYINDEX);

				/* table is in the stack at index 't' */
				lua_pushnil(L);
				while (lua_next(L, -2) != 0) {
					if (lua_istable(L, -1)) {
						if (lua_rawequal(L, -1, -4)) {
							printf("%d: lua_user_data, metatable: %s\n", index, lua_tostring(L, -2));
							lua_pop(L, 4); // pop the key-value pair, the registry and the metatable
							return;
						}
					}
					lua_pop(L, 1); // pop the value
				}
				lua_pop(L, 2); // pop the registry and the metatable
			}
		}

		printf("%d: lua_user_data\n", index);
	}
}

namespace LuaUtils {

	// modified version of luaL_checkudata
	bool isudata (lua_State *L, int ud, const char *tname) {
      bool found = false;
	  void *p = lua_touserdata(L, ud);
	  if (p != NULL) {  /* value is a userdata? */
		if (lua_getmetatable(L, ud)) {  /* does it have a metatable? */
		  lua_getfield(L, LUA_REGISTRYINDEX, tname);  /* get correct metatable */
		  if (lua_rawequal(L, -1, -2)) {  /* does it have the correct mt? */
			found = true;
		  }
		  lua_pop(L, 2);  /* remove both metatables */
		}
	  }
	  return found;
	}

    void printType(lua_State* L, int i) {
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
                handleUserdata(L, i);
                break;
            case LUA_TTHREAD:
                printf("%d: lua_thread\n", i);
                break;
            default:
                printf("%d: unkonwn type\n", i);
                break;
        }
    }

    int stackDump(lua_State *L)
	{
		int top = lua_gettop(L);
		for (int i = 1; i <= top; i++) {
            printType(L, i);
		}
        return 0;
	}

    int pushTraceBack(lua_State *L) {
        lua_getfield(L, LUA_GLOBALSINDEX, "debug");
        if (lua_istable(L, -1)) {
            lua_getfield(L, -1, "traceback");
            if (lua_isfunction(L, -1)) {
                lua_remove(L, -2);
                return lua_gettop(L);
            } else {
                lua_pop(L, 1);
            }
        } else {
            lua_pop(L, 1);
        }
        return 0;
    }

    void removeTraceBack(lua_State* L, int errIndex) {
        if (errIndex != 0) {
            lua_remove(L, errIndex);
        }
    }


    LuaStateHolder::LuaStateHolder(lua_State* L, const string& addLuaPath, const string& addCPath)
        : m_L(L)
    {

        luaL_openlibs(L);

        if (!addLuaPath.empty()) {
            lua_getglobal(L, "package");
            luaL_checktype(L, 1, LUA_TTABLE);
            lua_getfield(L, 1, "path");
            lua_pushstring(L, ";");
            lua_pushstring(L, addLuaPath.c_str());
            lua_concat(L, 3);
            lua_setfield(L, 1, "path");

            lua_settop(L, 0);
        }

        if (!addCPath.empty()) {
            lua_getglobal(L, "package");
            luaL_checktype(L, 1, LUA_TTABLE);
            lua_getfield(L, 1, "cpath");
            lua_pushstring(L, ";");
            lua_pushstring(L, addCPath.c_str());
            lua_concat(L, 3);
            lua_setfield(L, 1, "cpath");
            lua_settop(L, 0);
        }
    }

    LuaStateHolder::LuaStateHolder(const string& addLuaPath, const string& addCPath)
        : LuaStateHolder(luaL_newstate(), addLuaPath, addCPath) {}

}
