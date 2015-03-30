#pragma once

#include <stdio.h>
#include <iostream>
#include <vector>
#include <string>
#include <utility>
#include "common.h"
#include "common_list.h"

using std::string;
using namespace std;

class TNode 
{
public:
	TNode();
	TNode(TNODE_TYPE ast_node_type, STATEMENT stmtNum, VALUE value);
	TNode(TNODE_TYPE ast_node_type, STATEMENT stmtNum, VALUE value, TNode* parent);
	STATEMENT getStmtNumber();
	VAR_INDEX getNodeValueIdx();
	TNODE_TYPE getNodeType();
	TNode* getParent();

	void addChild(TNode* node);
	BOOLEAN_ hasChild();
	TNODE_LIST getChildren();

	void addLeftSibling(TNode* node);
	void addRightSibling(TNode* node);
	BOOLEAN_ hasLeftSibling();
	BOOLEAN_ hasRightSibling();
	TNode* getLeftSibling();
	TNode* getRightSibling();

	void setParent(TNode* parent);
	
	void increaseDescendent(INTEGER toAdd);
	int getDescendent();

private:
	TNODE_TYPE _nodeType;
	int _stmtNumber;
	int _nodeValueIdx;
	TNode* _parent;
	vector<TNode*> _children;
	TNode* _leftSibling;
	TNode* _rightSibling;
	int _descendents;

	friend std::ostream& operator<<(std::ostream&, const TNode&);

};