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

void ExpressionParserTest::testSingleNode() {
	exprParser->updateStmtNum(1);  // can pass in any dummy value for now. @todo ability to make nodes without stmt Num?
	
	const char* args[] = {"(", "x", ")"};
	vector<string> argVector(args, args + 3);

	TNode* top = exprParser->parseExpressionForAST(argVector);
	CPPUNIT_ASSERT_EQUAL(Variable, top->getNodeType());
	CPPUNIT_ASSERT_EQUAL(1, top->getNodeValueIdx());


	const char* args1[] = {"(", "(", "0", ")", ")"};
	vector<string> argVector1(args1, args1 + 5);

	top = exprParser->parseExpressionForAST(argVector1);
	CPPUNIT_ASSERT_EQUAL(Constant, top->getNodeType());
	CPPUNIT_ASSERT_EQUAL(0, top->getNodeValueIdx());

	const char* args2[] = {"7070707"};
	vector<string> argVector2(args2, args2 + 1);

	top = exprParser->parseExpressionForAST(argVector2);
	CPPUNIT_ASSERT_EQUAL(Constant, top->getNodeType());
	CPPUNIT_ASSERT_EQUAL(7070707, top->getNodeValueIdx());

	const char* args3[] = {"x"};
	vector<string> argVector3(args3, args3 + 1);

	top = exprParser->parseExpressionForAST(argVector3);
	CPPUNIT_ASSERT_EQUAL(Variable, top->getNodeType());
	CPPUNIT_ASSERT_EQUAL(1, top->getNodeValueIdx());
}

void
ExpressionParserTest::testQuery() {
	
	const char* args[] = {"i", "*", "o"}; // i and o are variables that have not been encountered before
	vector<string> argVector(args, args + 3);
	CPPUNIT_ASSERT_THROW_MESSAGE("Expression parser should throw runtime_error if a unseen variable is used", exprParser->parseExpressionForQuerying(argVector), runtime_error);
	

	VarTable* newVarTable = new VarTable();
	int iIndex = newVarTable->insertVar("i", 1);
	int oIndex = newVarTable->insertVar("o", 1);
	int nIndex = newVarTable->insertVar("n", 1);
	int longvarIndex = newVarTable->insertVar("longvar", 1);

	//  *
	// / \
	// i  o
	ExpressionParser* exprParserForQuerying = new ExpressionParser(newVarTable);  // use a vartable with test data for testing. 
																				  // for real usage, do not pass in a vartable and the expressionparser will
																				  // use the pkb singleton
	TNode* top = exprParserForQuerying->parseExpressionForQuerying(argVector);
	CPPUNIT_ASSERT_EQUAL(Times, top->getNodeType());

	TNode* leftChild = top->getChildren()->at(0);
	TNode* rightChild = top->getChildren()->at(1);

	CPPUNIT_ASSERT_EQUAL(Variable, leftChild->getNodeType());
	CPPUNIT_ASSERT_EQUAL(iIndex, leftChild->getNodeValueIdx());

	CPPUNIT_ASSERT_EQUAL(Variable, rightChild->getNodeType());
	CPPUNIT_ASSERT_EQUAL(oIndex, rightChild->getNodeValueIdx());

}

/**
 * Test that when readOnly is true, the expression parser does not write to the pkb. 
 */
