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
	ROOT, RESULT, CLAUSES,

	// Query QNODE_TYPEs
	ModifiesP, ModifiesS,
	UsesP, UsesS,
	Calls, CallsT,
	Parent, ParentT,
	Follows, FollowsT,
	Next, NextT,
	Affects, AffectsT,
	AttrCompare,

	// Select and Pattern QNODE_TYPEs
	Selection, Pattern, With
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

