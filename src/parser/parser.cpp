#include "llvm/Support/Host.h"

#include "clang/Driver/Compilation.h"
#include "clang/Driver/Driver.h"
#include "clang/Driver/Tool.h"
#include "clang/Frontend/CompilerInvocation.h"
#include "clang/Frontend/DiagnosticOptions.h"
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


#include <vector>
#include <string>
#include <set>
#include <iostream>
#include <sstream>

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
	bool m_inClass = false;
	list<string> m_namespaces;
	list<CXXRecordDecl*> m_visitLater;
	stringstream out;
	set<string> m_sourceFiles;
	SourceManager* m_sourceManager;

public:

	MyASTConsumer(SourceManager* sm) : m_sourceManager(sm) {

		FileID mainID = sm->getMainFileID();
		const FileEntry* entry = sm->getFileEntryForID(mainID);
		m_sourceFiles.insert(entry->getName());
	}

	ostream& print(ostream& os) {

		os << "#include \"reflection_impl.h\"" << endl;

		for(const string& filename: m_sourceFiles) {
			os << "#include \"" << filename << "\"" << endl;
		}

		os << out.str() << endl;
		return os;
	}

	void handleDecl(Decl* decl)
	{
		if (decl == nullptr) {
			return;
		}

		AccessSpecifier as = decl->getAccess();

		if (as == AS_protected || as == AS_private) {
			return;
		}

		if (NamespaceDecl* nd = dyn_cast<NamespaceDecl>(decl)) {
			if (nd->isAnonymousNamespace()) {
				return; // no interest in hidden symbols
			}

			m_namespaces.push_back(nd->getDeclName().getAsString());
			// recurse
			for (DeclContext::decl_iterator it = nd->decls_begin(); it != nd->decls_end(); ++it) {
				clang::Decl* subdecl = *it;
				handleDecl(subdecl);
			}
			m_namespaces.pop_back();

		} else if (FieldDecl *fd = dyn_cast<FieldDecl>(decl)) {
			out << "ATTRIBUTE(" << fd->getDeclName().getAsString() << ")" << endl;

		} else if (RecordDecl* rd = dyn_cast<RecordDecl>(decl)) {

			if (CXXRecordDecl* crd = dyn_cast<CXXRecordDecl>(rd)) {
				if (crd->hasDefinition()) {
					crd = crd->getDefinition();

					if (m_inClass) {
						m_visitLater.push_back(crd);
						return;
					}

					m_namespaces.push_back(crd->getDeclName().getAsString());

					m_inClass = true;

					out << "BEGIN_CLASS(" << join(m_namespaces, "::") << ")" << endl;

					for (auto it = crd->bases_begin(); it != crd->bases_end(); ++it) {
						QualType t = it->getType();
						out << "SUPER_CLASS(" << t.getAsString() << ")" << endl;
					}

					// recurse
					for (DeclContext::decl_iterator it = crd->decls_begin(); it != crd->decls_end(); ++it) {
						clang::Decl* subdecl = *it;
						handleDecl(subdecl);
					}
					m_inClass = false;
					out << "END_CLASS" << endl << endl;

					while( !m_visitLater.empty() ) {
						CXXRecordDecl* nested = m_visitLater.front();
						m_visitLater.pop_front();
						handleDecl(nested);
					}

					m_namespaces.pop_back();
				}
			} /*else {
				// don't know what to do with this, the only possibility left are unions, right?
			}*/

		} /*else if (VarDecl *vd = llvm::dyn_cast<clang::VarDecl>(decl)) {
			std::cout << vd << std::endl;
			if( vd->isFileVarDecl() && vd->hasExternalStorage() )
			{
				std::cerr << "Read top-level variable decl: '";
				std::cerr << vd->getDeclName().getAsString() ;
				std::cerr << std::endl;
			}

		}*/ else if (FunctionDecl* fd = llvm::dyn_cast<FunctionDecl>(decl)) {

			const string name = fd->getDeclName().getAsString();
			QualType qt = fd->getResultType();
			const string returnType =  qt.getAsString();

			list<string> args;


			for (auto it = fd->param_begin(); it != fd->param_end(); ++it) {
				QualType pt = (*it)->getType();
				// if we should need the names, this is how we get them *it)->getDeclName().getAsString()
				args.push_back(pt.getAsString());
			}

			const string argstr = join(args, ", ");

			if (CXXMethodDecl* md = dyn_cast<CXXMethodDecl>(decl)) {

				if (dyn_cast<CXXConstructorDecl>(decl)) {
					if (args.empty()) {
						out << "DEFAULT_CONSTRUCTOR()" << endl;
					} else {
						out << "CONSTRUCTOR(" << argstr << ")" << endl;
					}
				} else if (dyn_cast<CXXConversionDecl>(decl)) {
					// ex: operator bool();
					// dont't know what to do with this yet
				} else if (dyn_cast<CXXDestructorDecl>(decl)) {
					// this is pretty much expected :)
				} else {

					QualType mqt = md->getType();
					// this prints the method type: mqt.getAsString()

					const FunctionProtoType* proto = dyn_cast<const FunctionProtoType>(mqt.getTypePtr());

					Qualifiers quals = Qualifiers::fromCVRMask(proto->getTypeQuals());

					const bool isStatic   = md->isStatic();
					const bool isVirtual  = md->isVirtual();
					const bool isConst    = quals.hasConst();
					const bool isVolatile = quals.hasVolatile();

					if (isVirtual && (md->size_overridden_methods() > 0)) {
						return; // we don't need to repeat this
					}

					string a = string(args.empty() ? "" : ", ") + argstr;

					if (isStatic) {
						out << "STATIC_METHOD(" << name << ", " << returnType << a << ")" << endl;
					} else if (isConst && isVolatile) {
						out << "CONST_VOLATILE_METHOD(" << name << ", " << returnType << a << ")" << endl;
					} else if (isConst) {
						out << "CONST_METHOD(" << name << ", " << returnType << a << ")" << endl;
					} else if (isVolatile) {
						out << "VOLATILE_METHOD(" << name << ", " << returnType << a << ")" << endl;
					} else {
						out << "METHOD(" << name << ", " << returnType << a << ")" << endl;
					}
				}
			}
		} else if (clang::ClassTemplateDecl* td = llvm::dyn_cast<clang::ClassTemplateDecl>(decl)) {
			for (clang::ClassTemplateDecl::spec_iterator it = td->spec_begin(); it != td->spec_end(); ++it) {
				//specializations are classes too
				clang::ClassTemplateSpecializationDecl* spec = *it;
				handleDecl(spec);
			}
		}
	}
};


