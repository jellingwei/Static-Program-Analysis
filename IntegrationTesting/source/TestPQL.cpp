#include <cppunit/config/SourcePrefix.h>
#include <vector>
#include <string>
#include <list>
#include "TestPQL.h"
#include "InputQueryParser.h"
#include "SourceParser.h"
#include "QueryEvaluator.h"
#include "QueryParser.h"
#include "QueryTree.h"
#include "PKB.h"
#include "Synonym.h"
#include "ResultProjector.h"

using namespace std;

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION( PQLTest ); 
list<string> evaluate(string query);

void PQLTest::setUp()
{
}

void PQLTest::tearDown()
{
}


void PQLTest::testPQL()
{
	// Select
	CPPUNIT_ASSERT_EQUAL_MESSAGE("Select all a", 10, (int)evaluate("assign a; Select a").size());
	CPPUNIT_ASSERT_EQUAL_MESSAGE("Select all s", 15, (int)evaluate("stmt s; Select s").size());
	CPPUNIT_ASSERT_EQUAL_MESSAGE("Select all v", 6, (int)evaluate("variable v; Select v").size());
	CPPUNIT_ASSERT_EQUAL_MESSAGE("Select all w", 3, (int)evaluate("while w; Select w").size());
	CPPUNIT_ASSERT_EQUAL_MESSAGE("Select all c", 3, (int)evaluate("constant c; Select c").size());
	CPPUNIT_ASSERT_EQUAL_MESSAGE("Select all l", 15, (int)evaluate("prog_line l; Select l").size());

	// Parent
	CPPUNIT_ASSERT_EQUAL_MESSAGE("Parent(s, 4)", (string)"3", evaluate("stmt s; Select s such that Parent(s, 4)").front());
	CPPUNIT_ASSERT_EQUAL_MESSAGE("Parent(w, 8)", (string)"7", evaluate("while w; Select w such that Parent(w, 8)").front());
	CPPUNIT_ASSERT_EQUAL_MESSAGE("Parent(l, 11)", 0, (int)evaluate("prog_line l; Select l such that Parent(l, 11)").size());
	CPPUNIT_ASSERT_EQUAL_MESSAGE("Parent(a, 9)", 0, (int)evaluate("assign a; Select a such that Parent(a, 9)").size());
	CPPUNIT_ASSERT_EQUAL_MESSAGE("Parent(v, 7)", 0, (int)evaluate("variable v; Select v such that Parent(v, 7)").size());
	CPPUNIT_ASSERT_EQUAL_MESSAGE("Parent(v, 4)", 0, (int)evaluate("variable v; Select v such that Parent(v, 4)").size());
	CPPUNIT_ASSERT_EQUAL_MESSAGE("Parent(8, l)", (string)"9", evaluate("prog_line l; Select l such that Parent(8, l)").front());
	CPPUNIT_ASSERT_EQUAL_MESSAGE("Parent(3, a)", (string)"4", evaluate("assign a; Select a such that Parent(3, a)").front());
	CPPUNIT_ASSERT_EQUAL_MESSAGE("Parent(5, s)", 0, (int)evaluate("stmt s; Select s such that Parent(5, s)").size());
	CPPUNIT_ASSERT_EQUAL_MESSAGE("Parent(7, a)", (string)"10", evaluate("assign a; Select a such that Parent(7, a)").front());
	CPPUNIT_ASSERT_EQUAL_MESSAGE("Parent(7, w)", (string)"8", evaluate("while w; Select w such that Parent(7, w)").front());
	CPPUNIT_ASSERT_EQUAL_MESSAGE("Parent(3, v)", 0, (int)evaluate("variable v; Select v such that Parent(3, v)").size());
	CPPUNIT_ASSERT_EQUAL_MESSAGE("Parent(<w>, s)", 3, (int)evaluate("while w; stmt s; Select w such that Parent(w, s)").size());
	CPPUNIT_ASSERT_EQUAL_MESSAGE("Parent(w, <a>)", 3, (int)evaluate("assign a; while w; Select a such that Parent(w, a)").size());
	CPPUNIT_ASSERT_EQUAL_MESSAGE("Select s; Parent(1, 3)", 0, (int)evaluate("stmt s; Select s such that Parent(1, 3)").size());
	CPPUNIT_ASSERT_EQUAL_MESSAGE("Select l; Parent(7, 9)", 0, (int)evaluate("prog_line l; Select l such that Parent(7, 9)").size());
	CPPUNIT_ASSERT_EQUAL_MESSAGE("Select w; Parent(a1, a2)", 0, (int)evaluate("assign a1, a2; while w; Select w such that Parent(a1, a2)").size());
	CPPUNIT_ASSERT_EQUAL_MESSAGE("Select w; Parent(3, 4)", 3, (int)evaluate("while w; Select w such that Parent(3, 4)").size());
	CPPUNIT_ASSERT_EQUAL_MESSAGE("Select w; Parent(7, 10)", 15, (int)evaluate("stmt s; Select s such that Parent(7, 10)").size());
	CPPUNIT_ASSERT_EQUAL_MESSAGE("Select a1; Parent(w, a2)", 10, (int)evaluate("assign a1, a2; while w; Select a1 such that Parent(w, a2)").size());
	CPPUNIT_ASSERT_EQUAL_MESSAGE("Parent(_, w)", (string)"8", evaluate("while w; Select w such that Parent(_, w)").front());
	CPPUNIT_ASSERT_EQUAL_MESSAGE("Parent(_, a)", 5, (int)evaluate("assign a; Select a such that Parent(_, a)").size());
	CPPUNIT_ASSERT_EQUAL_MESSAGE("Parent(s, _)", 5, (int)evaluate("stmt s; Select s such that Parent(s, _)").size());
	//CPPUNIT_ASSERT_EQUAL_MESSAGE("Parent(_, _)", 6, (int)evaluate("variable v; Select v such that Parent(_, _)").size());
	CPPUNIT_ASSERT_EQUAL_MESSAGE("Parent(_, _)", 0, (int)evaluate("variable v; Select v such that Parent(_, _)").size());
	CPPUNIT_ASSERT_EQUAL_MESSAGE("Parent(l,)", 0, (int)evaluate("prog_line l; Select l such that Parent(l,)").size());
	CPPUNIT_ASSERT_EQUAL_MESSAGE("Parent(,l)", 0, (int)evaluate("prog_line l; Select l such that Parent(,l)").size());
	CPPUNIT_ASSERT_EQUAL_MESSAGE("Parent(,l)", 0, (int)evaluate("prog_line l; Select l such that Parent(,)").size());
	CPPUNIT_ASSERT_EQUAL_MESSAGE("Parent(2,)", 0, (int)evaluate("prog_line l; Select l such that Parent(2,)").size());
	CPPUNIT_ASSERT_EQUAL_MESSAGE("Parent(,2)", 0, (int)evaluate("prog_line l; Select l such that Parent(,2)").size());



	// Follows
	
	CPPUNIT_ASSERT_EQUAL_MESSAGE("Follows(_, 1)", 0, int(evaluate("assign a; Select a such that Follows(_, 1)").size()));
	CPPUNIT_ASSERT_EQUAL_MESSAGE("Follows(12, _)", 0, int(evaluate("assign a; Select a such that Follows(12, _)").size()));
	CPPUNIT_ASSERT_EQUAL_MESSAGE("Follows(1, a)", (string)"2", evaluate("assign a; Select a such that Follows(1, a)").front());
	CPPUNIT_ASSERT_EQUAL_MESSAGE("Follows(3, s)", (string)"5", evaluate("stmt s; Select s such that Follows(3, s)").front());
	CPPUNIT_ASSERT_EQUAL_MESSAGE("Follows(8, l)", (string)"10", evaluate("prog_line l; Select l such that Follows(8, l)").front());
	CPPUNIT_ASSERT_EQUAL_MESSAGE("Follows(7, v)", 0, (int)evaluate("variable v; Select v such that Follows(7, v)").size());
	CPPUNIT_ASSERT_EQUAL_MESSAGE("Follows(10, s)", 0, (int)evaluate("stmt s; Select s such that Follows(10, s)").size());
	CPPUNIT_ASSERT_EQUAL_MESSAGE("Follows(l, 6)", (string)"5", evaluate("prog_line l; Select l such that Follows(l, 6)").front());
	CPPUNIT_ASSERT_EQUAL_MESSAGE("Follows(s, 10)", (string)"8", evaluate("stmt s; Select s such that Follows(s, 10)").front());
	CPPUNIT_ASSERT_EQUAL_MESSAGE("Follows(l, 1)", 0, (int)evaluate("prog_line l; Select l such that Follows(l, 1)").size());
	CPPUNIT_ASSERT_EQUAL_MESSAGE("Follows(12, l)", 0, (int)evaluate("prog_line l; Select l such that Follows(12, l)").size());
	CPPUNIT_ASSERT_EQUAL_MESSAGE("Follows(w, 11)", (string)"7", evaluate("while w; Select w such that Follows(w, 11)").front());
	CPPUNIT_ASSERT_EQUAL_MESSAGE("Follows(<a1>, a2)", 2, (int)evaluate("assign a1, a2; Select a1 such that Follows(a1, a2)").size());
	CPPUNIT_ASSERT_EQUAL_MESSAGE("Follows(a, <w>)", 2, (int)evaluate("while w; assign a; Select w such that Follows(a, w)").size());
	CPPUNIT_ASSERT_EQUAL_MESSAGE("Follows(s, <l>)", 8, (int)evaluate("stmt s; prog_line l; Select l such that Follows(s, l)").size());
	CPPUNIT_ASSERT_EQUAL_MESSAGE("Select w; Follows(3, 4)", 0, (int)evaluate("while w; Select w such that Follows(3, 4)").size());
	CPPUNIT_ASSERT_EQUAL_MESSAGE("Select s; Follows(1, 11)", 0, (int)evaluate("stmt s; Select s such that Follows(1, 11)").size());
	CPPUNIT_ASSERT_EQUAL_MESSAGE("Select l; Follows(v, a)", 0, (int)evaluate("variable v; assign a; prog_line l; Select l such that Follows(v, a)").size());
	CPPUNIT_ASSERT_EQUAL_MESSAGE("Select l; Follows(3, 5)", 15, (int)evaluate("prog_line l; Select l such that Follows(3, 5)").size());
	CPPUNIT_ASSERT_EQUAL_MESSAGE("Select a; Follows(1, 2)", 10, (int)evaluate("assign a; Select a such that Follows(1, 2)").size());
	CPPUNIT_ASSERT_EQUAL_MESSAGE("Select s; Follows(a, w)", 15, (int)evaluate("stmt s; assign a; while w; Select s such that Follows(a, w)").size());
	CPPUNIT_ASSERT_EQUAL_MESSAGE("Follows(_, a)", 5, (int)evaluate("assign a; Select a such that Follows(_,a)").size());
	CPPUNIT_ASSERT_EQUAL_MESSAGE("Follows(s, _)", 8, (int)evaluate("stmt s; Select s such that Follows(s, _)").size());
	//CPPUNIT_ASSERT_EQUAL_MESSAGE("Follows(_, _)", 6, (int)evaluate("variable v; Select v such that Follows(_, _)").size());
	CPPUNIT_ASSERT_EQUAL_MESSAGE("Follows(_, _)", 0, (int)evaluate("variable v; Select v such that Follows(_, _)").size());

	// Uses
	CPPUNIT_ASSERT_EQUAL_MESSAGE("Uses(s, 'd')", 2, (int)evaluate("stmt s; Select s such that Uses(s, \"d\")").size());
	CPPUNIT_ASSERT_EQUAL_MESSAGE("Uses(l, 'a')", 2, (int)evaluate("prog_line l; Select l such that Uses(l, \"a\")").size());
	CPPUNIT_ASSERT_EQUAL_MESSAGE("Uses(a, 'z')", 0, (int)evaluate("assign a; Select a such that Uses(a, \"z\")").size()); 
	CPPUNIT_ASSERT_EQUAL_MESSAGE("Uses(w, 'b')", (string)"7", evaluate("while w; Select w such that Uses(w, \"b\")").front());
	CPPUNIT_ASSERT_EQUAL_MESSAGE("Uses(v, 'f')", 0, (int)evaluate("variable v; Select v such that Uses(v, \"f\")").size()); 
	CPPUNIT_ASSERT_EQUAL_MESSAGE("Uses(1, v)", 0, (int)evaluate("variable v; Select v such that Uses(1, v)").size());
	CPPUNIT_ASSERT_EQUAL_MESSAGE("Uses(2, v)", 2, (int)evaluate("variable v; Select v such that Uses(2, v)").size());
	CPPUNIT_ASSERT_EQUAL_MESSAGE("Uses(7, v)", 4, (int)evaluate("variable v; Select v such that Uses(7, v)").size());
	CPPUNIT_ASSERT_EQUAL_MESSAGE("Uses(<a>, v)", 5, (int)evaluate("assign a; variable v; Select a such that Uses(a, v)").size());
	CPPUNIT_ASSERT_EQUAL_MESSAGE("Uses(<s>, v)", 10, (int)evaluate("stmt s; variable v; Select s such that Uses(s, v)").size());
	CPPUNIT_ASSERT_EQUAL_MESSAGE("Uses(w, <v>)", 5, (int)evaluate("variable v; while w; Select v such that Uses(w, v)").size());
	CPPUNIT_ASSERT_EQUAL_MESSAGE("Uses(l, <v>)", 6, (int)evaluate("prog_line l; variable v; Select v such that Uses(l, v)").size());
	CPPUNIT_ASSERT_EQUAL_MESSAGE("Select l; Uses(1, 'a')", 0, (int)evaluate("prog_line l; Select l such that Uses(1, \"a\")").size());
	CPPUNIT_ASSERT_EQUAL_MESSAGE("Select s; Uses(7, 'a')", 0, (int)evaluate("stmt s; Select s such that Uses(7, \"a\")").size());
	CPPUNIT_ASSERT_EQUAL_MESSAGE("Select w; Uses(v, l)", 0, (int)evaluate("while w; variable v; prog_line l; Select w such that Uses(v, l)").size());
	CPPUNIT_ASSERT_EQUAL_MESSAGE("Select a; Uses(5, 'd')", 10, (int)evaluate("assign a; Select a such that Uses(5, \"d\")").size());
	CPPUNIT_ASSERT_EQUAL_MESSAGE("Select w; Uses(7, 'e')", 3, (int)evaluate("while w; Select w such that Uses(7, \"e\")").size());
	CPPUNIT_ASSERT_EQUAL_MESSAGE("Select s; Uses(a, v)", 15, (int)evaluate("stmt s; assign a; variable v; Select s such that Uses(a, v)").size());
	CPPUNIT_ASSERT_EQUAL_MESSAGE("Uses(a, _)", 5, (int)evaluate("assign a; Select a such that Uses(a,_)").size());
	CPPUNIT_ASSERT_EQUAL_MESSAGE("Uses(w, _)", 3, (int)evaluate("while w; Select w such that Uses(w, _)").size());
	CPPUNIT_ASSERT_EQUAL_MESSAGE("Uses(s, _)", 10, (int)evaluate("stmt s; Select s such that Uses(s, _)").size());

	// Modifies
	//CPPUNIT_ASSERT_EQUAL_MESSAGE("Modifies(a, 'a')", 4, (int)evaluate("assign a; Select a such that Modifies(a, \"a\")").size());
	CPPUNIT_ASSERT_EQUAL_MESSAGE("Modifies(a, 'a')", 0, (int)evaluate("assign a; Select a such that Modifies(a, \"a\")").size());
	CPPUNIT_ASSERT_EQUAL_MESSAGE("Modifies(w, 'f')", 2,(int)evaluate("while w; Select w such that Modifies(w, \"f\")").size());
	CPPUNIT_ASSERT_EQUAL_MESSAGE("Modifies(s, 'e')", 3, (int)evaluate("stmt s; Select s such that Modifies(s, \"e\")").size());
	CPPUNIT_ASSERT_EQUAL_MESSAGE("Modifies(l, 'b')", 2, (int)evaluate("prog_line l; Select l such that Modifies(l, \"b\")").size());
	CPPUNIT_ASSERT_EQUAL_MESSAGE("Modifies(v, 'd')", 0, (int)evaluate("variable v; Select v such that Modifies(v, \"d\")").size());
	CPPUNIT_ASSERT_EQUAL_MESSAGE("Modifies(1, v)", (string)"a", evaluate("variable v; Select v such that Modifies(1, v)").front());
	CPPUNIT_ASSERT_EQUAL_MESSAGE("Modifies(3, v)", 1, (int)evaluate("variable v; Select v such that Modifies(3, v)").size());
	CPPUNIT_ASSERT_EQUAL_MESSAGE("Modifies(7, v)", 2, (int)evaluate("variable v; Select v such that Modifies(7, v)").size());
	CPPUNIT_ASSERT_EQUAL_MESSAGE("Modifies(<a>, v)", 10,(int)evaluate("variable v; assign a; Select a such that Modifies(a, v)").size());
	CPPUNIT_ASSERT_EQUAL_MESSAGE("Modifies(<w>, v)", 3,(int)evaluate("while w; variable v; Select w such that Modifies(w, v)").size());
	CPPUNIT_ASSERT_EQUAL_MESSAGE("Modifies(s, <v>)", 6,(int)evaluate("stmt s; variable v; Select v such that Modifies(s, v)").size());
	CPPUNIT_ASSERT_EQUAL_MESSAGE("Modifies(v1, v2)", 0,(int)evaluate("variable v1, v2; Select v2 such that Modifies(v1, v2)").size());
	CPPUNIT_ASSERT_EQUAL_MESSAGE("Select s; Modifies(1, 'v')", 0, (int)evaluate("stmt s; Select s such that Modifies(1,\"v\")").size());
	CPPUNIT_ASSERT_EQUAL_MESSAGE("Select l; Modifies(7, 'a')", 0, (int)evaluate("prog_line l; Select l such that Modifies(7,\"a\")").size());
	CPPUNIT_ASSERT_EQUAL_MESSAGE("Select s1; Modifies(v, s2)", 0, (int)evaluate("stmt s1,s2; variable v; Select s1 such that Modifies(v, s2)").size());
	CPPUNIT_ASSERT_EQUAL_MESSAGE("Select v; Modifies(2, 'c')", 6, (int)evaluate("variable v; Select v such that Modifies(2,\"c\")").size());
	CPPUNIT_ASSERT_EQUAL_MESSAGE("Select w; Modifies(7, 'f')", 3, (int)evaluate("while w; Select w such that Modifies(7,\"f\")").size());
	CPPUNIT_ASSERT_EQUAL_MESSAGE("Select w; Modifies(a, v)", 3, (int)evaluate("while w; assign a; variable v; Select w such that Modifies(a, v)").size());
	CPPUNIT_ASSERT_EQUAL_MESSAGE("Modifies(a, _)", 10, (int)evaluate("assign a; Select a such that Modifies(a, _)").size());
	CPPUNIT_ASSERT_EQUAL_MESSAGE("Modifies(w, _)", 3, (int)evaluate("while w; Select w such that Modifies(w, _)").size());
	CPPUNIT_ASSERT_EQUAL_MESSAGE("Modifies(s, _)", 15, (int)evaluate("stmt s; Select s such that Modifies(s, _)").size());

	// Follows*
	CPPUNIT_ASSERT_EQUAL_MESSAGE("Follows*(s, 3)", 2, (int)evaluate("stmt s; Select s such that Follows*(s, 3)").size());
	CPPUNIT_ASSERT_EQUAL_MESSAGE("Follows*(s, 1)", 0, (int)evaluate("stmt s; Select s such that Follows*(s, 1)").size());
	CPPUNIT_ASSERT_EQUAL_MESSAGE("Follows*(w, 11)", 2, (int)evaluate("while w; Select w such that Follows*(w, 11)").size());
	CPPUNIT_ASSERT_EQUAL_MESSAGE("Follows*(8, a)", (string)"10", evaluate("assign a; Select a such that Follows*(8, a)").front());
	CPPUNIT_ASSERT_EQUAL_MESSAGE("Follows*(7, l)", (string)"11", evaluate("prog_line l; Select l such that Follows*(7, l)").front());
	CPPUNIT_ASSERT_EQUAL_MESSAGE("Follows*(3, a)", 3, (int)evaluate("assign a; Select a such that Follows*(3, a)").size());
	CPPUNIT_ASSERT_EQUAL_MESSAGE("Follows*(<a1>, a2)", 4, (int)evaluate("assign a1, a2; Select a1 such that Follows*(a1, a2)").size());
	CPPUNIT_ASSERT_EQUAL_MESSAGE("Follows*(<w>, a)", 3, (int)evaluate("assign a; while w; Select w such that Follows*(w, a)").size());
	CPPUNIT_ASSERT_EQUAL_MESSAGE("Follows*(<s>, w)", 5, (int)evaluate("stmt s; while w; Select s such that Follows*(s, w)").size());
	CPPUNIT_ASSERT_EQUAL_MESSAGE("Follows*(s, <w>)", 2, (int)evaluate("stmt s; while w; Select w such that Follows*(s, w)").size());
	CPPUNIT_ASSERT_EQUAL_MESSAGE("Follows*(a1, <a2>)", 4, (int)evaluate("assign a1, a2; Select a2 such that Follows*(a1, a2)").size());
	CPPUNIT_ASSERT_EQUAL_MESSAGE("Follows*(v, <s>)", 0, (int)evaluate("variable v; stmt s; Select s such that Follows*(v, s)").size()); 
	CPPUNIT_ASSERT_EQUAL_MESSAGE("Select v; Follows*(l, w)", 6, (int)evaluate("prog_line l; while w; variable v; Select v such that Follows*(l, w)").size());
	CPPUNIT_ASSERT_EQUAL_MESSAGE("Select s; Follows*(l1, l2)", 15, (int)evaluate("prog_line l1, l2; stmt s; Select s such that Follows*(l1, l2)").size());
	CPPUNIT_ASSERT_EQUAL_MESSAGE("Select a; Follows*(1, 4)", 0, (int)evaluate("assign a; Select a such that Follows*(1, 4)").size());
	CPPUNIT_ASSERT_EQUAL_MESSAGE("Select a; Follows*(4, 11)", 0, (int)evaluate("assign a; Select a such that Follows*(4, 11)").size());
	CPPUNIT_ASSERT_EQUAL_MESSAGE("Select l; Follows*(8, 9)", 0, (int)evaluate("prog_line l; Select l such that Follows*(8, 9)").size());

	// Parent*
	CPPUNIT_ASSERT_EQUAL_MESSAGE("Parent*(1, l)", 0, (int)evaluate("prog_line l; Select l such that Parent*(1, l)").size());
	CPPUNIT_ASSERT_EQUAL_MESSAGE("Parent*(3, s)", (string)"4", evaluate("stmt s; Select s such that Parent*(3, s)").front());
	CPPUNIT_ASSERT_EQUAL_MESSAGE("Parent*(1, l)", 0, (int)evaluate("prog_line l; Select l such that Parent*(1, l)").size());
	CPPUNIT_ASSERT_EQUAL_MESSAGE("Parent*(7, a)", 2, (int)evaluate("assign a; Select a such that Parent*(7, a)").size()); 
	CPPUNIT_ASSERT_EQUAL_MESSAGE("Parent*(7, v)", 0, (int)evaluate("variable v; Select v such that Parent*(7, v)").size()); 
	CPPUNIT_ASSERT_EQUAL_MESSAGE("Parent*(s, 9)", 2, (int)evaluate("stmt s; Select s such that Parent*(s, 9)").size()); 
	CPPUNIT_ASSERT_EQUAL_MESSAGE("Parent*(a, 10)", 0, (int)evaluate("assign a; Select a such that Parent*(a, 10)").size()); 
	CPPUNIT_ASSERT_EQUAL_MESSAGE("Parent*(l, 8)", (string)"7", evaluate("prog_line l; Select l such that Parent*(l, 8)").front());
	CPPUNIT_ASSERT_EQUAL_MESSAGE("Parent*(v, 9)", 0, (int)evaluate("variable v; Select v such that Parent*(v, 9)").size()); 
	CPPUNIT_ASSERT_EQUAL_MESSAGE("Parent*(w, <a>)", 3, (int)evaluate("while w; assign a; Select a such that Parent*(w, a)").size());
	CPPUNIT_ASSERT_EQUAL_MESSAGE("Parent*(w, <s>)", 4, (int)evaluate("stmt s; while w; Select s such that Parent*(w, s)").size());
	CPPUNIT_ASSERT_EQUAL_MESSAGE("Parent*(w1, <w2>)", 1, (int)evaluate("while w1, w2; Select w2 such that Parent*(w1, w2)").size());
	CPPUNIT_ASSERT_EQUAL_MESSAGE("Parent*(s, <v>)", 0, (int)evaluate("variable v; stmt s; Select v such that Parent*(s, v)").size()); 
	CPPUNIT_ASSERT_EQUAL_MESSAGE("Parent*(<l>, s)", 5, (int)evaluate("prog_line l; stmt s; Select l such that Parent*(l, s)").size()); 

	CPPUNIT_ASSERT_EQUAL_MESSAGE("Parent*(<s>, w)", (string)"7", evaluate("while w; stmt s; Select s such that Parent*(s, w)").front()); 
	CPPUNIT_ASSERT_EQUAL_MESSAGE("Parent*(<a>, w)", 0, (int)evaluate("assign a; while w; Select a such that Parent*(a, w)").size()); 
	CPPUNIT_ASSERT_EQUAL_MESSAGE("Select s; Parent*(w1, w2)", 15, (int)evaluate("while w1, w2; stmt s; Select s such that Parent*(w1, w2)").size());
	CPPUNIT_ASSERT_EQUAL_MESSAGE("Select v; Parent*(a, w)", 0, (int)evaluate("variable v; assign a; while w; Select v such that Parent*(a, w)").size());
	CPPUNIT_ASSERT_EQUAL_MESSAGE("Select v, Parent*(7, 9)", 6, (int)evaluate("variable v; Select v such that Parent*(7, 9)").size());
	CPPUNIT_ASSERT_EQUAL_MESSAGE("Select l, Parent*(7, 6)", 0, (int)evaluate("prog_line l; Select l such that Parent*(7, 6)").size());
	
	// Assign Pattern
	CPPUNIT_ASSERT_EQUAL_MESSAGE("pattern a('c', 'd+f')", (string)"2", evaluate("assign a; Select a pattern a(\"c\", \"d + f\")").front());
	CPPUNIT_ASSERT_EQUAL_MESSAGE("pattern a('d', 'd')", 0, (int)evaluate("assign a; Select a pattern a(\"d\", \"d\")").size());
	CPPUNIT_ASSERT_EQUAL_MESSAGE("pattern a('f', 'woot')", 0, (int)evaluate("assign a; Select a pattern a(\"f\", \"woot\")").size());
	CPPUNIT_ASSERT_EQUAL_MESSAGE("pattern a('e', _)", 2, (int)evaluate("assign a; Select a pattern a(\"e\", _)").size());
	CPPUNIT_ASSERT_EQUAL_MESSAGE("pattern a('b', _)", (string)"4", evaluate("assign a; Select a pattern a(\"b\", _)").front());
	CPPUNIT_ASSERT_EQUAL_MESSAGE("pattern a(_, 'f+2')", (string)"10", evaluate("assign a; Select a pattern a(_, \"f +2\")").front());
	CPPUNIT_ASSERT_EQUAL_MESSAGE("pattern a(_, '3')", (string)"11", evaluate("assign a; Select a pattern a(_, \"3\")").front());
	CPPUNIT_ASSERT_EQUAL_MESSAGE("pattern a(_, '1 + 2 + 3 + a')", 0, (int)evaluate("assign a; Select a pattern a(_, \"1+2+3+a\")").size());
	CPPUNIT_ASSERT_EQUAL_MESSAGE("pattern a(_, _)", 10, (int)evaluate("assign a; Select a pattern a(_, _)").size());
	CPPUNIT_ASSERT_EQUAL_MESSAGE("pattern a('e', '_f_')", 2, (int)evaluate("assign a; Select a pattern a(\"e\", _\"f\"_)").size());
	CPPUNIT_ASSERT_EQUAL_MESSAGE("pattern a('f', _'e+1'_)", (string)"9", evaluate("assign a; Select a pattern a(\"f\", _\"e+1\"_)").front());
	CPPUNIT_ASSERT_EQUAL_MESSAGE("pattern a(_, _'1'_)", 3, (int)evaluate("assign a; Select a pattern a(_, _ \"1\" _)").size());
	CPPUNIT_ASSERT_EQUAL_MESSAGE("pattern a('c', '_d+f+e_')", 0, (int)evaluate("assign a; Select a pattern a(\"c\", \"_d + f +e_\")").size());
	CPPUNIT_ASSERT_EQUAL_MESSAGE("pattern a('v', _)", 0, (int)evaluate("assign a; Select a pattern a(\"v\", _)").size());
	CPPUNIT_ASSERT_EQUAL_MESSAGE("Select v; pattern a(v, _'d'_)", 2, (int)evaluate("variable v; assign a; Select v pattern a(v, _\"d\"_)").size());
	CPPUNIT_ASSERT_EQUAL_MESSAGE("pattern a(_, _''_)'", 10, (int)evaluate("assign a; Select a pattern a(_, _\"\"_)").size());
	CPPUNIT_ASSERT_EQUAL_MESSAGE("pattern a(_, _)'", 10, (int)evaluate("assign a; Select a pattern a(_, _)").size());

	// while pattern
	CPPUNIT_ASSERT_EQUAL_MESSAGE("pattern w('a', _)", (string)"3", evaluate("while w; Select w pattern w(\"a\", _)").front());
	CPPUNIT_ASSERT_EQUAL_MESSAGE("pattern w('b', _)", (string)"7", evaluate("while w; Select w pattern w(\"b\", _)").front());
	CPPUNIT_ASSERT_EQUAL_MESSAGE("pattern w('a', _)", 1, (int)evaluate("while w; Select w pattern w(\"a\", _)").size());
	CPPUNIT_ASSERT_EQUAL_MESSAGE("pattern w(v, _)", 3, (int)evaluate("variable v; while w; Select v pattern w(v, _)").size());
	CPPUNIT_ASSERT_EQUAL_MESSAGE("pattern w(_, _)", 3, (int)evaluate("while w; Select w pattern w(_, _)").size());

	CPPUNIT_ASSERT_EQUAL_MESSAGE("Parent(7, a) pattern a(_, _'f'_)", (string)"10", evaluate("assign a; Select a such that Parent(7, a) pattern a(_, _\"f\"_)").front());
	CPPUNIT_ASSERT_EQUAL_MESSAGE("pattern a(_, _'f'_) Parent(7, a)", (string)"10", evaluate("assign a; Select a pattern a(_, _\"f\"_) such that Parent(7, a)").front());
	CPPUNIT_ASSERT_EQUAL_MESSAGE("Follows(a,6) pattern a('d', _)", (string)"5", evaluate("assign a; Select a such that Follows(a, 6) pattern a(\"d\", _)").front());
	CPPUNIT_ASSERT_EQUAL_MESSAGE("pattern a('d', _) Follows(a,6)", (string)"5", evaluate("assign a; Select a pattern a(\"d\", _) such that Follows(a, 6)").front());
	CPPUNIT_ASSERT_EQUAL_MESSAGE("Uses(6, 'f') pattern a('e', _'a'_)", (string)"6", evaluate("assign a; variable v; Select a such that Uses(6, \"f\") pattern a(\"e\", _\"a\"_)").front());
	CPPUNIT_ASSERT_EQUAL_MESSAGE("pattern a('e', _'a'_) Uses(6, 'f')", (string)"6", evaluate("assign a; variable v; Select a pattern a(\"e\", _\"a\"_) such that Uses(6, \"f\")").front());
	CPPUNIT_ASSERT_EQUAL_MESSAGE("Modifies(a, 'c') pattern a(_, _'b + c'_)", 0, (int)evaluate("assign a; Select a such that Modifies(a, \"c\") pattern a (_, _\"b+c\"_)").size());
	CPPUNIT_ASSERT_EQUAL_MESSAGE("pattern a(_, _'b + c'_) Modifies(a, 'c')", 0, (int)evaluate("assign a; Select a pattern a (_, _\"b+c\"_) such that Modifies(a, \"c\")").size());
	// Assertion
	CPPUNIT_ASSERT_EQUAL_MESSAGE("Parent(w, 4) pattern w(_, _)", (string)"3", evaluate("while w; Select w such that Parent (w, 4) pattern w(_, _)").front());
	CPPUNIT_ASSERT_EQUAL_MESSAGE("pattern w(_, _) Parent(w, 4)", (string)"3", evaluate("while w; Select w pattern w(_, _) such that Parent (w, 4)").front());

	// case sensitive synonym
	CPPUNIT_ASSERT_EQUAL_MESSAGE("while w, W; Parent(<W>, w)", (string)"7", evaluate("while w, W; Select W such that Parent(W, w)").front());
	CPPUNIT_ASSERT_EQUAL_MESSAGE("while W; assign w; Follows(W, <w>)", 3, (int)evaluate("while W; assign w; Select w such that Follows(W, w)").size());
	
	// Values that are not suppose to be in queries
	CPPUNIT_ASSERT_EQUAL_MESSAGE("assign a; Modifies(b, 'b')", 0, (int)evaluate("assign a; Select b such that Modifies(b, \"b\")").size());
	CPPUNIT_ASSERT_EQUAL_MESSAGE("Parent*('b', s)", 0, (int)evaluate("Select s; Select s such that Parent*(\"b\", s)").size()); 
	CPPUNIT_ASSERT_EQUAL_MESSAGE("Parent*(16, s)", 0, (int)evaluate("Select s; Select s such that Parent*(16, s)").size()); 
	CPPUNIT_ASSERT_EQUAL_MESSAGE("Parent*(s, 16)", 0, (int)evaluate("Select s; Select s such that Parent*(s, 16)").size());
	CPPUNIT_ASSERT_EQUAL_MESSAGE("Follows*(a, 'a')", 0, (int)evaluate("assign a; Select a such that Follows*(a, \"a\")").size()); 
	CPPUNIT_ASSERT_EQUAL_MESSAGE("Follows*(s, 16)", 0, (int)evaluate("stmt s; Select s such that Follows*(s, 16)").size());
	CPPUNIT_ASSERT_EQUAL_MESSAGE("Follows*(16, s)", 0, (int)evaluate("stmt s; Select s such that Follows*(16, s)").size());
	CPPUNIT_ASSERT_EQUAL_MESSAGE("Parent(w, 16)", 0, (int)evaluate("while w; Select w such that Parent(w, 16)").size());
	CPPUNIT_ASSERT_EQUAL_MESSAGE("Parent(16, a)", 0, (int)evaluate("assign a; Select a such that Parent(16, a)").size());
	CPPUNIT_ASSERT_EQUAL_MESSAGE("Modifies(16, v)", 0, (int)evaluate("variable v; Select v such that Modifies(16, v)").size());
	CPPUNIT_ASSERT_EQUAL_MESSAGE("Uses(16, v)", 0, (int)evaluate("variable v; Select v such that Uses(16, v)").size());
	CPPUNIT_ASSERT_EQUAL_MESSAGE("Follows(s, 16)", 0, (int)evaluate("stmt s; Select s such that Follows(s, 16)").size());
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
	CPPUNIT_ASSERT_EQUAL_MESSAGE("uses(a,v) pattern a(v, _)", 1, (int)evaluate("assign a; variable v; Select v such that Uses(a,v) pattern a(v,_)").size());
	CPPUNIT_ASSERT_EQUAL_MESSAGE("uses(a,v) pattern a(v, _)", 1, (int)evaluate("assign a; variable v; Select a such that Uses(a,v) pattern a(v,_)").size());
	CPPUNIT_ASSERT_EQUAL_MESSAGE("modifies(a,v) pattern a(v, _)", 6, (int)evaluate("assign a; variable v; Select v such that Modifies(a,v) pattern a(v,_)").size());
	CPPUNIT_ASSERT_EQUAL_MESSAGE("modifies(a,v) pattern a(v, _)", 10, (int)evaluate("assign a; variable v; Select a such that Modifies(a,v) pattern a(v,_)").size());
	CPPUNIT_ASSERT_EQUAL_MESSAGE("uses(w,v) pattern w(v, _)", 3, (int)evaluate("while w; variable v; Select v such that Uses(w,v) pattern w(v,_)").size());
	CPPUNIT_ASSERT_EQUAL_MESSAGE("uses(w,v) pattern w(v, _)", 3, (int)evaluate("while w; variable v; Select w such that Uses(w,v) pattern w(v,_)").size());
	CPPUNIT_ASSERT_EQUAL_MESSAGE("modifies(w,v) pattern w(v, _)", 0, (int)evaluate("while w; variable v; Select v such that Modifies(w,v) pattern w(v,_)").size());
	CPPUNIT_ASSERT_EQUAL_MESSAGE("modifies(w,v) pattern w(v, _)", 0, (int)evaluate("while w; variable v; Select w such that Modifies(w,v) pattern w(v,_)").size());
	CPPUNIT_ASSERT_EQUAL_MESSAGE("parent*(w,a) pattern a(v, _)", 3, (int)evaluate("assign a; while w; variable v; Select v such that Parent*(w,a) pattern a(v,_)").size());
}

list<string> evaluate(string query)
{
	list<string> results;
	QueryParser::initParser(query);
	QueryParser::parseQuery();
	QueryTree* qT = QueryParser::getQueryTree();
	vector<Synonym> queryResults = QueryEvaluator::processQueryTree(qT);
	ResultProjector::projectResultToList(queryResults, results);
	return results;
}

