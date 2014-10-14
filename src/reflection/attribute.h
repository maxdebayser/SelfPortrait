/*
** SelfPortrait API
** See Copyright Notice in reflection.h
*/
#ifndef ATTRIBUTE_H
#define ATTRIBUTE_H

#include <stdexcept>
#ifndef NO_RTTI
#include <typeinfo>
#endif
#include <memory>
#include <string>

#include "variant.h"
#include "reflection.h"
#include "str_utils.h"

namespace {

template<class Attr>
struct attribute_type;



template<class Clazz, class Type, bool assignable>
struct assignable_helper {
    typedef Type (Clazz::*ptr_to_attr);
    static void set(Clazz& object, ptr_to_attr ptr, const VariantValue& value) {
        bool success = false;
        Type v = value.convertTo<Type>(&success);
        if (!success) {
            throw ::std::runtime_error("wrong type in attibute attribution");
        }
        object.*ptr = v;
    }
};

template<class Clazz, class Type>
struct assignable_helper<Clazz, Type, false> {
    typedef Type (Clazz::*ptr_to_attr);
    static void set(Clazz&, ptr_to_attr, const VariantValue&) {
            throw ::std::runtime_error("this attribute is not assignable");
    }
};


template<class _Clazz, class _Type>
struct attribute_type<_Type _Clazz::*> {
	typedef _Clazz Clazz;
	typedef _Type Type;
	typedef Type (Clazz::*ptr_to_attr);
	
	enum { is_const = false };
	
	static VariantValue get(const Clazz& object, ptr_to_attr ptr) {
        VariantValue ret;
        return ret.construct<const Type&>(object.*ptr);
	}
	
	static void set(Clazz& object, ptr_to_attr ptr, const VariantValue& value) {
        assignable_helper<Clazz, Type, std::is_copy_assignable<Type>::value>::set(object, ptr, value);

        /*bool success = false;
        Type v = value.convertTo<Type>(&success);
        if (!success) {
            throw ::std::runtime_error("wrong type in attibute attribution");
        }
        object.*ptr = v;*/
	}
	
	static void set(const Clazz& object, ptr_to_attr, const VariantValue& ) {
		throw ::std::runtime_error("cannot change value of an attribute of a const object");
	}
};

template<class _Clazz, class _Type>
struct attribute_type<const _Type _Clazz::*> {
    typedef _Clazz Clazz;
    typedef _Type Type;
    typedef const Type (Clazz::*ptr_to_attr);

    enum { is_const = true };

    static VariantValue get(const Clazz& object, ptr_to_attr ptr) {
        VariantValue ret;
        return ret.construct<const Type&>(object.*ptr);
    }

    // const and non-const references bind to this
    static void set(const Clazz&, ptr_to_attr, const VariantValue&) {
        throw ::std::runtime_error("cannot change value of const attribute");
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
	
	static void set(ptr_to_variable ptr, const VariantValue& value) {
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
	
	static void set(ptr_to_variable, const VariantValue& ) {
		throw ::std::runtime_error("cannot change value of const variable");
	}
};

}

class AbstractAttributeImpl: public Annotated {
public:
	AbstractAttributeImpl(
			const char* name
			, const char* typeSpelling
			, bool isConst
			, bool isStatic
#ifndef NO_RTTI
			, const ::std::type_info& typeId
#endif
			)
		: m_name(name)
		, m_typeSpelling(typeSpelling)
		, m_isConst(isConst)
		, m_isStatic(isStatic)
#ifndef NO_RTTI
		, m_typeId(typeId)
#endif
	{}
	virtual ~AbstractAttributeImpl() {}

#ifndef NO_RTTI
	const ::std::type_info& type() const { return m_typeId; }
#endif

	bool isConst() const { return m_isConst; }
	bool isStatic() const { return m_isStatic; }

	const char* name() const { return m_name; }
	::std::string typeSpelling() const { return normalizedTypeName(m_typeSpelling); }

