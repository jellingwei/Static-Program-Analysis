#pragma once

#include <stdio.h>
#include <iostream>
#include <vector>
#include <string>
#include <utility>
using std::string;
using namespace std;

enum TNODE_TYPE
{
	Procedure, Assign, Plus, Variable, StmtLst, While, If, Constant
};

enum LINK_TYPE
{
	Par, Right_Sibling, Left_Sibling, Child
};

class TNode 
{
public:
	TNode();
	TNode(TNODE_TYPE ast_node_type, int stmtNo, int idx);
	TNode(TNODE_TYPE ast_node_type, int stmtNo, int idx, TNode* parent);
	int getStmtNumber();
	int getNodeValueIdx();
	TNODE_TYPE getNodeType();
	TNode* getParent();

	void addChild(TNode* node);
	bool hasChild();
	vector<TNode*>* getChildren();

	void addLeftSibling(TNode* node);
	void addRightSibling(TNode* node);
	bool hasLeftSibling();
	bool hasRightSibling();
	TNode* getLeftSibling();
	TNode* getRightSibling();

	void setParent(TNode* parent);

private:
	TNODE_TYPE _nodeType;
	int _stmtNumber;
	int _nodeValueIdx;
	TNode* _parent;
	vector<TNode*> _children;
	TNode* _leftSibling;
	TNode* _rightSibling;
};