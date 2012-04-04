#include "reflection.h"

#include "attribute.h"
#include "method.h"
#include "constructor.h"
#include "class.h"

//--------attribute-----------------------------------------------

class InvalidAttribute: public AbstractAttributeImpl {
public:
	virtual const ::std::string& name() const { return doThrow< ::std::string&>(); }
	virtual const ::std::type_info& type() const { return doThrow< ::std::type_info&>(); }
	virtual bool isConst() const { return doThrow<bool>(); }
	virtual bool isStatic() const { return doThrow<bool>(); }

	virtual VariantValue get() const  { return doThrow<VariantValue>(); }
	virtual void set(const VariantValue& value) const  { return doThrow<void>(); }

	virtual VariantValue get(const VariantValue& object) const  { return doThrow<VariantValue>(); }
	virtual void set(VariantValue& object, const VariantValue& value) const  { doThrow<void>(); }
	virtual void set(const VariantValue& object, const VariantValue& value) const { doThrow<void>(); }

	template<class T>
	T doThrow() const {
		throw std::runtime_error("Invalid use of uninitialized Attribute handle");
	}

	static InvalidAttribute* instance() {
		static InvalidAttribute instance;
		return &instance;
	}
};

Attribute::Attribute()
	: m_impl(InvalidAttribute::instance()) {}

Attribute::Attribute(AbstractAttributeImpl* impl)
	: m_impl(impl) {}

Attribute::Attribute(const Attribute& rhs)
	: m_impl(rhs.m_impl) {}
	
Attribute::Attribute(Attribute&& rhs)
	: m_impl(rhs.m_impl) {}
	
Attribute& Attribute::operator=(const Attribute& rhs) {
	m_impl = rhs.m_impl; return *this;
}

Attribute& Attribute::operator=(Attribute&& rhs) { 
	m_impl = rhs.m_impl; return *this;
}

const ::std::string& Attribute::name() const { 
	return m_impl->name();
}

const ::std::type_info& Attribute::type() const {
	return m_impl->type();
}

bool Attribute::isConst() const {
	return m_impl->isConst();
}

bool Attribute::isStatic() const {
	return m_impl->isStatic();
}

VariantValue Attribute::get() const {
	return m_impl->get();
}

void Attribute::set(const VariantValue& value) const {
	return m_impl->set(value);
}

VariantValue Attribute::get(const VariantValue& object) const {
	return m_impl->get(object);
}

void Attribute::set(VariantValue& object, const VariantValue& value) const  {
	return m_impl->set(object, value);
}

void Attribute::set(const VariantValue& object, const VariantValue& value) const {
	return m_impl->set(object, value);
}



//--------class---------------------------------------------------

class InvalidClass: public AbstractClassImpl {
public:

	virtual const std::string& fullyQualifiedName() const { return doThrow< const std::string&>(); }

	virtual const MethodList& methods() const { return doThrow< const MethodList&>(); }

	virtual const ConstructorList& constructors() const { return doThrow< const ConstructorList&>(); }

	virtual const ClassList& superclasses() const { return doThrow<const ClassList&>(); }

	virtual const AttributeList& attributes() const { return doThrow<const AttributeList&>(); }

	virtual const ::std::type_info& typeId() const { return doThrow< const ::std::type_info&>(); }


	template<class T>
	T doThrow() const {
		throw std::runtime_error("Invalid use of uninitialized Class handle");
	}

	static InvalidClass* instance() {
		static InvalidClass instance;
		return &instance;
	}
};

Class::Class()
	: m_impl(InvalidClass::instance()) {}
	
Class::Class(const AbstractClassImpl* impl)
	: m_impl(impl) {}

Class::Class(const Class& rhs)
	: m_impl(rhs.m_impl) {}

Class& Class::operator=(const Class& rhs) {
	m_impl = rhs.m_impl;
	return *this;
}

Class::Class(Class&& rhs)
	: m_impl(rhs.m_impl) {}

Class& Class::operator=(Class&& rhs) {
	m_impl = rhs.m_impl;
	return *this;
}

std::string Class::simpleName() const {
	::std::string tmp = m_impl->fullyQualifiedName();
	const int pos = tmp.find_last_of(':');
	return tmp.substr((pos == ::std::string::npos) ? 0 : pos+1);
}

const std::string& Class::fullyQualifiedName() const {
	return m_impl->fullyQualifiedName();
}

const type_info& Class::typeId() const {
	return m_impl->typeId();
}
	
bool Class::isSubClassOf(const Class& rhs) const {
	const ClassList& sups = m_impl->superclasses();
	return (::std::find(sups.begin(), sups.end(), rhs) != sups.end());
}
	
const Class::MethodList& Class::methods() const {
	return m_impl->methods();
}

const Class::ConstructorList& Class::constructors() const {
	return m_impl->constructors();
}

const Class::AttributeList& Class::attributes() const {
	return m_impl->attributes();
}

const Class::ClassList& Class::superclasses() const {
	return m_impl->superclasses();
}


//--------constructor---------------------------------------------