int main(int argc, const char* argv[])
{
	vector<const char*> args;
	bool foundCpp = false;
	bool foundCpp11 = false;
	bool foundSpellChecking = false;

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
		}
	}

	args.push_back(argv[0]);

	if (!foundCpp) {
		args.push_back("-xc++");
	}

	if (!foundCpp11) {
		args.push_back("-std=c++11");
	}

	if (!foundSpellChecking) {
		args.push_back("-fno-spell-checking");
	}

	args.push_back("-I/usr/lib/clang/3.1/include"); // why can't clang find this from the resource path?
	args.push_back("-Qunused-arguments"); // why do I keep getting warnings about the unused linker if I'm only creating an ASTunit

	for (int i = 1; i < argc; ++i) {
		args.push_back(argv[i]);
	}

	DiagnosticOptions options;
	options.ShowCarets = 1;
	options.ShowColors = 1;

	TextDiagnosticPrinter *DiagClient =	new TextDiagnosticPrinter(llvm::errs(), options);

	llvm::IntrusiveRefCntPtr<DiagnosticIDs> DiagID(new DiagnosticIDs());
	llvm::IntrusiveRefCntPtr<DiagnosticsEngine> Diags(new DiagnosticsEngine(DiagID, DiagClient));

	llvm::OwningPtr<ASTUnit> unit(ASTUnit::LoadFromCommandLine(&args[0], &args[0]+args.size(), Diags, "/usr/lib/clang/3.1/", /*OnlyLocalDecls=*/false, /*CaptureDiagnostics=*/false, 0, 0, true, /*PrecompilePreamble=*/false, /*TUKind=*/TU_Complete, /*CacheCodeCompletionResults=*/false, /*AllowPCHWithCompilerErrors=*/false));


	if (DiagClient->getNumErrors() > 0) {
		return 1;
	}

	ASTContext& astContext = unit->getASTContext();
	MyASTConsumer astConsumer(&unit->getSourceManager());


	for (auto it = astContext.getTranslationUnitDecl()->decls_begin(); it != astContext.getTranslationUnitDecl()->decls_end(); ++it) {
		Decl* subdecl = *it;
		SourceLocation location = subdecl->getLocation();

		if (unit->isInMainFileID(location)) {
			astConsumer.handleDecl(subdecl);
		}
	}

	astConsumer.print(cout);

	return 0;
}
