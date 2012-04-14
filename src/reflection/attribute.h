#ifndef ATTRIBUTE_H
#define ATTRIBUTE_H

#include <stdexcept>
#include <typeinfo>
#include <memory>
#include <string>

#include "variant.h"
#include "reflection.h"
#include "str_utils.h"

template<class Attr>
struct attribute_type;

template<class _Clazz, class _Type>
struct attribute_type<_Type _Clazz::*> {
	typedef _Clazz Clazz;
	typedef _Type Type;
	typedef Type (Clazz::*ptr_to_attr);
	
	enum { is_const = false };
	
	static VariantValue get(const Clazz& object, ptr_to_attr ptr) {
		return object.*ptr;
	}
	
	static void set(Clazz& object, ptr_to_attr ptr, VariantValue value) {
		bool success = false;
		Type v = value.convertTo<Type>(&success);
		if (!success) {
			throw ::std::runtime_error("wrong type in attibute attribution");
		}
		object.*ptr = v;
	}
	
	static void set(const Clazz& object, ptr_to_attr, VariantValue) {
		throw ::std::runtime_error("cannot change value of an attribute of a const object");
	}
};

template<class Type>
struct ptr_type;

template<class _Type>
struct ptr_type<_Type*> {
	typedef _Type Type;
	typedef Type *ptr_to_variable;
	
	enum { is_const = false };
	
	static VariantValue get(ptr_to_variable ptr) {
		return *ptr;
	}
	
	static void set(ptr_to_variable ptr, VariantValue value) {
		bool success = false;
		Type v = value.convertTo<Type>(&success);
		if (!success) {
			throw ::std::runtime_error("wrong type in variable attribution");
		}
		*ptr = v;
	}
};

template<class _Type>
struct ptr_type<const _Type*> {
	typedef _Type Type;
	typedef const Type *ptr_to_variable;
	
	enum { is_const = true };
	
	static VariantValue get(ptr_to_variable ptr) {
		return *ptr;
	}
	
	static void set(ptr_to_variable ptr, VariantValue value) {
		throw ::std::runtime_error("cannot change value of const variable");
	}
};


template<class _Clazz, class _Type>
struct attribute_type<const _Type _Clazz::*> {
	typedef _Clazz Clazz;
	typedef _Type Type;
	typedef const Type (Clazz::*ptr_to_attr);
	
	enum { is_const = true };
	
	static VariantValue get(const Clazz& object, ptr_to_attr ptr) {
		return object.*ptr;
	}
	
	// const and non-const references bind to this
	static void set(const Clazz&, ptr_to_attr, VariantValue) {
		throw ::std::runtime_error("cannot change value of const attribute");
	}
};


class AbstractAttributeImpl: public Annotated {
public:
	AbstractAttributeImpl() {}
	virtual ~AbstractAttributeImpl() {}
	
	virtual const ::std::string& name() const = 0;
	virtual const ::std::type_info& type() const = 0;
	virtual bool isConst() const = 0;
	virtual bool isStatic() const = 0;
	virtual const ::std::string& typeSpelling() const = 0;
	
	virtual VariantValue get() const = 0;
	virtual void set(const VariantValue& value) const = 0;
	
	virtual VariantValue get(const VariantValue& object) const = 0;
	virtual void set(VariantValue& object, const VariantValue& value) const = 0;
	virtual void set(const VariantValue& object, const VariantValue& value) const = 0;
	
	AbstractAttributeImpl(const AbstractAttributeImpl&) = delete;
	AbstractAttributeImpl(AbstractAttributeImpl&&) = delete;
	AbstractAttributeImpl& operator=(const AbstractAttributeImpl&) = delete;
	AbstractAttributeImpl& operator=(AbstractAttributeImpl&&) = delete;
	
};



template<class Attr>
class AttributeImpl: public AbstractAttributeImpl {
public:
	
	typedef attribute_type<Attr> ADescr;
	typedef typename ADescr::Clazz Clazz;
	typedef typename ADescr::Type Type;
	typedef typename ADescr::ptr_to_attr ptr_to_attr;
	
