#include <cppunit/config/SourcePrefix.h>

#include "TestAST.h"
#include "AST.h"
#include "TNode.h"

#include <vector>

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION( ASTTest ); 

void ASTTest::setUp() {
}

void ASTTest::testInit() {
	AST ast;
	TNode* root = ast.getRoot();

	// Test AST Constructor - Procedure and StmtLst Nodes are already initialised
	CPPUNIT_ASSERT_EQUAL(2, ast.getSize());
	CPPUNIT_ASSERT_EQUAL_MESSAGE("Test Root Node Type", StmtLst, root->getNodeType());
}

void ASTTest::testTNode() {
	AST ast;
	TNode *assignNode = ast.createTNode(Assign, 1, 1);
	TNode *whileNode = ast.createTNode(While, 2, 2);
	TNode *varNode = ast.createTNode(Variable, 3, 3);
	TNode *plusNode = ast.createTNode(Plus, 4, 4);
	TNode *constNode = ast.createTNode(Constant, 5, 5);

	CPPUNIT_ASSERT_EQUAL_MESSAGE("Test Assignment Node Type", Assign, assignNode->getNodeType());
	CPPUNIT_ASSERT_EQUAL(1, assignNode->getStmtNumber());
	CPPUNIT_ASSERT_EQUAL_MESSAGE("Test While Node Type", While, whileNode->getNodeType());
	CPPUNIT_ASSERT_EQUAL(2, whileNode->getNodeValueIdx());

	// Test For Children Node(s)
	CPPUNIT_ASSERT_EQUAL_MESSAGE("Test Variable Node Type", Variable, varNode->getNodeType());
	ast.createLink(Child, assignNode, varNode);
	vector<TNode*> *childrenList = assignNode->getChildren();
	int childrenListSize = childrenList->size();
	CPPUNIT_ASSERT_EQUAL_MESSAGE("Test Assignment Node Children Size", 1, childrenListSize);

	// Test For Parent Node
	TNode* childNode = childrenList->at(0);
	TNode* parentNode = childNode->getParent();
	CPPUNIT_ASSERT_EQUAL_MESSAGE("Test Parent Node", Assign, parentNode->getNodeType());

	// Test For Sibling Nodes
	CPPUNIT_ASSERT_EQUAL_MESSAGE("Test Plus Type", Plus, plusNode->getNodeType());
	CPPUNIT_ASSERT_EQUAL_MESSAGE("Test Constant Node Type", Constant, constNode->getNodeType());
	ast.createLink(Right_Sibling, plusNode, constNode);
	TNode* rightSibling = plusNode->getRightSibling();
	CPPUNIT_ASSERT_EQUAL_MESSAGE("Test Right Sibling of Plus Node", Constant, rightSibling->getNodeType());
	TNode* leftSibling = rightSibling->getLeftSibling();
	CPPUNIT_ASSERT_EQUAL_MESSAGE("Test Left Sibling of Constant Node", Plus, leftSibling->getNodeType());

}

void ASTTest::testASTTree() {
	AST ast;
	TNode *newNode = ast.createTNode(Assign, 1, 1);

	// Test For Size
	CPPUNIT_ASSERT_EQUAL(3, ast.getSize());

	AST ast2;
	TNode *newNode2 = ast2.createTNode(Assign, 2, 2);
	
	// Test If AST Tree Contains Node
	CPPUNIT_ASSERT_EQUAL(false, ast.isExists(newNode2));
	CPPUNIT_ASSERT_EQUAL(true, ast.isExists(newNode));

	// Test For Last Important Node
	TNode *newNode3 = ast.createTNode(While, 3, 3);
	CPPUNIT_ASSERT_EQUAL_MESSAGE("Test Last Important Node", newNode3, ast.getLastImpt());

	// Test For Child-Parent Node Relation
	TNode *newNode4 = ast.createTNode(Plus, 1, 1);
	ast.createLink(Child, newNode, newNode4);
	CPPUNIT_ASSERT_EQUAL_MESSAGE("Test Child-Parent Node Relation", true, ast.isChildNode(newNode, newNode4));
	CPPUNIT_ASSERT_EQUAL_MESSAGE("Test Child-Parent Node Relation", false, ast.isChildNode(newNode4, newNode));
	CPPUNIT_ASSERT_EQUAL_MESSAGE("Test Child-Parent Node Relation", false, ast.isChildNode(newNode, newNode3));

}

void ASTTest::testPatternMatch() {
	AST ast;
	// Create a = x+y+z+1
	TNode *assignNode = ast.createTNode(Assign, 1, 1);



}
