#ifndef LUAMODULE_H
#define LUAMODULE_H

#include <lua.hpp>
#include "reflection_impl.h"
#include "lua_utils.h"

#include <ctype.h>


#include <map>
#include <string>
#include <vector>
#include <exception>

namespace SelfPortraitLua {

typedef map<string, lua_CFunction> MethodTable;

class LuaBindingException: public std::exception
{
    std::string m_msg;
public:
    LuaBindingException(const std::string& msg) : m_msg(msg) { }
    LuaBindingException() : LuaBindingException("") {}

    virtual ~LuaBindingException() noexcept {}
    virtual const char* what() const noexcept { return m_msg.c_str(); }
};



void setExceptionTranslator(std::function<int(lua_State*, lua_CFunction)> f);
std::function<int(lua_State*, lua_CFunction)>& getExceptionTranslator();

/* For an explanation:
 * http://maxdebayser.blogspot.com.br/2012/11/semi-automatic-conversion-of-c.html
 */
template<lua_CFunction func>
static int exception_translator(lua_State* L)
{
    return getExceptionTranslator()(L, func);
}



//=====================Declarations=============================================

template<class T>
class LuaAdapter {
public:
    typedef T Adapted;

    static void _register(lua_State* L) {
        Adapted::initialize();



#ifdef LUA51
        luaL_newmetatable(L, Adapted::metatableName);
        luaL_register(L, NULL, Adapted::lib_m);
        lua_newtable(L);
        lua_replace(L, LUA_ENVIRONINDEX);
        luaL_register(L, Adapted::userDataName, Adapted::lib_f);
#else

        luaL_newmetatable(L, Adapted::metatableName);
        luaL_setfuncs (L, Adapted::lib_m, 0);

        luaL_newlib(L, Adapted::lib_f);
        lua_pushvalue(L,-1);        // pluck these lines out if they offend you
        lua_setglobal(L,Adapted::userDataName); // for they clobber the Holy _G
#endif
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
        Adapted::checkUserData(L);
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
        new(f) Adapted(std::forward<Args>(args)...);
        luaL_getmetatable(L, Adapted::metatableName);
        lua_setmetatable(L, -2);
    }

    static const struct luaL_Reg lib_m[];
};

template<class T>
const struct luaL_Reg LuaAdapter<T>::lib_m[] = {
    { "__gc", exception_translator<gc> },
    { "__index", exception_translator<index> },
    { "__eq", exception_translator<eq> },
    { NULL, NULL }
};

//---------------Variant--------------------------------------------------------
class Lua_Variant: public LuaAdapter<Lua_Variant> {
public:

    template<class Arg>
    Lua_Variant(Arg&& arg) : m_variant(arg), m_class() {}

    Lua_Variant() : m_variant(), m_class() {}

    static void initialize();



    static VariantValue getFromStack(lua_State* L, int idx = 1);

    static int newInstance(lua_State* L);
    static int assign(lua_State* L);
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
    static int ptrToValue(lua_State* L);
    static int _class(lua_State* L);

    static int add(lua_State* L);
    static int sub(lua_State* L);
    static int mul(lua_State* L);
    static int div(lua_State* L);
    static int mod(lua_State* L);
    static int unm(lua_State* L);

    static int attribute_stub(lua_State* L);
    static int method_stub(lua_State* L);

    static const char * metatableName;
    static const char * userDataName;

    static void create(lua_State* L, Class c, VariantValue&& v) {
        void * f = lua_newuserdata(L, sizeof(Lua_Variant));
        Lua_Variant * lc = new(f) Lua_Variant();
        lc->m_variant = ::std::move(v);

        lc->m_class = c;
        luaL_getmetatable(L, Lua_Variant::metatableName);
        lua_setmetatable(L, -2);
    }

    static int index(lua_State* L);
    static int newindex(lua_State* L);

    template<class... Args>
    static void create(lua_State* L, Class c, Args&&... args) {
        void * f = lua_newuserdata(L, sizeof(Adapted));
        Adapted * lc = new(f) Adapted(args...);
        lc->m_class = c;
        luaL_getmetatable(L, Adapted::metatableName);
        lua_setmetatable(L, -2);
    }

    const VariantValue& wrapped() const { return m_variant; }

private:
    VariantValue m_variant;
    Class m_class;
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

    static int construct(lua_State* L);
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


struct LuaClosureWrapper {
    lua_State * const L = nullptr;
    struct shared_state {
        shared_state(int index) : envIndex(index) {}
        const int envIndex;
    };

    struct deleter {
        lua_State * const L;
        void operator()(shared_state* s) const;
    };


    std::shared_ptr<shared_state> ss;

    LuaClosureWrapper(const LuaClosureWrapper& that) = default;

    LuaClosureWrapper(lua_State* ls, int index) : L(ls), ss(new shared_state(index), deleter{L}) {

    }

    VariantValue operator()(const ArgArray& vargs) const;
};



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
    Proxy& wrapped() { return m_proxy; }

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

//---------------Library----------------------------------------------------------

class Lua_Library: public LuaAdapter<Lua_Library> {
public:
    Lua_Library(string library);
    ~Lua_Library();

    static int newInstance(lua_State* L);
    static int close(lua_State* L);

    static void initialize();

    static const char * metatableName;
    static const char * userDataName;

    const string wrapped() const { return m_libraryName; }

private:
    string m_libraryName;
    void* m_library;
    static MethodTable methods;
    static const struct luaL_Reg lib_f[];
    static const struct luaL_Reg lib_m[];
    friend class LuaAdapter<Lua_Library>;
};

}


#endif // LUAMODULE_H
