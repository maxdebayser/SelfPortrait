#include <lua.hpp>
#include "reflection_impl.h"
#include <ctype.h>

#include <map>
#include <string>
#include <vector>
#include <iostream>
using namespace std;


typedef map<string, lua_CFunction> MethodTable;

struct Foo {

	Foo(int i) : attr(i) {}

	int attr;

	void print() {
		cout << "hello world: attr = " << attr << endl;
	}

	void printArg(int i) {
		cout << "hello world: attr+1 = " << (attr+i) << endl;
	}

	static void staticPrint() {
		cout << "hello world" << endl;
	}

	static void staticPrintArg(int i) {
		cout << "hello world " << i << endl;
	}

	Foo(const Foo&) = delete;
	Foo& operator=(const Foo&) = delete;
	Foo(Foo&&) = delete;
	Foo& operator=(Foo&&) = delete;
};

int function1(int i) {
	return i + 3;
}

double function2(double i) {
	return i + 3;
}

double function2(double i, double s) {
	return i + s;
}

int getFooAttr(const Foo& foo) {
	return foo.attr;
}

REFL_BEGIN_CLASS(Foo)
REFL_CONSTRUCTOR(int)
REFL_ATTRIBUTE(attr, int)
REFL_METHOD(print, void)
REFL_METHOD(printArg, void, int)
REFL_STATIC_METHOD(staticPrint, void)
REFL_STATIC_METHOD(staticPrintArg, void, int)
REFL_END_CLASS

REFL_FUNCTION(function1, int, int)
REFL_FUNCTION(function2, double, double)
REFL_FUNCTION(function2, double, double, double)
REFL_FUNCTION(getFooAttr, int, const Foo&)

//=====================Declarations=============================================

template<class T>
class LuaAdapter {
public:
	typedef T Adapted;

	static void _register(lua_State* L) {
		Adapted::initialize();

		luaL_newmetatable(L, Adapted::metatableName);
		luaL_register(L, NULL, Adapted::lib_m);

		luaL_register(L, Adapted::userDataName, Adapted::lib_f);
	}

	static int gc(lua_State* L){
		Adapted* c = Adapted::checkUserData(L);
		c->~Adapted();
		return 0;
	}

	static int index(lua_State* L) {
		Adapted* c = Adapted::checkUserData(L);
		const char * index = luaL_checkstring(L, 2);

		auto it = Adapted::methods.find(index);

		if (it != Adapted::methods.end()) {
			lua_pushcfunction(L, it->second);
			return 1;
		} else {
			luaL_error(L, "class has no property %s\n", index);
		}
		return 0;
	}

	template<class... Args>
	static void create(lua_State* L, Args&&... args) {
		void * f = lua_newuserdata(L, sizeof(Adapted));
		Adapted * lc = new(f) Adapted(args...);
		luaL_getmetatable(L, Adapted::metatableName);
		lua_setmetatable(L, -2);
	}

	static const struct luaL_Reg lib_m[];
};

template<class T>
const struct luaL_Reg LuaAdapter<T>::lib_m[] = {
	{ "__gc", gc },
	{ "__index", index },
	{ NULL, NULL }
};

//---------------Variant--------------------------------------------------------

class Lua_Variant: public LuaAdapter<Lua_Variant> {
public:

	template<class Arg>
	Lua_Variant(Arg&& arg) : m_variant(arg) {}

	Lua_Variant() : m_variant() {}

	static void initialize();

	static Lua_Variant* checkVariant(lua_State* L, int idx = 1) { return (Lua_Variant*)luaL_checkudata(L, idx, Lua_Variant::metatableName); }

	static Lua_Variant* checkUserData(lua_State* L) { return checkVariant(L); }

	static VariantValue getFromStack(lua_State* L, int idx = 1);

	static int newInstance(lua_State* L);

	static int tostring(lua_State* L);

	static const char * metatableName;
	static const char * userDataName;

