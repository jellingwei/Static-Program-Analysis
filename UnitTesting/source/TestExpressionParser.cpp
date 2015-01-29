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

	TNode* top = exprParser->parseExpression(argVector);
	CPPUNIT_ASSERT_EQUAL(Variable, top->getNodeType());
	CPPUNIT_ASSERT_EQUAL(1, top->getNodeValueIdx());


	const char* args1[] = {"(", "(", "0", ")", ")"};
	vector<string> argVector1(args1, args1 + 5);

	top = exprParser->parseExpression(argVector1);
	CPPUNIT_ASSERT_EQUAL(Constant, top->getNodeType());
	CPPUNIT_ASSERT_EQUAL(0, top->getNodeValueIdx());

	const char* args2[] = {"7070707"};
	vector<string> argVector2(args2, args2 + 1);

	top = exprParser->parseExpression(argVector2);
	CPPUNIT_ASSERT_EQUAL(Constant, top->getNodeType());
	CPPUNIT_ASSERT_EQUAL(7070707, top->getNodeValueIdx());

	const char* args3[] = {"x"};
	vector<string> argVector3(args3, args3 + 1);

	top = exprParser->parseExpression(argVector3);
	CPPUNIT_ASSERT_EQUAL(Variable, top->getNodeType());
	CPPUNIT_ASSERT_EQUAL(1, top->getNodeValueIdx());
}

void ExpressionParserTest::testSimpleCases() {
	
	exprParser->updateStmtNum(1);  // can pass in any dummy value for now. @todo ability to make nodes without stmt Num?
	
	const char* args[] = {"x", "+", "y"};
	vector<string> argVector(args, args + 3);

	TNode* top = exprParser->parseExpression(argVector);
	CPPUNIT_ASSERT_EQUAL(Plus, top->getNodeType());

	TNode* leftChild = top->getChildren()->at(0);
	CPPUNIT_ASSERT_EQUAL(Variable, leftChild->getNodeType());
	CPPUNIT_ASSERT_EQUAL(1, leftChild->getNodeValueIdx());

	TNode* rightChild = top->getChildren()->at(1);
	CPPUNIT_ASSERT_EQUAL(Variable, rightChild->getNodeType());
	CPPUNIT_ASSERT_EQUAL(2, rightChild->getNodeValueIdx());



	const char* args1[] = {"x", "+", "y", "+", "z"};
	vector<string> argVector1(args1, args1 + 5);
	
	top = exprParser->parseExpression(argVector1);
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

	
	const char* args3[] = {"(", "x", "+", "y", ")", "+", "(", "z", "+", "1", ")"};
	//       + 
	//     /   \
	//    +     +
	//  / \    / \
	// x  y   z   1
	vector<string> argVector3(args3, args3 + 11);
	exprParser->updateBuffer(argVector3);

	top = exprParser->parse();
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
	exprParser->updateBuffer(argVector4);

	top = exprParser->parse();
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
	exprParser->updateBuffer(argVector5);

	top = exprParser->parse();
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
	exprParser->updateBuffer(argVector6);

	top = exprParser->parse();
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

	TNode* top = exprParser->parseExpression(argVector);

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

	top = exprParser->parseExpression(argVector1);

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

	top = exprParser->parseExpression(argVector2);
	 
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