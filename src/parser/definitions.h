#ifndef DEFINITIONS_H
#define DEFINITIONS_H

#include <vector>
#include <string>
#include <sstream>
#include <ostream>
#include <memory>
#include <unordered_map>

/* These classes represent the information that our parser extracts from clang's
 * AST. Actually we are duplicating the information contained in the AST but in
 * a much simpler format that makes the processing we are doing much simpler.
 * Also we can do all the processing on a stable set of classes since only the
 * core parser functionality depends on the moving target that is clang's API.
 */

namespace definitions {

	enum Access {
		Public, Protected, Private
	};

	struct Function {

		std::string name;
		std::string return_type_spelling;
		std::string argument_type_spellings;

	};


	struct Attribute {

		//Attribute() = default;

		//Attribute(const std::string& n, const std::string& ts, Access a) : name(n), type_spelling(ts), access(a) {}

		std::string name;
		std::string type_spelling;

		Access access;
	};


	struct Constructor {

		std::string argument_type_spellings;

		bool is_default() const { return argument_type_spellings.empty(); }

		Access access;
	};


	struct Method {

		std::string name;

		std::string return_type_spelling;

		std::string argument_type_spellings;

		bool is_const;

		bool is_volatile;

		bool is_pure_virtual;

		bool is_static;

		Access access;

		bool isUserProvided;

	};

	struct Class;

	struct Inheritance {
		std::shared_ptr<Class> clazz;
		Access access;
	};

	struct Class {

		Class(const std::string& n) : name(n), destructor_is_public_virtual(false) {}

		std::string name;

		bool destructor_is_public_virtual;

		std::vector<Constructor> constructors;

		std::vector<Method> methods;

		std::vector<Attribute> attributes;

		std::vector<Inheritance> inherited;

		std::vector<std::shared_ptr<Class> > inner;

		/*! We consider a class an interface if:
		 * 1) It has no attributes
		 * 2) It has only public pure virtual methods
		 * 3) It has a public default constructor
		 * 4) It has a public virtual destructor
		 * 5) It inherits only from interfaces
		 * 6) It has no inner classes
		 *
		 * Other constuctors are allowed.
		 */
		// Should we restrict overloaded methods?
		bool is_interface(std::ostream& diag) const;

	};

	struct TranslationUnit {

		std::vector<std::string> include_directives;

		std::vector<Function> functions;

		std::vector<std::shared_ptr<Class> > classes;
	};

	class TranslationUnitBuilder {
	public:
		TranslationUnitBuilder(TranslationUnit& tu);

		void pushClass(Class&& c);

		void popClass();

		Class& currentClass();

		bool inClass();

		void addMethod(Method&& m);

		void addAttribute(Attribute&& m);

		void addConstructor(Constructor&& m);

		void addFunction(Function&& m);

		void addInheritance(const std::string& baseName, Access access);

	private:
		TranslationUnit& m_tu;
		std::vector<std::shared_ptr<Class> > m_cStack;
		std::unordered_map<std::string, std::shared_ptr<Class> > m_cIndex;
	};


	void print(const TranslationUnit& u, std::ostream& o, bool diagOn = false);

}

#endif /* DEFINITIONS_H */