	static void create(lua_State* L, VariantValue&& v) {
		void * f = lua_newuserdata(L, sizeof(Lua_Variant));
		Lua_Variant * lc = new(f) Lua_Variant();
		lc->m_variant = ::std::move(v);
		luaL_getmetatable(L, Lua_Variant::metatableName);
		lua_setmetatable(L, -2);
	}

	template<class... Args>
	static void create(lua_State* L, Args&&... args) {
		void * f = lua_newuserdata(L, sizeof(Adapted));
		Adapted * lc = new(f) Adapted(args...);
		luaL_getmetatable(L, Adapted::metatableName);
		lua_setmetatable(L, -2);
	}

private:
	VariantValue m_variant;
	static MethodTable methods;
	static const struct luaL_Reg lib_f[];
	static const struct luaL_Reg lib_m[];

	friend class LuaAdapter<Lua_Variant>;
};


//---------------Class----------------------------------------------------------

class Lua_Class: public LuaAdapter<Lua_Class> {
public:
	Lua_Class(Class c) : m_class(c) {}

	static int lookup(lua_State* L);

	static int fullyQualifiedName(lua_State* L);
	static int simpleName(lua_State* L);
	static int getMethods(lua_State* L);
	static int getConstructors(lua_State* L);
	static int getAttributes(lua_State* L);
	static int getSuperClasses(lua_State* L);

	static void initialize();
	static Lua_Class* checkUserData(lua_State* L) { return (Lua_Class*)luaL_checkudata(L, 1, Lua_Class::metatableName); }

	static const char * metatableName;
	static const char * userDataName;

private:
	Class m_class;
	static MethodTable methods;
	static const struct luaL_Reg lib_f[];
	static const struct luaL_Reg lib_m[];
	friend class LuaAdapter<Lua_Class>;
};

//---------------Method---------------------------------------------------------

class Lua_Method: public LuaAdapter<Lua_Method> {
public:
	Lua_Method(Method m) : m_method(m) {}

	static int name(lua_State* L);
	static int call(lua_State* L);

	static void initialize();
	static Lua_Method* checkUserData(lua_State* L) { return (Lua_Method*)luaL_checkudata(L, 1, Lua_Method::metatableName); }

	static const char * metatableName;
	static const char * userDataName;

private:
	Method m_method;
	static MethodTable methods;
	static const struct luaL_Reg lib_f[];
	friend class LuaAdapter<Lua_Method>;
};


//---------------Constructor----------------------------------------------------

class Lua_Constructor: public LuaAdapter<Lua_Constructor> {
public:
	Lua_Constructor(Constructor c) : m_constructor(c) {}

	static int call(lua_State* L);
	static int numberOfArguments(lua_State* L);
	static int argumentSpellings(lua_State* L);
	static int isDefaultConstructor(lua_State* L);

	static void initialize();
	static Lua_Constructor* checkUserData(lua_State* L) { return (Lua_Constructor*)luaL_checkudata(L, 1, Lua_Constructor::metatableName); }

	static const char * metatableName;
	static const char * userDataName;

private:
	Constructor m_constructor;
	static MethodTable methods;
	static const struct luaL_Reg lib_f[];
	friend class LuaAdapter<Lua_Constructor>;
};

//---------------Attribute------------------------------------------------------

class Lua_Attribute: public LuaAdapter<Lua_Attribute> {
public:
	Lua_Attribute(Attribute a) : m_attribute(a) {}

	static int get(lua_State* L);
	static int set(lua_State* L);
	static int name(lua_State* L);

	static void initialize();
	static Lua_Attribute* checkUserData(lua_State* L) { return (Lua_Attribute*)luaL_checkudata(L, 1, Lua_Attribute::metatableName); }

	static const char * metatableName;
	static const char * userDataName;

private:
	Attribute m_attribute;
	static MethodTable methods;
	static const struct luaL_Reg lib_f[];
	friend class LuaAdapter<Lua_Attribute>;
};


//---------------Function-------------------------------------------------------

class Lua_Function: public LuaAdapter<Lua_Function> {
public:
	Lua_Function(Function f) : m_function(f) {}

