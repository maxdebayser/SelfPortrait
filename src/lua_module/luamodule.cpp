#include <lua.hpp>
#include "reflection_impl.h"
#include "lua_utils.h"

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

		lua_newtable(L);
		lua_replace(L, LUA_ENVIRONINDEX);
		luaL_register(L, Adapted::userDataName, Adapted::lib_f);
	}

	static int gc(lua_State* L){
		Adapted* c = Adapted::checkUserData(L);
		c->~Adapted();
		return 0;
	}


	static int eq(lua_State* L){
		const int nargs = lua_gettop(L);
		if (nargs != 2) {
			luaL_error(L, "expected exactly two arguments to __eq but %d were provided", nargs);
		}

		if (Adapted::isUserData(L, 1) && Adapted::isUserData(L, 2)) {
			Adapted* a1 = Adapted::checkUserData(L, 1);
			Adapted* a2 = Adapted::checkUserData(L, 2);

			lua_pushboolean(L, a1->wrapped() == a2->wrapped());

		} else {
			lua_pushboolean(L, false);
		}

		return 1;
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

	static Adapted* checkUserData(lua_State* L, int pos = 1) { return (Adapted*)luaL_checkudata(L, pos, Adapted::metatableName); }
	static bool isUserData(lua_State* L, int pos = 1) { return LuaUtils::isudata(L, pos, Adapted::metatableName); }

	template<class... Args>
	static void create(lua_State* L, Args&&... args) {
		void * f = lua_newuserdata(L, sizeof(Adapted));
		Adapted * lc = new(f) Adapted(std::forward<Args>(args)...);
		luaL_getmetatable(L, Adapted::metatableName);
		lua_setmetatable(L, -2);
	}

	static const struct luaL_Reg lib_m[];
};

template<class T>
const struct luaL_Reg LuaAdapter<T>::lib_m[] = {
	{ "__gc", gc },
	{ "__index", index },
	{ "__eq", eq },
	{ NULL, NULL }
};

//---------------Variant--------------------------------------------------------

class Lua_Variant: public LuaAdapter<Lua_Variant> {
public:

	template<class Arg>
	Lua_Variant(Arg&& arg) : m_variant(arg) {}

	Lua_Variant() : m_variant() {}

	static void initialize();



	static VariantValue getFromStack(lua_State* L, int idx = 1);

	static int newInstance(lua_State* L);
	static int tostring(lua_State* L);
	static int tonumber(lua_State* L);
	static int isValid(lua_State* L);
	static int isStdString(lua_State* L);
	static int isIntegral(lua_State* L);
	static int isFloatingPoint(lua_State* L);
	static int isArithmetical(lua_State* L);
	static int isPOD(lua_State* L);
	static int sizeOf(lua_State* L);
	static int alignOf(lua_State* L);

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

	const VariantValue& wrapped() const { return m_variant; }

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
	static int isInterface(lua_State* L);
	static int getMethods(lua_State* L);
	static int getConstructors(lua_State* L);
	static int getAttributes(lua_State* L);
	static int getSuperClasses(lua_State* L);
	static int findAttribute(lua_State* L);
	static int findAllAttributes(lua_State* L);
	static int findConstructor(lua_State* L);
	static int findAllConstructors(lua_State* L);
	static int findMethod(lua_State* L);
	static int findAllMethods(lua_State* L);
	static int findSuperClass(lua_State* L);
	static int findAllSuperClasses(lua_State* L);

	static void initialize();

	template<class Elem, class MPtr>
	static int findFirst(lua_State* L, MPtr ptr);

	template<class ElemList, class MPtr>
	static int findAll(lua_State* L, MPtr ptr);

	static const char * metatableName;
	static const char * userDataName;

	const Class& wrapped() const { return m_class; }

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

	static int call(lua_State* L);
	static int name(lua_State* L);
	static int fullName(lua_State* L);
	static int numberOfArguments(lua_State* L);
	static int returnSpelling(lua_State* L);
	static int argumentSpellings(lua_State* L);
	static int isConst(lua_State* L);
	static int isVolatile(lua_State* L);
	static int isStatic(lua_State* L);
	static int getClass(lua_State* L);

	static void initialize();

	static const char * metatableName;
	static const char * userDataName;

	const Method& wrapped() const { return m_method; }

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
	static int getClass(lua_State* L);

	static void initialize();

	static const char * metatableName;
	static const char * userDataName;