void 
ExpressionParserTest::testSideEffects() {
	exprParser->updateStmtNum(1);  // can pass in any dummy value for now.
	 
	// Test 1: test that readonly mode works when the expression parser is not using the global pkb (in setUp, an empty vartable is passed into exprParser)
	const char* args[] = {"unusedx", "+", "400"}; // unusedx and unusedy are variables that have not been encountered before
	vector<string> argVector(args, args + 3);
	
	CPPUNIT_ASSERT_THROW_MESSAGE("expression parser failed to throw exception for new variable", exprParser->parseExpressionForQuerying(argVector), runtime_error);

	CPPUNIT_ASSERT_EQUAL_MESSAGE("Expression parser wrote to the UsesTable in readonly mode", 0, (int)PKB::getInstance().getUsesVarForStmt(1).size());
	PKB pkb = PKB::getInstance();
	int index = pkb.getVarIndex("unusedx");
	bool constant = pkb.isConstant(400);
	CPPUNIT_ASSERT_EQUAL_MESSAGE("Expression parser, under unit testing, is writing to VarTable in the PKB! This may be causing other unit tests to fail.", -1, index);
	CPPUNIT_ASSERT_EQUAL_MESSAGE("Expression parser, under unit testing, is writing to ConstantTable in the PKB! This may be causing other unit tests to fail.", false, constant);
	CPPUNIT_ASSERT_EQUAL(0, (int)pkb.getAllConstant().size());

	// Test 2: test when expression parser is not using the global pkb and under testing mode
	exprParser->parseExpressionForAST(argVector);

	pkb = PKB::getInstance();
	index = pkb.getVarIndex("unusedx");
	constant = pkb.isConstant(400);
	CPPUNIT_ASSERT_EQUAL_MESSAGE("Expression parser wrote to the UsesTable in unit testing", 0, (int)PKB::getInstance().getUsesVarForStmt(1).size());
	CPPUNIT_ASSERT_EQUAL_MESSAGE("Expression parser, under unit testing, is leaking side effects into the PKB! This may be causing other unit tests to fail.", -1, index);
	CPPUNIT_ASSERT_EQUAL_MESSAGE("Expression parser, under unit testing, is leaking side effects in the PKB! This may be causing other unit tests to fail.", false, constant);
	CPPUNIT_ASSERT_EQUAL(0, (int)pkb.getAllConstant().size());

	// Test 3: test that readonly mode works when the expression parser is using the global pkb
	ExpressionParser* exprParser1 = new ExpressionParser();
	CPPUNIT_ASSERT_THROW(exprParser1->parseExpressionForQuerying(argVector), runtime_error);

	index = pkb.getVarIndex("unusedx");
	constant = pkb.isConstant(400);
	CPPUNIT_ASSERT_EQUAL_MESSAGE("Expression parser wrote to the UsesTable in readonly mode", 0, (int)PKB::getInstance().getUsesVarForStmt(1).size());
	CPPUNIT_ASSERT_EQUAL_MESSAGE("Expression parser is leaking side effects into the PKB! This may be causing other unit tests to fail.", -1, index);
	CPPUNIT_ASSERT_EQUAL_MESSAGE("Expression parser is leaking side effects in the PKB! This may be causing other unit tests to fail.", false, constant);
	CPPUNIT_ASSERT_EQUAL(0, (int)pkb.getAllConstant().size());

}


