/*
** SelfPortrait API
** See Copyright Notice in reflection.h
*/
#include "llvm/Support/Host.h"

#include "clang/Driver/Compilation.h"
#include "clang/Driver/Driver.h"
#include "clang/Driver/Tool.h"
#include "clang/Frontend/CompilerInvocation.h"
#include "clang/Basic/DiagnosticOptions.h"
#include "clang/Frontend/FrontendDiagnostic.h"
#include "clang/Frontend/TextDiagnosticPrinter.h"

#include "clang/Frontend/CompilerInstance.h"
#include "clang/Frontend/Utils.h"
#include "clang/Basic/TargetOptions.h"
#include "clang/Basic/TargetInfo.h"
#include "clang/Basic/FileManager.h"
#include "clang/Basic/SourceManager.h"
#include "clang/Lex/Preprocessor.h"
#include "clang/Basic/Diagnostic.h"
#include "clang/AST/ASTContext.h"
#include "clang/AST/ASTConsumer.h"
#include "clang/Frontend/ASTUnit.h"
#include "clang/Parse/Parser.h"
#include "clang/Parse/ParseAST.h"
#include "clang/AST/Decl.h"
#include "clang/AST/DeclTemplate.h"
#include "clang/Basic/LangOptions.h"
#include "clang/Sema/Sema.h"
#include "clang/Sema/Template.h"


#include <vector>
#include <string>
#include <set>
#include <iostream>
#include <fstream>
#include <sstream>
#include <stdexcept>
#include <unordered_map>

#include "definitions.h"
using namespace definitions;

using namespace std;
using namespace clang;
using namespace llvm;

template<class... Args, template<class...> class Associative>
bool contains(const Associative<Args...>& container, const typename Associative<Args...>::key_type& key)
{
	auto it = container.find(key);
	return (it != container.end());
}

template<class... Args, template<class...> class Container>
std::string join(const Container<Args...>& container, const std::string& separator)
{
	if (container.empty()) {
		return "";
	} else {

		auto it = container.begin();

		std::string ret = *it;

		++it;
		for (; it != container.end(); ++it) {
			ret += separator + *it;
		}
		return ret;
	}
}


class MyASTConsumer
{
	TranslationUnit m_tu;
	TranslationUnitBuilder m_builder;

	SourceManager* m_sourceManager;
	PrintingPolicy m_printPol;
	set<Decl*> m_visited;
	Sema& m_sema;
	bool m_instantiateTemplates;


	struct IncompleteType {
		QualType type;
		SourceRange range;
	};

	struct PrivateType {};

	string printType(string orig) {
		string ret;

		for (auto c: orig) {
			if (c == ',') {
				ret += " COMMA ";
			} else {
				ret += c;
			}
		}
		return ret;
	}

	string printType(QualType t) {
		return printType(t.getAsString(m_printPol));
	}

	void treatIncompleteType (IncompleteType t, Decl* decl) {
		SourceLocation start = t.range.getBegin();
		const FileEntry* entry = m_sourceManager->getFileEntryForID(m_sourceManager->getFileID(start));
		int line = m_sourceManager->getSpellingLineNumber(start);
		int column = m_sourceManager->getSpellingColumnNumber(start);



        cerr << (entry ? entry->getName() : "unknown location") << ":" <<
				line << ":" << column <<
				": warning: Incomplete type " <<
				t.type.getAsString(m_printPol) <<

				",\nmeta data will not be emitted for ";

		if (NamedDecl* nd = dyn_cast<NamedDecl>(decl)) {
			cerr << nd->getNameAsString() << endl;
		} else {
			cerr << "the current declaration" << endl;
		}
	}

