/*
** SelfPortrait API
** See Copyright Notice in reflection.h
*/

#if defined(_WIN32)
#include <windows.h>
#endif

#include "luamodule.h"

namespace SelfPortraitLua {

//=====================Definitions==============================================


//---------------Variant--------------------------------------------------------

const char * Lua_Variant::metatableName = "SelfPortraitVariant";
const char * Lua_Variant::userDataName  = "Variant";
MethodTable Lua_Variant::methods;


const struct luaL_Reg Lua_Variant::lib_f[] = {
	{ "new", exception_translator<newInstance> },
	{ NULL, NULL }
};

const struct luaL_Reg Lua_Variant::lib_m[] = {
	{ "__gc", exception_translator<gc> },
    { "__index", exception_translator<index> },
    { "__newindex", exception_translator<newindex> },
	{ "__tostring", exception_translator<tostring> },
	{ "__eq", exception_translator<eq> },
    { "__add", exception_translator<add> },
    { "__sub", exception_translator<sub> },
    { "__mul", exception_translator<mul> },
    { "__div", exception_translator<div> },
    { "__mod", exception_translator<mod> },
    { "__unm", exception_translator<unm> },
	{ NULL, NULL }
};



void Lua_Variant::initialize()
{
	methods["tostring"]        = exception_translator<tostring>;
	methods["tonumber"]        = exception_translator<tonumber>;
	methods["isValid"]         = exception_translator<isValid>;
	methods["isStdString"]     = exception_translator<isStdString>;
	methods["isIntegral"]      = exception_translator<isIntegral>;
	methods["isFloatingPoint"] = exception_translator<isFloatingPoint>;
	methods["isArithmetical"]  = exception_translator<isArithmetical>;
	methods["isPOD"]           = exception_translator<isPOD>;
	methods["sizeOf"]          = exception_translator<sizeOf>;
	methods["alignOf"]         = exception_translator<alignOf>;
}

int Lua_Variant::index(lua_State* L) {
    Lua_Variant* c = Lua_Variant::checkUserData(L);
    const char * index = luaL_checkstring(L, 2);

    auto it = Adapted::methods.find(index);

    if (it != Adapted::methods.end()) {
        lua_pushcfunction(L, it->second);
        return 1;
    } else {

        if(c->m_class.isValid()) {

            Class clazz = c->m_class;

            Attribute attr;

            if (clazz.findMethod([&](const Method& m){ return m.name() == index; }).isValid()) {
                LuaAdapter<Lua_Class>::create(L,clazz);
                lua_pushstring(L, index);
                lua_pushcclosure(L, method_stub, 2);
                return 1;

            } else if ((attr = clazz.findAttribute([&](const Attribute& a){ return a.name() == index; })).isValid()) {
                Lua_Attribute::create(L,attr);
                lua_pushvalue(L, 1);
                lua_remove(L, 1);
                lua_remove(L, 1);
                return Lua_Attribute::get(L);
            }
        }

        luaL_error(L, "class has no property %s\n", index);
    }
    return 0;
}

int Lua_Variant::newindex(lua_State* L) {
    Lua_Variant* c = Lua_Variant::checkUserData(L);
    const char * index = luaL_checkstring(L, 2);

    if(c->m_class.isValid()) {

        Class clazz = c->m_class;
        Attribute attr;

        if ((attr = clazz.findAttribute([&](const Attribute& a){ return a.name() == index; })).isValid()) {
            Lua_Attribute::create(L,attr);
            lua_pushvalue(L, 1);
            lua_remove(L, 1);
            lua_remove(L, 1);
            lua_pushvalue(L, 1);
            lua_remove(L, 1);
            return Lua_Attribute::set(L);

            return 1;
        }
    }

    luaL_error(L, "class has no property %s\n", index);

    return 0;
}

VariantValue Lua_Variant::getFromStack(lua_State* L, int idx)
{
	int t = lua_type(L, idx);

	switch (t) {
		case LUA_TSTRING: {
            VariantValue v;
            v.construct<const char*>(lua_tostring(L, idx));
            return ::std::move(v);
			break;
		}
		case LUA_TBOOLEAN: {
            return ::std::move(VariantValue( bool(lua_toboolean(L, idx))));
			break;
		}
		case LUA_TNUMBER: {
			return ::std::move(VariantValue( lua_tonumber(L, idx) ));
			break;
		}
		case LUA_TTABLE: {
			luaL_error(L, "variant from table not implemented yet");
			break;
		}
		case LUA_TUSERDATA: {
			void *p = lua_touserdata(L, idx);
			if (p != nullptr) {
				lua_getmetatable(L, idx);
				lua_getfield(L, LUA_REGISTRYINDEX, Lua_Variant::metatableName);  /* get correct metatable */
				if (lua_rawequal(L, -1, -2)) {  /* does it have the correct mt? */
					lua_pop(L, 2);  /* remove both metatables */
					return ::std::move(reinterpret_cast<Lua_Variant*>(p)->m_variant.createReference());
				} else {
					luaL_error(L, "unknown userdata cannot be converted to variant");
				}
			} else {
				luaL_error(L, "unexpected null userdata");
			}
			break;
		}
		default: {
			luaL_error(L, "unknown type cannot be converted to variant");
			break;
		}
	}
	return VariantValue();
}

int Lua_Variant::newInstance(lua_State* L)
{
	int n = lua_gettop(L);

	if (n == 0) {
        create(L, Class());
	} else {
        create(L, Class(), getFromStack(L, 1));
	}
	return 1;
}

int Lua_Variant::tostring(lua_State* L)
{
	Lua_Variant* v = checkUserData(L);

	if (v != nullptr && v->m_variant.isValid()) {
		lua_pushstring(L, v->m_variant.convertTo<std::string>().c_str());
	} else {
		lua_pushstring(L, "uninitialized variant");
	}

	return 1;
}

int Lua_Variant::tonumber(lua_State* L)
{
	Lua_Variant* v = checkUserData(L);
	LuaUtils::LuaValue<double>::pushValue(L, v->m_variant.convertTo<double>());
	return 1;
}

template<typename T = double>
T getArithOperand(lua_State* L, int index)
{
    if (lua_type(L, index) == LUA_TNUMBER) {
        return lua_tonumber(L, index);
    } else if (Lua_Variant::isUserData(L, index)) {
        Lua_Variant* v = (Lua_Variant*)lua_touserdata(L, index);
        T t = v->wrapped().convertToThrow<T>();
        return t;
    } else {
        luaL_error(L, "invalid operand to arithmetic operation");
        return 0;
    }
}

int Lua_Variant::add(lua_State* L)
{
    lua_pushnumber(L, getArithOperand(L,1)+getArithOperand(L,2));
    return 1;
}

int Lua_Variant::sub(lua_State* L)
{
    lua_pushnumber(L, getArithOperand(L,1)-getArithOperand(L,2));
    return 1;
}

int Lua_Variant::mul(lua_State* L)
{
    lua_pushnumber(L, getArithOperand(L,1)*getArithOperand(L,2));
    return 1;
}

int Lua_Variant::div(lua_State* L)
{
    lua_pushnumber(L, getArithOperand(L,1)/getArithOperand(L,2));
    return 1;
}

int Lua_Variant::mod(lua_State* L)
{
    lua_pushnumber(L, getArithOperand<int>(L,1)%getArithOperand<int>(L,2));
    return 1;
}

int Lua_Variant::unm(lua_State* L)
{
    lua_pushnumber(L, -getArithOperand(L,1));
    return 1;
}

int Lua_Variant::isValid(lua_State* L)
{
	Lua_Variant* v = checkUserData(L);
	LuaUtils::LuaValue<bool>::pushValue(L, v->m_variant.isValid());
	return 1;
}

int Lua_Variant::isStdString(lua_State* L)
{
	Lua_Variant* v = checkUserData(L);
	LuaUtils::LuaValue<bool>::pushValue(L, v->m_variant.isStdString());
	return 1;
}

int Lua_Variant::isIntegral(lua_State* L)
{
	Lua_Variant* v = checkUserData(L);
	LuaUtils::LuaValue<bool>::pushValue(L, v->m_variant.isIntegral());
	return 1;
}

int Lua_Variant::isFloatingPoint(lua_State* L)
{
	Lua_Variant* v = checkUserData(L);
	LuaUtils::LuaValue<bool>::pushValue(L, v->m_variant.isFloatingPoint());
	return 1;
}

int Lua_Variant::isArithmetical(lua_State* L)
{
	Lua_Variant* v = checkUserData(L);
	LuaUtils::LuaValue<bool>::pushValue(L, v->m_variant.isArithmetical());
	return 1;
}

int Lua_Variant::isPOD(lua_State* L)
{
	Lua_Variant* v = checkUserData(L);
	LuaUtils::LuaValue<bool>::pushValue(L, v->m_variant.isPOD());
	return 1;
}

int Lua_Variant::sizeOf(lua_State* L)
{
	Lua_Variant* v = checkUserData(L);
	LuaUtils::LuaValue<int>::pushValue(L, v->m_variant.sizeOf());
	return 1;
}

int Lua_Variant::alignOf(lua_State* L)
{
	Lua_Variant* v = checkUserData(L);
	LuaUtils::LuaValue<int>::pushValue(L, v->m_variant.alignOf());
	return 1;
}

int Lua_Variant::method_stub(lua_State* L)
{
    Lua_Class& c = *Lua_Class::checkUserData(L, lua_upvalueindex(1));
    const string name = luaL_checkstring(L, lua_upvalueindex(2));
    const int numArgs = lua_gettop(L) - 1;

    auto methods = c.wrapped().findAllMethods([&](const Method& m){ return m.name() == name && m.numberOfArguments() == numArgs;});

    if (methods.size() == 0) {
        luaL_error(L, strconv::fmt_str("Class %1 has no method named %2", c.wrapped().fullyQualifiedName(), name).c_str());
    } else if (methods.size() > 1) {
        luaL_error(L, strconv::fmt_str("Class %1 has more than one method named %2 with %3 arguments", c.wrapped().fullyQualifiedName(), name, numArgs).c_str());
    }

    Method& m = methods.front();

    Lua_Method::create(L, m);
    for (int i = 1; i <= numArgs+1; ++i) {
        lua_pushvalue(L, 1);
        lua_remove(L, 1);
    }

    return Lua_Method::call(L);
}


//---------------Class----------------------------------------------------------

const char * Lua_Class::metatableName = "SelfPortrait.Class";
const char * Lua_Class::userDataName  = "Class";
MethodTable Lua_Class::methods;


const struct luaL_Reg Lua_Class::lib_f[] = {
	{ "lookup", exception_translator<lookup> },
	{ NULL, NULL }
};

const struct luaL_Reg Lua_Class::lib_m[] = {
	{ "__gc", exception_translator<gc> },
	{ "__index", exception_translator<index> },
	{ "__tostring", exception_translator<fullyQualifiedName> },
	{ "__eq", exception_translator<eq> },
	{ NULL, NULL }
};

void Lua_Class::initialize()
{
	methods["fullyQualifiedName"]  = exception_translator<fullyQualifiedName>;
	methods["simpleName"]          = exception_translator<simpleName>;
	methods["isInterface"]         = exception_translator<isInterface>;
	methods["methods"]             = exception_translator<getMethods>;
	methods["constructors"]        = exception_translator<getConstructors>;
    methods["construct"]           = exception_translator<construct>;
	methods["attributes"]          = exception_translator<getAttributes>;
	methods["superclasses"]        = exception_translator<getSuperClasses>;
	methods["findAttribute"]       = exception_translator<findAttribute>;
	methods["findAllAttributes"]   = exception_translator<findAllAttributes>;
	methods["findConstructor"]     = exception_translator<findConstructor>;
	methods["findAllConstructors"] = exception_translator<findAllConstructors>;
	methods["findMethod"]          = exception_translator<findMethod>;
	methods["findAllMethods"]      = exception_translator<findAllMethods>;
	methods["findSuperClass"]      = exception_translator<findSuperClass>;
	methods["findAllSuperClasses"] = exception_translator<findAllSuperClasses>;
}

int Lua_Class::lookup(lua_State* L)
{
	size_t length = 0;
	const char * name = luaL_checklstring(L, 1, &length);

	luaL_argcheck(L, name && (length > 0), 1, "empty class name");

	Class c = Class::lookup(name);

	if (c.isValid()) {
		LuaAdapter<Lua_Class>::create(L,c);
	} else {
		lua_pushnil(L);
	}
	return 1;
}

int Lua_Class::fullyQualifiedName(lua_State* L)
{
	Lua_Class* c = checkUserData(L);

	lua_pushstring(L, c->m_class.fullyQualifiedName().c_str());
	return 1;
}

int Lua_Class::simpleName(lua_State* L)
{
	Lua_Class* c = checkUserData(L);

	lua_pushstring(L, c->m_class.simpleName().c_str());
	return 1;
}

int Lua_Class::isInterface(lua_State* L)
{
	Lua_Class* c = checkUserData(L);

	lua_pushboolean(L, c->m_class.isInterface());
	return 1;
}

int Lua_Class::getMethods(lua_State* L)
{
	Lua_Class* c = checkUserData(L);

	const Class::MethodList&  m = c->m_class.methods();


	lua_createtable(L, m.size(), 0);

	int i = 0;

	for (auto it = m.begin(); it != m.end(); ++it) {
		Lua_Method::create(L, *it);
		lua_rawseti(L, -2, ++i);
	}

	return 1;
}

int Lua_Class::getConstructors(lua_State* L)
{
	Lua_Class* c = checkUserData(L);

	const Class::ConstructorList&  l = c->m_class.constructors();

	lua_createtable(L, l.size(), 0);

	int i = 0;
	for (auto it = l.begin(); it != l.end(); ++it) {
		Lua_Constructor::create(L, *it);
		lua_rawseti(L, -2, ++i);
	}
	return 1;
}


int Lua_Class::construct(lua_State* L)
{
    Lua_Class* c = checkUserData(L);
    const int numArgs = lua_gettop(L) - 1;

    const Class::ConstructorList&  l = c->m_class.findAllConstructors([&](const Constructor& cons){ return cons.numberOfArguments() == numArgs; });
    if (l.size() == 0) {
        luaL_error(L, strconv::fmt_str("Class %1 has no constructor with %2 arguments", c->m_class.fullyQualifiedName(), numArgs).c_str());
    } else if (l.size() > 1) {
        luaL_error(L, strconv::fmt_str("Class %1 has more than one constructor with %2 arguments", c->m_class.fullyQualifiedName(), numArgs).c_str());
    }
    lua_remove(L, 1);

    const Constructor& cons = l.front();

    Lua_Constructor::create(L, cons);
    for (int i = 1; i <= numArgs; ++i) {
        lua_pushvalue(L, 1);
        lua_remove(L, 1);
    }

    return Lua_Constructor::call(L);
}

int Lua_Class::getAttributes(lua_State* L)
{
	Lua_Class* c = checkUserData(L);

	const Class::AttributeList&  l = c->m_class.attributes();

	lua_createtable(L, l.size(), 0);

	int i = 0;
	for (auto it = l.begin(); it != l.end(); ++it) {
		Lua_Attribute::create(L, *it);
		lua_rawseti(L, -2, ++i);
	}
	return 1;
}

int Lua_Class::getSuperClasses(lua_State* L)
{
	Lua_Class* c = checkUserData(L);

	const Class::ClassList&  l = c->m_class.superclasses();

	lua_createtable(L, l.size(), 0);

	int i = 0;
	for (auto it = l.begin(); it != l.end(); ++it) {
		Lua_Class::create(L, *it);
		lua_rawseti(L, -2, ++i);
	}
	return 1;
}

template<class Elem, class MPtr>
int Lua_Class::findFirst(lua_State* L, MPtr ptr)
{
	Lua_Class* c = checkUserData(L);

	Elem a = (c->m_class.*ptr)([&](const Elem& a) -> bool {
		luaL_checktype(L, -1, LUA_TFUNCTION);
		lua_pushvalue(L, -1);
		binding_mapper<Elem>::type::create(L, a);

		if (lua_pcall(L, 1, 1, 0) != 0) {
            std::string msg = strconv::fmt_str("Error running anonymous lua function: %1", lua_tostring(L, -1));
			lua_pop(L, 1);
			throw LuaBindingException(msg);
		}
		luaL_checktype(L, -1, LUA_TBOOLEAN);
		bool ret = lua_toboolean(L, -1);
		lua_pop(L, 1);
		return ret;
	});
	if (a.isValid()) {
		binding_mapper<Elem>::type::create(L, a);
		return 1;
	}
	lua_pop(L, 1);
	return 0;
}


template<class ElemList, class MPtr>
int Lua_Class::findAll(lua_State* L, MPtr ptr)
{
	Lua_Class* c = checkUserData(L);
	typedef typename ElemList::value_type Elem;

	ElemList l = (c->m_class.*ptr)([&](const Elem& a) -> bool {
		luaL_checktype(L, -1, LUA_TFUNCTION);
		lua_pushvalue(L, -1);
		binding_mapper< Elem >::type::create(L, a);

		if (lua_pcall(L, 1, 1, 0) != 0) {
            std::string msg = strconv::fmt_str("Error running anonymous lua function: %1", lua_tostring(L, -1));
			lua_pop(L, 1);
			throw LuaBindingException(msg);
		}
		luaL_checktype(L, -1, LUA_TBOOLEAN);
		bool ret = lua_toboolean(L, -1);
		lua_pop(L, 1);
		return ret;
	});

	lua_createtable(L, l.size(), 0);

	int i = 0;
	for (auto it = l.begin(); it != l.end(); ++it) {
		binding_mapper< Elem >::type::create(L, *it);
		lua_rawseti(L, -2, ++i);
	}
	return 1;
}


int Lua_Class::findAttribute(lua_State* L)
{
	return findFirst<Attribute>(L, &Class::findAttribute);
}

int Lua_Class::findAllAttributes(lua_State* L)
{
	return findAll<Class::AttributeList>(L, &Class::findAllAttributes);
}

int Lua_Class::findConstructor(lua_State* L)
{
	return findFirst<Constructor>(L, &Class::findConstructor);
}

int Lua_Class::findAllConstructors(lua_State* L)
{
	return findAll<Class::ConstructorList>(L, &Class::findAllConstructors);
}

int Lua_Class::findMethod(lua_State* L)
{
	return findFirst<Method>(L, &Class::findMethod);
}

int Lua_Class::findAllMethods(lua_State* L)
{
	return findAll<Class::MethodList>(L, &Class::findAllMethods);
}

int Lua_Class::findSuperClass(lua_State* L)
{
	return findFirst<Class>(L, &Class::findSuperClass);
}

int Lua_Class::findAllSuperClasses(lua_State* L)
{
	return findAll<Class::ClassList>(L, &Class::findAllSuperClasses);
}



//---------------Method---------------------------------------------------------

const char * Lua_Method::metatableName = "SelfPortrait.Method";
const char * Lua_Method::userDataName  = "Method";
MethodTable Lua_Method::methods;


const struct luaL_Reg Lua_Method::lib_f[] = {
	{ NULL, NULL }
};

void Lua_Method::initialize()
{
	methods["name"]              = exception_translator<name>;
	methods["fullName"]          = exception_translator<fullName>;
	methods["call"]              = exception_translator<call>;
	methods["numberOfArguments"] = exception_translator<numberOfArguments>;
	methods["returnSpelling"]    = exception_translator<returnSpelling>;
	methods["argumentSpellings"] = exception_translator<argumentSpellings>;
	methods["isConst"]           = exception_translator<isConst>;
	methods["isVolatile"]        = exception_translator<isVolatile>;
	methods["isStatic"]          = exception_translator<isStatic>;
	methods["getClass"]          = exception_translator<getClass>;
}

int Lua_Method::name(lua_State* L)
{
	Lua_Method* c = checkUserData(L);
	LuaUtils::LuaValue<std::string>::pushValue(L, c->m_method.name());
	return 1;
}

int Lua_Method::fullName(lua_State* L)
{
	Lua_Method* c = checkUserData(L);
	LuaUtils::LuaValue<std::string>::pushValue(L, c->m_method.fullName());
	return 1;
}

int Lua_Method::numberOfArguments(lua_State* L)
{
	Lua_Method* c = checkUserData(L);
	LuaUtils::LuaValue<int>::pushValue(L, c->m_method.numberOfArguments());
	return 1;
}

int Lua_Method::returnSpelling(lua_State* L)
{
	Lua_Method* c = checkUserData(L);
	LuaUtils::LuaValue<std::string>::pushValue(L, c->m_method.returnSpelling());
	return 1;
}

int Lua_Method::argumentSpellings(lua_State* L)
{
	Lua_Method* c = checkUserData(L);
	LuaUtils::LuaValue<std::vector<std::string>>::pushValue(L, c->m_method.argumentSpellings());
	return 1;
}

int Lua_Method::isConst(lua_State* L)
{
	Lua_Method* c = checkUserData(L);
	LuaUtils::LuaValue<bool>::pushValue(L, c->m_method.isConst());
	return 1;
}

int Lua_Method::isVolatile(lua_State* L)
{
	Lua_Method* c = checkUserData(L);
	LuaUtils::LuaValue<bool>::pushValue(L, c->m_method.isVolatile());
	return 1;
}

int Lua_Method::isStatic(lua_State* L)
{
	Lua_Method* c = checkUserData(L);
	LuaUtils::LuaValue<bool>::pushValue(L, c->m_method.isStatic());
	return 1;
}


int Lua_Method::call(lua_State* L)
{
	Lua_Method* m = checkUserData(L);
    vector<VariantValue> args;

	VariantValue obj;

	int n = lua_gettop(L);

	int begin = 2;

	if (!m->m_method.isStatic()) {
		if (n <= 1) {
			luaL_error(L, "cannot call non-static member function without object");
		}
		obj = Lua_Variant::getFromStack(L, begin++);
	}

    for (int i = begin; i <= n; ++i) {
		args.push_back(Lua_Variant::getFromStack(L, i));
	}

	VariantValue ret;

	ret = m->m_method.callArgArray(obj, args); // if the method is static, it ignores the first arg

	if (ret.isValid()) {

		if (ret.isArithmetical()) {
			lua_pushnumber(L, ret.convertTo<lua_Number>());
		} else if (ret.isStdString()) {
			lua_pushstring(L, ret.convertTo<std::string>().c_str());
		} else {
            Class clazz;
#ifndef NO_RTTI
            clazz = Class::lookup(ret.typeId());
#endif
            Lua_Variant::create(L, clazz, std::move(ret));
		}

		return 1;
	} else {
		return 0;
	}
}


int Lua_Method::getClass(lua_State* L)
{
	Lua_Method* c = checkUserData(L);
	Lua_Class::create(L, c->m_method.getClass());
	return 1;
}


//---------------Constructor----------------------------------------------------

const char * Lua_Constructor::metatableName = "SelfPortrait.Constructor";
const char * Lua_Constructor::userDataName = "Constructor";
MethodTable Lua_Constructor::methods;

const struct luaL_Reg Lua_Constructor::lib_f[] = {
	{ NULL, NULL }
};

void Lua_Constructor::initialize()
{
	methods["call"]                 = exception_translator<call>;
	methods["numberOfArguments"]    = exception_translator<numberOfArguments>;
	methods["argumentSpellings"]    = exception_translator<argumentSpellings>;
	methods["isDefaultConstructor"] = exception_translator<isDefaultConstructor>;
	methods["getClass"]             = exception_translator<getClass>;
}


int Lua_Constructor::call(lua_State* L)
{
	Lua_Constructor* c = checkUserData(L);
	vector<VariantValue> args;

	int n = lua_gettop(L);

	for (int i = 2; i <= n; ++i) {
		args.push_back(Lua_Variant::getFromStack(L, i));
	}

    Lua_Variant::create(L, c->wrapped().getClass(), ::std::move(c->m_constructor.callArgArray(args)));
	return 1;
}

int Lua_Constructor::numberOfArguments(lua_State* L)
{
	Lua_Constructor* c = checkUserData(L);
	LuaUtils::LuaValue<int>::pushValue(L, c->m_constructor.numberOfArguments());
	return 1;
}

int Lua_Constructor::argumentSpellings(lua_State* L)
{
	Lua_Constructor* c = checkUserData(L);
	LuaUtils::LuaValue<std::vector<std::string>>::pushValue(L, c->m_constructor.argumentSpellings());
	return 1;
}

int Lua_Constructor::isDefaultConstructor(lua_State* L)
{
	Lua_Constructor* c = checkUserData(L);
	LuaUtils::LuaValue<bool>::pushValue(L, c->m_constructor.isDefaultConstructor());
	return 1;
}

int Lua_Constructor::getClass(lua_State* L)
{
	Lua_Constructor* c = checkUserData(L);
	Lua_Class::create(L, c->m_constructor.getClass());
	return 1;
}

//---------------Attribute------------------------------------------------------

const char * Lua_Attribute::metatableName = "SelfPortrait.Attribute";
const char * Lua_Attribute::userDataName  = "Attribute";

MethodTable Lua_Attribute::methods;

const struct luaL_Reg Lua_Attribute::lib_f[] = {
	{ NULL, NULL }
};

void Lua_Attribute::initialize()
{
	methods["get"]          = exception_translator<get>;
	methods["set"]          = exception_translator<set>;
	methods["name"]         = exception_translator<name>;
	methods["typeSpelling"] = exception_translator<typeSpelling>;
	methods["isConst"]      = exception_translator<isConst>;
	methods["isStatic"]     = exception_translator<isStatic>;
	methods["getClass"]     = exception_translator<getClass>;
}

int Lua_Attribute::get(lua_State* L)
{
	Lua_Attribute* c = checkUserData(L);
	VariantValue obj;

	if (!c->m_attribute.isStatic()) {

		if (lua_gettop(L) < 2) {
			luaL_error(L, "cannot get non-static attribute without object");
		}
		obj = Lua_Variant::getFromStack(L, 2);
	}

	VariantValue ret = c->m_attribute.get(obj);
	if (ret.isArithmetical()) {
		lua_pushnumber(L, ret.convertTo<lua_Number>());
	} else if (ret.isStdString()) {
		lua_pushstring(L, ret.convertTo<std::string>().c_str());
	} else {
        Class clazz;
#ifndef NO_RTTI
        clazz = Class::lookup(ret.typeId());
#endif
        Lua_Variant::create(L, clazz, std::move(ret));
	}

	return 1;
}

int Lua_Attribute::set(lua_State* L)
{
	Lua_Attribute* c = checkUserData(L);
	VariantValue obj;

	int begin = 2;

	if (!c->m_attribute.isStatic()) {
		if (lua_gettop(L) < 3) {
			luaL_error(L, "cannot set non-static attribute without object");
		}
		obj = Lua_Variant::getFromStack(L, begin++);
	} else {
		if (lua_gettop(L) < 2) {
			luaL_error(L, "no parameter given to set");
		}
	}

	VariantValue val = Lua_Variant::getFromStack(L, begin);

	c->m_attribute.set(obj, val);
	return 0;
}

int Lua_Attribute::name(lua_State* L)
{
	Lua_Attribute* c = checkUserData(L);
	LuaUtils::LuaValue<std::string>::pushValue(L, c->m_attribute.name());
	return 1;
}

int Lua_Attribute::typeSpelling(lua_State* L)
{
	Lua_Attribute* c = checkUserData(L);
	LuaUtils::LuaValue<std::string>::pushValue(L, c->m_attribute.typeSpelling());
	return 1;
}
int Lua_Attribute::isConst(lua_State* L)
{
	Lua_Attribute* c = checkUserData(L);
	LuaUtils::LuaValue<bool>::pushValue(L, c->m_attribute.isConst());
	return 1;
}
int Lua_Attribute::isStatic(lua_State* L)
{
	Lua_Attribute* c = checkUserData(L);
	LuaUtils::LuaValue<bool>::pushValue(L, c->m_attribute.isStatic());
	return 1;
}

int Lua_Attribute::getClass(lua_State* L)
{
	Lua_Attribute* c = checkUserData(L);
	Lua_Class::create(L, c->m_attribute.getClass());
	return 1;
}

//---------------Function-------------------------------------------------------

const char * Lua_Function::metatableName = "SelfPortrait.Function";
const char * Lua_Function::userDataName  = "Function";
MethodTable Lua_Function::methods;

const struct luaL_Reg Lua_Function::lib_f[] = {
	{ "lookup", exception_translator<lookup> },
	{ NULL, NULL }
};


void Lua_Function::initialize()
{
	methods["call"]              = exception_translator<call>;
	methods["name"]              = exception_translator<name>;
	methods["numberOfArguments"] = exception_translator<numberOfArguments>;
	methods["returnSpelling"]    = exception_translator<returnSpelling>;
	methods["argumentSpellings"] = exception_translator<argumentSpellings>;
}

int Lua_Function::name(lua_State* L)
{
	Lua_Function* f = checkUserData(L);
	lua_pushstring(L, f->m_function.name().c_str());
	return 1;
}

int Lua_Function::call(lua_State* L)
{
	Lua_Function* f = checkUserData(L);
	vector<VariantValue> args;


	int n = lua_gettop(L);

	for (int i = 2; i <= n; ++i) {
		args.push_back(Lua_Variant::getFromStack(L, i));
	}

	VariantValue ret;

	ret = f->m_function.callArgArray(args);

	if (ret.isValid()) {

		if (ret.isArithmetical()) {
			lua_pushnumber(L, ret.convertTo<lua_Number>());
		} else if (ret.isStdString()) {
			lua_pushstring(L, ret.convertTo<std::string>().c_str());
		} else {      
            Class clazz;
#ifndef NO_RTTI
            clazz = Class::lookup(ret.typeId());
#endif
            Lua_Variant::create(L, clazz, std::move(ret));
		}
		return 1;
	} else {
		return 0;
	}
}

int Lua_Function::numberOfArguments(lua_State* L)
{
	Lua_Function* c = checkUserData(L);
	LuaUtils::LuaValue<int>::pushValue(L, c->m_function.numberOfArguments());
	return 1;
}

int Lua_Function::returnSpelling(lua_State* L)
{
	Lua_Function* c = checkUserData(L);
	LuaUtils::LuaValue<std::string>::pushValue(L, c->m_function.returnSpelling());
	return 1;
}

int Lua_Function::argumentSpellings(lua_State* L)
{
	Lua_Function* c = checkUserData(L);
	LuaUtils::LuaValue<std::vector<std::string>>::pushValue(L, c->m_function.argumentSpellings());
	return 1;
}


int Lua_Function::lookup(lua_State* L)
{
	size_t length = 0;
	const char * name = luaL_checklstring(L, 1, &length);

	luaL_argcheck(L, name && (length > 0), 1, "empty function name");

	const Function::FunctionList& l = Function::findFunctions(name);

	if (l.empty()) {
		lua_pushnil(L);
	} else if (l.size() == 1) {
		Lua_Function::create(L, l.front());
	} else {
		lua_createtable(L, l.size(), 0);
		int i = 0;
		for (auto it = l.begin(); it != l.end(); ++it) {
			Lua_Function::create(L, *it);
			lua_rawseti(L, -2, ++i);
		}
	}
	return 1;
}

//---------------Proxy----------------------------------------------------------

const char * Lua_Proxy::metatableName = "SelfPortrait.Proxy";
const char * Lua_Proxy::userDataName  = "Proxy";
MethodTable Lua_Proxy::methods;


const struct luaL_Reg Lua_Proxy::lib_f[] = {
	{ "create", exception_translator<create> },
	{ NULL, NULL }
};

const struct luaL_Reg Lua_Proxy::lib_m[] = {
	{ "__gc", exception_translator<gc> },
	{ "__index", exception_translator<index> },
	{ "__eq", exception_translator<eq> },
	{ NULL, NULL }
};

void Lua_Proxy::initialize()
{
	methods["interfaces"]        = exception_translator<interfaces>;
	methods["addImplementation"] = exception_translator<addImplementation>;
	methods["hasImplementation"] = exception_translator<hasImplementation>;
	methods["reference"]         = exception_translator<reference>;
}

int Lua_Proxy::create(lua_State* L)
{
	const int args = lua_gettop(L);
	if (args == 0) {
		luaL_error(L, "Expected at least one class to create a proxy");
	}
	std::vector<Class> ifaces(args);

	for (int i = args; i > 0; --i) {
		Lua_Class* c = Lua_Class::checkUserData(L, i);
		ifaces[args-i] = c->wrapped();
	}
	Proxy p(ifaces);

	LuaAdapter<Lua_Proxy>::create(L, std::move(p));
	return 1;
}

int Lua_Proxy::interfaces(lua_State* L)
{
	Lua_Proxy* p = checkUserData(L);

	Proxy::IFaceList  il = p->m_proxy.interfaces();

	lua_createtable(L, il.size(), 0);

	int i = 0;

	for (auto it = il.begin(); it != il.end(); ++it) {
		Lua_Class::create(L, *it);
		lua_rawseti(L, -2, ++i);
	}

	return 1;
}


void LuaClosureWrapper::deleter::operator()(shared_state* s) const {
    luaL_unref(L, LUA_REGISTRYINDEX, s->envIndex);
    delete s;
}

VariantValue LuaClosureWrapper::operator()(const std::vector<VariantValue>& vargs) const {


    lua_rawgeti(L, LUA_REGISTRYINDEX, ss->envIndex);
    luaL_checktype(L, -1, LUA_TFUNCTION);

    for(const VariantValue& v: vargs) {
        Class clazz;
#ifndef NO_RTTI
        if (!v.isArithmetical()) {
            clazz = Class::lookup(v.typeId());
        }
#endif
        Lua_Variant::create(L, clazz, std::move(v));
    }

    if (lua_pcall(L, vargs.size(), 1, 0) != 0) {
        std::string msg = strconv::fmt_str("Error running proxy method handler: %1", lua_tostring(L, -1));
        lua_pop(L, 1);
        throw LuaBindingException(msg);
    }
    VariantValue ret = Lua_Variant::getFromStack(L, -1);
    lua_pop(L, 1);

    return ret;
}


int Lua_Proxy::addImplementation(lua_State* L)
{
	Lua_Proxy* p = checkUserData(L,1);
	Lua_Method* m = Lua_Method::checkUserData(L,2);
	luaL_checktype(L, 3, LUA_TFUNCTION);

	const int ref = luaL_ref(L, LUA_REGISTRYINDEX); // ja fez o push da funcao

    p->m_proxy.addImplementation(m->wrapped(), LuaClosureWrapper(L, ref));

	return 0;
}


int Lua_Proxy::hasImplementation(lua_State* L)
{
	Lua_Proxy* p = checkUserData(L,1);
	Lua_Method* m = Lua_Method::checkUserData(L,2);
	lua_pushboolean(L, p->m_proxy.hasImplementation(m->wrapped()));
	return 1;
}


int Lua_Proxy::reference(lua_State* L)
{
	Lua_Proxy* p = checkUserData(L,1);
	Lua_Class* c = Lua_Class::checkUserData(L,2);
    VariantValue v = p->m_proxy.reference(c->wrapped());
    Class clazz;
#ifndef NO_RTTI
    clazz = Class::lookup(v.typeId());
#endif
    Lua_Variant::create(L, clazz, std::move(v));

	return 1;
}


/* Library loading functions adapted from loadlib.c from the lua source code
 *
 */

#if defined(__unix__)
/*
** {========================================================================
** This is an implementation of loadlib based on the dlfcn interface.
** The dlfcn interface is available in Linux, SunOS, Solaris, IRIX, FreeBSD,
** NetBSD, AIX 4.2, HPUX 11, and  probably most other Unix flavors, at least
** as an emulation layer on top of native functions.
** =========================================================================
*/

#include <dlfcn.h>

static void unloadlib (void *lib) {
  dlclose(lib);
}

static void *loadlib(const char *path) {
  void *lib = dlopen(path, RTLD_NOW | RTLD_GLOBAL);
  if (lib == nullptr) {
    throw std::runtime_error(dlerror());
  }
  return lib;
}


/* }====================================================== */



#elif defined(LUA_DL_DLL)
/*
** {======================================================================
** This is an implementation of loadlib for Windows using native functions.
** =======================================================================
*/


/*
** optional flags for LoadLibraryEx
*/

static void unloadlib (void *lib) {
  FreeLibrary((HMODULE)lib);
}

static void *loadlib (const char *path) {
  HMODULE lib = LoadLibraryExA(path, NULL, 0);
  if (lib == nullptr) {
    throw std::runtime_error(std::string("could not load library ")+path);
  }
  return lib;
}

/* }====================================================== */


#else
/*
** {======================================================
** Fallback for other systems
** =======================================================
*/

static void unloadlib (void *lib) {
  (void)(lib);  /* not used */
}


static void *loadlib (const char *path) {
  throw std::runtime_error("loading dynamic libraries is not supported on this system");
  return NULL;
}


/* }====================================================== */
#endif


//---------------Library----------------------------------------------------------

Lua_Library::Lua_Library(string library)
    : m_libraryName(library)
    , m_library(loadlib(m_libraryName.c_str()))
{}



Lua_Library::~Lua_Library()
{
    if (m_library) {
        unloadlib(m_library);
        m_library = nullptr;
    }
}

const char * Lua_Library::metatableName = "SelfPortrait.Library";
const char * Lua_Library::userDataName  = "Library";
MethodTable Lua_Library::methods;


const struct luaL_Reg Lua_Library::lib_f[] = {
    { "new", exception_translator<newInstance> },
    { NULL, NULL }
};

const struct luaL_Reg Lua_Library::lib_m[] = {
    { "__gc", exception_translator<gc> },
    { "__index", exception_translator<index> },
    { "__eq", exception_translator<eq> },
    { NULL, NULL }
};

void Lua_Library::initialize()
{
    methods["close"] = exception_translator<close>;
}

int Lua_Library::newInstance(lua_State* L)
{
    size_t length = 0;
    string name = luaL_checklstring(L, 1, &length);
    create(L, name);
    return 1;
}

int Lua_Library::close(lua_State* L)
{
    Lua_Library* l = checkUserData(L);

    if (l->m_library) {
        unloadlib(l->m_library);
        l->m_library = nullptr;
    }

    return 1;
}

}

extern "C" {

    int luaopen_libluaselfportrait ( lua_State* L )
    {
        using namespace SelfPortraitLua;
		Lua_Class::_register(L);
		Lua_Attribute::_register(L);
		Lua_Method::_register(L);
		Lua_Constructor::_register(L);
		Lua_Variant::_register(L);
		Lua_Function::_register(L);
		Lua_Proxy::_register(L);
        Lua_Library::_register(L);
		return 1;
    }
}