	static int name(lua_State* L);
	static int call(lua_State* L);
	static int lookup(lua_State* L);

	static void initialize();
	static Lua_Function* checkUserData(lua_State* L) { return (Lua_Function*)luaL_checkudata(L, 1, Lua_Function::metatableName); }

	static const char * metatableName;
	static const char * userDataName;

private:
	Function m_function;
	static MethodTable methods;
	static const struct luaL_Reg lib_f[];
	friend class LuaAdapter<Lua_Function>;
};





//=====================Definitions==============================================


//---------------Variant--------------------------------------------------------

const char * Lua_Variant::metatableName = "IoC.Variant";
const char * Lua_Variant::userDataName  = "Variant";
MethodTable Lua_Variant::methods;


const struct luaL_Reg Lua_Variant::lib_f[] = {
	{ "new", newInstance },
	{ NULL, NULL }
};

const struct luaL_Reg Lua_Variant::lib_m[] = {
	{ "__gc", gc },
	{ "__index", index },
	{ "__tostring", tostring },
	{ NULL, NULL }
};

void Lua_Variant::initialize()
{
}

VariantValue Lua_Variant::getFromStack(lua_State* L, int idx)
{
	int t = lua_type(L, idx);

	switch (t) {
		case LUA_TSTRING: {
			return ::std::move(VariantValue(string(lua_tostring(L, idx))));
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
		create(L);
	} else {
		create(L, getFromStack(L, 1));
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


//---------------Class----------------------------------------------------------

const char * Lua_Class::metatableName = "IoC.Class";
const char * Lua_Class::userDataName  = "Class";
MethodTable Lua_Class::methods;


const struct luaL_Reg Lua_Class::lib_f[] = {
	{ "lookup", lookup },
	{ NULL, NULL }
};

const struct luaL_Reg Lua_Class::lib_m[] = {
	{ "__gc", gc },
	{ "__index", index },
	{ "__tostring", fullyQualifiedName },
	{ NULL, NULL }
};

void Lua_Class::initialize()
{
	methods["fullyQualifiedName"] = &fullyQualifiedName;
	methods["simpleName"] = &simpleName;
	methods["methods"] = &getMethods;
	methods["constructors"] = &getConstructors;
	methods["attributes"] = &getAttributes;
	methods["superclasses"] = &getSuperClasses;
}

int Lua_Class::lookup(lua_State* L)
{
	size_t length = 0;
	const char * name = luaL_checklstring(L, 1, &length);

	luaL_argcheck(L, name && (length > 0), 1, "empty class name");

	Class c = Class::lookup(name);

	try {
		c.fullyQualifiedName();
		LuaAdapter<Lua_Class>::create(L,c);
	} catch (...) {
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


//---------------Method---------------------------------------------------------

const char * Lua_Method::metatableName = "IoC.Method";
const char * Lua_Method::userDataName  = "Method";
MethodTable Lua_Method::methods;


const struct luaL_Reg Lua_Method::lib_f[] = {
	{ NULL, NULL }
};

void Lua_Method::initialize()
{
	methods["name"] = &name;
	methods["call"] = &call;
}

int Lua_Method::name(lua_State* L)
{
	Lua_Method* c = checkUserData(L);
	lua_pushstring(L, c->m_method.name().c_str());
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

	try {
		ret = m->m_method.callArgArray(obj, args); // if the method is static, it ignores the first arg
	} catch (const std::exception& ex) {
		luaL_error(L, "call threw exception: %s", ex.what());
	} catch (...) {
		luaL_error(L, "call threw exception");
	}

	if (ret.isValid()) {

		if (ret.isArithmetical()) {
			lua_pushnumber(L, ret.convertTo<lua_Number>());
		} else if (ret.isStdString()) {
			lua_pushstring(L, ret.convertTo<std::string>().c_str());
		} else {
			Lua_Variant::create(L, ret);
		}

		return 1;
	} else {
		return 0;
	}
}

//---------------Constructor----------------------------------------------------

const char * Lua_Constructor::metatableName = "IoC.Constructor";
const char * Lua_Constructor::userDataName = "Constructor";
MethodTable Lua_Constructor::methods;
const struct luaL_Reg Lua_Constructor::lib_f[] = {
	{ NULL, NULL }
};

void Lua_Constructor::initialize()
{
	methods["call"] = &call;
	methods["numberOfArguments"] = &numberOfArguments;
	methods["argumentSpellings"] = &argumentSpellings;
	methods["isDefaultConstructor"] = &isDefaultConstructor;
}


int Lua_Constructor::call(lua_State* L)
{
	Lua_Constructor* c = checkUserData(L);
	vector<VariantValue> args;

	int n = lua_gettop(L);

	for (int i = 2; i <= n; ++i) {
		args.push_back(Lua_Variant::getFromStack(L, i));
	}

	try {
		Lua_Variant::create(L, ::std::move(c->m_constructor.callArgArray(args)));
	} catch (const std::exception& ex) {
		luaL_error(L, "call threw exception: %s", ex.what());
	} catch (...) {
		luaL_error(L, "call threw exception");
	}
	return 1;
}

int Lua_Constructor::numberOfArguments(lua_State* L)
{

	return 1;
}

int Lua_Constructor::argumentSpellings(lua_State* L)
{
	return 1;
}

int Lua_Constructor::isDefaultConstructor(lua_State* L)
{
	return 1;
}

//---------------Attribute------------------------------------------------------

const char * Lua_Attribute::metatableName = "IoC.Attribute";
const char * Lua_Attribute::userDataName  = "Attribute";

MethodTable Lua_Attribute::methods;

const struct luaL_Reg Lua_Attribute::lib_f[] = {
	{ NULL, NULL }
};

void Lua_Attribute::initialize()
{
	methods["get"] = &get;
	methods["set"] = &set;
	methods["name"] = &name;
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
	try {
		Lua_Variant::create(L, c->m_attribute.get(obj));
	} catch (const std::exception& ex) {
		luaL_error(L, "call threw exception: %s", ex.what());
	} catch (...) {
		luaL_error(L, "call threw exception");
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

	try {
		c->m_attribute.set(obj, val);
	} catch (const std::exception& ex) {
		luaL_error(L, "call threw exception: %s", ex.what());
	} catch (...) {
		luaL_error(L, "call threw exception");
	}
	return 0;
}

int Lua_Attribute::name(lua_State* L)
{
	Lua_Attribute* c = checkUserData(L);
	lua_pushstring(L, c->m_attribute.name().c_str());
	return 1;
}

//---------------Function-------------------------------------------------------

const char * Lua_Function::metatableName = "IoC.Function";
const char * Lua_Function::userDataName  = "Function";
MethodTable Lua_Function::methods;

const struct luaL_Reg Lua_Function::lib_f[] = {
	{ "lookup", lookup },
	{ NULL, NULL }
};


void Lua_Function::initialize()
{
	methods["call"] = &call;
	methods["name"] = &name;
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

	try {
		ret = f->m_function.callArgArray(args);
	} catch (const std::exception& ex) {
		luaL_error(L, "call threw exception: %s", ex.what());
	} catch (...) {
		luaL_error(L, "call threw exception");
	}

	if (ret.isValid()) {

		if (ret.isArithmetical()) {
			lua_pushnumber(L, ret.convertTo<lua_Number>());
		} else if (ret.isStdString()) {
			lua_pushstring(L, ret.convertTo<std::string>().c_str());
		} else {
			Lua_Variant::create(L, ret);
		}
		return 1;
	} else {
		return 0;
	}
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





extern "C" {

	int luaopen_libluacppreflect ( lua_State* L )
	{
		Lua_Class::_register(L);
		Lua_Attribute::_register(L);
		Lua_Method::_register(L);
		Lua_Constructor::_register(L);
		Lua_Variant::_register(L);
		Lua_Function::_register(L);
		return 1;
	}
}
