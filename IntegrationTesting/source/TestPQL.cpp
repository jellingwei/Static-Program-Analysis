#include <cppunit/config/SourcePrefix.h>
#include <vector>
#include <string>
#include <list>
#include "TestPQL.h"
#include "SourceParser.h"
#include "QueryOptimizer.h"
#include "QueryEvaluator.h"
#include "QueryParser.h"
#include "QueryTree.h"
#include "PKB.h"
#include "Synonym.h"
#include "ResultProjector.h"
#include "PQLController.h"

using namespace std;

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION( PQLTest ); 
list<string> evaluate(string query);

void PQLTest::setUp()
{
	PKB& pkb = PKB::getInstance();
	
	int procIndex1 = pkb.insertProc("PQLTest");
	int procIndex2 = pkb.insertProc("PQLTest2");
	int procIndex3 = pkb.insertProc("PQLTest3");
	pkb.setCalls(procIndex1,procIndex2);
	pkb.setCalls(procIndex2,procIndex3);
	
}

void PQLTest::tearDown()
{
}


void PQLTest::testPQL()
{
	
	CPPUNIT_ASSERT_EQUAL_MESSAGE("procedure a, b; Call(a, PQLTest2)", 1, (int)evaluate("procedure a, b; Select a such that Calls(a, \"PQLTest2\")").size());
	CPPUNIT_ASSERT_EQUAL_MESSAGE("procedure a, b; Call(a, PQLTest2)", 1, (int)evaluate("procedure a, b; Select a such that Calls(\"PQLTest2\", a)").size());
	// Values that are not suppose to be in queries
	
	CPPUNIT_ASSERT_EQUAL_MESSAGE("assign a; Modifies(b, 'b')", 0, (int)evaluate("assign a; Select b such that Modifies(b, \"b\")").size());
	CPPUNIT_ASSERT_EQUAL_MESSAGE("Parent*('b', s)", 0, (int)evaluate("Select s; Select s such that Parent*(\"b\", s)").size()); 
	CPPUNIT_ASSERT_EQUAL_MESSAGE("Follows(0, s)", 0, (int)evaluate("stmt s; Select s such that Follows(0, s)").size());
	CPPUNIT_ASSERT_EQUAL_MESSAGE("pattern a(a, '_a_')", 0, (int)evaluate("assign a; Select a pattern a(_, \"_f_\"").size());
	CPPUNIT_ASSERT_EQUAL_MESSAGE("pattern a(a, _'a')", 0, (int)evaluate("assign a; Select a pattern a(_, \"_\"a\"").size());
	CPPUNIT_ASSERT_EQUAL_MESSAGE("pattern a(a, _'a')", 0, (int)evaluate("assign a; Select a pattern a(_, \"\"a\"_").size());
	CPPUNIT_ASSERT_EQUAL_MESSAGE("Parent(c, _)", 0, (int)evaluate("constant c; Select c such that Parent(c, _)").size());
	CPPUNIT_ASSERT_EQUAL_MESSAGE("Parent(_, c)", 0, (int)evaluate("constant c; Select c such that Parent(_, c)").size());
	CPPUNIT_ASSERT_EQUAL_MESSAGE("Modifies(c, v)", 0, (int)evaluate("constant c; variable v; Select v such that Modifies(c, v)").size());
	CPPUNIT_ASSERT_EQUAL_MESSAGE("Modifies(w, c)", 0, (int)evaluate("constant c; while w; Select c such that Modifies(w, c)").size());
	CPPUNIT_ASSERT_EQUAL_MESSAGE("Uses(c, v)", 0, (int)evaluate("variable v; constant c; Select c such that Uses(c, v)").size());
	CPPUNIT_ASSERT_EQUAL_MESSAGE("Uses(8, c)", 0, (int)evaluate("constant c; Select c such that Uses(8, c)").size());
	CPPUNIT_ASSERT_EQUAL_MESSAGE("Follows(s, c)", 0, (int)evaluate("constant c; Select c such that Follows(s, c)").size());
	CPPUNIT_ASSERT_EQUAL_MESSAGE("Follows(c, 5)", 0, (int)evaluate("constant c; Select c such that Follows(c, 5)").size());
	CPPUNIT_ASSERT_EQUAL_MESSAGE("pattern w(c, _)", 0, (int)evaluate("while w; constant c; Select c pattern w(c, _)").size());
	CPPUNIT_ASSERT_EQUAL_MESSAGE("pattern a(c, _)", 0, (int)evaluate("assign a; constant c; Select c pattern a(c, _)").size());
	
}

list<string> evaluate(string query)
{
	list<string> results;
	PQLController::evaluate(query, results);
	return results;
}