	AttributeImpl(::std::string name, ptr_to_attr ptr, ::std::string typeSpelling) : m_name(name), m_ptr(ptr), m_typeSpelling(typeSpelling) {}
	
	virtual const ::std::string& name() const override { return m_name; }
	virtual const ::std::type_info& type() const override { return typeid(Type); }
	virtual bool isConst() const override { return ADescr::is_const; }
	virtual bool isStatic() const override { return false; }
	virtual const ::std::string& typeSpelling() const { return m_typeSpelling; }
	
	virtual VariantValue get() const override {
		throw ::std::runtime_error("cannot get value of non-static property without an object");
	}
	virtual void set(const VariantValue& value) const  override {
		throw ::std::runtime_error("cannot set value of non-static property without an object");
	}
	
	virtual VariantValue get(const VariantValue& object) const override {
		bool success = false;
		const Clazz& ref = object.convertTo<const Clazz&>(&success);

		if (!success) {
			throw ::std::runtime_error("accessing attribute of an object of a different class");
		}
		return ADescr::get(ref, m_ptr);
	}
	
	virtual void set(VariantValue& object, const VariantValue& value) const override {
		bool success = false;
		Clazz& ref = object.convertTo<Clazz&>(&success);

		if (!success) {
			throw ::std::runtime_error("accessing attribute of an object of a different class");
		}
		return ADescr::set(ref, m_ptr, value);
	}
	
	virtual void set(const VariantValue& object, const VariantValue& value) const override {
		bool success = false;
		const Clazz& ref = object.convertTo<const Clazz&>(&success);

		if (!success) {
			throw ::std::runtime_error("accessing attribute of an object of a different class");
		}
		return ADescr::set(ref, m_ptr, value);
	}
	
private:
	::std::string m_name;
	ptr_to_attr m_ptr;
	::std::string m_typeSpelling;
	
};

template<class _Clazz, class _Type>
class StaticAttributeImpl: public AbstractAttributeImpl {
public:
	typedef _Clazz Clazz;
	typedef ptr_type<_Type> ADescr;
	typedef typename ADescr::Type Type;
	typedef typename ADescr::ptr_to_variable ptr_to_attr;
	
	StaticAttributeImpl(::std::string name, ptr_to_attr ptr, ::std::string typeSpelling) : m_name(name), m_ptr(ptr), m_typeSpelling(typeSpelling) {}
	
	virtual const ::std::string& name() const override { return m_name; }
	virtual const ::std::type_info& type() const override { return typeid(Type); }
	virtual bool isConst() const override { return ADescr::is_const; }
	virtual bool isStatic() const override { return true; }
	virtual const ::std::string& typeSpelling() const { return m_typeSpelling; }
	
	virtual VariantValue get() const override { return ADescr::get(m_ptr); }
	virtual void set(const VariantValue& value) const override { ADescr::set(m_ptr, value); }
	
	virtual VariantValue get(const VariantValue&) const override { return get(); }
	
	virtual void set(VariantValue&, const VariantValue& value) const  override { set(value); }
	
	virtual void set(const VariantValue&, const VariantValue& value) const  override { set(value); }
	
private:
	::std::string m_name;
	ptr_to_attr m_ptr;
	::std::string m_typeSpelling;
	
};



template<class Attr>
Attribute make_attribute(::std::string name, Attr ptr, const char* arg) {
	// call split to canonicalize the type spelling
	static AttributeImpl<Attr> impl(name, ptr, normalizedTypeName(arg));
	return Attribute(&impl);
}

template<class Clazz, class Attr>
Attribute make_static_attribute(::std::string name, Attr ptr, const char* arg) {
	// call split to canonicalize the type spelling
	static StaticAttributeImpl<Clazz,Attr> impl(name, ptr, normalizedTypeName(arg));
	return Attribute(&impl);
}

#endif /* ATTRIBUTE_H */
