#include "parser_test.h"

#include <stdlib.h>

#include "test_utils.h"

#include <fstream>
#include <iostream>
using namespace std;

namespace {

	string input_file(const string& name) {
		return fmt_str("%1/parser_data/input/%2", srcpath(), name);
	}

	string exp_output_file(const string& name) {
		return fmt_str("%1/parser_data/output/%2", srcpath(), name);
	}

	string output_file(const string& name) {
		return fmt_str("%1/parser_test_data/%2", binpath(), name);
	}

	string parser() {
		return fmt_str("%1/../parser/parser", binpath());
	}

	void runtest(const string& inputName, const string& referenceName, const string& outputName)
	{
		string source_file = input_file(inputName);
		string expected_output = exp_output_file(referenceName);
		string output = output_file(outputName);

		if (system(fmt_str("%1 %2 -o %3", parser(), source_file, output).c_str()) != 0) {
			std::cerr << "parser command may have failed" << std::endl;
		}

		ifstream exp_o_file(expected_output);
		TS_ASSERT(exp_o_file.is_open());

		ifstream o_file(output);
		TS_ASSERT(o_file.is_open());

		int lines = 0;

		while(exp_o_file.good() && o_file.good()) {

			string e, o;

			getline(exp_o_file, e);
			getline(o_file, o);

			++lines;

			if (e.find("#include") != string::npos) {
				// dirty hack:
				// because the header file path is not relative the the include path
				// the include directive can differ
				continue;
			}

			if (e != o) {
				TS_FAIL(fmt_str("files %1 and %2 differ at line %3:\n-%4\n+%5\n", expected_output, output, lines, e, o).c_str());
			}

		}

		if (exp_o_file.eof() != o_file.eof()) {
			TS_FAIL("length of refence and output files differs");
		}
	}

}



void ParserTestSuite::testFunctions()
{
	runtest("functions.h", "functions.cpp", "functions_out.cpp");
}


void ParserTestSuite::testSimpleClass()
{
	runtest("simple_class.h", "simple_class.cpp", "simple_class_out.cpp");
}


void ParserTestSuite::testInheritance()
{
	runtest("inheritance.h", "inheritance.cpp", "inheritance_out.cpp");
}


void ParserTestSuite::testInterface()
{
	runtest("interfaces.h", "interfaces.cpp", "interfaces_out.cpp");
}


void ParserTestSuite::testTemplates()
{
	runtest("templates.h", "templates.cpp", "templates.cpp");
}