	QualType treatType(QualType&& t, SourceRange&& range, bool isInMainFile) {

		const Type* orig = t.getTypePtr();
		const Type* type = nullptr;


		QualType qt = t;

		begin_access:

		type = qt.getTypePtr();

		AccessSpecifier as = AS_public;

		if (const auto* tt = dyn_cast<TypedefType>(type)) {
			// If it is a privately defined typedef, we ignore it
			if (TypedefNameDecl* tnd = tt->getDecl()) {
				as = tnd->getAccess();
			}
		} else if (const TagType* tt = dyn_cast<TagType>(type)) {
			as = tt->getDecl()->getAccess();
		} else if (const TemplateSpecializationType* tt = dyn_cast<TemplateSpecializationType>(type)) {
			as = tt->getTemplateName().getAsTemplateDecl()->getAccess();
		} else if (const PointerType* pt = dyn_cast<PointerType>(type)) {
			qt = pt->getPointeeType();
			goto begin_access;
		} else if (const ReferenceType* rt = dyn_cast<ReferenceType>(type)) {
			qt = rt->getPointeeType();
			goto begin_access;
		}

		if (as == AS_protected || as == AS_private) {
			throw PrivateType();
		}


		if (!dyn_cast<PointerType>(orig) && type->isIncompleteType() && !type->isSpecificBuiltinType(BuiltinType::Void)) {

			if (const TypedefType* tt = dyn_cast<TypedefType>(type)) {
				if (TypedefNameDecl* tnd = tt->getDecl()) {
					return treatType(tnd->getUnderlyingType(), ::std::move(range), isInMainFile);
				}
			} else if (const auto* tt = dyn_cast<TemplateSpecializationType>(type)) {
				if (m_instantiateTemplates) {
					if (ClassTemplateDecl* td = dyn_cast<ClassTemplateDecl>(tt->getTemplateName().getAsTemplateDecl())) {
						for (clang::ClassTemplateDecl::spec_iterator it = td->spec_begin(); it != td->spec_end(); ++it) {
							clang::ClassTemplateSpecializationDecl* spec = *it;
							if (!spec->hasDefinition()) {
								m_sema.InstantiateClassTemplateSpecialization(range.getBegin(), spec, TSK_ImplicitInstantiation, false);
								if (spec->hasDefinition()) {
									handleDecl(spec->getDefinition(), isInMainFile);
								}
							}
						}
						if (!type->isIncompleteType()) {
							return t.getCanonicalType();
						}
					}
				}
			}

			throw IncompleteType{t, range};
		}
		return t.getCanonicalType();
	}

public:

	MyASTConsumer(SourceManager* sm, ASTContext& astContext, Sema& sema, bool instantiateTemplates = true)
		: m_builder(m_tu)
		, m_sourceManager(sm)
		, m_printPol(astContext.getLangOpts())
		, m_sema(sema)
		, m_instantiateTemplates(instantiateTemplates)
	{
		m_printPol.Bool = 1;
		FileID mainID = sm->getMainFileID();
		const FileEntry* entry = sm->getFileEntryForID(mainID);

		m_tu.include_directives.push_back("#include <SelfPortrait/reflection_impl.h>");
		m_tu.include_directives.push_back(std::string("#include \"") + entry->getName() + "\"");
	}

	ostream& print(ostream& os, bool diagsOn = false) {

		definitions::print(m_tu, os, diagsOn);
		return os;
	}

	Access convertClangsAccessSpec(AccessSpecifier as) const {
		if (as == AS_none) {
			throw std::logic_error("stumbled upon an AS_none, figure out what it means in this context");
		}
		return (as == AS_public) ?
					Public :
					(as == AS_protected) ?
						Protected :
						Private;
	}

