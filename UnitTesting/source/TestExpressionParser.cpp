#include <cppunit/config/SourcePrefix.h>
#include "ExpressionParser.h"
#include "TestExpressionParser.h"
#include "TNode.h"
#include "PKB.h"

#include <iostream>
#include <string>
#include <fstream>

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION( ExpressionParserTest ); 

void 
ExpressionParserTest::setUp() {
	VarTable* vartable = new VarTable();
	exprParser = new ExpressionParser(vartable);
}

void 
ExpressionParserTest::tearDown() {
	delete exprParser;
}

void ExpressionParserTest::testInit() {
	
	exprParser->updateStmtNum(1);  // can pass in any dummy value for now. @Todo ability to make nodes without stmt Num?
	
	const char* args[] = {"x", "+", "y"};
	vector<string> argVector(args, args + 3);
	exprParser->updateBuffer(argVector);
	
	TNode* top = exprParser->parse();
	CPPUNIT_ASSERT_EQUAL(Plus, top->getNodeType());

	TNode* leftChild = top->getChildren()->at(0);
	CPPUNIT_ASSERT_EQUAL(Variable, leftChild->getNodeType());
	CPPUNIT_ASSERT_EQUAL(1, leftChild->getNodeValueIdx());

	TNode* rightChild = top->getChildren()->at(1);
	CPPUNIT_ASSERT_EQUAL(Variable, rightChild->getNodeType());
	CPPUNIT_ASSERT_EQUAL(2, rightChild->getNodeValueIdx());



	const char* args1[] = {"x", "+", "y", "+", "z"};
	vector<string> argVector1(args1, args1 + 5);
	exprParser->updateBuffer(argVector1);

	top = exprParser->parse();
	CPPUNIT_ASSERT_EQUAL(Plus, top->getNodeType());

	leftChild = top->getChildren()->at(0);
	CPPUNIT_ASSERT_EQUAL(Plus, leftChild->getNodeType());

	rightChild = top->getChildren()->at(1);
	CPPUNIT_ASSERT_EQUAL(Variable, rightChild->getNodeType());
	CPPUNIT_ASSERT_EQUAL(3, rightChild->getNodeValueIdx());
	
	TNode* leftMostChild = leftChild->getChildren()->at(0);
	TNode* rightMostChild = leftChild->getChildren()->at(1);
	CPPUNIT_ASSERT_EQUAL(Variable, leftMostChild->getNodeType());
	CPPUNIT_ASSERT_EQUAL(1, leftMostChild->getNodeValueIdx());
	CPPUNIT_ASSERT_EQUAL(Variable, rightMostChild->getNodeType());
	CPPUNIT_ASSERT_EQUAL(2, rightMostChild->getNodeValueIdx());

	
	const char* args2[] = {"x", "+", "(", "y", "+", "z", ")"};
	vector<string> argVector2(args2, args2 + 7);
	exprParser->updateBuffer(argVector2);

	top = exprParser->parse();
	CPPUNIT_ASSERT_EQUAL(Plus, top->getNodeType());

	leftChild = top->getChildren()->at(0);
	CPPUNIT_ASSERT_EQUAL(Variable, leftChild->getNodeType());
	CPPUNIT_ASSERT_EQUAL(1, leftChild->getNodeValueIdx());

	rightChild = top->getChildren()->at(1);
	CPPUNIT_ASSERT_EQUAL(Plus, rightChild->getNodeType());
	
	
	leftMostChild = rightChild->getChildren()->at(0);
	rightMostChild = rightChild->getChildren()->at(1);
	CPPUNIT_ASSERT_EQUAL(Variable, leftMostChild->getNodeType());
	CPPUNIT_ASSERT_EQUAL(2, leftMostChild->getNodeValueIdx());
	CPPUNIT_ASSERT_EQUAL(Variable, rightMostChild->getNodeType());
	CPPUNIT_ASSERT_EQUAL(3, rightMostChild->getNodeValueIdx());

	return;
}
