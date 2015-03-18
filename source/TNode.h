#pragma once

#include <stdio.h>
#include <iostream>
#include <vector>
#include <string>
#include <utility>
#include "common.h"
using std::string;
using namespace std;

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
	
	void increaseDescendent(int toAdd);
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