	void handleDecl(Decl* decl, bool isInMainFile)
	{
		try {

			if (contains(m_visited, decl)) {
				return;
			} else {
				m_visited.insert(decl);
			}

			if (decl == nullptr) {
				return;
			}

			if (NamespaceDecl* nd = dyn_cast<NamespaceDecl>(decl)) {
				if (nd->isAnonymousNamespace()) {
					return; // no interest in hidden symbols
				}
				// recurse
				for (DeclContext::decl_iterator it = nd->decls_begin(); it != nd->decls_end(); ++it) {
					clang::Decl* subdecl = *it;
					handleDecl(subdecl, isInMainFile);
				}

			} else if (FieldDecl *fd = dyn_cast<FieldDecl>(decl)) {
				QualType t = treatType(fd->getType(), fd->getSourceRange(), isInMainFile);
				Access access = convertClangsAccessSpec(decl->getAccess());
				m_builder.addAttribute({fd->getDeclName().getAsString(), printType(t), access});

			} else if (RecordDecl* rd = dyn_cast<RecordDecl>(decl)) {

				if (CXXRecordDecl* crd = dyn_cast<CXXRecordDecl>(rd)) {

					if (m_instantiateTemplates) {
						// maybe we can force the definition to exist

						if (ClassTemplateSpecializationDecl* spec = dyn_cast<ClassTemplateSpecializationDecl>(crd)) {

                            string nameWithNamespace;
                            raw_string_ostream ss(nameWithNamespace);
                            spec->getNameForDiagnostic(ss, m_printPol, true);
                            cout << "1 template name = "<< ss.str() << " is valid " << spec->getPointOfInstantiation().isValid() << endl;
                            cout << "instantiation: " << crd->getTemplateSpecializationKind() << endl;
                            const TemplateArgumentList& list = spec->getTemplateArgs();
                            if (nameWithNamespace.find("type-parameter") == string::npos) {
                                cout << "treating " << endl;
                                m_sema.InstantiateClassTemplateSpecialization(crd->getSourceRange().getBegin(), spec, TSK_ImplicitInstantiation, true);
                                m_sema.InstantiateClassMembers(crd->getSourceRange().getBegin(), spec, MultiLevelTemplateArgumentList(spec->getTemplateArgs()),TSK_ImplicitInstantiation);
                            } else {
                                cout << "not treating " << endl;
                            }
						}

						if (ClassTemplateSpecializationDecl* spec = dyn_cast<ClassTemplateSpecializationDecl>(crd->getDeclContext())) {
                            string nameWithNamespace;
                            raw_string_ostream ss(nameWithNamespace);
                            spec->getNameForDiagnostic(ss, m_printPol, true);
                            cout << "2 template name = "<< ss.str() << " is valid " << spec->getPointOfInstantiation().isValid() << endl;
                            const TemplateArgumentList& list = spec->getTemplateArgs();

                            if (nameWithNamespace.find("type-parameter") == string::npos) {
							m_sema.InstantiateClassTemplateSpecialization(crd->getSourceRange().getBegin(), spec, TSK_ImplicitInstantiation, true);
							m_sema.InstantiateClassMembers(crd->getSourceRange().getBegin(), spec, MultiLevelTemplateArgumentList(spec->getTemplateArgs()),TSK_ImplicitInstantiation);
							return;
                            }
						}
					}

					if (crd->hasDefinition()) {

						CXXRecordDecl* definition = crd->getDefinition();

						m_visited.insert(definition);

                        string name;
                        raw_string_ostream ss(name);
                        definition->getNameForDiagnostic(ss, m_printPol, true);

                        m_builder.pushClass({printType(ss.str()), isInMainFile});


						for (auto it = definition->bases_begin(); it != definition->bases_end(); ++it) {
							Access access = convertClangsAccessSpec(it->getAccessSpecifier());
							QualType t = treatType(it->getType(), it->getSourceRange(), isInMainFile);
							string classname = printType(t);
							m_builder.addInheritance(classname, access);
						}

						// recurse
						if (definition->needsImplicitDefaultConstructor()) {
							m_builder.addConstructor({"", Public});
						}

						// recurse
						for (DeclContext::decl_iterator it = definition->decls_begin(); it != definition->decls_end(); ++it) {
							clang::Decl* subdecl = *it;
							handleDecl(subdecl, isInMainFile);
						}
						m_builder.popClass();

					} /*else if (m_instantiateTemplates) {
						// maybe we can force the definition to exist
						if (ClassTemplateSpecializationDecl* spec = dyn_cast<ClassTemplateSpecializationDecl>(crd)) {
							m_sema.InstantiateClassTemplateSpecialization(crd->getSourceRange().getBegin(), spec, TSK_ImplicitInstantiation, true);
							m_sema.InstantiateClassMembers(crd->getSourceRange().getBegin(), spec, MultiLevelTemplateArgumentList(spec->getTemplateArgs()),TSK_ImplicitInstantiation);
							if (crd->hasDefinition()) {
								m_visited.erase(crd->getDefinition());
								handleDecl(crd->getDefinition());
							}
						}

						if (ClassTemplateSpecializationDecl* spec = dyn_cast<ClassTemplateSpecializationDecl>(crd->getDeclContext())) {
							m_sema.InstantiateClassTemplateSpecialization(crd->getSourceRange().getBegin(), spec, TSK_ImplicitInstantiation, true);
							m_sema.InstantiateClassMembers(crd->getSourceRange().getBegin(), spec, MultiLevelTemplateArgumentList(spec->getTemplateArgs()),TSK_ImplicitInstantiation);

							if (crd->hasDefinition()) {
								m_visited.erase(crd->getDefinition());
								handleDecl(crd->getDefinition());
							}
						}
					}*/
				} /*else {
				// don't know what to do with this, the only possibility left are unions, right?
			}*/

			} /*else if (VarDecl *vd = llvm::dyn_cast<clang::VarDecl>(decl)) {
			std::cout << vd << std::endl;
			if( vd->isFileVarDecl() && vd->hasExternalStorage() )
			{
				std::cerr << "Read top-level variable decl: '";
				std::cerr << vd->getDeclName().getAsString(m_printPol) ;
				std::cerr << std::endl;
			}

		}*/ else if (FunctionDecl* fd = llvm::dyn_cast<FunctionDecl>(decl)) {

				if (fd->isDeleted()) {
					return;
				}

				string name = fd->getNameAsString();
				QualType qt = treatType(fd->getResultType(), fd->getSourceRange(), isInMainFile);
				const string returnType =  printType(qt);

				list<string> args;


				for (auto it = fd->param_begin(); it != fd->param_end(); ++it) {
					ParmVarDecl* param = *it;
					QualType pt = treatType(param->getType(), param->getSourceRange(), isInMainFile);
					// if we should need the names, this is how we get them *it)->getDeclName().getAsString(m_printPol)
					args.push_back(printType(pt));
				}

				const string argstr = join(args, ", ");

				if (CXXMethodDecl* md = dyn_cast<CXXMethodDecl>(decl)) {

					if (!m_builder.inClass()) {
						// out-of-class definitions are of no interest to us
						return;
					}

					if (dyn_cast<CXXConstructorDecl>(decl)) {
						Access access = convertClangsAccessSpec(decl->getAccess());
						m_builder.addConstructor({argstr, access});
					} else if (dyn_cast<CXXConversionDecl>(decl)) {
						// TODO
						// ex: operator bool();
						// dont't know what to do with this yet
					} else if (dyn_cast<CXXDestructorDecl>(decl)) {
						const bool isVirtual  = md->isVirtual();
						Access access = convertClangsAccessSpec(decl->getAccess());
						if (access == Public && isVirtual) {
							m_builder.currentClass().destructor_is_public_virtual = true;
						}
					} else {

						QualType mqt = treatType(md->getType(), md->getSourceRange(), isInMainFile);
						// this prints the method type: mqt.getAsString(m_printPol)

						const FunctionProtoType* proto = dyn_cast<const FunctionProtoType>(mqt.getTypePtr());

						Qualifiers quals = Qualifiers::fromCVRMask(proto->getTypeQuals());

						const bool isStatic      = md->isStatic();
						const bool isPureVirtual = md->isPure();
						const bool isConst       = quals.hasConst();
						const bool isVolatile    = quals.hasVolatile();

						string a = string(args.empty() ? "" : ", ") + argstr;
						Access access = convertClangsAccessSpec(decl->getAccess());
						const bool user = md->isUserProvided();
						if (isStatic) {
							m_builder.addMethod({name, returnType, argstr, false, false, false, true, access, user});
						} else if (isConst && isVolatile) {
							m_builder.addMethod({name, returnType, argstr, true, true, isPureVirtual, false, access, user});
						} else if (isConst) {
							m_builder.addMethod({name, returnType, argstr, true, false, isPureVirtual, false, access, user});
						} else if (isVolatile) {
							m_builder.addMethod({name, returnType, argstr, false, true, isPureVirtual, false, access, user});
						} else {
							m_builder.addMethod({name, returnType, argstr, false, false, isPureVirtual, false, access, user});
						}
					}
				} else if (fd->hasLinkage() && fd->getLinkage() == ExternalLinkage) {
					if (!isInMainFile) {
						return;
					}
					// is not a method
                    string nameWithNamespace;
                    raw_string_ostream ss(nameWithNamespace);
                    fd->getNameForDiagnostic(ss, m_printPol, true);
					string a = string(args.empty() ? "" : ", ") + argstr;
                    m_tu.functions.push_back({ss.str(), returnType, argstr});
				}
			} else if (clang::ClassTemplateDecl* td = llvm::dyn_cast<clang::ClassTemplateDecl>(decl)) {
				if (m_instantiateTemplates) {
					for (clang::ClassTemplateDecl::spec_iterator it = td->spec_begin(); it != td->spec_end(); ++it) {
						//specializations are classes too
						handleDecl(*it, isInMainFile);
					}
				}
			} else if (clang::FunctionTemplateDecl* td = llvm::dyn_cast<clang::FunctionTemplateDecl>(decl)) {
				if (m_instantiateTemplates) {
					for (clang::FunctionTemplateDecl::spec_iterator it = td->spec_begin(); it != td->spec_end(); ++it) {
						handleDecl(*it, isInMainFile);
					}
				}
			}
		} catch (const IncompleteType& t) {
			treatIncompleteType(t, decl);
		} catch (const PrivateType& t) {
		}
	}
};