	VariantValue get() const {
		if (!m_isStatic) {
			throw ::std::runtime_error("cannot get value of non-static property without an object");
		}
		return this->get(VariantValue());
	}

	void set(const VariantValue& value) const {
		if (m_isStatic) {
			this->set(VariantValue(), value);
		} else {
			throw ::std::runtime_error("cannot set value of non-static property without an object");
		}
	}
	
	virtual VariantValue get(const VariantValue& object) const = 0;

	virtual void set(bool isConst, const VariantValue& object, const VariantValue& value) const = 0;

	void set(VariantValue& object, const VariantValue& value) const {
		set(false, object, value);
	}
	void set(const VariantValue& object, const VariantValue& value) const {
		set(true, object, value);
	}
	
	AbstractAttributeImpl(const AbstractAttributeImpl&) = delete;
	AbstractAttributeImpl(AbstractAttributeImpl&&) = delete;
	AbstractAttributeImpl& operator=(const AbstractAttributeImpl&) = delete;
	AbstractAttributeImpl& operator=(AbstractAttributeImpl&&) = delete;

private:
	const char* m_name;
	const char* m_typeSpelling;
	const unsigned int m_isConst : 1;
	const unsigned int m_isStatic : 1;
#ifndef NO_RTTI
	const ::std::type_info& m_typeId;
#endif
};

namespace {

template<class Attr>
class AttributeImpl: public AbstractAttributeImpl {
public:
	
	typedef attribute_type<Attr> ADescr;
	typedef typename ADescr::Clazz Clazz;
	typedef typename ADescr::Type Type;
	typedef typename ADescr::ptr_to_attr ptr_to_attr;
	
	AttributeImpl(const char* name, ptr_to_attr ptr, const char* typeSpelling)
		: AbstractAttributeImpl(
			  name
			  , typeSpelling
			  , ADescr::is_const
			  , false
#ifndef NO_RTTI
			  , typeid(Type)
#endif
			  )
		, m_ptr(ptr) {}

	virtual VariantValue get(const VariantValue& object) const override {
		bool success = false;
		const Clazz& ref = object.convertTo<const Clazz&>(&success);

		if (!success) {
			throw ::std::runtime_error("accessing attribute of an object of a different class");
		}
		return ADescr::get(ref, m_ptr);
	}

	virtual void set(bool isConst, const VariantValue& object, const VariantValue& value) const override {
		if (!object.isA<Clazz>()) {
			throw ::std::runtime_error("accessing attribute of an object of a different class");
		}
		if (!isConst) {
			Clazz& ref = object.convertTo<Clazz&>();
			return ADescr::set(ref, m_ptr, value);
		} else {
			const Clazz& ref = object.convertTo<const Clazz&>();
			return ADescr::set(ref, m_ptr, value);
		}
	}
	
private:
	ptr_to_attr m_ptr;
};

template<class _Clazz, class _Type>
class StaticAttributeImpl: public AbstractAttributeImpl {
public:
	typedef _Clazz Clazz;
	typedef ptr_type<_Type> ADescr;
	typedef typename ADescr::Type Type;
	typedef typename ADescr::ptr_to_variable ptr_to_attr;


	StaticAttributeImpl(const char* name, ptr_to_attr ptr, const char* typeSpelling)
		: AbstractAttributeImpl(
			  name
			  , typeSpelling
			  , ADescr::is_const
			  , true
#ifndef NO_RTTI
			  , typeid(Type)
#endif
			  )
		, m_ptr(ptr) {}

	virtual VariantValue get(const VariantValue&) const override { return ADescr::get(m_ptr); }

	virtual void set(bool isConst, const VariantValue& object, const VariantValue& value) const override {
		ADescr::set(m_ptr, value);
	}
	
private:
	ptr_to_attr m_ptr;
};


}

#endif /* ATTRIBUTE_H */