void ExpressionParserTest::testSimpleCases() {
	
	exprParser->updateStmtNum(1);  // can pass in any dummy value for now. @todo ability to make nodes without stmt Num?
	
	const char* args[] = {"x", "+", "y"};
	vector<string> argVector(args, args + 3);

	TNode* top = exprParser->parseExpressionForAST(argVector);
	CPPUNIT_ASSERT_EQUAL(Plus, top->getNodeType());

	TNode* leftChild = top->getChildren()->at(0);
	CPPUNIT_ASSERT_EQUAL(Variable, leftChild->getNodeType());
	CPPUNIT_ASSERT_EQUAL(1, leftChild->getNodeValueIdx());

	TNode* rightChild = top->getChildren()->at(1);
	CPPUNIT_ASSERT_EQUAL(Variable, rightChild->getNodeType());
	CPPUNIT_ASSERT_EQUAL(2, rightChild->getNodeValueIdx());



	const char* args1[] = {"x", "+", "y", "+", "z"};
	vector<string> argVector1(args1, args1 + 5);
	
	top = exprParser->parseExpressionForAST(argVector1);
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
	//     +
	//    / \
	//   x   +
	//      / \
	//     y   z
	vector<string> argVector2(args2, args2 + 7);
	top = exprParser->parseExpressionForAST(argVector2);

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

	
	const char* args3[] = {"(", "x", "+", "y", ")", "+", "(", "z", "+", "1", ")"};
	//       + 
	//     /   \
	//    +     +
	//  / \    / \
	// x  y   z   1
	vector<string> argVector3(args3, args3 + 11);
	top = exprParser->parseExpressionForAST(argVector3);

	CPPUNIT_ASSERT_EQUAL(Plus, top->getNodeType());

	leftChild = top->getChildren()->at(0);
	CPPUNIT_ASSERT_EQUAL(Plus, leftChild->getNodeType());

	rightChild = top->getChildren()->at(1);
	CPPUNIT_ASSERT_EQUAL(Plus, rightChild->getNodeType());
	
	
	TNode* leftLeftChild = leftChild->getChildren()->at(0);
	TNode* leftRightChild = leftChild->getChildren()->at(1);
	TNode* rightLeftChild = rightChild->getChildren()->at(0);
	TNode* rightRightChild = rightChild->getChildren()->at(1);


	CPPUNIT_ASSERT_EQUAL(Variable, leftLeftChild->getNodeType());
	CPPUNIT_ASSERT_EQUAL(1, leftLeftChild->getNodeValueIdx());
	CPPUNIT_ASSERT_EQUAL(Variable, leftRightChild->getNodeType());
	CPPUNIT_ASSERT_EQUAL(2, leftRightChild->getNodeValueIdx());
	CPPUNIT_ASSERT_EQUAL(Variable, rightLeftChild->getNodeType());
	CPPUNIT_ASSERT_EQUAL(3, rightLeftChild->getNodeValueIdx());
	CPPUNIT_ASSERT_EQUAL(Constant, rightRightChild->getNodeType());
	CPPUNIT_ASSERT_EQUAL(1, rightRightChild->getNodeValueIdx());


	const char* args4[] = {"x", "+", "y", "*", "z", };
	//     +
	//    / \
	//   x   *
	//      / \
	//     y   z
	vector<string> argVector4(args4, args4 + 5);
	top = exprParser->parseExpressionForAST(argVector4);

	CPPUNIT_ASSERT_EQUAL(Plus, top->getNodeType());

	leftChild = top->getChildren()->at(0);
	CPPUNIT_ASSERT_EQUAL(Variable, leftChild->getNodeType());
	CPPUNIT_ASSERT_EQUAL(1, leftChild->getNodeValueIdx());

	rightChild = top->getChildren()->at(1);
	CPPUNIT_ASSERT_EQUAL(Times, rightChild->getNodeType());
	
	
	leftMostChild = rightChild->getChildren()->at(0);
	rightMostChild = rightChild->getChildren()->at(1);
	CPPUNIT_ASSERT_EQUAL(Variable, leftMostChild->getNodeType());
	CPPUNIT_ASSERT_EQUAL(2, leftMostChild->getNodeValueIdx());
	CPPUNIT_ASSERT_EQUAL(Variable, rightMostChild->getNodeType());
	CPPUNIT_ASSERT_EQUAL(3, rightMostChild->getNodeValueIdx());

	const char* args5[] = {"x", "-", "y", "*", "z", };
	//     -
	//    / \
	//   x   *
	//      / \
	//     y   z
	vector<string> argVector5(args5, args5 + 5);
	top = exprParser->parseExpressionForAST(argVector5);

	CPPUNIT_ASSERT_EQUAL(Minus, top->getNodeType());

	leftChild = top->getChildren()->at(0);
	CPPUNIT_ASSERT_EQUAL(Variable, leftChild->getNodeType());
	CPPUNIT_ASSERT_EQUAL(1, leftChild->getNodeValueIdx());

	rightChild = top->getChildren()->at(1);
	CPPUNIT_ASSERT_EQUAL(Times, rightChild->getNodeType());
	
	
	leftMostChild = rightChild->getChildren()->at(0);
	rightMostChild = rightChild->getChildren()->at(1);
	CPPUNIT_ASSERT_EQUAL(Variable, leftMostChild->getNodeType());
	CPPUNIT_ASSERT_EQUAL(2, leftMostChild->getNodeValueIdx());
	CPPUNIT_ASSERT_EQUAL(Variable, rightMostChild->getNodeType());
	CPPUNIT_ASSERT_EQUAL(3, rightMostChild->getNodeValueIdx());

	const char* args6[] = {"(", "x", "-", "y", ")", "*", "z", };
	//     *
	//    / \
	//   -   z
	//  / \
	// x   y
	vector<string> argVector6(args6, args6 + 7);
	top = exprParser->parseExpressionForAST(argVector6);

	CPPUNIT_ASSERT_EQUAL(Times, top->getNodeType());

	leftChild = top->getChildren()->at(0);
	CPPUNIT_ASSERT_EQUAL(Minus, leftChild->getNodeType());

	rightChild = top->getChildren()->at(1);
	CPPUNIT_ASSERT_EQUAL(Variable, rightChild->getNodeType());
	CPPUNIT_ASSERT_EQUAL(3, rightChild->getNodeValueIdx());
	
	
	leftMostChild = leftChild->getChildren()->at(0);
	rightMostChild = leftChild->getChildren()->at(1);
	CPPUNIT_ASSERT_EQUAL(Variable, leftMostChild->getNodeType());
	CPPUNIT_ASSERT_EQUAL(1, leftMostChild->getNodeValueIdx());
	CPPUNIT_ASSERT_EQUAL(Variable, rightMostChild->getNodeType());
	CPPUNIT_ASSERT_EQUAL(2, rightMostChild->getNodeValueIdx());

	return;

}


