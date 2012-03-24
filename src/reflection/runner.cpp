/* Generated file, do not edit */

#ifndef CXXTEST_RUNNING
#define CXXTEST_RUNNING
#endif

#define _CXXTEST_HAVE_STD
#define _CXXTEST_HAVE_EH
#include <cxxtest/TestListener.h>
#include <cxxtest/TestTracker.h>
#include <cxxtest/TestRunner.h>
#include <cxxtest/RealDescriptions.h>
#include <cxxtest/TestMain.h>
#include <cxxtest/ErrorPrinter.h>

int main( int argc, char *argv[] ) {
 int status;
    CxxTest::ErrorPrinter tmp;
    status = CxxTest::Main<CxxTest::ErrorPrinter>( tmp, argc, argv );
    return status;
}
bool suite_VariantTestSuite_init = false;
#include "variant_test.h"

static VariantTestSuite suite_VariantTestSuite;

static CxxTest::List Tests_VariantTestSuite = { 0, 0 };
CxxTest::StaticSuiteDescription suiteDescription_VariantTestSuite( "variant_test.h", 7, "VariantTestSuite", suite_VariantTestSuite, Tests_VariantTestSuite );

static class TestDescription_suite_VariantTestSuite_testInvalid : public CxxTest::RealTestDescription {
public:
 TestDescription_suite_VariantTestSuite_testInvalid() : CxxTest::RealTestDescription( Tests_VariantTestSuite, suiteDescription_VariantTestSuite, 11, "testInvalid" ) {}
 void runTest() { suite_VariantTestSuite.testInvalid(); }
} testDescription_suite_VariantTestSuite_testInvalid;

static class TestDescription_suite_VariantTestSuite_testValue : public CxxTest::RealTestDescription {
public:
 TestDescription_suite_VariantTestSuite_testValue() : CxxTest::RealTestDescription( Tests_VariantTestSuite, suiteDescription_VariantTestSuite, 12, "testValue" ) {}
 void runTest() { suite_VariantTestSuite.testValue(); }
} testDescription_suite_VariantTestSuite_testValue;

static class TestDescription_suite_VariantTestSuite_testConversions : public CxxTest::RealTestDescription {
public:
 TestDescription_suite_VariantTestSuite_testConversions() : CxxTest::RealTestDescription( Tests_VariantTestSuite, suiteDescription_VariantTestSuite, 13, "testConversions" ) {}
 void runTest() { suite_VariantTestSuite.testConversions(); }
} testDescription_suite_VariantTestSuite_testConversions;

#include <cxxtest/Root.cpp>
const char* CxxTest::RealWorldDescription::_worldName = "cxxtest";
