#ifndef OBJECT_H
#define OBJECT_H

#include <memory>

#include <iostream>
using namespace std;

class AbstractObjectImpl {
public:
	virtual ~AbstractObjectImpl() {}
};

template<class T>
class ObjectImpl: public AbstractObjectImpl {
public:
	
	typedef T ObjectType;
	
	// TODO: construtor de teste
	
	ObjectImpl(ObjectType* ptr) : m_ptr(ptr) {}
	
	ObjectType* object() { return m_ptr.get(); }
			
	const ObjectType* object() const { return const_cast<ObjectImpl*>(this)->object(); }
		
	volatile ObjectType* object() volatile { return const_cast<ObjectImpl*>(this)->object(); }
	
	const volatile ObjectType* object() const volatile { return const_cast<ObjectImpl*>(this)->object(); }
	
private:
	::std::unique_ptr<ObjectType> m_ptr;
};



class Object {
public:
	
	// TODO construtor de teste
	Object(AbstractObjectImpl* ptr) : m_impl(ptr) {}
	
	::std::shared_ptr<AbstractObjectImpl> impl() const volatile { return const_cast< ::std::shared_ptr<AbstractObjectImpl>&>(m_impl); }
	
private:
	::std::shared_ptr<AbstractObjectImpl> m_impl;
};


#endif /* OBJECT_H */
