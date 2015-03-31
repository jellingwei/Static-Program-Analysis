#pragma once

//Node for the query tree
#include <string>
#include <vector>
#include <iostream>
#include "common.h"
#include "Synonym.h"

using std::string;
using std::vector;

class QNode
{
public:
	QNode();
	QNode(QNODE_TYPE QNODE_TYPE,Synonym arg0,Synonym arg1, Synonym arg2);
	Synonym getArg0();
	Synonym getArg1();
	Synonym getArg2();
	void setArg0(Synonym);
	void setArg1(Synonym);
	void setArg2(Synonym);

	QNODE_TYPE QNode::getNodeType();
	void setParent(QNode* parent_node);
	void setChild(QNode* child_node);
	void setDirection(DIRECTION direction);

	DIRECTION getDirection();
	int getNumberOfChildren();
	QNode* getParent();
	QNode* getChild();
	QNode* getChild(int index);
	QNode* getNextChild();
	QNode* getPreviousChild();

private:
	QNODE_TYPE _QNODE_TYPE;
	DIRECTION _direction;
	Synonym _arg0;
	Synonym _arg1;
	Synonym _arg2;
	QNode* _parent;
	vector<QNode*> _child_list;
	int _current_child_index;
};