	const Constructor& wrapped() const { return m_constructor; }

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
	static int typeSpelling(lua_State* L);
	static int isConst(lua_State* L);
	static int isStatic(lua_State* L);
	static int getClass(lua_State* L);

	static void initialize();

	static const char * metatableName;
	static const char * userDataName;

	const Attribute& wrapped() const { return m_attribute; }

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
	static int numberOfArguments(lua_State* L);
	static int returnSpelling(lua_State* L);
	static int argumentSpellings(lua_State* L);
	static int lookup(lua_State* L);

	static void initialize();

	static const char * metatableName;
	static const char * userDataName;

	const Function& wrapped() const { return m_function; }

private:
	Function m_function;
	static MethodTable methods;
	static const struct luaL_Reg lib_f[];
	friend class LuaAdapter<Lua_Function>;
};

//---------------Proxy----------------------------------------------------------

class Lua_Proxy: public LuaAdapter<Lua_Proxy> {
public:
	Lua_Proxy(Proxy&& p) : m_proxy(std::move(p)) {}

	static int create(lua_State* L);

	static int interfaces(lua_State* L);
	static int addImplementation(lua_State* L);
	static int hasImplementation(lua_State* L);
	static int reference(lua_State* L);

	static void initialize();

	static const char * metatableName;
	static const char * userDataName;

	const Proxy& wrapped() const { return m_proxy; }

private:
	Proxy m_proxy;
	static MethodTable methods;
	static const struct luaL_Reg lib_f[];
	static const struct luaL_Reg lib_m[];
	friend class LuaAdapter<Lua_Proxy>;
};

template<class Entity>
struct binding_mapper;

template<>
struct binding_mapper<Class> {
	typedef Lua_Class type;
};

template<>
struct binding_mapper<Attribute> {
	typedef Lua_Attribute type;
};

template<>
struct binding_mapper<Method> {
	typedef Lua_Method type;
};

template<>
struct binding_mapper<Constructor> {
	typedef Lua_Constructor type;
};

template<>
struct binding_mapper<Function> {
	typedef Lua_Function type;
};



//=====================Definitions==============================================


//---------------Variant--------------------------------------------------------

const char * Lua_Variant::metatableName = "SelfPortraitVariant";
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
	{ "__eq", eq },
	{ NULL, NULL }
};

