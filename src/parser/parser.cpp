/***   tutorial4_CI.cpp   *****************************************************
 * This code is licensed under the New BSD license.
 * See LICENSE.txt for details.
 *
 * The CI tutorials remake the original tutorials but using the
 * CompilerInstance object which has as one of its purpose to create commonly
 * used Clang types.
 *****************************************************************************/
#include <iostream>

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

#include "clang/AST/Mangle.h"

#include <vector>
#include <string>
#include <set>
using namespace std;
using namespace clang;

template<class... Args, template<class...> class Associative>
bool contains(const Associative<Args...>& container, const typename Associative<Args...>::key_type& key) {

	auto it = container.find(key);
	return (it != container.end());
}

/******************************************************************************
 *
 *****************************************************************************/
class MyASTConsumer : public clang::ASTConsumer
{
public:
	MyASTConsumer() : clang::ASTConsumer() { }
	virtual ~MyASTConsumer() { }

	virtual bool HandleTopLevelDecl( clang::DeclGroupRef d)
	{
		static int count = 0;
		clang::DeclGroupRef::iterator it;
		for( it = d.begin(); it != d.end(); it++)
		{
			count++;
			handleDecl(*it);
		}
		return true;
	}

	void handleDecl(clang::Decl* decl)
	{
		cout << "decl = " << ((void*)decl) << endl;
		if (decl == nullptr) {
			std::cout << "checkpoint 1" << std::endl;
			return;
		}


		if (clang::VarDecl *vd = llvm::dyn_cast<clang::VarDecl>(decl)) {
			std::cout << vd << std::endl;
			if( vd->isFileVarDecl() && vd->hasExternalStorage() )
			{
				std::cerr << "Read top-level variable decl: '";
				std::cerr << vd->getDeclName().getAsString() ;
				std::cerr << std::endl;
			}

		} else if (clang::FunctionDecl* fd = llvm::dyn_cast<clang::FunctionDecl>(decl)) {
			std::cerr << "Read top-level func decl: '";
			std::cerr << fd->getDeclName().getAsString() ;
			std::cerr << std::endl;



			if (!fd->isExternC()) {
				std::string mangledNameIfNeeded;
				llvm::raw_string_ostream RawStr(mangledNameIfNeeded);
				m_mangle->mangleName(fd, RawStr);
				RawStr.flush();
				std::cout << "O mangled name eh " << mangledNameIfNeeded << std::endl;
			} else {
				std::cout << "o cara é extern C" << std::endl;
			}

		} else if (clang::ClassTemplateDecl* td = llvm::dyn_cast<clang::ClassTemplateDecl>(decl)) {
			std::cerr << "Read top-level class template decl: '";
			std::cerr << td->getDeclName().getAsString() ;
			std::cerr << std::endl;

			std::cerr << "no specs: " << (td->spec_begin() == td->spec_end()) << std::endl;

			for (clang::ClassTemplateDecl::spec_iterator it = td->spec_begin(); it != td->spec_end(); ++it) {
				std::cerr << "achei uma spec, recursing" << std::endl;
				clang::ClassTemplateSpecializationDecl* spec = *it;
				handleDecl(spec);
			}


		} else if (clang::ClassTemplateSpecializationDecl* sd = llvm::dyn_cast<clang::ClassTemplateSpecializationDecl>(decl)) {
			std::cerr << "Read top-level class template spec decl: '";
			std::cerr << sd->getDeclName().getAsString() ;
			std::cerr << std::endl;

		} else if (clang::NamedDecl* nd = llvm::dyn_cast<clang::NamedDecl>(decl)) {
			std::cerr << "Read top-level named decl: '";
			std::cerr << nd->getDeclName().getAsString() ;
			std::cerr << std::endl;
		}

		clang::DeclContext* dc = llvm::dyn_cast<clang::DeclContext>(decl);
		if (dc) {
			std::cerr << "found decl context, recursing" << std::endl;
			for (clang::DeclContext::decl_iterator it = dc->decls_begin(); it != dc->decls_end(); ++it) {
				clang::Decl* subdecl = *it;
				std::cerr << "checkpoint" << std::endl;
				handleDecl(subdecl);
			}
		}
	}


	void setMangleContext(clang::MangleContext* mc)
	{
		m_mangle.reset(mc);
	}
private:
	llvm::OwningPtr<clang::MangleContext> m_mangle;
};

/******************************************************************************
 *
 *****************************************************************************/
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


	MyASTConsumer astConsumer;

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

	astConsumer.setMangleContext(astContext.createMangleContext());


	for (clang::DeclContext::decl_iterator it = astContext.getTranslationUnitDecl()->decls_begin(); it != astContext.getTranslationUnitDecl()->decls_end(); ++it) {
		clang::Decl* subdecl = *it;
		SourceLocation location = subdecl->getLocation();

		if (unit->isInMainFileID(location)) {
			astConsumer.handleDecl(subdecl);
		}


	}


	return 0;
}