class InvalidConstructor: public AbstractConstructorImpl {
public:
	virtual ::std::size_t numberOfArguments() const { return doThrow< ::std::size_t>(); }
	virtual ::std::vector<const ::std::type_info*> argumentTypes() const { return doThrow< ::std::vector<const ::std::type_info*> >(); }
	virtual VariantValue call(const ::std::vector<VariantValue>& args) const { return doThrow<VariantValue>(); }

	template<class T>
	T doThrow() const {
		throw std::runtime_error("Invalid use of uninitialized Constructor handle");
	}

	static InvalidConstructor* instance() {
		static InvalidConstructor instance;
		return &instance;
	}
};

Constructor::Constructor() : m_impl(InvalidConstructor::instance()) {}

Constructor::Constructor(const Constructor& rhs) : m_impl(rhs.m_impl) {}

Constructor& Constructor::operator=(const Constructor& rhs) { m_impl = rhs.m_impl; return *this; }

Constructor::Constructor(Constructor&& rhs) : m_impl(rhs.m_impl) {}

Constructor& Constructor::operator=(Constructor&& rhs) { m_impl = rhs.m_impl; return *this; }

Constructor::Constructor(AbstractConstructorImpl* impl)	: m_impl(impl) {}

::std::size_t Constructor::numberOfArguments() const {
	return m_impl->numberOfArguments();
}

bool Constructor::isDefaultConstructor() const {
	return numberOfArguments() == 0;
}

::std::vector<const ::std::type_info*> Constructor::argumentTypes() const {
	return m_impl->argumentTypes();
}

VariantValue Constructor::call_helper(const ::std::vector<VariantValue>& vargs) const {
	return m_impl->call(vargs);
}


//--------method---------------------------------------------

class InvalidMethod: public AbstractMethodImpl {
public:
	const ::std::string& name() const { return doThrow< ::std::string&>(); }
	::std::size_t numberOfArguments() const { return doThrow< ::std::size_t>(); }
	::std::vector<const ::std::type_info*> argumentTypes() const { return doThrow< ::std::vector<const ::std::type_info*> >(); }
	const ::std::type_info& returnType() const  { return doThrow< ::std::type_info&>(); }
	bool isConst() const { return doThrow< bool>(); }
	bool isVolatile() const { return doThrow< bool>(); }
	bool isStatic() const { return doThrow< bool>(); }
	VariantValue call(const ::std::vector<VariantValue>& args) const { return doThrow< VariantValue>(); }
	VariantValue call(VariantValue& object, const ::std::vector<VariantValue>& args) const { return doThrow< VariantValue>(); }
	VariantValue call(const VariantValue& object, const ::std::vector<VariantValue>& args) const { return doThrow< VariantValue>(); }
	VariantValue call(volatile VariantValue& object, const ::std::vector<VariantValue>& args) const { return doThrow< VariantValue>(); }
	VariantValue call(const volatile VariantValue& object, const ::std::vector<VariantValue>& args) const { return doThrow< VariantValue>(); }

	template<class T>
	T doThrow() const {
		throw std::runtime_error("Invalid use of uninitialized Method handle");
	}

	static InvalidMethod* instance() {
		static InvalidMethod instance;
		return &instance;
	}
};

Method::Method()
	: m_impl(InvalidMethod::instance()) {}

Method::Method(AbstractMethodImpl* impl)
	: m_impl(impl) {}

Method::Method(const Method& rhs)
	: m_impl(rhs.m_impl) {}

Method::Method(Method&& rhs)
	: m_impl(rhs.m_impl) {}
	

Method& Method::operator=(const Method& rhs) { 
	m_impl = rhs.m_impl; 
	return *this;
}
	
Method& Method::operator=(Method&& rhs) {
	m_impl = rhs.m_impl;
	return *this;
}

const ::std::string& Method::name() const {
	return m_impl->name();
}

::std::size_t Method::numberOfArguments() const {
	return m_impl->numberOfArguments();
}

::std::vector<const ::std::type_info*> Method::argumentTypes() const {
	return m_impl->argumentTypes();	
}

const ::std::type_info& Method::returnType() const {
	return m_impl->returnType();
}

bool Method::isConst() const {
	return m_impl->isConst();
}

bool Method::isVolatile() const {
	return m_impl->isVolatile();	
}

bool Method::isStatic() const {
	return m_impl->isStatic();
}

VariantValue Method::call_helper(const ::std::vector<VariantValue>& vargs) const {
	return m_impl->call(vargs );	
}

VariantValue Method::call_helper(VariantValue& object, const ::std::vector<VariantValue>& vargs) const {
	return m_impl->call(object, vargs );
}

VariantValue Method::call_helper(const VariantValue& object, const ::std::vector<VariantValue>& vargs) const {
	return m_impl->call(object, vargs );	
}

VariantValue Method::call_helper(volatile VariantValue& object, const ::std::vector<VariantValue>& vargs) const {
	return m_impl->call(object, vargs );	
}

VariantValue Method::call_helper(const volatile VariantValue& object, const ::std::vector<VariantValue>& vargs) const {
	return m_impl->call(object, vargs );	
}
