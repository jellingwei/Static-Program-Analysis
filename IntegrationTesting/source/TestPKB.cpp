#include <cppunit/config/SourcePrefix.h>
#include <vector>
#include <algorithm>
#include "TestPKB.h"
#include "SourceParser.h"
#include "FrontEndController.h"
#include "PKB.h"
#include "ConstantTable.h"
#include "PatternMatch.h"

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
	CPPUNIT_ASSERT_EQUAL_MESSAGE("Number of procedures", 4, pkb.getProcTableSize());

	// ConstantTable
	cout << "Constant Table" << endl;
	CPPUNIT_ASSERT_EQUAL_MESSAGE("Number of constants", 3, pkb.getConstantTableSize());

	// VarTable
	cout << "Var Table" << endl;
	CPPUNIT_ASSERT_EQUAL_MESSAGE("Number of variables", 6, pkb.getVarTableSize());
	CPPUNIT_ASSERT(pkb.getVarIndex("f"));
	CPPUNIT_ASSERT_EQUAL_MESSAGE("Get invalid variable", -1, pkb.getVarIndex("z"));

	// statements
	CPPUNIT_ASSERT_EQUAL_MESSAGE("Number of statements", 26, (int)pkb.getStmtNumForType(STMT).size());
	CPPUNIT_ASSERT_EQUAL_MESSAGE("Number of assign", 12, (int)pkb.getStmtNumForType(ASSIGN).size());
	CPPUNIT_ASSERT_EQUAL_MESSAGE("Number of if", 4, (int)pkb.getStmtNumForType(IF).size());
	CPPUNIT_ASSERT_EQUAL_MESSAGE("Number of while", 6, (int)pkb.getStmtNumForType(WHILE).size());
	CPPUNIT_ASSERT_EQUAL_MESSAGE("Number of call", 4, (int)pkb.getStmtNumForType(CALL).size());

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
	CPPUNIT_ASSERT_EQUAL_MESSAGE("Parent*(12, s)", 4, (int)pkb.getChildS(12).size());
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
	CPPUNIT_ASSERT_EQUAL_MESSAGE("Parent*(21, 25)", true, pkb.isParentS(21, 25));
	
	CPPUNIT_ASSERT_EQUAL_MESSAGE("Parent(_, )", 10, (int)pkb.getParentLhs().size());
	CPPUNIT_ASSERT_EQUAL_MESSAGE("Parent(, _)", 14, (int)pkb.getParentRhs().size());

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
	CPPUNIT_ASSERT_EQUAL_MESSAGE("Follows(17, 18)", true, pkb.isFollows(17,18));
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
	CPPUNIT_ASSERT_EQUAL_MESSAGE("Follows(17, s)", 1, (int)pkb.getStmtFollowedFrom(17).size());
	CPPUNIT_ASSERT_EQUAL_MESSAGE("Follows(21, s)", 0, (int)pkb.getStmtFollowedFrom(21).size());
	CPPUNIT_ASSERT_EQUAL_MESSAGE("Follows(26, s)", 0, (int)pkb.getStmtFollowedFrom(26).size());
	CPPUNIT_ASSERT_EQUAL_MESSAGE("Follows(9999, 10000)", false, pkb.isFollows(9999,10000));


	CPPUNIT_ASSERT_EQUAL_MESSAGE("Follows(_, )", 10, (int)pkb.getFollowsLhs().size());
	CPPUNIT_ASSERT_EQUAL_MESSAGE("Follows(, _)", 10, (int)pkb.getFollowsRhs().size());

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
	CPPUNIT_ASSERT_EQUAL_MESSAGE("Follows*(17, 18)", true, pkb.isFollowsS(17, 18));
	CPPUNIT_ASSERT_EQUAL_MESSAGE("Follows*(17, 19)", false, pkb.isFollowsS(17, 19));
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
	CPPUNIT_ASSERT_EQUAL_MESSAGE("Uses(s, 'c')", 13, (int)pkb.getUsesStmtNum(pkb.getVarIndex("c")).size());
	CPPUNIT_ASSERT_EQUAL_MESSAGE("Uses(s, 'f')", 5, (int)pkb.getUsesStmtNum(pkb.getVarIndex("f")).size());
	CPPUNIT_ASSERT_EQUAL_MESSAGE("Uses(1, _)", 0, (int)pkb.getUsesVarForStmt(1).size());
	CPPUNIT_ASSERT_EQUAL_MESSAGE("Uses(20, _)", 3, (int)pkb.getUsesVarForStmt(20).size());
	CPPUNIT_ASSERT_EQUAL_MESSAGE("Uses(26, _)", 3, (int)pkb.getUsesVarForStmt(26).size());
	

	CPPUNIT_ASSERT_EQUAL_MESSAGE("Uses(_, )", 22, (int)pkb.getUsesLhs().size());
	CPPUNIT_ASSERT_EQUAL_MESSAGE("Uses(, _)", 6, (int)pkb.getUsesRhs().size());

	CPPUNIT_ASSERT_EQUAL_MESSAGE("Uses for procIndex 0", 6, (int)pkb.getUsesVarForProc(0).size());
	CPPUNIT_ASSERT_EQUAL_MESSAGE("Uses for procIndex 1", 3, (int)pkb.getUsesVarForProc(1).size());
	CPPUNIT_ASSERT_EQUAL_MESSAGE("Uses for procIndex 2", 3, (int)pkb.getUsesVarForProc(2).size());
	CPPUNIT_ASSERT_EQUAL_MESSAGE("Uses for procIndex 3", 3, (int)pkb.getUsesVarForProc(3).size());
	CPPUNIT_ASSERT_EQUAL_MESSAGE("Uses for procIndex 100", 0, (int)pkb.getUsesVarForProc(100).size());
	CPPUNIT_ASSERT_EQUAL_MESSAGE("Uses(p, 'a')", 4, (int)pkb.getUsesProcIndex(pkb.getVarIndex("a")).size());
	CPPUNIT_ASSERT_EQUAL_MESSAGE("Uses(p, 'c')", 4, (int)pkb.getUsesProcIndex(pkb.getVarIndex("c")).size());
	CPPUNIT_ASSERT_EQUAL_MESSAGE("Uses(p, 'd')", 1, (int)pkb.getUsesProcIndex(pkb.getVarIndex("d")).size());

	// Modifies
	cout << "Modifies" << endl;
	CPPUNIT_ASSERT_EQUAL_MESSAGE("Modifies(1, 'a')", true, pkb.isModifies(1, pkb.getVarIndex("a")));
	CPPUNIT_ASSERT_EQUAL_MESSAGE("Modifies(5, 'e')", false, pkb.isModifies(5, pkb.getVarIndex("e")));
	CPPUNIT_ASSERT_EQUAL_MESSAGE("Modifies(7, 'b')", false, pkb.isModifies(7, pkb.getVarIndex("b")));
	CPPUNIT_ASSERT_EQUAL_MESSAGE("Modifies(7, 'f')", true, pkb.isModifies(7, pkb.getVarIndex("f")));
	CPPUNIT_ASSERT_EQUAL_MESSAGE("Modifies(15, 'a')", true, pkb.isModifies(15, pkb.getVarIndex("a")));
	CPPUNIT_ASSERT_EQUAL_MESSAGE("Modifies(17, 'a')", true, pkb.isModifies(17, pkb.getVarIndex("a")));
	CPPUNIT_ASSERT_EQUAL_MESSAGE("Modifies(20, 'b')", false, pkb.isModifies(20, pkb.getVarIndex("b")));
	CPPUNIT_ASSERT_EQUAL_MESSAGE("Modifies(20, 'a')", true, pkb.isModifies(20, pkb.getVarIndex("a")));
	CPPUNIT_ASSERT_EQUAL_MESSAGE("Modifies(26, 'b')", false, pkb.isModifies(26, pkb.getVarIndex("b")));
	CPPUNIT_ASSERT_EQUAL_MESSAGE("Modifies(26, 'a')", true, pkb.isModifies(26, pkb.getVarIndex("a")));
	CPPUNIT_ASSERT_EQUAL_MESSAGE("Modifies(s, 'd')", 5, pkb.getModStmtNum(pkb.getVarIndex("d")).front());
	CPPUNIT_ASSERT_EQUAL_MESSAGE("Modifies(s, 'b')", 2, (int)pkb.getModStmtNum(pkb.getVarIndex("b")).size());
	CPPUNIT_ASSERT_EQUAL_MESSAGE("Modifies(7, v)", 2, (int)pkb.getModVarForStmt(7).size());
	CPPUNIT_ASSERT_EQUAL_MESSAGE("Modifies(8, v)", (string)"f", pkb.getVarName(pkb.getModVarForStmt(8).front()));
	CPPUNIT_ASSERT_EQUAL_MESSAGE("Modifies(11, v)", (string)"a", pkb.getVarName(pkb.getModVarForStmt(11).front()));

	CPPUNIT_ASSERT_EQUAL_MESSAGE("Modifies(_, )", 26 , (int)pkb.getModifiesLhs().size());
	CPPUNIT_ASSERT_EQUAL_MESSAGE("Modifies(, _)", 6, (int)pkb.getModifiesRhs().size());


	CPPUNIT_ASSERT_EQUAL_MESSAGE("Modifies for proc index 0", 6, (int)pkb.getModVarForProc(0).size());
	CPPUNIT_ASSERT_EQUAL_MESSAGE("Modifies for proc index 1", 1, (int)pkb.getModVarForProc(1).size());
	CPPUNIT_ASSERT_EQUAL_MESSAGE("Modifies(p, 'a')", 4, (int)pkb.getModProcIndex(pkb.getVarIndex("a")).size());

	// Pattern
	cout << "Pattern" << endl;
	CPPUNIT_ASSERT_EQUAL_MESSAGE("pattern a(_, _'f'_)", 4, (int)pkb.patternMatchAssign("_\"f\"_").size());
	CPPUNIT_ASSERT_EQUAL_MESSAGE("pattern a(_, _'a+f'_)", 6, pkb.patternMatchAssign("_\"a+f\"_").front());
	CPPUNIT_ASSERT_EQUAL_MESSAGE("pattern a(_, _'a + f'_)", 6, pkb.patternMatchAssign("_\"a + f\"_").front());
	CPPUNIT_ASSERT_EQUAL_MESSAGE("pattern a(_, _'d + e'_)", 0, (int)pkb.patternMatchAssign("_\"d + e\"_").size());
	CPPUNIT_ASSERT_EQUAL_MESSAGE("pattern a(_, _'1'_)", 1, pkb.patternMatchAssign("_\"1\"_").front());
	CPPUNIT_ASSERT_EQUAL_MESSAGE("pattern a(_, _)", 12, (int)pkb.patternMatchAssign("_").size());
	CPPUNIT_ASSERT_EQUAL_MESSAGE("pattern a(_, 'a + f')", 6, (int)pkb.patternMatchAssign("a + f").front());
	CPPUNIT_ASSERT_EQUAL_MESSAGE("pattern a(_, '2')", 4, pkb.patternMatchAssign("2").front());
	CPPUNIT_ASSERT_EQUAL_MESSAGE("pattern a(_, '2')", 2, (int)pkb.patternMatchAssign("2").size());
	CPPUNIT_ASSERT_EQUAL_MESSAGE("pattern a(_, 'd')", 0, (int)pkb.patternMatchAssign("d").size());

	CPPUNIT_ASSERT_EQUAL_MESSAGE("pattern a(_, _'e + (3 * b)'_)", 11, pkb.patternMatchAssign("_\"e + (3 * b)\"_").front());
	CPPUNIT_ASSERT_EQUAL_MESSAGE("pattern a(_, _'e + 3 * b'_)", 11, pkb.patternMatchAssign("_\"e + 3 * b\"_").front());
	CPPUNIT_ASSERT_EQUAL_MESSAGE("pattern a(_, _'e + (3 * b) + (4 * f) * 3 - f'_)", 0, (int)pkb.patternMatchAssign("_\"e + (3 * b) + (4 * f) * 3 - f\"_").size());
	CPPUNIT_ASSERT_EQUAL_MESSAGE("pattern a(_, _'e + (3 * (2 + f))'_)", 0, (int)pkb.patternMatchAssign("_\"e + (3 * (2 + f))\"_").size());
	CPPUNIT_ASSERT_EQUAL_MESSAGE("pattern a(_, _'3 * b'_)", 11, pkb.patternMatchAssign("_\"3 * b\"_").front());
	CPPUNIT_ASSERT_EQUAL_MESSAGE("pattern a(_, _'e + 1'_)", 9, pkb.patternMatchAssign("_\"e + 1\"_").front());
	
	CPPUNIT_ASSERT_EQUAL_MESSAGE("pattern a(_, _'u + (4 * z)'_) - Variable Dont Exist", 0, (int)pkb.patternMatchAssign("_\"u + (4 * z)\"_").size());

	CPPUNIT_ASSERT_EQUAL_MESSAGE("pattern a(_, '')", 12, (int)pkb.patternMatchAssign("\"\"").size());
	// Query Validator will check
	CPPUNIT_ASSERT_EQUAL_MESSAGE("pattern a(_, _''_)", 12, (int)pkb.patternMatchAssign("_\"\"_").size());

	// PatternMatch
	cout << "new pattern design" << endl;
	const char* args[] = {"d", "+", "f"};
	vector<string> argVector(args, args + 3);
	ExpressionParser exprParser;
	TNode* top = exprParser.parseExpressionForQuerying(argVector);
	CPPUNIT_ASSERT_EQUAL_MESSAGE("ok", Plus, top->getNodeType());
	PatternMatch pattern;
	CPPUNIT_ASSERT_EQUAL_MESSAGE("this", 2, pattern.PatternMatchAssign(top, true).at(0));

	const char* args2[] = {"1"};
	vector<string> argVector2(args2, args2 + 1);
	TNode* top2 = exprParser.parseExpressionForQuerying(argVector2);

	CPPUNIT_ASSERT_EQUAL_MESSAGE("this", 9, pattern.PatternMatchAssign(top2, false).at(2));

	const char* args3[] = {"e", "+", "3", "*", "b", "+", "f"};
	vector<string> argVector3(args3, args3 + 7);
	TNode* top3 = exprParser.parseExpressionForQuerying(argVector3);
	// full pattern match for e+3*b+f
	CPPUNIT_ASSERT_EQUAL_MESSAGE("this", 11, pattern.PatternMatchAssign(top3, false).at(0));

	const char* args4[] = {"3", "*", "b"};
	vector<string> argVector4(args4, args4 + 3);
	TNode* top4 = exprParser.parseExpressionForQuerying(argVector4);
	// partial match from 3 * b
	CPPUNIT_ASSERT_EQUAL_MESSAGE("this", 1, (int)pattern.PatternMatchAssign(top4, false).size());

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
	CPPUNIT_ASSERT_EQUAL_MESSAGE("pattern ifstat('b', _, _)", 12, pkb.patternMatchIf(" b ").front());
	CPPUNIT_ASSERT_EQUAL_MESSAGE("pattern ifstat('c', _, _)", 13, pkb.patternMatchIf("c").front());
	CPPUNIT_ASSERT_EQUAL_MESSAGE("pattern ifstat('unusedvar', _, _)", 0, (int)pkb.patternMatchIf("unusedvar").size());

	CPPUNIT_ASSERT_EQUAL_MESSAGE("pattern ifstat(v, _, _) given stmtnum 12", pkb.getVarIndex("b"), pkb.getControlVariable(12));
	CPPUNIT_ASSERT_EQUAL_MESSAGE("pattern ifstat(v, _, _) given stmtnum 8", pkb.getVarIndex("c"), pkb.getControlVariable(13));
	CPPUNIT_ASSERT_EQUAL_MESSAGE("pattern ifstat(v, _, _) given stmtnum which is not a if statement", -1, pkb.getControlVariable(14));

	// All pairs for Follows
	pair<vector<int>, vector<int>> allFollows = pkb.getAllFollowsPairs(false);
	CPPUNIT_ASSERT_EQUAL_MESSAGE("All Follows pairs size", 10, (int)allFollows.first.size());
	allFollows = pkb.getAllFollowsPairs(true);
	CPPUNIT_ASSERT_EQUAL_MESSAGE("All Follows* pairs size", 30, (int)allFollows.first.size());

	// All pairs for parent
	pair<vector<int>, vector<int>> allParent = pkb.getAllParentPairs(false);
	CPPUNIT_ASSERT_EQUAL_MESSAGE("All Parent pairs size", 13, (int)allParent.first.size());

	allParent = pkb.getAllParentPairs(true);
	CPPUNIT_ASSERT_EQUAL_MESSAGE("All Parent* pairs size", 23, (int)allParent.first.size());


	// Calls Table
	cout << "Calls table" << endl;
	CPPUNIT_ASSERT_EQUAL_MESSAGE("calls(0, p)", 1, (int)pkb.getProcsCalledBy(0).size());
	CPPUNIT_ASSERT_EQUAL_MESSAGE("calls*(0, p)", 3, (int)pkb.getProcsCalledByS(0).size());
	CPPUNIT_ASSERT_EQUAL_MESSAGE("calls(p, 2)", 2, (int)pkb.getProcsCalling(2).size());

	CPPUNIT_ASSERT_EQUAL_MESSAGE("Calls(_, )", 3, (int)pkb.getCallsLhs().size());
	CPPUNIT_ASSERT_EQUAL_MESSAGE("Calls(, _)", 3, (int)pkb.getCallsRhs().size());

	CPPUNIT_ASSERT_EQUAL_MESSAGE("get procName of call statement", string("Test4"), pkb.getProcNameCalledByStatement(20));
	CPPUNIT_ASSERT_EQUAL_MESSAGE("get procName of call statement", string("Test3"), pkb.getProcNameCalledByStatement(17));
	CPPUNIT_ASSERT_EQUAL_MESSAGE("get procName of call statement", string("Test2"), pkb.getProcNameCalledByStatement(16));
	
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
	CPPUNIT_ASSERT_EQUAL_MESSAGE("Next(12, _)", 1, (int)pkb.getNextAfter(12).size());
	CPPUNIT_ASSERT_EQUAL_MESSAGE("Next(12, _)", 13, pkb.getNextAfter(12).front());
	CPPUNIT_ASSERT_EQUAL_MESSAGE("Next(_, 13)", 12, pkb.getNextBefore(13).front());


	CPPUNIT_ASSERT_EQUAL_MESSAGE("Next*(4, _)", 14, (int)pkb.getNextAfterS(4).size());
	CPPUNIT_ASSERT_EQUAL_MESSAGE("Next*(1, _)", 15, (int)pkb.getNextAfterS(1).size());
	CPPUNIT_ASSERT_EQUAL_MESSAGE("Next*(_, 12)", 11, (int)pkb.getNextBeforeS(12).size());
	CPPUNIT_ASSERT_EQUAL_MESSAGE("Next*(_, 16)", 14, (int)pkb.getNextBeforeS(16).size());

	CPPUNIT_ASSERT_EQUAL_MESSAGE("Next*(17, _)", 3, (int)pkb.getNextAfterS(17).size());
	CPPUNIT_ASSERT_EQUAL_MESSAGE("Next*(_, 25)", 5, (int)pkb.getNextBeforeS(25).size());

	CPPUNIT_ASSERT_EQUAL_MESSAGE("Next(20, 18)", true, pkb.isNext(20, 18));
	CPPUNIT_ASSERT_EQUAL_MESSAGE("Next(20, 19)", true, pkb.isNextS(20, 19));

	CPPUNIT_ASSERT_EQUAL_MESSAGE("Next(23, 24)", false, pkb.isNext(23, 24));
	CPPUNIT_ASSERT_EQUAL_MESSAGE("Next*(23, 24)", true, pkb.isNextS(23, 24));
	CPPUNIT_ASSERT_EQUAL_MESSAGE("Next*(23, 25), proglines in if-then and if-else nested in while loop", true, pkb.isNextS(23, 25)); 

	CPPUNIT_ASSERT_EQUAL_MESSAGE("Next(22, _)", 2, (int)pkb.getNextAfter(22).size());
	CPPUNIT_ASSERT_EQUAL_MESSAGE("Next(23, _)", 1, (int)pkb.getNextAfter(23).size());
	CPPUNIT_ASSERT_EQUAL_MESSAGE("Next(23, 21), nested last stmt in stmtlst links back to while", true, pkb.isNext(23, 21));
	
	CPPUNIT_ASSERT_EQUAL_MESSAGE("Next(16, 17), lines in different procedures", false, pkb.isNextS(16, 17));


	CPPUNIT_ASSERT_EQUAL_MESSAGE("Next lhs", 23, (int)pkb.getNextLhs().size());
	CPPUNIT_ASSERT_EQUAL_MESSAGE("Next rhs", 23, (int)pkb.getNextRhs().size());

	// Affects... 
	cout << "Affects" << endl;
	CPPUNIT_ASSERT_EQUAL_MESSAGE("Affects(6, _)", 2, (int)pkb.getAffectedBy(6).size());

	CPPUNIT_ASSERT_EQUAL_MESSAGE("Affects(9, _)", 2, (int)pkb.getAffectedBy(9).size());
	CPPUNIT_ASSERT_EQUAL_MESSAGE("Affects*(9, _)", 4, (int)pkb.getAffectedBy(9, true).size());

	CPPUNIT_ASSERT_EQUAL_MESSAGE("Affects(2, _)", 0, (int)pkb.getAffectedBy(2).size());

	CPPUNIT_ASSERT_EQUAL_MESSAGE("Affects(4, _)", 1, (int)pkb.getAffectedBy(4).size());
	CPPUNIT_ASSERT_EQUAL_MESSAGE("Affects*(4, _)", 2, (int)pkb.getAffectedBy(4, true).size());

	CPPUNIT_ASSERT_EQUAL_MESSAGE("Affects(_, 2)", 0, (int)pkb.getAffecting(2).size());
	CPPUNIT_ASSERT_EQUAL_MESSAGE("Affects(_, 6)", 1, (int)pkb.getAffecting(6).size());
	CPPUNIT_ASSERT_EQUAL_MESSAGE("Affects(_, 11)", 4, (int)pkb.getAffecting(11).size());
	CPPUNIT_ASSERT_EQUAL_MESSAGE("Affects*(_, 11)", 5, (int)pkb.getAffecting(11, true).size());

	CPPUNIT_ASSERT_EQUAL_MESSAGE("Affects(23, 25)", true, pkb.isAffects(23, 25));
	CPPUNIT_ASSERT_EQUAL_MESSAGE("Affects(25, 23)", false, pkb.isAffects(25, 23));

	CPPUNIT_ASSERT_EQUAL_MESSAGE("Affects*(14, 15)", false, pkb.isAffects(14, 15, true));
	CPPUNIT_ASSERT_EQUAL_MESSAGE("Affects*(1, 11)", true, pkb.isAffects(1, 11, true));


	cout << "End TestPkb" << endl;
}

