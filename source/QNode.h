#pragma once

//Node for the query tree
#include <string>
#include "Synonym.h"
#include <vector>
#include <iostream>

using std::string;
using std::vector;

enum QNODE_TYPE
{
	// Default QueryTree Placeholder QNODE_TYPEs
	ROOT, RESULT, SUCHTHAT, PATTERN, WITH,

	// Query QNODE_TYPEs
	Affects, AffectsS,
	Next, NextS,
	Modifies,
	Calls, CallsS,
	Uses, UsesS, 
	Parent, ParentS, 
	Follows, FollowsS,

	// Select and Pattern QNODE_TYPEs
	Selection, Pattern
};

class QNode
{
public:
	QNode();
	QNode(QNODE_TYPE QNODE_TYPE,Synonym arg0,Synonym arg1, Synonym arg2);
	Synonym getArg0();
	Synonym getArg1();
	Synonym getArg2();
	QNODE_TYPE QNode::getNodeType();
	void setParent(QNode* parent_node);
	void setChild(QNode* child_node);
	int getNumberOfChildren();
	QNode* getParent();
	QNode* getChild();
	QNode* getChild(int index);
	QNode* getNextChild();
	QNode* getPreviousChild();

private:
	QNODE_TYPE _QNODE_TYPE;
	Synonym _arg0;
	Synonym _arg1;
	Synonym _arg2;
	QNode* _parent;
	vector<QNode*> _child_list;
	int _current_child_index;
};

