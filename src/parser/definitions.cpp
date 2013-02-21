#include "definitions.h"
#include <stdexcept>
#include <iostream>
#include <algorithm>
using namespace std;

namespace {
	using namespace definitions;
	std::ostream& operator<<(std::ostream& o, const Method& m) {
		o << m.return_type_spelling << " " << m.name << "(" << m.argument_type_spellings << ")" << (m.is_const ? " const" : m.is_volatile ? " volatile" : "");
		return o;
	}

	bool compare_methods(const Method& m1, const Method& m2) {
		if (m1.name < m2.name) {
			return true;
		} else if (m2.name < m1.name) {
			return false;
		} else {
			if (m1.argument_type_spellings < m2.argument_type_spellings) {
				return true;
			} else if (m2.argument_type_spellings < m1.argument_type_spellings) {
				return false;
			} else {
				
				if (m1.is_const && !m2.is_const) {
					return false;
				} else if (m2.is_const && !m1.is_const) {
					return true;
				} else {
					
					if (m1.is_volatile && !m2.is_volatile) {
						return false;
					} else if (m2.is_volatile && !m1.is_volatile) {
						return true;
					} else {
						return false;
					}
				}
			}
		}
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

		auto include_directives = u.include_directives;

		sort(include_directives.begin(), include_directives.end());
		for(const auto& x: include_directives) {
			o << x << "\n";
		}
		o << "\n";
		auto functions = u.functions;
		sort(functions.begin(), functions.end(), [](const Function& f1, const Function& f2) -> bool {
			if (f1.name < f2.name) {
				return true;
			} else if (f2.name < f1.name) {
				return false;
			} else {
				return f1.argument_type_spellings < f2.argument_type_spellings;
			}
		});
		for (const auto& x: functions) {
			print(x, o);
			o << "\n";
		}
		auto classes = u.classes;
		sort(classes.begin(), classes.end(), [](const shared_ptr<Class>& c1, const shared_ptr<Class>& c2) -> bool {
			 return c1->name < c2->name;
		});
		for (const auto& x: classes) {
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
		auto methods = c.methods;
		sort(methods.begin(), methods.end(), compare_methods);
		for (const auto& x: methods) {
			printAsStub(x, c.name, o);
		}
		auto inherited = c.inherited;
		sort(inherited.begin(), inherited.end(), [](const Inheritance& i1, const Inheritance& i2) -> bool {
			 return i1.name < i2.name;
		});
		for (const Inheritance& i: inherited) {
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

		auto inherited = c.inherited;
		sort(inherited.begin(), inherited.end(), [](const Inheritance& i1, const Inheritance& i2) -> bool {
			 return i1.name < i2.name;
		});
		for (const auto& x: inherited) {
			print(x, o);
		}

		auto constructors = c.constructors;
		sort(constructors.begin(), constructors.end(), [](const Constructor& c1, const Constructor& c2) -> bool {
			 return c1.argument_type_spellings < c2.argument_type_spellings;
		});
		for (const auto& x: constructors) {
			print(x, o);
		}

		auto attributes = c.attributes;
		sort(attributes.begin(), attributes.end(), [](const Attribute& a1, const Attribute& a2) -> bool {
			 return a1.name < a2.name;
		});
		for (const auto& x: attributes) {
			print(x, o);
		}

		auto methods = c.methods;
		sort(methods.begin(), methods.end(), compare_methods);
		for (const auto& x: methods) {
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