int main(int argc, const char* argv[])
{
	vector<const char*> args;
	set<int> skip;
	bool foundCpp = false;
	bool foundCpp11 = false;
	bool foundSpellChecking = false;
	bool iFaceDiags = false;
	bool forceTemplates = true;
	string output;

	args.push_back(argv[0]);
    args.push_back("-I/usr/lib64/clang/3.2/include"); // why can't clang find this from the resource path?
    //args.push_back("-I/usr/lib/clang/3.2/include"); // why can't clang find this from the resource path?
	args.push_back("-Qunused-arguments"); // why do I keep getting warnings about the unused linker if I'm only creating an ASTunit

	for (int i = 1; i < argc; ++i) {
		if (strcmp(argv[i], "-std=c++11") == 0) {
			foundCpp11 = true;
		} else if (strcmp(argv[i], "-x") == 0) {
			if (((i+1) < argc) && (strcmp(argv[i+1], "c++") == 0)) {
				foundCpp = true;
			}
		} else if (strcmp(argv[i], "-xc++") == 0) {
			foundCpp = true;
		} else if (strcmp(argv[i], "-fno-spell-checking") == 0) {
			foundSpellChecking = true;
		} else if (strcmp(argv[i], "-fspell-checking") == 0) {
			foundSpellChecking = true;
		} else if (strncmp(argv[i], "-o", 2) == 0) {

			if (argv[i][2] != '\0') {
				output = &argv[i][2];
			} else {
				if ((i+1) == argc) {
					cerr << "No argument given to -o option" << endl;
					exit(1);
				}
				output = argv[i+1];
			}
		} else if (strcmp(argv[i], "--interface-diagnostics") == 0) {
			iFaceDiags = true;
			skip.insert(i);
		} else if (strcmp(argv[i], "--no-force-templates") == 0) {
			forceTemplates = false;
			skip.insert(i);
		}
	}

	if (output.empty()) {
		cerr << "No output given" << endl;
		exit(1);
	}


	if (!foundCpp) {
		args.push_back("-xc++");
	}

	if (!foundCpp11) {
		args.push_back("-std=c++11");
	}

	if (!foundSpellChecking) {
		args.push_back("-fno-spell-checking");
	}

	for (int i = 1; i < argc; ++i) {
		if (!contains(skip, i)) {
			args.push_back(argv[i]);
		}
	}

    // TextDiagnosticPrinter deletes this on on destruction
    DiagnosticOptions* options = new DiagnosticOptions();
    options->ShowCarets = 1;
    options->ShowColors = 1;


    TextDiagnosticPrinter DiagClient(llvm::errs(), options, /*OwnsOutputStream*/ false);

	llvm::IntrusiveRefCntPtr<DiagnosticIDs> DiagID(new DiagnosticIDs());
    llvm::IntrusiveRefCntPtr<DiagnosticsEngine> Diags(new DiagnosticsEngine(DiagID, options, &DiagClient, false));

    llvm::OwningPtr<ASTUnit> unit(ASTUnit::LoadFromCommandLine(&args[0], &args[0]+args.size(), Diags, "/usr/lib64/clang/3.2/", /*OnlyLocalDecls=*/false, /*CaptureDiagnostics=*/false, 0, 0, true, /*PrecompilePreamble=*/false, /*TUKind=*/TU_Complete, /*CacheCodeCompletionResults=*/false, /*AllowPCHWithCompilerErrors=*/false));
    //llvm::OwningPtr<ASTUnit> unit(ASTUnit::LoadFromCommandLine(&args[0], &args[0]+args.size(), Diags, "/usr/lib/clang/3.1/", /*OnlyLocalDecls=*/false, /*CaptureDiagnostics=*/false, 0, 0, true, /*PrecompilePreamble=*/false, /*TUKind=*/TU_Complete, /*CacheCodeCompletionResults=*/false, /*AllowPCHWithCompilerErrors=*/false));


    if (DiagClient.getNumErrors() > 0) {
		return 1;
	}

	ASTContext& astContext = unit->getASTContext();
	MyASTConsumer astConsumer(&unit->getSourceManager(), astContext, unit->getSema(), forceTemplates);

	for (auto it = astContext.getTranslationUnitDecl()->decls_begin(); it != astContext.getTranslationUnitDecl()->decls_end(); ++it) {
		Decl* subdecl = *it;
		SourceLocation location = subdecl->getLocation();

		astConsumer.handleDecl(subdecl, unit->isInMainFileID(location));
	}

	std::ofstream out;
	if (output != "-") {
		out.open(output);
		if (!out.is_open()) {
			cerr << "cannot open output file " << output << endl;
			exit(1);
		}
	} else {
		out.copyfmt(std::cout);
		out.clear(std::cout.rdstate());
		out.basic_ios<char>::rdbuf(std::cout.rdbuf());
	}

	astConsumer.print(out, iFaceDiags);


	return 0;
}