void Lua_Variant::initialize()
{
	methods["tostring"]        = &tostring;
	methods["tonumber"]        = &tonumber;
	methods["isValid"]         = &isValid;
	methods["isStdString"]     = &isStdString;
	methods["isIntegral"]      = &isIntegral;
	methods["isFloatingPoint"] = &isFloatingPoint;
	methods["isArithmetical"]  = &isArithmetical;
	methods["isPOD"]           = &isPOD;
	methods["sizeOf"]          = &sizeOf;
	methods["alignOf"]         = &alignOf;
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

int Lua_Variant::tonumber(lua_State* L)
{
	Lua_Variant* v = checkUserData(L);
	LuaUtils::LuaValue<double>::pushValue(L, v->m_variant.convertTo<double>());
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



//---------------Class----------------------------------------------------------

const char * Lua_Class::metatableName = "SelfPortrait.Class";
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
	{ "__eq", eq },
	{ NULL, NULL }
};

void Lua_Class::initialize()
{
	methods["fullyQualifiedName"] = &fullyQualifiedName;
	methods["simpleName"] = &simpleName;
	methods["isInterface"] = &isInterface;
	methods["methods"] = &getMethods;
	methods["constructors"] = &getConstructors;
	methods["attributes"] = &getAttributes;
	methods["superclasses"] = &getSuperClasses;
	methods["findAttribute"] = &findAttribute;
	methods["findAllAttributes"] = &findAllAttributes;
	methods["findConstructor"] = &findConstructor;
	methods["findAllConstructors"] = &findAllConstructors;
	methods["findMethod"] = &findMethod;
	methods["findAllMethods"] = &findAllMethods;
	methods["findSuperClass"] = &findSuperClass;
	methods["findAllSuperClasses"] = &findAllSuperClasses;
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

	Elem a = (c->m_class.*ptr)([&](const Elem& a) {
		luaL_checktype(L, -1, LUA_TFUNCTION);
		lua_pushvalue(L, -1);
		binding_mapper<Elem>::type::create(L, a);

		if (lua_pcall(L, 1, 1, 0) != 0) {
			lua_pushfstring(L, "Error running anonymous function");
			lua_insert(L, -2);
			lua_concat(L, 2);
			lua_error(L);
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

	ElemList l = (c->m_class.*ptr)([&](const Elem& a) {
		luaL_checktype(L, -1, LUA_TFUNCTION);
		lua_pushvalue(L, -1);
		binding_mapper< Elem >::type::create(L, a);

		if (lua_pcall(L, 1, 1, 0) != 0) {
			lua_pushfstring(L, "Error running anonymous function");
			lua_insert(L, -2);
			lua_concat(L, 2);
			lua_error(L);
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
	methods["name"]              = &name;
	methods["fullName"]              = &fullName;
	methods["call"]              = &call;
	methods["numberOfArguments"] = &numberOfArguments;
	methods["returnSpelling"]    = &returnSpelling;
	methods["argumentSpellings"] = &argumentSpellings;
	methods["isConst"]           = &isConst;
	methods["isVolatile"]        = &isVolatile;
	methods["isStatic"]          = &isStatic;
	methods["getClass"]          = &getClass;
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
	methods["call"] = &call;
	methods["numberOfArguments"] = &numberOfArguments;
	methods["argumentSpellings"] = &argumentSpellings;
	methods["isDefaultConstructor"] = &isDefaultConstructor;
	methods["getClass"]          = &getClass;
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
	methods["get"]          = &get;
	methods["set"]          = &set;
	methods["name"]         = &name;
	methods["typeSpelling"] = &typeSpelling;
	methods["isConst"]      = &isConst;
	methods["isStatic"]     = &isStatic;
	methods["getClass"]          = &getClass;
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

		VariantValue ret = c->m_attribute.get(obj);
		if (ret.isArithmetical()) {
			lua_pushnumber(L, ret.convertTo<lua_Number>());
		} else if (ret.isStdString()) {
			lua_pushstring(L, ret.convertTo<std::string>().c_str());
		} else {
			Lua_Variant::create(L, ret);
		}

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
	{ "lookup", lookup },
	{ NULL, NULL }
};


void Lua_Function::initialize()
{
	methods["call"] = &call;
	methods["name"] = &name;
	methods["numberOfArguments"] = &numberOfArguments;
	methods["returnSpelling"]    = &returnSpelling;
	methods["argumentSpellings"] = &argumentSpellings;
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
	{ "create", create },
	{ NULL, NULL }
};

const struct luaL_Reg Lua_Proxy::lib_m[] = {
	{ "__gc", gc },
	{ "__index", index },
	{ "__eq", eq },
	{ NULL, NULL }
};

void Lua_Proxy::initialize()
{
	methods["interfaces"] = &interfaces;
	methods["addImplementation"] = &addImplementation;
	methods["hasImplementation"] = &hasImplementation;
	methods["reference"] = &reference;
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

	try {
		LuaAdapter<Lua_Proxy>::create(L, std::move(p));
	} catch (std::exception& ex) {
		luaL_error(L, ex.what());
	} catch (...) {
		luaL_error(L, "unknown error in creation of proxy");
	}
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


namespace {
	struct LuaClosureWrapper {
		lua_State * const L;
		const int envIndex;
		LuaClosureWrapper() = default;
		LuaClosureWrapper(const LuaClosureWrapper&) = default;

		~LuaClosureWrapper() {
			luaL_unref(L, LUA_ENVIRONINDEX, envIndex);
		}

		LuaClosureWrapper(lua_State* ls, int index) : L(ls), envIndex(index) {}

		VariantValue operator()(const std::vector<VariantValue>& vargs) const {

			lua_rawgeti(L, LUA_REGISTRYINDEX, envIndex);
			luaL_checktype(L, -1, LUA_TFUNCTION);

			for(const VariantValue& v: vargs) {
				Lua_Variant::create(L, v);
			}

			if (lua_pcall(L, vargs.size(), 1, 0) != 0) {
				lua_pushfstring(L, "Error running proxy method handler");
				lua_insert(L, -2);
				lua_concat(L, 2);
				lua_error(L);
			}
			VariantValue ret = Lua_Variant::getFromStack(L, -1);
			lua_pop(L, 1);

			return ret;
		}
	};
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

	Lua_Variant::create(L, p->m_proxy.reference(c->wrapped()));

	return 1;
}




extern "C" {

	int luaopen_libluaselfportrait ( lua_State* L )
	{
		Lua_Class::_register(L);
		Lua_Attribute::_register(L);
		Lua_Method::_register(L);
		Lua_Constructor::_register(L);
		Lua_Variant::_register(L);
		Lua_Function::_register(L);
		Lua_Proxy::_register(L);
		return 1;
	}
}
