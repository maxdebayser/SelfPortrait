#include "definitions.h"
#include <stdexcept>
using namespace std;
#include <iostream>

namespace {
	using namespace definitions;
	std::ostream& operator<<(std::ostream& o, const Method& m) {
		o << m.return_type_spelling << " " << m.name << "(" << m.argument_type_spellings << ")" << (m.is_const ? " const" : m.is_volatile ? " volatile" : "");
		return o;
	}
}

namespace definitions {

	bool Class::is_interface(std::ostream& diag, const ClassIndex& index) const
	{
		bool is = true;

		if (!attributes.empty()) {
			is = false;
			diag << "class " << name << " has attributes" << endl;
		}

		if (!destructor_is_public_virtual) {
			is = false;
			diag << "class " << name << " has no public virtual destructor" << endl;
		}

		// Only public pure virtual methods
		for (const Method& m: methods) {

			if (!m.isUserProvided) {
				// let's not force the user to definite harmless implicit methods
				continue;
			}

			if (!m.is_pure_virtual) {
				is = false;
				diag << "method " << m << " of class " << name << " is not pure virtual" << endl;
			}

			if (m.access != Public) {
				is = false;
				diag << "method " << m << " of class " << name << " is not public" << endl;
			}

			if (m.is_static) {
				is = false;
				diag << "method " << m << " of class " << name << " is static" << endl;
			}
		}

		bool found_default = constructors.empty();
		//bool found_default = constructors.empty();
		for (const Constructor& c: constructors) {
			if (c.is_default() && c.access == Public) {
				found_default = true;
				break;
			}
		}

		if (!found_default) {
			is = false;
			diag << "class " << name << " has no public default constructor" << endl;
		}


		// Can inherit only from interfaces
		for (const Inheritance& i: inherited) {

			auto it = index.find(i.name);
			if (it == index.end()) {
				diag << "the defintion for the base class " << i.name << " of class " << name << " could not be found" << endl;
				is = false;
			} else {
				std::shared_ptr<Class> clazz = it->second;
				if (!clazz->is_interface(diag, index)) {
					is = false;
					diag << "class " << name << " inherits from " << clazz->name << " which is not an interface" << endl;
				} else if (i.access != Public) {
					is = false;
					diag << "inheritance of interface " << clazz->name << " by class " << name << " is not public" << endl;
				}
			}
		}

		if (!inner.empty()) {
			is = false;
			diag << "class " << name << " has inner class definitions" << endl;
		}

		return is;
	}


	void print(const Class& c, std::ostream& o, bool diagOn, const ClassIndex&);
	void print(const Inheritance& i, std::ostream& o);
	void print(const Method& m, std::ostream& o);
	void printAsStub(const Method& m, const std::string& className, std::ostream& o);
	void print(const Function& f, std::ostream& o);
	void print(const Attribute& a, std::ostream& o);
	void print(const Constructor& c, std::ostream& o);


	void print(const TranslationUnit& u, std::ostream& o, bool diagOn) {
		for(const auto& x: u.include_directives) {
			o << x << "\n";
		}
		o << "\n";
		for (const auto& x: u.functions) {
			print(x, o);
			o << "\n";
		}

		for (const auto& x: u.classes) {
			if (x->inMainFile) {
				print(*x, o, diagOn, u.classIndex);
				o << "\n";
			}
		}
	}

	std::string stubName(const std::string& className) {
		string stubname = className;
		for (size_t i = 0; i < stubname.size(); ++i) {
			if (stubname.at(i) == ':')  {
				stubname.at(i) = '_';
			}
		}
		stubname.append("Stub");
		return stubname;
	}

	void printStubMethods(const Class& c, const ClassIndex& index, std::ostream& o) {
		for (const auto& x: c.methods) {
			printAsStub(x, c.name, o);
		}
		for (const Inheritance& i: c.inherited) {
			auto it = index.find(i.name);
			if (it == index.end()) {
				throw std::logic_error("no base class can be unresolved at this point");
			} else {
				std::shared_ptr<Class> clazz = it->second;

				printStubMethods(*clazz, index, o);
			}
		}
	}


	void print(const Class& c, std::ostream& o, bool diagOn, const ClassIndex& index) {

		stringstream devnull;

		ostream& diagOut = diagOn ? std::cerr : devnull;

		bool isInterface = c.is_interface(diagOut, index);

		string stubname = stubName(c.name);

		if (isInterface) {
			o << "REFL_BEGIN_STUB(" << c.name << ", " << stubname << ")\n";
			printStubMethods(c, index, o);
			o << "REFL_END_STUB\n\n";
		}

		o << "REFL_BEGIN_CLASS(" << c.name << ")\n";
		if (isInterface) {
			o << "REFL_STUB(" << stubname << ")\n";
		}


		for (const auto& x: c.inherited) {
			print(x, o);
		}

		for (const auto& x: c.constructors) {
			print(x, o);
		}

		for (const auto& x: c.attributes) {
			print(x, o);
		}

		for (const auto& x: c.methods) {
			print(x, o);
		}
		// inner classes also go to TranslationUnit.classes and are printed in that loop

		o << "REFL_END_CLASS\n";
	}

