#include <cppunit/config/SourcePrefix.h>
#include <vector>
#include <algorithm>
#include "TestPKB.h"
#include "SourceParser.h"
#include "FrontEndController.h"
#include "PKB.h"
#include "ConstantTable.h"
#include "PatternMatch.h"
#include "Contain.h"
#include "Siblings.h"

#include "ExpressionParser.h"

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
	cout << "TestPKB" << endl;
	FrontEndController::constructPkb("test/i_src.txt");

	PKB pkb = PKB::getInstance();
	
	// ProcTable
	cout << "Proc Table" << endl;
	CPPUNIT_ASSERT_EQUAL_MESSAGE("Number of procedures", 5, pkb.getProcTableSize());

	// ConstantTable
	cout << "Constant Table" << endl;
	CPPUNIT_ASSERT_EQUAL_MESSAGE("Number of constants", 3, pkb.getConstantTableSize());

	// VarTable
	cout << "Var Table" << endl;
	CPPUNIT_ASSERT_EQUAL_MESSAGE("Number of variables", 6, pkb.getVarTableSize());
	CPPUNIT_ASSERT(pkb.getVarIndex("f"));
	CPPUNIT_ASSERT_EQUAL_MESSAGE("Get invalid variable", -1, pkb.getVarIndex("z"));

	// statements
	CPPUNIT_ASSERT_EQUAL_MESSAGE("Number of statements", 38, (int)pkb.getStmtNumForType(STMT).size());
	CPPUNIT_ASSERT_EQUAL_MESSAGE("Number of assign", 20, (int)pkb.getStmtNumForType(ASSIGN).size());
	CPPUNIT_ASSERT_EQUAL_MESSAGE("Number of if", 5, (int)pkb.getStmtNumForType(IF).size());
	CPPUNIT_ASSERT_EQUAL_MESSAGE("Number of while", 7, (int)pkb.getStmtNumForType(WHILE).size());
	CPPUNIT_ASSERT_EQUAL_MESSAGE("Number of call", 6, (int)pkb.getStmtNumForType(CALL).size());

	CPPUNIT_ASSERT_EQUAL_MESSAGE("Number of plus", 10, (int)pkb.getStmtNumForType(PLUS).size());
	CPPUNIT_ASSERT_EQUAL_MESSAGE("Number of minus", 1, (int)pkb.getStmtNumForType(MINUS).size());
	CPPUNIT_ASSERT_EQUAL_MESSAGE("Number of times", 3, (int)pkb.getStmtNumForType(TIMES).size());
	CPPUNIT_ASSERT_EQUAL_MESSAGE("Number of STMTLST", 22, (int)pkb.getStmtNumForType(STMTLST).size());

	vector<int> stmtList = pkb.getStmtNumForType(STMTLST);
	CPPUNIT_ASSERT_MESSAGE("STMTLST contains 1, start of proc", find(stmtList.begin(), stmtList.end(), 1) != stmtList.end());
	CPPUNIT_ASSERT_MESSAGE("STMTLST contains 8, first in while loop", find(stmtList.begin(), stmtList.end(), 8) != stmtList.end());
	CPPUNIT_ASSERT_MESSAGE("STMTLST contains 27, in if", find(stmtList.begin(), stmtList.end(), 27) != stmtList.end());
	CPPUNIT_ASSERT_MESSAGE("STMTLST contains 28, in else", find(stmtList.begin(), stmtList.end(), 28) != stmtList.end());

	// Test Design Extractor

	// Parent
	cout << "Parent" << endl;
	CPPUNIT_ASSERT_EQUAL_MESSAGE("Parent(s, 4)", 3, pkb.getParent(4).front());
	CPPUNIT_ASSERT_EQUAL_MESSAGE("Parent(s, 8)", 7, pkb.getParent(8).front());
	CPPUNIT_ASSERT_EQUAL_MESSAGE("Parent(s, 11)", 0, (int)pkb.getParent(11).size());
	CPPUNIT_ASSERT_EQUAL_MESSAGE("Parent(s, 14)", 13, pkb.getParent(14).front());
	CPPUNIT_ASSERT_EQUAL_MESSAGE("Parent(s, 15)", 13, pkb.getParent(15).front());

	CPPUNIT_ASSERT_EQUAL_MESSAGE("Parent(8, s)", 9, pkb.getChild(8).front());
	CPPUNIT_ASSERT_EQUAL_MESSAGE("Parent(7, s)", 8, pkb.getChild(7).front());
	CPPUNIT_ASSERT_EQUAL_MESSAGE("Parent(5, s)", 0, (int)pkb.getChild(5).size());
	CPPUNIT_ASSERT_EQUAL_MESSAGE("Parent(13, s)", 3, (int)pkb.getChild(13).size());

	CPPUNIT_ASSERT_EQUAL_MESSAGE("Parent(7, 9)", false, pkb.isParent(7,9));
	CPPUNIT_ASSERT_EQUAL_MESSAGE("Parent(8, 9)", true, pkb.isParent(8,9));
	CPPUNIT_ASSERT_EQUAL_MESSAGE("Parent(1, 2)", false, pkb.isParent(1,2));
	CPPUNIT_ASSERT_EQUAL_MESSAGE("Parent(1, 2)", false, pkb.isParent(1,2));
	CPPUNIT_ASSERT_EQUAL_MESSAGE("Parent(0, 1)", false, pkb.isParent(0,1));
	CPPUNIT_ASSERT_EQUAL_MESSAGE("Parent(21, 25)", false, pkb.isParent(21, 25));
	CPPUNIT_ASSERT_EQUAL_MESSAGE("Parent(13, 15)", true, pkb.isParent(13,15));
	CPPUNIT_ASSERT_EQUAL_MESSAGE("Parent(19, 20)", true, pkb.isParent(19,20));
	CPPUNIT_ASSERT_EQUAL_MESSAGE("Parent(9999, 10000)", false, pkb.isParent(9999,10000));
	

	//Parent*
	cout << "Parent*" << endl;
	CPPUNIT_ASSERT_EQUAL_MESSAGE("Parent*(s, 4)", 3, pkb.getParentS(4).front());
	CPPUNIT_ASSERT_EQUAL_MESSAGE("Parent*(s, 9)", 2, (int)pkb.getParentS(9).size());
	CPPUNIT_ASSERT_EQUAL_MESSAGE("Parent*(s, 10)", 1, (int)pkb.getParentS(10).size());
	CPPUNIT_ASSERT_EQUAL_MESSAGE("Parent*(s, 11)", 0, (int)pkb.getParentS(11).size());
	CPPUNIT_ASSERT_EQUAL_MESSAGE("Parent*(s, 13)", 1, (int)pkb.getParentS(13).size());
	CPPUNIT_ASSERT_EQUAL_MESSAGE("Parent*(s, 14)", 2, (int)pkb.getParentS(14).size());

	CPPUNIT_ASSERT_EQUAL_MESSAGE("Parent*(7, s)", 3, (int)pkb.getChildS(7).size());
	CPPUNIT_ASSERT_EQUAL_MESSAGE("Parent*(8, s)", 1, (int)pkb.getChildS(8).size());
	CPPUNIT_ASSERT_EQUAL_MESSAGE("Parent*(9, s)", 0, (int)pkb.getChildS(9).size());
	CPPUNIT_ASSERT_EQUAL_MESSAGE("Parent*(12, s)", 5, (int)pkb.getChildS(12).size());
	CPPUNIT_ASSERT_EQUAL_MESSAGE("Parent*(13, s)", 3, (int)pkb.getChildS(13).size());

	CPPUNIT_ASSERT_EQUAL_MESSAGE("Parent*(0, 1)", false, pkb.isParentS(0, 1));
	CPPUNIT_ASSERT_EQUAL_MESSAGE("Parent*(1, 2)", false, pkb.isParentS(1, 2));
	CPPUNIT_ASSERT_EQUAL_MESSAGE("Parent*(3, 4)", true, pkb.isParentS(3, 4));
	CPPUNIT_ASSERT_EQUAL_MESSAGE("Parent*(7, 8)", true, pkb.isParentS(7, 8));
	CPPUNIT_ASSERT_EQUAL_MESSAGE("Parent*(7, 9)", true, pkb.isParentS(7, 9));
	CPPUNIT_ASSERT_EQUAL_MESSAGE("Parent*(12, 13)", true, pkb.isParentS(12, 13));
	CPPUNIT_ASSERT_EQUAL_MESSAGE("Parent*(19, 20)", true, pkb.isParentS(19, 20));
	CPPUNIT_ASSERT_EQUAL_MESSAGE("Parent*(13, 16)", true, pkb.isParentS(13, 16));
	CPPUNIT_ASSERT_EQUAL_MESSAGE("Parent*(12, 14)", true, pkb.isParentS(12, 14));
	CPPUNIT_ASSERT_EQUAL_MESSAGE("Parent*(19, 21)", true, pkb.isParentS(19, 21));
	CPPUNIT_ASSERT_EQUAL_MESSAGE("Parent*(19, 23)", true, pkb.isParentS(19, 23));
	
	CPPUNIT_ASSERT_EQUAL_MESSAGE("Parent(_, )", 12, (int)pkb.getParentLhs().size());
	CPPUNIT_ASSERT_EQUAL_MESSAGE("Parent(, _)", 23, (int)pkb.getParentRhs().size());

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
	CPPUNIT_ASSERT_EQUAL_MESSAGE("Follows(18, 19)", true, pkb.isFollows(18,19));
	CPPUNIT_ASSERT_EQUAL_MESSAGE("Follows(s, 6)", 5, pkb.getStmtFollowedTo(6).front());
	CPPUNIT_ASSERT_EQUAL_MESSAGE("Follows(s, 10)", 8, pkb.getStmtFollowedTo(10).front());
	CPPUNIT_ASSERT_EQUAL_MESSAGE("Follows(s, 1)", 0, (int)pkb.getStmtFollowedTo(1).size());
	CPPUNIT_ASSERT_EQUAL_MESSAGE("Follows(s, 12)", 11, pkb.getStmtFollowedTo(12).front());
	CPPUNIT_ASSERT_EQUAL_MESSAGE("Follows(s, 13)", 0, (int)pkb.getStmtFollowedTo(13).size());
	CPPUNIT_ASSERT_EQUAL_MESSAGE("Follows(6, s)", 7, pkb.getStmtFollowedFrom(6).front());
	CPPUNIT_ASSERT_EQUAL_MESSAGE("Follows(7, s)", 11, pkb.getStmtFollowedFrom(7).front());
	CPPUNIT_ASSERT_EQUAL_MESSAGE("Follows(8, s)", 10, pkb.getStmtFollowedFrom(8).front());
	CPPUNIT_ASSERT_EQUAL_MESSAGE("Follows(13, s)", 0, (int)pkb.getStmtFollowedFrom(13).size());
	CPPUNIT_ASSERT_EQUAL_MESSAGE("Follows(14, s)", 0, (int)pkb.getStmtFollowedFrom(14).size());
	CPPUNIT_ASSERT_EQUAL_MESSAGE("Follows(18, s)", 1, (int)pkb.getStmtFollowedFrom(18).size());
	CPPUNIT_ASSERT_EQUAL_MESSAGE("Follows(25, s)", 0, (int)pkb.getStmtFollowedFrom(25).size());
	CPPUNIT_ASSERT_EQUAL_MESSAGE("Follows(30, s)", 0, (int)pkb.getStmtFollowedFrom(30).size());
	CPPUNIT_ASSERT_EQUAL_MESSAGE("Follows(9999, 10000)", false, pkb.isFollows(9999,10000));


	CPPUNIT_ASSERT_EQUAL_MESSAGE("Follows(_, )", 16, (int)pkb.getFollowsLhs().size());
	CPPUNIT_ASSERT_EQUAL_MESSAGE("Follows(, _)", 16, (int)pkb.getFollowsRhs().size());

	// Follows*
	cout << "Follows*" << endl;
	CPPUNIT_ASSERT_EQUAL_MESSAGE("Follows*(s, 1)", 0, (int)pkb.getStmtFollowedToS(1).size());
	CPPUNIT_ASSERT_EQUAL_MESSAGE("Follows*(s, 2)", 1, (int)pkb.getStmtFollowedToS(2).size());
	CPPUNIT_ASSERT_EQUAL_MESSAGE("Follows*(s, 4)", 0, (int)pkb.getStmtFollowedToS(4).size());
	CPPUNIT_ASSERT_EQUAL_MESSAGE("Follows*(s, 10)", 1, (int)pkb.getStmtFollowedToS(10).size());
	CPPUNIT_ASSERT_EQUAL_MESSAGE("Follows*(s, 12)", 7, (int)pkb.getStmtFollowedToS(12).size());

	CPPUNIT_ASSERT_EQUAL_MESSAGE("Follows*(0, s)", 0, (int)pkb.getStmtFollowedFromS(0).size());
	CPPUNIT_ASSERT_EQUAL_MESSAGE("Follows*(1, s)", 7, (int)pkb.getStmtFollowedFromS(1).size());
	CPPUNIT_ASSERT_EQUAL_MESSAGE("Follows*(4, s)", 0, (int)pkb.getStmtFollowedFromS(4).size());
	CPPUNIT_ASSERT_EQUAL_MESSAGE("Follows*(8, s)", 1, (int)pkb.getStmtFollowedFromS(8).size());
	CPPUNIT_ASSERT_EQUAL_MESSAGE("Follows*(13, s)", 0, (int)pkb.getStmtFollowedFromS(13).size());

	CPPUNIT_ASSERT_EQUAL_MESSAGE("Follows*(13, 14)", false, pkb.isFollowsS(13, 14));
	CPPUNIT_ASSERT_EQUAL_MESSAGE("Follows*(2, 3)", true, pkb.isFollowsS(2, 3));
	CPPUNIT_ASSERT_EQUAL_MESSAGE("Follows*(3, 4)", false, pkb.isFollowsS(3, 4));
	CPPUNIT_ASSERT_EQUAL_MESSAGE("Follows*(7, 11)", true, pkb.isFollowsS(7, 11));
	CPPUNIT_ASSERT_EQUAL_MESSAGE("Follows*(7, 12)", true, pkb.isFollowsS(7, 12));
	CPPUNIT_ASSERT_EQUAL_MESSAGE("Follows*(7, 13)", false, pkb.isFollowsS(7, 13));
	CPPUNIT_ASSERT_EQUAL_MESSAGE("Follows*(12, 7)", false, pkb.isFollowsS(12, 7));
	CPPUNIT_ASSERT_EQUAL_MESSAGE("Follows*(1, 12)", true, pkb.isFollowsS(1, 12));
	CPPUNIT_ASSERT_EQUAL_MESSAGE("Follows*(0, 12)", false, pkb.isFollowsS(0, 12));
	CPPUNIT_ASSERT_EQUAL_MESSAGE("Follows*(13, 14)", false, pkb.isFollowsS(13, 14));
	CPPUNIT_ASSERT_EQUAL_MESSAGE("Follows*(14, 15)", false, pkb.isFollowsS(14, 15));
	CPPUNIT_ASSERT_EQUAL_MESSAGE("Follows*(18, 19)", true, pkb.isFollowsS(18, 19));
	CPPUNIT_ASSERT_EQUAL_MESSAGE("Follows*(18, 20)", false, pkb.isFollowsS(18, 20));
	CPPUNIT_ASSERT_EQUAL_MESSAGE("Follows*(17, 21)", false, pkb.isFollowsS(17, 21));

	// Uses
	cout << "Uses" << endl;

	CPPUNIT_ASSERT_EQUAL_MESSAGE("Uses(2, 'd')", true, pkb.isUses(2, pkb.getVarIndex("d")));
	CPPUNIT_ASSERT_EQUAL_MESSAGE("Uses(2, 'f')", true, pkb.isUses(2, pkb.getVarIndex("f")));
	CPPUNIT_ASSERT_EQUAL_MESSAGE("Uses(12, 'a')", true, pkb.isUses(12, pkb.getVarIndex("a")));
	CPPUNIT_ASSERT_EQUAL_MESSAGE("Uses(12, 'b')", true, pkb.isUses(12, pkb.getVarIndex("b")));
	CPPUNIT_ASSERT_EQUAL_MESSAGE("Uses(15, 'a')", true, pkb.isUses(15, pkb.getVarIndex("a")));
	CPPUNIT_ASSERT_EQUAL_MESSAGE("Uses(2, 'a')", false, pkb.isUses(2, pkb.getVarIndex("a")));
	CPPUNIT_ASSERT_EQUAL_MESSAGE("Uses(17, 'c')", true, pkb.isUses(17, pkb.getVarIndex("c")));
	CPPUNIT_ASSERT_EQUAL_MESSAGE("Uses(18, 'a')", true, pkb.isUses(18, pkb.getVarIndex("a")));
	CPPUNIT_ASSERT_EQUAL_MESSAGE("Uses(18, 'c')", true, pkb.isUses(18, pkb.getVarIndex("c")));
	CPPUNIT_ASSERT_EQUAL_MESSAGE("Uses(26, 'a')", true, pkb.isUses(26, pkb.getVarIndex("a")));
	CPPUNIT_ASSERT_EQUAL_MESSAGE("Uses(26, 'c')", true, pkb.isUses(26, pkb.getVarIndex("c")));
	CPPUNIT_ASSERT_EQUAL_MESSAGE("Uses(26, 'd')", false, pkb.isUses(26, pkb.getVarIndex("d")));
	CPPUNIT_ASSERT_EQUAL_MESSAGE("Uses(3, v)", (string)"a", (string)pkb.getVarName(pkb.getUsesVarForStmt(3).front())); 
	CPPUNIT_ASSERT_EQUAL_MESSAGE("Uses(7, v)", 4, (int)pkb.getUsesVarForStmt(7).size());
	CPPUNIT_ASSERT_EQUAL_MESSAGE("Uses(11, v)", 3, (int)pkb.getUsesVarForStmt(11).size()); 
	CPPUNIT_ASSERT_EQUAL_MESSAGE("Uses(1, v)", 0, (int)pkb.getUsesVarForStmt(1).size()); 
	CPPUNIT_ASSERT_EQUAL_MESSAGE("Uses(s, 'c')", 19, (int)pkb.getUsesStmtNum(pkb.getVarIndex("c")).size());
	CPPUNIT_ASSERT_EQUAL_MESSAGE("Uses(s, 'f')", 15, (int)pkb.getUsesStmtNum(pkb.getVarIndex("f")).size());
	CPPUNIT_ASSERT_EQUAL_MESSAGE("Uses(1, _)", 0, (int)pkb.getUsesVarForStmt(1).size());
	CPPUNIT_ASSERT_EQUAL_MESSAGE("Uses(20, _)", 5, (int)pkb.getUsesVarForStmt(20).size());
	CPPUNIT_ASSERT_EQUAL_MESSAGE("Uses(26, _)", 3, (int)pkb.getUsesVarForStmt(26).size());
	

	CPPUNIT_ASSERT_EQUAL_MESSAGE("Uses(_, )", 30, (int)pkb.getUsesLhs().size());
	CPPUNIT_ASSERT_EQUAL_MESSAGE("Uses(, _)", 6, (int)pkb.getUsesRhs().size());

	CPPUNIT_ASSERT_EQUAL_MESSAGE("Uses for procIndex 0", 6, (int)pkb.getUsesVarForProc(0).size());
	CPPUNIT_ASSERT_EQUAL_MESSAGE("Uses for procIndex 1", 5, (int)pkb.getUsesVarForProc(1).size());
	CPPUNIT_ASSERT_EQUAL_MESSAGE("Uses for procIndex 2", 5, (int)pkb.getUsesVarForProc(2).size());
	CPPUNIT_ASSERT_EQUAL_MESSAGE("Uses for procIndex 3", 3, (int)pkb.getUsesVarForProc(3).size());
	CPPUNIT_ASSERT_EQUAL_MESSAGE("Uses for procIndex 100", 0, (int)pkb.getUsesVarForProc(100).size());
	CPPUNIT_ASSERT_EQUAL_MESSAGE("Uses(p, 'a')", 5, (int)pkb.getUsesProcIndex(pkb.getVarIndex("a")).size());
	CPPUNIT_ASSERT_EQUAL_MESSAGE("Uses(p, 'c')", 5, (int)pkb.getUsesProcIndex(pkb.getVarIndex("c")).size());
	CPPUNIT_ASSERT_EQUAL_MESSAGE("Uses(p, 'd')", 3, (int)pkb.getUsesProcIndex(pkb.getVarIndex("d")).size());

	CPPUNIT_ASSERT_EQUAL_MESSAGE("Uses(_, _)", true, pkb.isUsesValid());

	// Modifies
	cout << "Modifies" << endl;
	CPPUNIT_ASSERT_EQUAL_MESSAGE("Modifies(1, 'a')", true, pkb.isModifies(1, pkb.getVarIndex("a")));
	CPPUNIT_ASSERT_EQUAL_MESSAGE("Modifies(5, 'e')", false, pkb.isModifies(5, pkb.getVarIndex("e")));
	CPPUNIT_ASSERT_EQUAL_MESSAGE("Modifies(7, 'b')", false, pkb.isModifies(7, pkb.getVarIndex("b")));
	CPPUNIT_ASSERT_EQUAL_MESSAGE("Modifies(7, 'f')", true, pkb.isModifies(7, pkb.getVarIndex("f")));
	CPPUNIT_ASSERT_EQUAL_MESSAGE("Modifies(15, 'a')", true, pkb.isModifies(15, pkb.getVarIndex("a")));
	CPPUNIT_ASSERT_EQUAL_MESSAGE("Modifies(17, 'a')", true, pkb.isModifies(17, pkb.getVarIndex("a")));
	CPPUNIT_ASSERT_EQUAL_MESSAGE("Modifies(21, 'b')", false, pkb.isModifies(21, pkb.getVarIndex("b")));
	CPPUNIT_ASSERT_EQUAL_MESSAGE("Modifies(21, 'a')", true, pkb.isModifies(21, pkb.getVarIndex("a")));
	CPPUNIT_ASSERT_EQUAL_MESSAGE("Modifies(30, 'b')", false, pkb.isModifies(30, pkb.getVarIndex("b")));
	CPPUNIT_ASSERT_EQUAL_MESSAGE("Modifies(30, 'a')", true, pkb.isModifies(30, pkb.getVarIndex("a")));
	CPPUNIT_ASSERT_EQUAL_MESSAGE("Modifies(s, 'd')", 5, pkb.getModStmtNum(pkb.getVarIndex("d")).front());
	CPPUNIT_ASSERT_EQUAL_MESSAGE("Modifies(s, 'b')", 13, (int)pkb.getModStmtNum(pkb.getVarIndex("b")).size());
	CPPUNIT_ASSERT_EQUAL_MESSAGE("Modifies(7, v)", 2, (int)pkb.getModVarForStmt(7).size());
	CPPUNIT_ASSERT_EQUAL_MESSAGE("Modifies(8, v)", (string)"f", pkb.getVarName(pkb.getModVarForStmt(8).front()));
	CPPUNIT_ASSERT_EQUAL_MESSAGE("Modifies(11, v)", (string)"a", pkb.getVarName(pkb.getModVarForStmt(11).front()));

	CPPUNIT_ASSERT_EQUAL_MESSAGE("Modifies(_, )", 38 , (int)pkb.getModifiesLhs().size());
	CPPUNIT_ASSERT_EQUAL_MESSAGE("Modifies(, _)", 6, (int)pkb.getModifiesRhs().size());


	CPPUNIT_ASSERT_EQUAL_MESSAGE("Modifies for proc index 0", 6, (int)pkb.getModVarForProc(0).size());
	CPPUNIT_ASSERT_EQUAL_MESSAGE("Modifies for proc index 1", 3, (int)pkb.getModVarForProc(1).size());
	CPPUNIT_ASSERT_EQUAL_MESSAGE("Modifies(p, 'a')", 5, (int)pkb.getModProcIndex(pkb.getVarIndex("a")).size());

	// Pattern
	cout << "Pattern" << endl;
	CPPUNIT_ASSERT_EQUAL_MESSAGE("pattern a(_, _'f'_)", 5, (int)pkb.patternMatchAssign("_\"f\"_").size());
	CPPUNIT_ASSERT_EQUAL_MESSAGE("pattern a(_, _'a+f'_)", 6, pkb.patternMatchAssign("_\"a+f\"_").front());
	CPPUNIT_ASSERT_EQUAL_MESSAGE("pattern a(_, _'a + f'_)", 6, pkb.patternMatchAssign("_\"a + f\"_").front());
	CPPUNIT_ASSERT_EQUAL_MESSAGE("pattern a(_, _'d + e'_)", 0, (int)pkb.patternMatchAssign("_\"d + e\"_").size());
	CPPUNIT_ASSERT_EQUAL_MESSAGE("pattern a(_, _'1'_)", 1, pkb.patternMatchAssign("_\"1\"_").front());
	CPPUNIT_ASSERT_EQUAL_MESSAGE("pattern a(_, _)", 20, (int)pkb.patternMatchAssign("_").size());
	CPPUNIT_ASSERT_EQUAL_MESSAGE("pattern a(_, 'a + f')", 6, (int)pkb.patternMatchAssign("a + f").front());
	CPPUNIT_ASSERT_EQUAL_MESSAGE("pattern a(_, '2')", 4, pkb.patternMatchAssign("2").front());
	CPPUNIT_ASSERT_EQUAL_MESSAGE("pattern a(_, '2')", 3, (int)pkb.patternMatchAssign("2").size());
	CPPUNIT_ASSERT_EQUAL_MESSAGE("pattern a(_, 'd')", 0, (int)pkb.patternMatchAssign("d").size());

	CPPUNIT_ASSERT_EQUAL_MESSAGE("pattern a(_, _'e + (3 * b)'_)", 11, pkb.patternMatchAssign("_\"e + (3 * b)\"_").front());
	CPPUNIT_ASSERT_EQUAL_MESSAGE("pattern a(_, _'e + 3 * b'_)", 11, pkb.patternMatchAssign("_\"e + 3 * b\"_").front());
	CPPUNIT_ASSERT_EQUAL_MESSAGE("pattern a(_, _'e + (3 * b) + (4 * f) * 3 - f'_)", 0, (int)pkb.patternMatchAssign("_\"e + (3 * b) + (4 * f) * 3 - f\"_").size());
	CPPUNIT_ASSERT_EQUAL_MESSAGE("pattern a(_, _'e + (3 * (2 + f))'_)", 0, (int)pkb.patternMatchAssign("_\"e + (3 * (2 + f))\"_").size());
	CPPUNIT_ASSERT_EQUAL_MESSAGE("pattern a(_, _'3 * b'_)", 11, pkb.patternMatchAssign("_\"3 * b\"_").front());
	CPPUNIT_ASSERT_EQUAL_MESSAGE("pattern a(_, _'e + 1'_)", 9, pkb.patternMatchAssign("_\"e + 1\"_").front());
	
	CPPUNIT_ASSERT_EQUAL_MESSAGE("pattern a(_, _'u + (4 * z)'_) - Variable Dont Exist", 0, (int)pkb.patternMatchAssign("_\"u + (4 * z)\"_").size());

	CPPUNIT_ASSERT_EQUAL_MESSAGE("pattern a(_, '')", 20, (int)pkb.patternMatchAssign("\"\"").size());
	// Query Validator will check
	CPPUNIT_ASSERT_EQUAL_MESSAGE("pattern a(_, _''_)", 20, (int)pkb.patternMatchAssign("_\"\"_").size());

	// PatternMatch
	cout << "new pattern design" << endl;
	const char* args[] = {"d", "+", "f"};
	vector<string> argVector(args, args + 3);
	ExpressionParser exprParser;
	TNode* top = exprParser.parseExpressionForQuerying(argVector);
	CPPUNIT_ASSERT_EQUAL_MESSAGE("ok", Plus, top->getNodeType());
	PatternMatch pattern;
	CPPUNIT_ASSERT_EQUAL_MESSAGE("this", 2, pattern.patternMatchAssign(top, true, "d").at(0));

	const char* args2[] = {"1"};
	vector<string> argVector2(args2, args2 + 1);
	TNode* top2 = exprParser.parseExpressionForQuerying(argVector2);

	CPPUNIT_ASSERT_EQUAL_MESSAGE("this", 9, pattern.patternMatchAssign(top2, false, "1").at(2));

	const char* args3[] = {"e", "+", "3", "*", "b", "+", "f"};
	vector<string> argVector3(args3, args3 + 7);
	TNode* top3 = exprParser.parseExpressionForQuerying(argVector3);
	// full pattern match for e+3*b+f
	CPPUNIT_ASSERT_EQUAL_MESSAGE("this", 11, pattern.patternMatchAssign(top3, false, "e").at(0));

	const char* args4[] = {"3", "*", "b"};
	vector<string> argVector4(args4, args4 + 3);
	TNode* top4 = exprParser.parseExpressionForQuerying(argVector4);
	// partial match from 3 * b
	CPPUNIT_ASSERT_EQUAL_MESSAGE("this", 1, (int)pattern.patternMatchAssign(top4, false, "3").size());

	// Pattern for while
	cout << "Pattern for while" << endl;
	CPPUNIT_ASSERT_EQUAL_MESSAGE("pattern w('b', _)", 1, (int)pkb.patternMatchWhile("b").size());
	CPPUNIT_ASSERT_EQUAL_MESSAGE("pattern w('b', _)", 7, pkb.patternMatchWhile("b").front());
	CPPUNIT_ASSERT_EQUAL_MESSAGE("pattern w('c', _)", 8, pkb.patternMatchWhile("  c  ").front());
	CPPUNIT_ASSERT_EQUAL_MESSAGE("pattern w('unusedvar', _)", 0, (int)pkb.patternMatchWhile("unusedvar").size());

	CPPUNIT_ASSERT_EQUAL_MESSAGE("pattern w(v, _) given stmtnum 3", pkb.getVarIndex("a"), pkb.getControlVariable(3));
	CPPUNIT_ASSERT_EQUAL_MESSAGE("pattern w(v, _) given stmtnum 8", pkb.getVarIndex("c"), pkb.getControlVariable(8));
	CPPUNIT_ASSERT_EQUAL_MESSAGE("pattern w(v, _) given stmtnum which is not a while loop", -1, pkb.getControlVariable(5));
	CPPUNIT_ASSERT_EQUAL_MESSAGE("pattern w(v, _) given invalid stmtnum", -1, pkb.getControlVariable(8000));
	CPPUNIT_ASSERT_EQUAL_MESSAGE("pattern w(v, _) given invalid stmtnum", -1, pkb.getControlVariable(0));

	// Pattern for if 
	cout << "pattern for if" << endl;
	CPPUNIT_ASSERT_EQUAL_MESSAGE("pattern ifstat('b', _, _)", 3, (int)pkb.patternMatchIf(" b ").size());
	CPPUNIT_ASSERT_EQUAL_MESSAGE("pattern ifstat('c', _, _)", 13, pkb.patternMatchIf("c").front());
	CPPUNIT_ASSERT_EQUAL_MESSAGE("pattern ifstat('unusedvar', _, _)", 0, (int)pkb.patternMatchIf("unusedvar").size());

	CPPUNIT_ASSERT_EQUAL_MESSAGE("pattern ifstat(v, _, _) given stmtnum 12", pkb.getVarIndex("b"), pkb.getControlVariable(12));
	CPPUNIT_ASSERT_EQUAL_MESSAGE("pattern ifstat(v, _, _) given stmtnum 8", pkb.getVarIndex("c"), pkb.getControlVariable(13));
	CPPUNIT_ASSERT_EQUAL_MESSAGE("pattern ifstat(v, _, _) given stmtnum which is not a if statement", -1, pkb.getControlVariable(14));

	// All pairs for Follows
	pair<vector<int>, vector<int>> allFollows = pkb.getAllFollowsPairs(false);
	CPPUNIT_ASSERT_EQUAL_MESSAGE("All Follows pairs size", 16, (int)allFollows.first.size());
	allFollows = pkb.getAllFollowsPairs(true);
	CPPUNIT_ASSERT_EQUAL_MESSAGE("All Follows* pairs size", 34, (int)allFollows.first.size());

	CPPUNIT_ASSERT_EQUAL_MESSAGE("Follows(_, _)", true, pkb.isFollowsValid());

	// All pairs for parent
	pair<vector<int>, vector<int>> allParent = pkb.getAllParentPairs(false);
	CPPUNIT_ASSERT_EQUAL_MESSAGE("All Parent pairs size", 20, (int)allParent.first.size());

	allParent = pkb.getAllParentPairs(true);
	CPPUNIT_ASSERT_EQUAL_MESSAGE("All Parent* pairs size", 39, (int)allParent.first.size());
	CPPUNIT_ASSERT_EQUAL_MESSAGE("Parent(_, _)", true, pkb.isParentValid());


	// Calls Table
	cout << "Calls table" << endl;
	CPPUNIT_ASSERT_EQUAL_MESSAGE("calls(0, p)", 2, (int)pkb.getProcsCalledBy(0).size());
	CPPUNIT_ASSERT_EQUAL_MESSAGE("calls*(0, p)", 4, (int)pkb.getProcsCalledByS(0).size());
	CPPUNIT_ASSERT_EQUAL_MESSAGE("calls(p, 2)", 2, (int)pkb.getProcsCalling(2).size());

	CPPUNIT_ASSERT_EQUAL_MESSAGE("calls(0, 3)", false, pkb.isCalls(0, 3));
	CPPUNIT_ASSERT_EQUAL_MESSAGE("calls(0, 3)", true, pkb.isCallsS(0, 3));

	CPPUNIT_ASSERT_EQUAL_MESSAGE("Calls(_, )", 3, (int)pkb.getCallsLhs().size());
	CPPUNIT_ASSERT_EQUAL_MESSAGE("Calls(, _)", 4, (int)pkb.getCallsRhs().size());

	CPPUNIT_ASSERT_EQUAL_MESSAGE("get procName of call statement", string("Test4"), pkb.getProcNameCalledByStatement(21));
	CPPUNIT_ASSERT_EQUAL_MESSAGE("get procName of call statement", string("Test3"), pkb.getProcNameCalledByStatement(18));
	CPPUNIT_ASSERT_EQUAL_MESSAGE("get procName of call statement", string("Test2"), pkb.getProcNameCalledByStatement(16));

	CPPUNIT_ASSERT_EQUAL_MESSAGE("Calls(_, _)", true, pkb.isCallsValid());
	
	// ProcTable
	cout << "Proc table" << endl;
	CPPUNIT_ASSERT_EQUAL_MESSAGE("proc name", string("Test"), pkb.getProcName(0));
	CPPUNIT_ASSERT_EQUAL_MESSAGE("proc name", string("Test2"), pkb.getProcName(1));

	// NextTable
	cout << "Next" << endl;
	CPPUNIT_ASSERT_EQUAL_MESSAGE("Next(1,2)", true, pkb.isNext(1, 2));
	CPPUNIT_ASSERT_EQUAL_MESSAGE("Next(3,4), test that the stmt in while is next after while", true, pkb.isNext(3, 4));
	CPPUNIT_ASSERT_EQUAL_MESSAGE("Next(3,5), test the the stmt following while is next after while", true, pkb.isNext(3, 5));
	CPPUNIT_ASSERT_EQUAL_MESSAGE("Next(1,4)", false, pkb.isNext(1, 4));
	CPPUNIT_ASSERT_EQUAL_MESSAGE("Next(7,8)", true, pkb.isNext(7, 8));
	CPPUNIT_ASSERT_EQUAL_MESSAGE("Next(11,12)", true, pkb.isNext(11, 12));
	CPPUNIT_ASSERT_EQUAL_MESSAGE("Next(13,14), stmt in if-then is Next after if", true, pkb.isNext(13, 14));
	CPPUNIT_ASSERT_EQUAL_MESSAGE("Next(13,15), stmt in if-else is Next after if", true, pkb.isNext(13, 15));

	CPPUNIT_ASSERT_EQUAL_MESSAGE("Next*(4,4), stmt in while loop is Next* of itself", true, pkb.isNextS(4, 4));
	CPPUNIT_ASSERT_EQUAL_MESSAGE("Next(4,4), stmt in while loop is not Next after itself", false, pkb.isNext(4, 4));
	CPPUNIT_ASSERT_EQUAL_MESSAGE("Next*(9,8), while stmt is Next after stmt in while loop ", true, pkb.isNextS(9, 8));
	CPPUNIT_ASSERT_EQUAL_MESSAGE("Next(9,8)", true, pkb.isNext(9, 8));
	CPPUNIT_ASSERT_EQUAL_MESSAGE("Next(10,8), two stmts in while loop", true, pkb.isNextS(10, 8));
	CPPUNIT_ASSERT_EQUAL_MESSAGE("Next(8,10)", true, pkb.isNext(8, 10));

	CPPUNIT_ASSERT_EQUAL_MESSAGE("Next(3, _)", 2, (int)pkb.getNextAfter(3).size());
	CPPUNIT_ASSERT_EQUAL_MESSAGE("Next(12, _)", 2, (int)pkb.getNextAfter(12).size());
	CPPUNIT_ASSERT_EQUAL_MESSAGE("Next(12, _)", 13, pkb.getNextAfter(12).front());
	CPPUNIT_ASSERT_EQUAL_MESSAGE("Next(_, 13)", 12, pkb.getNextBefore(13).front());
	CPPUNIT_ASSERT_EQUAL_MESSAGE("Next(_, 25)", 2, (int)pkb.getNextBefore(25).size());


	CPPUNIT_ASSERT_EQUAL_MESSAGE("Next*(4, _)", 15, (int)pkb.getNextAfterS(4).size());
	CPPUNIT_ASSERT_EQUAL_MESSAGE("Next*(1, _)", 16, (int)pkb.getNextAfterS(1).size());
	CPPUNIT_ASSERT_EQUAL_MESSAGE("Next*(_, 12)", 11, (int)pkb.getNextBeforeS(12).size());
	CPPUNIT_ASSERT_EQUAL_MESSAGE("Next*(_, 16)", 14, (int)pkb.getNextBeforeS(16).size());

	CPPUNIT_ASSERT_EQUAL_MESSAGE("Next*(18, _)", 6, (int)pkb.getNextAfterS(18).size());
	CPPUNIT_ASSERT_EQUAL_MESSAGE("Next*(_, 24)", 7, (int)pkb.getNextBeforeS(24).size());

	CPPUNIT_ASSERT_EQUAL_MESSAGE("Next(21, 19)", true, pkb.isNext(21, 19));
	CPPUNIT_ASSERT_EQUAL_MESSAGE("Next(21, 20)", true, pkb.isNextS(21, 20));

	CPPUNIT_ASSERT_EQUAL_MESSAGE("Next(27, 28)", false, pkb.isNext(27, 28));
	CPPUNIT_ASSERT_EQUAL_MESSAGE("Next*(27, 28)", true, pkb.isNextS(27, 28));
	CPPUNIT_ASSERT_EQUAL_MESSAGE("Next*(27, 29), proglines in if-then and if-else nested in while loop", true, pkb.isNextS(27, 29)); 

	CPPUNIT_ASSERT_EQUAL_MESSAGE("Next(26, _)", 2, (int)pkb.getNextAfter(26).size());
	CPPUNIT_ASSERT_EQUAL_MESSAGE("Next(29, _)", 1, (int)pkb.getNextAfter(29).size());
	CPPUNIT_ASSERT_EQUAL_MESSAGE("Next(24, 19), nested last stmt in stmtlst links back to while", true, pkb.isNext(24, 19));
	
	CPPUNIT_ASSERT_EQUAL_MESSAGE("Next(24, 25), lines in different procedures", false, pkb.isNextS(16, 17));
	CPPUNIT_ASSERT_EQUAL_MESSAGE("Next lhs", 33, (int)pkb.getNextLhs().size());
	CPPUNIT_ASSERT_EQUAL_MESSAGE("Next rhs", 34, (int)pkb.getNextRhs().size());

	CPPUNIT_ASSERT_EQUAL_MESSAGE("Next(_, _)", true, pkb.isNextValid());
	
	CPPUNIT_ASSERT_EQUAL_MESSAGE("Next(26, _) is valid", true, pkb.isNextLhsValid(26));
	CPPUNIT_ASSERT_EQUAL_MESSAGE("Next(27, _) is valid", true, pkb.isNextLhsValid(27));
	CPPUNIT_ASSERT_EQUAL_MESSAGE("Next(17, _) is valid", false, pkb.isNextLhsValid(17));
	CPPUNIT_ASSERT_EQUAL_MESSAGE("Next(16, _) is valid", false, pkb.isNextLhsValid(16));
	CPPUNIT_ASSERT_EQUAL_MESSAGE("Next(30, _) is valid", false, pkb.isNextLhsValid(30));
	
	CPPUNIT_ASSERT_EQUAL_MESSAGE("Next(_, 18) is valid", false, pkb.isNextRhsValid(18));
	CPPUNIT_ASSERT_EQUAL_MESSAGE("Next(_, 25) is valid", true, pkb.isNextRhsValid(25));
	CPPUNIT_ASSERT_EQUAL_MESSAGE("Next(_, 12) is valid", true, pkb.isNextRhsValid(12));
	CPPUNIT_ASSERT_EQUAL_MESSAGE("Next(_, 9) is valid", true, pkb.isNextRhsValid(9));

	// Affects... 
	cout << "Affects" << endl;
	CPPUNIT_ASSERT_EQUAL_MESSAGE("Affects(6, _)", 2, (int)pkb.getAffectedBy(6).size());
	CPPUNIT_ASSERT_EQUAL_MESSAGE("Affects(3, _): lhs is while loop", 0, (int)pkb.getAffectedBy(3).size());
	CPPUNIT_ASSERT_EQUAL_MESSAGE("Affects(12, _): lhs is if ", 0, (int)pkb.getAffectedBy(12).size());
	CPPUNIT_ASSERT_EQUAL_MESSAGE("Affects(17, _): lhs is call ", 0, (int)pkb.getAffectedBy(17).size());

	CPPUNIT_ASSERT_EQUAL_MESSAGE("Affects(9, _)", 2, (int)pkb.getAffectedBy(9).size());
	CPPUNIT_ASSERT_EQUAL_MESSAGE("Affects*(9, _)", 4, (int)pkb.getAffectedBy(9, true).size());

	CPPUNIT_ASSERT_EQUAL_MESSAGE("Affects(2, _)", 0, (int)pkb.getAffectedBy(2).size());
	CPPUNIT_ASSERT_EQUAL_MESSAGE("Affects(_, 3): lhs is while loop", 0, (int)pkb.getAffecting(3).size());
	CPPUNIT_ASSERT_EQUAL_MESSAGE("Affects(_, 12): lhs is if ", 0, (int)pkb.getAffecting(12).size());
	CPPUNIT_ASSERT_EQUAL_MESSAGE("Affects(), 17): lhs is call ", 0, (int)pkb.getAffecting(17).size());

	CPPUNIT_ASSERT_EQUAL_MESSAGE("Affects(4, _)", 1, (int)pkb.getAffectedBy(4).size());
	CPPUNIT_ASSERT_EQUAL_MESSAGE("Affects*(4, _)", 2, (int)pkb.getAffectedBy(4, true).size());

	CPPUNIT_ASSERT_EQUAL_MESSAGE("Affects(_, 2)", 0, (int)pkb.getAffecting(2).size());
	CPPUNIT_ASSERT_EQUAL_MESSAGE("Affects(_, 6)", 1, (int)pkb.getAffecting(6).size());
	CPPUNIT_ASSERT_EQUAL_MESSAGE("Affects(_, 11)", 4, (int)pkb.getAffecting(11).size());
	CPPUNIT_ASSERT_EQUAL_MESSAGE("Affects*(_, 11)", 5, (int)pkb.getAffecting(11, true).size());

	CPPUNIT_ASSERT_EQUAL_MESSAGE("Affects(27, 29), stmts in if-stmt nested in a while loop", true, pkb.isAffects(27, 29));
	CPPUNIT_ASSERT_EQUAL_MESSAGE("Affects(29, 27)", false, pkb.isAffects(29, 27));

	CPPUNIT_ASSERT_EQUAL_MESSAGE("Affects*(14, 15)", false, pkb.isAffects(14, 15, true));
	CPPUNIT_ASSERT_EQUAL_MESSAGE("Affects*(1, 11)", true, pkb.isAffects(1, 11, true));
	CPPUNIT_ASSERT_EQUAL_MESSAGE("Affects(9, 10), stmt in while loop can affect outside of loop", true, pkb.isAffects(9, 10, false));


	CPPUNIT_ASSERT_EQUAL_MESSAGE("Affects(31, 38)", true, pkb.isAffects(31, 38));
	CPPUNIT_ASSERT_EQUAL_MESSAGE("Affects(37, 38)", true, pkb.isAffects(37, 38));
	CPPUNIT_ASSERT_EQUAL_MESSAGE("Affects*(34, 38)", true, pkb.isAffects(34, 38, true));
	CPPUNIT_ASSERT_EQUAL_MESSAGE("Affects*(37, 38)", true, pkb.isAffects(37, 38, true));
	CPPUNIT_ASSERT_EQUAL_MESSAGE("Affects*(35, 38)", true, pkb.isAffects(35, 38, true));

	CPPUNIT_ASSERT_EQUAL_MESSAGE("Affects(22, 24)", false, pkb.isAffects(22, 24));
	CPPUNIT_ASSERT_EQUAL_MESSAGE("Affects(_, _)", true, pkb.isAffectsValid());

	
	CPPUNIT_ASSERT_EQUAL_MESSAGE("Affects(31, _) is valid", true, pkb.isAffectsLhsValid(31));
	CPPUNIT_ASSERT_EQUAL_MESSAGE("Affects(37, _) is valid", true, pkb.isAffectsLhsValid(37));
	CPPUNIT_ASSERT_EQUAL_MESSAGE("Affects(38, _) is valid", false, pkb.isAffectsLhsValid(38));
	CPPUNIT_ASSERT_EQUAL_MESSAGE("Affects(29, _) is valid", true, pkb.isAffectsLhsValid(29));
	CPPUNIT_ASSERT_EQUAL_MESSAGE("Affects(3,_) is valid", false, pkb.isAffectsLhsValid(3));
	
	CPPUNIT_ASSERT_EQUAL_MESSAGE("Affects(_, 19) is valid", false, pkb.isAffectsRhsValid(19));
	CPPUNIT_ASSERT_EQUAL_MESSAGE("Affects(_, 24) is valid", false, pkb.isAffectsRhsValid(24));
	CPPUNIT_ASSERT_EQUAL_MESSAGE("Affects(_, 31) is valid", false, pkb.isAffectsRhsValid(31));
	CPPUNIT_ASSERT_EQUAL_MESSAGE("Affects*(_, 27) is valid", false, pkb.isAffectsRhsValid(27));
	CPPUNIT_ASSERT_EQUAL_MESSAGE("Affects*(_,6) is valid", true, pkb.isAffectsRhsValid(6));

	// NextBip
	cout << "NextBip" << endl;
	
	CPPUNIT_ASSERT_EQUAL_MESSAGE("NextBip(12,_)", 2, (int)pkb.getNextBipAfter(12).size());
	CPPUNIT_ASSERT_EQUAL_MESSAGE("NextBip*(17,_), 17 is a call statement", 8, (int)pkb.getNextBipAfter(17, true).size());
	CPPUNIT_ASSERT_EQUAL_MESSAGE("NextBip*(18,_), 18 is a call statement and first line in proc", 20, (int)pkb.getNextBipAfter(18, true).size());
	
	CPPUNIT_ASSERT_EQUAL_MESSAGE("NextBip(,25)", 4, (int)pkb.getNextBipBefore(25).size());
	CPPUNIT_ASSERT_EQUAL_MESSAGE("NextBip(,38)", 1, (int)pkb.getNextBipBefore(38).size());
	CPPUNIT_ASSERT_EQUAL_MESSAGE("NextBip(,23)", 1, (int)pkb.getNextBipBefore(23).size());

	vector<int> nextBipBefore19 = pkb.getNextBipBefore(19);
	CPPUNIT_ASSERT_EQUAL_MESSAGE("NextBip(,19), previous line is call", 2, (int)nextBipBefore19.size());
	CPPUNIT_ASSERT_MESSAGE("NextBip(,19)", find(nextBipBefore19.begin(), nextBipBefore19.end(), 25) != nextBipBefore19.end() );

	CPPUNIT_ASSERT_EQUAL_MESSAGE("NextBip(,31), first line of proc", 37, (int)pkb.getNextBipBefore(31, true).size());

	// AffectsBip
	cout << "AffectsBip" << endl;
		
	CPPUNIT_ASSERT_EQUAL_MESSAGE("AffectsBip(35,_), normal affects", 2, (int)pkb.getAffectsBipAfter(35).size());
	CPPUNIT_ASSERT_EQUAL_MESSAGE("AffectsBip(31,_), normal affects", 2, (int)pkb.getAffectsBipAfter(31).size());
	CPPUNIT_ASSERT_EQUAL_MESSAGE("AffectsBip(29,_), normal affects", 1, (int)pkb.getAffectsBipAfter(29).size());
	CPPUNIT_ASSERT_EQUAL_MESSAGE("AffectsBip(27,_), normal affects", 1, (int)pkb.getAffectsBipAfter(27).size());

	CPPUNIT_ASSERT_EQUAL_MESSAGE("AffectsBip(22,_), include call to procedure without calls", 3, (int)pkb.getAffectsBipAfter(22).size());

	CPPUNIT_ASSERT_EQUAL_MESSAGE("AffectsBip(24,_), inside while to loop back to call 2 other procs", 3, (int)pkb.getAffectsBipAfter(24).size());
	CPPUNIT_ASSERT_EQUAL_MESSAGE("AffectsBip(30,_), non-assign stmt", 0, (int)pkb.getAffectsBipAfter(30).size());
	CPPUNIT_ASSERT_EQUAL_MESSAGE("AffectsBip(15,_), call only 1 other call stmt with nested calls", 1, (int)pkb.getAffectsBipAfter(15).size());
	
	CPPUNIT_ASSERT_EQUAL_MESSAGE("AffectsBip(11,_)", 2, (int)pkb.getAffectsBipAfter(11).size());

	CPPUNIT_ASSERT_EQUAL_MESSAGE("AffectsBip*(35,_)", 5, (int)pkb.getAffectsBipAfter(35,true).size());
	
	CPPUNIT_ASSERT_EQUAL_MESSAGE("AffectsBip*(31,_), normal affects", 5, (int)pkb.getAffectsBipAfter(31,true).size());
	CPPUNIT_ASSERT_EQUAL_MESSAGE("AffectsBip*(29,_), normal affects", (int)pkb.getAffectedBy(29,true).size(), (int)pkb.getAffectsBipAfter(29,true).size());
	CPPUNIT_ASSERT_EQUAL_MESSAGE("AffectsBip*(27,_), normal affects", (int)pkb.getAffectedBy(27,true).size(), (int)pkb.getAffectsBipAfter(27,true).size());
	

	CPPUNIT_ASSERT_EQUAL_MESSAGE("AffectsBip*(11,_)", 5, (int)pkb.getAffectsBipAfter(11,true).size());

	CPPUNIT_ASSERT_EQUAL_MESSAGE("AffectsBip(,5)", 0, (int)pkb.getAffectsBipBefore(5).size());
	CPPUNIT_ASSERT_EQUAL_MESSAGE("AffectsBip*(,5)", 0, (int)pkb.getAffectsBipBefore(5,true).size());

	CPPUNIT_ASSERT_EQUAL_MESSAGE("AffectsBip(,6), normal affects", (int)pkb.getAffecting(6).size(), (int)pkb.getAffectsBipBefore(6).size());
	CPPUNIT_ASSERT_EQUAL_MESSAGE("AffectsBip(,11), normal affects", (int)pkb.getAffecting(11).size(), (int)pkb.getAffectsBipBefore(11).size());
	CPPUNIT_ASSERT_EQUAL_MESSAGE("AffectsBip(,15), normal affects", (int)pkb.getAffecting(15).size(), (int)pkb.getAffectsBipBefore(15).size());

	CPPUNIT_ASSERT_EQUAL_MESSAGE("AffectsBip(,24), ", 2, (int)pkb.getAffectsBipBefore(24).size());	
	CPPUNIT_ASSERT_EQUAL_MESSAGE("AffectsBip(,36), ", 8, (int)pkb.getAffectsBipBefore(36).size());

	CPPUNIT_ASSERT_EQUAL_MESSAGE("AffectsBip*(,36), ", 14, (int)pkb.getAffectsBipBefore(36, true).size());

	//Contains
	cout << "Contains" << endl;

	/*vector<pair<int, vector<int>>> testing;
	testing = pkb.contains(PLUS, TIMES, false);
	for(int i=0; i<testing.size(); i++) {
		pair <int, vector<int>> tester = testing.at(i);
		cout << (i+1) << ". PLUS at stmtNo " << tester.first << endl;
		for(int j=0; j<tester.second.size(); j++) {
			cout << "TIMES StmtNo " << tester.second.at(j) << endl;
		}
	}*/

	/*vector<int> testing;
	testing = pkb.contains(19, CALL, true);
	for(int i=0; i<testing.size(); i++) {
		cout << (i+1) << ". CALL at stmtNo " << testing.at(i) << endl;
	}*/

	CPPUNIT_ASSERT_EQUAL_MESSAGE("Contains(While, StmtLst)", 7, (int)pkb.contains(WHILE, STMTLST, false).size());
	CPPUNIT_ASSERT_EQUAL_MESSAGE("Contains*(While, Assign)", 7, (int)pkb.contains(WHILE, ASSIGN, true).size());
	CPPUNIT_ASSERT_EQUAL_MESSAGE("Contains(Procedure, StmtLst)", 5, (int)pkb.contains(PROCEDURE, STMTLST, false).size());
	CPPUNIT_ASSERT_EQUAL_MESSAGE("Contains*(Procedure, If)", 4, (int)pkb.contains(PROCEDURE, IF, true).size());
	CPPUNIT_ASSERT_EQUAL_MESSAGE("Contains(If, StmtLst)", 5, (int)pkb.contains(IF, STMTLST, false).size());
	CPPUNIT_ASSERT_EQUAL_MESSAGE("Contains*(If, StmtLst)", 5, (int)pkb.contains(IF, STMTLST, true).size());
	CPPUNIT_ASSERT_EQUAL_MESSAGE("Contains(If, Var)", 5, (int)pkb.contains(IF, VARIABLE, false).size());
	CPPUNIT_ASSERT_EQUAL_MESSAGE("Contains*(If, While)", 1, (int)pkb.contains(IF, WHILE, true).size());
	CPPUNIT_ASSERT_EQUAL_MESSAGE("Contains(Assign, Plus)", 10, (int)pkb.contains(ASSIGN, PLUS, false).size());
	CPPUNIT_ASSERT_EQUAL_MESSAGE("Contains*(Assign, Plus)", 10, (int)pkb.contains(ASSIGN, PLUS, true).size());
	CPPUNIT_ASSERT_EQUAL_MESSAGE("Contains(StmtLst, While)", 6, (int)pkb.contains(STMTLST, WHILE, false).size());
	CPPUNIT_ASSERT_EQUAL_MESSAGE("Contains*(StmtLst, While)", 7, (int)pkb.contains(STMTLST, WHILE, true).size());
	CPPUNIT_ASSERT_EQUAL_MESSAGE("Contains(StmtLst, StmtLst)", 0, (int)pkb.contains(STMTLST, STMTLST, false).size());
	CPPUNIT_ASSERT_EQUAL_MESSAGE("Contains*(StmtLst, StmtLst)", 10, (int)pkb.contains(STMTLST, STMTLST, true).size());
	CPPUNIT_ASSERT_EQUAL_MESSAGE("Contains(StmtLst, If)", 5, (int)pkb.contains(STMTLST, IF, false).size());
	CPPUNIT_ASSERT_EQUAL_MESSAGE("Contains*(StmtLst, If)", 8, (int)pkb.contains(STMTLST, IF, true).size());
	CPPUNIT_ASSERT_EQUAL_MESSAGE("Contains(Plus, Variable)", 11, (int)pkb.contains(PLUS, VARIABLE, false).size());
	CPPUNIT_ASSERT_EQUAL_MESSAGE("Contains*(Plus, Variable)", 12, (int)pkb.contains(PLUS, VARIABLE, true).size());
	CPPUNIT_ASSERT_EQUAL_MESSAGE("Contains(Plus, Times)", 3, (int)pkb.contains(PLUS, TIMES, false).size());
	CPPUNIT_ASSERT_EQUAL_MESSAGE("Contains*(Plus, Times)", 4, (int)pkb.contains(PLUS, TIMES, true).size());
	CPPUNIT_ASSERT_EQUAL_MESSAGE("Contains(Program, Assign)", 0, (int)pkb.contains(PROGRAM, ASSIGN, false).size());
	CPPUNIT_ASSERT_EQUAL_MESSAGE("Contains*(Program, Assign)", 1, (int)pkb.contains(PROGRAM, ASSIGN, true).size());
	CPPUNIT_ASSERT_EQUAL_MESSAGE("Contains(Program, If)", 0, (int)pkb.contains(PROGRAM, IF, false).size());
	CPPUNIT_ASSERT_EQUAL_MESSAGE("Contains*(Program, If)", 1, (int)pkb.contains(PROGRAM, IF, true).size());
	CPPUNIT_ASSERT_EQUAL_MESSAGE("Contains*(Stmt, If)", 4, (int)pkb.contains(STMT, IF, true).size());
	CPPUNIT_ASSERT_EQUAL_MESSAGE("Contains*(Stmt, Stmt)", 12, (int)pkb.contains(STMT, STMT, true).size());

	CPPUNIT_ASSERT_EQUAL_MESSAGE("Contains*(7, PLUS)", 2, (int)pkb.contains(7, PLUS, true).size());
	CPPUNIT_ASSERT_EQUAL_MESSAGE("Contains*(19, CALL)", 2, (int)pkb.contains(19, CALL, true).size());
	CPPUNIT_ASSERT_EQUAL_MESSAGE("Contains*(STMTLST, 7)", 1, (int)pkb.contains(STMTLST, 7, true).size());
	CPPUNIT_ASSERT_EQUAL_MESSAGE("Contains*(STMTLST, 29)", 4, (int)pkb.contains(STMTLST, 29, true).size());
	CPPUNIT_ASSERT_EQUAL_MESSAGE("Contains*(STMTLST, 16)", 3, (int)pkb.contains(STMTLST, 16, true).size());

	//New Pattern Syntax
	cout << "New Pattern Syntax" << endl;
	vector<pair<int, vector<int>>> testing2 = pkb.patternMatchIfElse("_", STMTLST);
	//vector<int> testing = pkb.patternMatchIf("a", Plus);
	for(int i=0; i<testing2.size(); i++) {
		pair <int, vector<int>> tester = testing2.at(i);
		cout << (i+1) << ". if at stmtNo " << tester.first << endl;
		for(int j=0; j<tester.second.size(); j++) {
			cout << "STMTLST StmtNo " << tester.second.at(j) << endl;
		}
	}
	/*vector<pair<int, int>> testing2 = pkb.patternMatchIf("_", ASSIGN, ASSIGN);
	for(int i=0; i<testing2.size(); i++) {
		cout << "First is " << testing2.at(i).first << endl;
		cout << "Second is " << testing2.at(i).second << endl;
	}*/

	//CPPUNIT_ASSERT_EQUAL_MESSAGE("pattern w('b', Plus)", 9, pkb.patternMatchWhile("b", PLUS).front());
	//CPPUNIT_ASSERT_EQUAL_MESSAGE("pattern w('b', Plus)", 2, (int)pkb.patternMatchWhile("b", PLUS).size());
	//CPPUNIT_ASSERT_EQUAL_MESSAGE("pattern ifstat('b', Plus, _)", 2, (int)pkb.patternMatchIfThen("b", PLUS).size());
	//CPPUNIT_ASSERT_EQUAL_MESSAGE("pattern ifstat('b', _, Variable)", 4, (int)pkb.patternMatchIfElse("b", VARIABLE).size());
	//CPPUNIT_ASSERT_EQUAL_MESSAGE("pattern ifstat('b', Plus, Variable)", 5, (int)pkb.patternMatchIf("b", PLUS, VARIABLE).size());
	//CPPUNIT_ASSERT_EQUAL_MESSAGE("pattern ifstat('_', Assign, Assign)", 6, (int)pkb.patternMatchIf("_", ASSIGN, ASSIGN).size());

	//Siblings
	cout << "Siblings" << endl;
	/*vector<pair<int, vector<int>>> testing3;
	testing3 = pkb.siblings(VARIABLE, CONSTANT);
	for(int i=0; i<testing3.size(); i++) {
		pair <int, vector<int>> tester2 = testing3.at(i);
		cout << (i+1) << ". VAR at stmtNo " << tester2.first << endl;
		for(int j=0; j<tester2.second.size(); j++) {
			cout << "CONST StmtNo " << tester2.second.at(j) << endl;
		}
	}*/

	/*vector<int> testing;
	testing = pkb.siblings(ASSIGN, 6);
	for(int i=0; i<testing.size(); i++) {
		cout << (i+1) << ". CALL at stmtNo " << testing.at(i) << endl;
	}*/

	CPPUNIT_ASSERT_EQUAL_MESSAGE("Siblings(Procedure, Procedure)", 5, (int)pkb.siblings(PROCEDURE, PROCEDURE).size());
	CPPUNIT_ASSERT_EQUAL_MESSAGE("Siblings(Plus, Times)", 1, (int)pkb.siblings(PLUS, TIMES).size());
	CPPUNIT_ASSERT_EQUAL_MESSAGE("Siblings(Plus, Variable)", 11, (int)pkb.siblings(PLUS, VARIABLE).size());
	CPPUNIT_ASSERT_EQUAL_MESSAGE("Siblings(Times, Variable)", 3, (int)pkb.siblings(TIMES, VARIABLE).size());
	CPPUNIT_ASSERT_EQUAL_MESSAGE("Siblings(Variable, Times)", 3, (int)pkb.siblings(VARIABLE, TIMES).size());
	CPPUNIT_ASSERT_EQUAL_MESSAGE("Siblings(While, Assign)", 4, (int)pkb.siblings(WHILE, ASSIGN).size());
	CPPUNIT_ASSERT_EQUAL_MESSAGE("Siblings(Constant, Variable)", 14, (int)pkb.siblings(CONSTANT, VARIABLE).size());
	CPPUNIT_ASSERT_EQUAL_MESSAGE("Siblings(StmtLst, StmtLst)", 10, (int)pkb.siblings(STMTLST, STMTLST).size());
	CPPUNIT_ASSERT_EQUAL_MESSAGE("Siblings(Stmt, Stmt)", 24, (int)pkb.siblings(STMT, STMT).size());
	CPPUNIT_ASSERT_EQUAL_MESSAGE("Siblings(Stmt, Variable)", 0, (int)pkb.siblings(STMT, VARIABLE).size());
	CPPUNIT_ASSERT_EQUAL_MESSAGE("Siblings(StmtLst, Variable)", 17, (int)pkb.siblings(STMTLST, VARIABLE).size());
	CPPUNIT_ASSERT_EQUAL_MESSAGE("Siblings(Variable, StmtLst)", 12, (int)pkb.siblings(VARIABLE, STMTLST).size());
	CPPUNIT_ASSERT_EQUAL_MESSAGE("Siblings(VARIABLE, CONSTANT)", 14, (int)pkb.siblings(VARIABLE, CONSTANT).size());

	CPPUNIT_ASSERT_EQUAL_MESSAGE("Siblings(Assign, 6)", 4, (int)pkb.siblings(ASSIGN, 6).size());
	CPPUNIT_ASSERT_EQUAL_MESSAGE("Siblings(6, Assign)", 4, (int)pkb.siblings(6, ASSIGN).size());
	CPPUNIT_ASSERT_EQUAL_MESSAGE("Siblings(11, While)", 2, (int)pkb.siblings(11, WHILE).size());
	

	cout << "End TestPkb" << endl;
}


