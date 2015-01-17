#include <cppunit/config/SourcePrefix.h>
#include <vector>
#include <algorithm>
#include "TestPKB.h"
#include "SourceParser.h"
#include "PKB.h"
#include "ConstantTable.h"

#define DEBUG(x) do { std::cerr << x << endl; } while (0)

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION( PKBTest ); 

void PKBTest::setUp()
{
}

void PKBTest::tearDown()
{
}

void PKBTest::testPKB()
{
	cout << "start" << endl;
	// Test Parser
	Parser::initParser("test/i_src.txt");
	bool parse = Parser::parseProgram();
	CPPUNIT_ASSERT_EQUAL_MESSAGE("Parsed source file", parse, true);

	PKB pkb = PKB::getInstance();
	
	// ConstantTable
	cout << "constanttable" << endl;
	CPPUNIT_ASSERT_EQUAL_MESSAGE("Number of constants", 3, pkb.getConstantTableSize());

	// VarTable
	CPPUNIT_ASSERT_EQUAL_MESSAGE("Number of variables", 6, pkb.getVarTableSize());
	CPPUNIT_ASSERT(pkb.getVarIndex("f"));
	CPPUNIT_ASSERT_EQUAL_MESSAGE("Get invalid variable", -1, pkb.getVarIndex("z"));

	// Test Design Extractor
	// Parent
	cout << "Parent" << endl;
	CPPUNIT_ASSERT_EQUAL_MESSAGE("Parent(s, 4)", 3, pkb.getParent(4).front());
	CPPUNIT_ASSERT_EQUAL_MESSAGE("Parent(s, 8)", 7, pkb.getParent(8).front());
	CPPUNIT_ASSERT_EQUAL_MESSAGE("Parent(s, 11)", 0, (int)pkb.getParent(11).size());

	CPPUNIT_ASSERT_EQUAL_MESSAGE("Parent(8, s)", 9, pkb.getChild(8).front());
	CPPUNIT_ASSERT_EQUAL_MESSAGE("Parent(7, s)", 8, pkb.getChild(7).front());
	CPPUNIT_ASSERT_EQUAL_MESSAGE("Parent(5, s)", 0, (int)pkb.getChild(5).size());

	CPPUNIT_ASSERT_EQUAL_MESSAGE("Parent(7, 9)", false, pkb.isParent(7,9));
	CPPUNIT_ASSERT_EQUAL_MESSAGE("Parent(8, 9)", true, pkb.isParent(8,9));
	CPPUNIT_ASSERT_EQUAL_MESSAGE("Parent(1, 2)", false, pkb.isParent(1,2));
	CPPUNIT_ASSERT_EQUAL_MESSAGE("Parent(1, 2)", false, pkb.isParent(1,2));
	CPPUNIT_ASSERT_EQUAL_MESSAGE("Parent(0, 1)", false, pkb.isParent(0,1));
	CPPUNIT_ASSERT_EQUAL_MESSAGE("Parent(9999, 10000)", false, pkb.isParent(9999,10000));

	//Parent*
	cout << "Parent*" << endl;
	CPPUNIT_ASSERT_EQUAL_MESSAGE("Parent*(s, 4)", 3, pkb.getParent(4, true).front());
	CPPUNIT_ASSERT_EQUAL_MESSAGE("Parent*(s, 9)", 2, (int)pkb.getParent(9, true).size());
	CPPUNIT_ASSERT_EQUAL_MESSAGE("Parent*(s, 10)", 1, (int)pkb.getParent(10, true).size());
	CPPUNIT_ASSERT_EQUAL_MESSAGE("Parent*(s, 11)", 0, (int)pkb.getParent(11, true).size());
	CPPUNIT_ASSERT_EQUAL_MESSAGE("Parent*(s, 13)", 1, (int)pkb.getParent(13, true).size());
	CPPUNIT_ASSERT_EQUAL_MESSAGE("Parent*(s, 14)", 2, (int)pkb.getParent(14, true).size());

	CPPUNIT_ASSERT_EQUAL_MESSAGE("Parent*(7, s)", 3, (int)pkb.getChild(7, true).size());
	CPPUNIT_ASSERT_EQUAL_MESSAGE("Parent*(8, s)", 1, (int)pkb.getChild(8, true).size());
	CPPUNIT_ASSERT_EQUAL_MESSAGE("Parent*(9, s)", 0, (int)pkb.getChild(9, true).size());
	CPPUNIT_ASSERT_EQUAL_MESSAGE("Parent*(12, s)", 2, (int)pkb.getChild(12, true).size());
	CPPUNIT_ASSERT_EQUAL_MESSAGE("Parent*(13, s)", 1, (int)pkb.getChild(13, true).size());
	

	// Follows
	cout << "Follows" << endl;
	CPPUNIT_ASSERT_EQUAL_MESSAGE("Follows(1, 2)", true, pkb.isFollows(1,2));
	CPPUNIT_ASSERT_EQUAL_MESSAGE("Follows(3, 5)", true, pkb.isFollows(3,5));
	CPPUNIT_ASSERT_EQUAL_MESSAGE("Follows(7, 8)", false, pkb.isFollows(7,8));
	CPPUNIT_ASSERT_EQUAL_MESSAGE("Follows(7, 10)", false, pkb.isFollows(7,10));
	CPPUNIT_ASSERT_EQUAL_MESSAGE("Follows(4, 11)", false, pkb.isFollows(4,11));
	CPPUNIT_ASSERT_EQUAL_MESSAGE("Follows(8, 9)", false, pkb.isFollows(8,9));
	CPPUNIT_ASSERT_EQUAL_MESSAGE("Follows(8, 10)", true, pkb.isFollows(8,10));
	CPPUNIT_ASSERT_EQUAL_MESSAGE("Follows(7, 10)", false, pkb.isFollows(7,10));
	CPPUNIT_ASSERT_EQUAL_MESSAGE("Follows(s, 6)", 5, pkb.getStmtFollowedTo(6).front());
	CPPUNIT_ASSERT_EQUAL_MESSAGE("Follows(s, 10)", 8, pkb.getStmtFollowedTo(10).front());
	CPPUNIT_ASSERT_EQUAL_MESSAGE("Follows(s, 1)", 0, (int)pkb.getStmtFollowedTo(1).size());
	CPPUNIT_ASSERT_EQUAL_MESSAGE("Follows(6, s)", 7, pkb.getStmtFollowedFrom(6).front());
	CPPUNIT_ASSERT_EQUAL_MESSAGE("Follows(7, s)", 11, pkb.getStmtFollowedFrom(7).front());
	CPPUNIT_ASSERT_EQUAL_MESSAGE("Follows(9, s)", 0, (int)pkb.getStmtFollowedFrom(9).size());
	CPPUNIT_ASSERT_EQUAL_MESSAGE("Follows(9999, 10000)", false, pkb.isFollows(9999,10000));

	// Uses
	CPPUNIT_ASSERT_EQUAL_MESSAGE("Uses(2, 'd')", true, pkb.isUses(2, pkb.getVarIndex("d")));
	CPPUNIT_ASSERT_EQUAL_MESSAGE("Uses(2, 'f')", true, pkb.isUses(2, pkb.getVarIndex("f")));
	CPPUNIT_ASSERT_EQUAL_MESSAGE("Uses(2, 'a')", false, pkb.isUses(2, pkb.getVarIndex("a")));
	CPPUNIT_ASSERT_EQUAL_MESSAGE("Uses(3, v)", (string)"a", (string)pkb.getVarName(pkb.getUsesVarForStmt(3).front())); 
	CPPUNIT_ASSERT_EQUAL_MESSAGE("Uses(7, v)", 4, (int)pkb.getUsesVarForStmt(7).size());
	CPPUNIT_ASSERT_EQUAL_MESSAGE("Uses(11, v)", 0, (int)pkb.getUsesVarForStmt(11).size()); 
	CPPUNIT_ASSERT_EQUAL_MESSAGE("Uses(s, 'c')", 8, pkb.getUsesStmtNum(pkb.getVarIndex("c")).front());
	CPPUNIT_ASSERT_EQUAL_MESSAGE("Uses(s, 'f')", 4, (int)pkb.getUsesStmtNum(pkb.getVarIndex("f")).size());
	CPPUNIT_ASSERT_EQUAL_MESSAGE("Uses(1, _)", 0, (int)pkb.getUsesVarForStmt(1).size());

	// Modifies
	CPPUNIT_ASSERT_EQUAL_MESSAGE("Modifies(1, 'a')", true, pkb.isModifies(1, pkb.getVarIndex("a")));
	CPPUNIT_ASSERT_EQUAL_MESSAGE("Modifies(7, 'f')", true, pkb.isModifies(7, pkb.getVarIndex("f")));
	CPPUNIT_ASSERT_EQUAL_MESSAGE("Modifies(5, 'e')", false, pkb.isModifies(5, pkb.getVarIndex("e")));
	CPPUNIT_ASSERT_EQUAL_MESSAGE("Modifies(7, 'b')", false, pkb.isModifies(7, pkb.getVarIndex("b")));
	CPPUNIT_ASSERT_EQUAL_MESSAGE("Modifies(s, 'd')", 5, pkb.getModStmtNum(pkb.getVarIndex("d")).front());
	CPPUNIT_ASSERT_EQUAL_MESSAGE("Modifies(s, 'b')", 2, (int)pkb.getModStmtNum(pkb.getVarIndex("b")).size());
	CPPUNIT_ASSERT_EQUAL_MESSAGE("Modifies(7, v)", 2, (int)pkb.getModVarForStmt(7).size());
	CPPUNIT_ASSERT_EQUAL_MESSAGE("Modifies(8, v)", (string)"f", pkb.getVarName(pkb.getModVarForStmt(8).front()));
	CPPUNIT_ASSERT_EQUAL_MESSAGE("Modifies(11, v)", (string)"a", pkb.getVarName(pkb.getModVarForStmt(11).front()));

	// Pattern
	CPPUNIT_ASSERT_EQUAL_MESSAGE("pattern a(_, _'f'_)", 3, (int)pkb.patternMatchAssign("_\"f\"_").size());
	CPPUNIT_ASSERT_EQUAL_MESSAGE("pattern a(_, _'a+f'_)", 6, pkb.patternMatchAssign("_\"a+f\"_").front());
	CPPUNIT_ASSERT_EQUAL_MESSAGE("pattern a(_, _'a + f'_)", 6, pkb.patternMatchAssign("_\"a + f\"_").front());
	CPPUNIT_ASSERT_EQUAL_MESSAGE("pattern a(_, _'d + e'_)", 0, (int)pkb.patternMatchAssign("_\"d + e\"_").size());
	CPPUNIT_ASSERT_EQUAL_MESSAGE("pattern a(_, _'1'_)", 1, pkb.patternMatchAssign("_\"1\"_").front());
	CPPUNIT_ASSERT_EQUAL_MESSAGE("pattern a(_, _)", 9, (int)pkb.patternMatchAssign("_").size());
	CPPUNIT_ASSERT_EQUAL_MESSAGE("pattern a(_, 'a + f')", 6, (int)pkb.patternMatchAssign("a + f").front());
	CPPUNIT_ASSERT_EQUAL_MESSAGE("pattern a(_, '3')", 11, pkb.patternMatchAssign("3").front());
	CPPUNIT_ASSERT_EQUAL_MESSAGE("pattern a(_, 'd')", 0, (int)pkb.patternMatchAssign("d").size());

	// Pattern for while
	CPPUNIT_ASSERT_EQUAL_MESSAGE("pattern w('b', _)", 1, (int)pkb.patternMatchWhile("b").size());
	CPPUNIT_ASSERT_EQUAL_MESSAGE("pattern w('b', _)", 7, pkb.patternMatchWhile("b").front());
	CPPUNIT_ASSERT_EQUAL_MESSAGE("pattern w('c', _)", 8, pkb.patternMatchWhile("  c  ").front());
	CPPUNIT_ASSERT_EQUAL_MESSAGE("pattern w('unusedvar', _)", 0, (int)pkb.patternMatchWhile("unusedvar").size());

	CPPUNIT_ASSERT_EQUAL_MESSAGE("pattern w(v, _) given stmtnum 3", pkb.getVarIndex("a"), pkb.getControlVariable(3));
	CPPUNIT_ASSERT_EQUAL_MESSAGE("pattern w(v, _) given stmtnum 8", pkb.getVarIndex("c"), pkb.getControlVariable(8));
	CPPUNIT_ASSERT_EQUAL_MESSAGE("pattern w(v, _) given stmtnum which is not a while loop", -1, pkb.getControlVariable(5));
	CPPUNIT_ASSERT_EQUAL_MESSAGE("pattern w(v, _) given invalid stmtnum", -1, pkb.getControlVariable(8000));
	CPPUNIT_ASSERT_EQUAL_MESSAGE("pattern w(v, _) given invalid stmtnum", -1, pkb.getControlVariable(0));

	// All pairs for Follows
	pair<vector<int>, vector<int>> allFollows = pkb.getAllFollowsPairs(false);
	CPPUNIT_ASSERT_EQUAL_MESSAGE("All Follows pairs size", 8, (int)allFollows.first.size());
	allFollows = pkb.getAllFollowsPairs(true);
	CPPUNIT_ASSERT_EQUAL_MESSAGE("All Follows* pairs size", 29, (int)allFollows.first.size());

	// All pairs for parent
	pair<vector<int>, vector<int>> allParent = pkb.getAllParentPairs(false);
	CPPUNIT_ASSERT_EQUAL_MESSAGE("All Parent pairs size", 4, (int)allParent.first.size());
	allParent = pkb.getAllParentPairs(true);
	CPPUNIT_ASSERT_EQUAL_MESSAGE("All Parent* pairs size", 5, (int)allParent.first.size());
}