	void print(const Inheritance& i, std::ostream& o) {
		o << "REFL_SUPER_CLASS(" << i.name << ")\n";
	}

	void print(const Method& m, std::ostream& o) {

		if (m.access != Public) {
			return;
		}

		const string argstr = (m.argument_type_spellings.empty() ? "" : ", ") + m.argument_type_spellings;
		if (m.is_static) {
			o << "REFL_STATIC_METHOD(" << m.name << ", " << m.return_type_spelling << argstr << ")\n";
		} else {

			if (!m.is_const && !m.is_volatile) {
				o << "REFL_METHOD(" << m.name << ", " << m.return_type_spelling << argstr << ")\n";
			} else if (m.is_const && !m.is_volatile) {
				o << "REFL_CONST_METHOD(" << m.name << ", " << m.return_type_spelling << argstr << ")\n";
			} else if (!m.is_const && m.is_volatile) {
				o << "REFL_VOLATILE_METHOD(" << m.name << ", " << m.return_type_spelling << argstr << ")\n";
			} else {
				o << "REFL_CONST_VOLATILE_METHOD(" << m.name << ", " << m.return_type_spelling << argstr << ")\n";
			}

		}
	}

	void printAsStub(const Method& m, const std::string& className, std::ostream& o) {

		if (!m.is_pure_virtual && !m.isUserProvided) {
			return;
		}

		if (m.access != Public || m.is_static) {
			throw std::logic_error("stub method is static or not public, this is an internal error");
		}

		const string argstr = (m.argument_type_spellings.empty() ? "" : ", ") + m.argument_type_spellings;
		if (!m.is_const && !m.is_volatile) {
			o << "REFL_STUB_METHOD(" << className << ", " << m.name << ", " << m.return_type_spelling << argstr << ")\n";
		} else if (m.is_const && !m.is_volatile) {
			o << "REFL_STUB_CONST_METHOD(" << className << ", " << m.name << ", " << m.return_type_spelling << argstr << ")\n";
		} else if (!m.is_const && m.is_volatile) {
			o << "REFL_STUB_VOLATILE_METHOD(" << className << ", " << m.name << ", " << m.return_type_spelling << argstr << ")\n";
		} else {
			o << "REFL_STUB_CONST_VOLATILE_METHOD(" << className << ", " << m.name << ", " << m.return_type_spelling << argstr << ")\n";
		}
	}

	void print(const Function& f, std::ostream& o) {
		const string argstr = (f.argument_type_spellings.empty() ? "" : ", ") + f.argument_type_spellings;
		o << "REFL_FUNCTION(" << f.name << ", " << f.return_type_spelling << argstr << ")\n";
	}

	void print(const Attribute& a, std::ostream& o) {
		if (a.access != Public) {
			return;
		}
		o << "REFL_ATTRIBUTE(" << a.name << ", " << a.type_spelling << ")\n";
	}

	void print(const Constructor& c, std::ostream& o) {
		if (c.access != Public) {
			return;
		}
		if (c.argument_type_spellings.empty()) {
			o << "REFL_DEFAULT_CONSTRUCTOR()\n";
		} else {
			o << "REFL_CONSTRUCTOR(" << c.argument_type_spellings << ")\n";
		}
	}

	TranslationUnitBuilder::TranslationUnitBuilder(TranslationUnit& tu)
		: m_tu(tu) {}

	void TranslationUnitBuilder::pushClass(Class&& cl) {
		// if the class stack is not empty, it is an inner class

		shared_ptr<Class> c = make_shared<Class>(std::move(cl));

		if (!m_cStack.empty()) {
			m_cStack.back()->inner.push_back(c);
		}
		m_tu.classes.push_back(c);

		m_cStack.push_back(c);
		m_tu.classIndex[c->name] = c;
	}

	void TranslationUnitBuilder::popClass() {
		if (m_cStack.empty()) {
			throw std::logic_error("programming arror, class stack is empty");
		}
		m_cStack.pop_back();
	}

	Class& TranslationUnitBuilder::currentClass() {
		if (m_cStack.empty()) {
			throw std::logic_error("programming arror, class stack is empty");
		}
		return *m_cStack.back();
	}

	bool TranslationUnitBuilder::inClass() {
		return !m_cStack.empty();
	}

	void TranslationUnitBuilder::addMethod(Method&& m) {
		currentClass().methods.emplace_back(m);
	}

	void TranslationUnitBuilder::addAttribute(Attribute&& m) {
		currentClass().attributes.emplace_back(m);
	}

	void TranslationUnitBuilder::addConstructor(Constructor&& m) {
		currentClass().constructors.emplace_back(m);
	}

	void TranslationUnitBuilder::addFunction(Function&& m) {
		m_tu.functions.emplace_back(m);
	}

	void TranslationUnitBuilder::addInheritance(const std::string& baseName, Access access) {
		currentClass().inherited.push_back({baseName, access});
	}

}
