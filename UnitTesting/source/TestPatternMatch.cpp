#include <cppunit/config/SourcePrefix.h>

#include "TestPatternMatch.h"
#include "PatternMatch.h"
#include "TNode.h"
#include "ExpressionParser.h"

#include "SourceParser.h"
#include "PKB.h"

#include <vector>

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION( PatternMatchTest ); 

void PatternMatchTest::setUp()
{
/*	Parser::initParser("test/i_src.txt");
	bool parse = Parser::parseProgram();
	CPPUNIT_ASSERT_EQUAL_MESSAGE("Parsed source file", parse, true);*/
}

void PatternMatchTest::testAssign()
{
	PKB pkb = PKB::getInstance();
	
	cout << "constant Table" << endl;
	CPPUNIT_ASSERT_EQUAL_MESSAGE("Number of constants", 0, pkb.getConstantTableSize());
	cout << "var Table" << endl;
	CPPUNIT_ASSERT_EQUAL_MESSAGE("Number of variables", 0, pkb.getVarTableSize());

	/*Parser::initParser("test/i_src2.txt");
	AST* ast = PKB::getInstance().ast;
cout << "ast size " << ast->getSize() << " ";
cout << "ast root is " << ast->getRoot()->getStmtNumber() << " ";*/
	
/*	AST ast;
	TNode* assign = ast.createTNode(Assign, 1, 1);
	TNode* plus = ast.createTNode(Plus, 1, 1);
	ast.createLink(Child, assign, plus);
	TNode* d = ast.createTNode(Variable, 1, 1);
	TNode* f = ast.createTNode(Variable, 1, 1);
	ast.createLink(Child, plus, d);
	ast.createLink(Child, plus, f);
cout << "ast size " << ast.getSize() << " ";

	//const char* args[] = {"d", "+", "f", ";"};
	const char* args2[] = {"d", "+", "f"};
	//vector<string> argVector(args, args + 4);
	vector<string> argVector2(args2, args2 + 3);
	ExpressionParser exprParser, exprParser2;
	//TNode* top = exprParser.parseExpressionForAST(argVector);
	//exprParser.updateStmtNum(1);
	TNode* top2 = exprParser2.parseExpressionForQuerying(argVector2);
	PatternMatch pattern;
	CPPUNIT_ASSERT_EQUAL_MESSAGE("this", true, pattern.checkPatternMatchAssign(plus, top2, "*"));		*/

	//Parser::initParser("test/i_src2.txt");

	
}

void PatternMatchTest::testWhile()
{
}