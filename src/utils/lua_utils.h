/******************************************************************************
* Copyright (C) 2012-2014 Maximilien de Bayser
*
* SelfPortrait API - http://github.com/maxdebayser/SelfPortrait
*
* Permission is hereby granted, free of charge, to any person obtaining
* a copy of this software and associated documentation files (the
* "Software"), to deal in the Software without restriction, including
* without limitation the rights to use, copy, modify, merge, publish,
* distribute, sublicense, and/or sell copies of the Software, and to
* permit persons to whom the Software is furnished to do so, subject to
* the following conditions:
*
* The above copyright notice and this permission notice shall be
* included in all copies or substantial portions of the Software.
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
* EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
* MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
* IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
* CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
* TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
* SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
******************************************************************************/

#ifndef LUA_UTILS_H
#define LUA_UTILS_H

#include <lua.hpp>
#include <string>
#include <tuple>
#include <list>
#include <map>
#include <unordered_map>
#include <vector>
#include <type_traits>

#include <stdexcept>

namespace LuaUtils {

	bool isudata (lua_State *L, int ud, const char *tname);

    void printType(lua_State* L, int i);

    int stackDump(lua_State *L);

    int pushTraceBack(lua_State *L);

    void removeTraceBack(lua_State* L, int errIndex);

	struct popper {
		popper(lua_State* L, int size = 1) : m_L(L), m_size(size) {}
		~popper() { if (m_size > 0) lua_pop(m_L, m_size); }
	private:
		lua_State* m_L;
		int m_size;
	};

	template<typename T>
	struct LuaValue;

	template<class T>
	struct LuaInteger {
		static_assert( std::is_integral<T>::value, "type must be integral" );
		static T getStackValue(lua_State* L, int pos) {
			return luaL_checkinteger(L, pos);
		}
		static void pushValue(lua_State* L, T value) {
			lua_pushinteger(L, value);
		}
		static int size() { return 1; }
	};

	template<> struct LuaValue<int>       : public LuaInteger<int> {};
	template<> struct LuaValue<char>      : public LuaInteger<char> {};
	template<> struct LuaValue<short>     : public LuaInteger<short> {};
	template<> struct LuaValue<long>      : public LuaInteger<long> {};
	template<> struct LuaValue<long long> : public LuaInteger<long long> {};

	template<> struct LuaValue<unsigned int>       : public LuaInteger<unsigned int> {};
	template<> struct LuaValue<unsigned char>      : public LuaInteger<unsigned char> {};
	template<> struct LuaValue<unsigned short>     : public LuaInteger<unsigned short> {};
	template<> struct LuaValue<unsigned long>      : public LuaInteger<unsigned long> {};
	template<> struct LuaValue<unsigned long long> : public LuaInteger<unsigned long long> {};

	template<class T> struct LuaFloatingPoint {
		static_assert( std::is_floating_point<T>::value, "Type must be floating point number");
		static T getStackValue(lua_State* L, int pos) {
			return luaL_checknumber(L, pos);
		}
		static void pushValue(lua_State* L, T value) {
			lua_pushnumber(L, value);
		}
		static int size() { return 1; }
	};

	template<> struct LuaValue<double>      : public LuaFloatingPoint<double> {};
	template<> struct LuaValue<long double> : public LuaFloatingPoint<long double> {};
	template<> struct LuaValue<float>       : public LuaFloatingPoint<float> {};

	template<> struct LuaValue<bool> {
		static bool getStackValue(lua_State* L, int pos) {
			luaL_checktype(L, pos, LUA_TBOOLEAN);
			return lua_toboolean(L, pos);
		}
		static void pushValue(lua_State* L, bool value) {
			lua_pushboolean(L, value);
		}
		static int size() { return 1; }
	};

	template<> struct LuaValue<std::string> {
		static std::string getStackValue(lua_State* L, int pos) {
			return luaL_checkstring(L, pos);
		}
		static void pushValue(lua_State* L, const std::string& value) {
			lua_pushstring(L, value.c_str());
		}
		static int size() { return 1; }
	};

	template<> struct LuaValue<const char*> {
		static void pushValue(lua_State* L, const char* value) {
			lua_pushstring(L, value);
		}
		static int size() { return 1; }
	};

	struct LuaNil {};

	template<> struct LuaValue<LuaNil> {
		static LuaNil getStackValue(lua_State* L, int pos) {
			luaL_checktype(L, pos, LUA_TNIL);
			return LuaNil();
		}
		static void pushValue(lua_State* L, LuaNil) {
			lua_pushnil(L);
		}
		static int size() { return 1; }
	};

	template<> struct LuaValue<void> {
		static void getStackValue(lua_State* L, int pos) {}
		static int size() { return 0; }
	};

