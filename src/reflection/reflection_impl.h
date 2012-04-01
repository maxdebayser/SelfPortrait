#ifndef REFLECTION_IMPL_H
#define REFLECTION_IMPL_H

#include "attribute.h"
#include "class.h"
#include "constructor.h"
#include "function.h"
#include "method.h"
#include "reflection.h"

#define BEGIN_CLASS(CLASS_NAME) \
template<> ClassImpl<CLASS_NAME>* ClassImpl<CLASS_NAME>::instance() {\
	typedef CLASS_NAME ThisClass;\
	static ClassImpl instance;\
	if (instance.open()) {\
		instance.setFqn(#CLASS_NAME);

#define END_CLASS \
	instance.close();\
}\
return &instance;\
}

#define SUPER_CLASS(CLASS_NAME) \
instance.registerSuperClassPriv(ClassOf<CLASS_NAME>());


#define METHOD(METHOD_NAME) \
	instance.registerMethodPriv(make_method(#METHOD_NAME, &ThisClass::METHOD_NAME));

#define STATIC_METHOD(METHOD_NAME) \
	instance.registerMethodPriv(make_static_method<ThisClass>(#METHOD_NAME, &ThisClass::METHOD_NAME));

#define ATTRIBUTE(ATTRIBUTE_NAME) \
	instance.registerAttributePriv(make_attribute(#ATTRIBUTE_NAME, &ThisClass::ATTRIBUTE_NAME));

#define DEFAULT_CONSTRUCTOR(...) \
	instance.registerConstructorPriv(make_constructor<ThisClass>());

#define CONSTRUCTOR(...) \
	instance.registerConstructorPriv(make_constructor<ThisClass, __VA_ARGS__>());


#endif /* REFLECTION_IMPL_H */