void ExpressionParserTest::testComplexCases() {
	exprParser->updateStmtNum(1);  // can pass in any dummy value for now. @todo ability to make nodes without stmt Num?
	
	//      +
	//     / \
	//    +   3
	//   / \
	//  +   z
	// /\
	//x  y
	char* args[] = {"x", "+", "y", "+", "z", "+", "3"};
	vector<string> argVector(args, args + 7);

	TNode* top = exprParser->parseExpressionForAST(argVector);

	CPPUNIT_ASSERT_EQUAL(Plus, top->getNodeType());

	TNode* leftChild = top->getChildren()->at(0);
	CPPUNIT_ASSERT_EQUAL(Plus, leftChild->getNodeType());

	TNode* rightChild = top->getChildren()->at(1);
	CPPUNIT_ASSERT_EQUAL(Constant, rightChild->getNodeType());
	CPPUNIT_ASSERT_EQUAL(3, rightChild->getNodeValueIdx());

	TNode* leftLeftChild = leftChild->getChildren()->at(0);
	CPPUNIT_ASSERT_EQUAL(Plus, leftLeftChild->getNodeType());

	TNode* leftRightChild = leftChild->getChildren()->at(1);
	CPPUNIT_ASSERT_EQUAL(Variable, leftRightChild->getNodeType());
	CPPUNIT_ASSERT_EQUAL(3, leftRightChild->getNodeValueIdx());

	TNode* leftLeftLeftChild = leftLeftChild->getChildren()->at(0);
	CPPUNIT_ASSERT_EQUAL(Variable, leftLeftLeftChild->getNodeType());
	CPPUNIT_ASSERT_EQUAL(1, leftLeftLeftChild->getNodeValueIdx());

	TNode* leftLeftRightChild = leftLeftChild->getChildren()->at(1);
	CPPUNIT_ASSERT_EQUAL(Variable, leftLeftRightChild->getNodeType());
	CPPUNIT_ASSERT_EQUAL(2, leftLeftRightChild->getNodeValueIdx());


	//      +
	//     / \
	//    +   3
	//   / \
	//  *   z
	// /\
	//x  y
	char* args1[] = {"(", "x", "*", "y", ")", "+", "z", "+", "(", "3", ")"};
	vector<string> argVector1(args1, args1 + 11);

	top = exprParser->parseExpressionForAST(argVector1);

	CPPUNIT_ASSERT_EQUAL(Plus, top->getNodeType());

	leftChild = top->getChildren()->at(0);
	CPPUNIT_ASSERT_EQUAL(Plus, leftChild->getNodeType());

	rightChild = top->getChildren()->at(1);
	CPPUNIT_ASSERT_EQUAL(Constant, rightChild->getNodeType());
	CPPUNIT_ASSERT_EQUAL(3, rightChild->getNodeValueIdx());

	leftLeftChild = leftChild->getChildren()->at(0);
	CPPUNIT_ASSERT_EQUAL(Times, leftLeftChild->getNodeType());

	leftRightChild = leftChild->getChildren()->at(1);
	CPPUNIT_ASSERT_EQUAL(Variable, leftRightChild->getNodeType());
	CPPUNIT_ASSERT_EQUAL(3, leftRightChild->getNodeValueIdx());

	leftLeftLeftChild = leftLeftChild->getChildren()->at(0);
	CPPUNIT_ASSERT_EQUAL(Variable, leftLeftLeftChild->getNodeType());
	CPPUNIT_ASSERT_EQUAL(1, leftLeftLeftChild->getNodeValueIdx());

	leftLeftRightChild = leftLeftChild->getChildren()->at(1);
	CPPUNIT_ASSERT_EQUAL(Variable, leftLeftRightChild->getNodeType());
	CPPUNIT_ASSERT_EQUAL(2, leftLeftRightChild->getNodeValueIdx());


	//@todo refactor somehow, if possible

	//           *
	//       /      \
	//   150          -
	//               /  \
	//              +     z
	//            /  \ 
	//           x    *
	//               / \
	//              *   x 
	//             / \
	//           x    y
	//
	char* args2[] = {"150", "*", "(", "x", "+", "x", "*", "y", "*", "x", "-", "z" ,")"};
	vector<string> argVector2(args2, args2 + 13);

	top = exprParser->parseExpressionForAST(argVector2);
	 
	CPPUNIT_ASSERT_EQUAL(Times, top->getNodeType());                                 // topmost layer

	leftChild = top->getChildren()->at(0);						//    *                 //2nd layer
	CPPUNIT_ASSERT_EQUAL(Constant, leftChild->getNodeType());   //    ^ 
	CPPUNIT_ASSERT_EQUAL(150, leftChild->getNodeValueIdx());    // 150  -

	rightChild = top->getChildren()->at(1);                     
	CPPUNIT_ASSERT_EQUAL(Minus, rightChild->getNodeType());     

	TNode* rightLeftChild = rightChild->getChildren()->at(0);                            //3rd layer
	CPPUNIT_ASSERT_EQUAL(Plus, rightLeftChild->getNodeType());

	TNode* rightRightChild = rightChild->getChildren()->at(1);
	CPPUNIT_ASSERT_EQUAL(Variable, rightRightChild->getNodeType());
	CPPUNIT_ASSERT_EQUAL(3, rightRightChild->getNodeValueIdx());

	TNode* rightLeftLeftChild = rightLeftChild->getChildren()->at(0);                    // 4th layer
	CPPUNIT_ASSERT_EQUAL(Variable, rightLeftLeftChild->getNodeType());
	CPPUNIT_ASSERT_EQUAL(1, rightLeftLeftChild->getNodeValueIdx());

	TNode* rightLeftRightChild = rightLeftChild->getChildren()->at(1);
	CPPUNIT_ASSERT_EQUAL(Times, rightLeftRightChild->getNodeType());

	TNode* rightLeftRightLeftChild = rightLeftRightChild->getChildren()->at(0);           // 2nd lowest layer
	CPPUNIT_ASSERT_EQUAL(Times, rightLeftRightLeftChild->getNodeType());
	
	TNode* rightLeftRightRightChild = rightLeftRightChild->getChildren()->at(1);
	CPPUNIT_ASSERT_EQUAL(Variable, rightLeftRightRightChild->getNodeType());
	CPPUNIT_ASSERT_EQUAL(1, rightLeftRightRightChild->getNodeValueIdx());
	
	TNode* rightLeftRightLeftLeftChild = rightLeftRightLeftChild->getChildren()->at(0);   // lowest layer
	CPPUNIT_ASSERT_EQUAL(Variable, rightLeftRightLeftLeftChild->getNodeType());
	CPPUNIT_ASSERT_EQUAL(1, rightLeftRightLeftLeftChild->getNodeValueIdx());

	TNode* rightLeftRightLeftRightChild = rightLeftRightLeftChild->getChildren()->at(1);
	CPPUNIT_ASSERT_EQUAL(Variable, rightLeftRightLeftRightChild->getNodeType());
	CPPUNIT_ASSERT_EQUAL(2, rightLeftRightLeftRightChild->getNodeValueIdx());
	
}