	template<class... T> struct LuaValue<std::tuple<T...> > {

		template<class Tuple, int I, int N>
		struct helper {
			static void pushValue(lua_State* L, const Tuple& tuple) {
				LuaValue< typename std::tuple_element< I, Tuple >::type >::pushValue(L, std::get<I>(tuple));
				helper<Tuple, I+1, N>::pushValue(L, tuple);
			}
			static void getValue(lua_State* L, Tuple& tuple, int pos) {
				std::get<I>(tuple) = LuaValue< typename std::tuple_element<I, Tuple>::type>::getStackValue(L, pos);
				helper<Tuple, I+1, N>::getValue(L, tuple, ++pos);
			}
		};

		template<class Tuple, int N>
		struct helper<Tuple, N, N> {
			static void pushValue(lua_State*, const Tuple&) {}
			static void getValue(lua_State*, Tuple&, int) {}
		};

		typedef std::tuple<T...> TType;
		static TType getStackValue(lua_State* L, int pos) {
			TType ret;
			helper<TType, 0, std::tuple_size<TType>::value>::getValue(L, ret, pos-size()+1);
			return ret;
		}
		static void pushValue(lua_State* L, const TType& tuple) {
			helper<TType, 0, std::tuple_size<TType>::value>::pushValue(L, tuple);
		}
		static int size() { return std::tuple_size<TType>::value; }
	};

	template<class C> struct LuaCollection {
		typedef typename C::value_type value_type;
		static C getStackValue(lua_State* L, int pos) {
			C ret;
			luaL_checktype(L, pos, LUA_TTABLE);
#ifdef LUA52
            const int size = lua_rawlen(L, pos);
#else
            const int size = lua_objlen(L, pos);
#endif

			for (int i = 1; i <= size; ++i) {
				lua_rawgeti(L, pos, i);
				ret.emplace_back(LuaValue<value_type>::getStackValue(L, -1));
				lua_pop(L, 1);
			}
			return std::move(ret);
		}
		static void pushValue(lua_State* L, const C& list) {
			lua_createtable(L, list.size(), 0);
			int i = 1;
			for (const value_type& el: list) {
				LuaValue<value_type>::pushValue(L, el);
				lua_rawseti(L, -2, i++);
			}
		}
		static int size() { return 1; }
	};

	template<class T> struct LuaValue<std::list<T>> : public LuaCollection<std::list<T>> {};
	template<class T> struct LuaValue<std::vector<T>> : public LuaCollection<std::vector<T>> {};

	template<class M> struct LuaAssociativeArray {
		typedef typename M::key_type key_type;
		typedef typename M::mapped_type mapped_type;

		static M getStackValue(lua_State* L, int pos) {

			const int ppos = (pos < 0) ? lua_gettop(L) + pos + 1 : pos;
			luaL_checktype(L, ppos, LUA_TTABLE);

			M ret;

			lua_pushnil(L);
			while (lua_next(L, ppos) != 0) {
				ret[LuaValue<key_type>::getStackValue(L, -2)]
						= LuaValue<mapped_type>::getStackValue(L, -1);
				lua_pop(L, 1);
			}
			return std::move(ret);
		}
		static void pushValue(lua_State* L, const M& map) {
			lua_createtable(L, 0, map.size());

			for (auto el: map) {
				LuaValue<key_type>::pushValue(L, el.first);
				LuaValue<mapped_type>::pushValue(L, el.second);
				lua_settable(L, -3);
			}
		}
		static int size() { return 1; }
	};

	template<class K, class V> struct LuaValue<std::map<K,V>> : public LuaAssociativeArray<std::map<K,V>> {};
	template<class K, class V> struct LuaValue<std::unordered_map<K,V>> : public LuaAssociativeArray<std::unordered_map<K,V>> {};

	inline int pushArgs(lua_State* L) {
		return 0;
	}

	template<class H, class... T>
	inline int pushArgs(lua_State* L, const H& h, const T&... t) {
		int size = LuaValue<H>::size();
		LuaValue<H>::pushValue(L, h);
		return size + pushArgs(L, t...);
	}

	template<class R, class... Args>
	R callFuncPriv(lua_State* L, const std::string& name, const Args... args) {

        const int funcPos = lua_gettop(L);
        luaL_checktype(L, funcPos, LUA_TFUNCTION);
        const int errIndex = LuaUtils::pushTraceBack(L);
        lua_pushvalue(L, funcPos);

		const int size = pushArgs(L, args...);

        if (lua_pcall(L, size, LuaValue<R>::size(), errIndex) != 0) {
			lua_pushfstring(L, "Error running function %s:\n", name.c_str());
			lua_insert(L, -2);
			lua_concat(L, 2);
			lua_error(L);
		}
        popper p(L, LuaValue<R>::size()+1+(errIndex != 0));
		return LuaValue<R>::getStackValue(L, -1);
	}

	template<class R, class... Args>
	R callFunc(lua_State* L, const std::string& name, const Args... args) {
		lua_getglobal(L, name.c_str());
		return callFuncPriv<R, Args...>(L, name, args...);
	}

	template<class R, class... Args>
	R callStackedFunc(lua_State* L, const Args... args) {
		return callFuncPriv<R, Args...>(L, "anonymous", args...);
	}

    struct LuaStateHolder {
        explicit LuaStateHolder(lua_State* L, const std::string& addLuaPath = "", const std::string& addCPath = "");
        LuaStateHolder(const std::string& addLuaPath = "", const std::string& addCPath = "");
        LuaStateHolder(const LuaStateHolder&) = delete;
        LuaStateHolder(LuaStateHolder&& that) : m_L(that.m_L) { that.m_L = nullptr; }
        ~LuaStateHolder() {
            if (m_L != nullptr) {
                lua_close(m_L);
            }
        }
        operator lua_State*() { return m_L; }
    private:
        lua_State* m_L = nullptr;
    };

}

#endif /* LUA_UTILS_H */
