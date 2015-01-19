#include <stdio.h>
#include <iostream>
#include <string>
#include <vector>
#include <algorithm>

#include "TNode.h"




TNode::TNode() 
{}

TNode::TNode(TNODE_TYPE ast_node_type, int stmtNo, int value) 
{
	/*if(stmtNo <= 0) 
	{
		throw exception("TNode error: Negative statment number");
	} else if(value <= 0) 
	{
		throw exception("TNode error: Negative value");
	} else if(ast_node_type!=Procedure || ast_node_type!=Assign || ast_node_type!=Plus || ast_node_type!=Variable || ast_node_type!=StmtLst || ast_node_type!=While || ast_node_type!=Constant) 
	{
		throw exception("TNode error: Invalid TNode_Type");
	}*/

	_nodeType = ast_node_type;
	_stmtNumber = stmtNo;
	_nodeValueIdx = value;
	_leftSibling = _rightSibling = NULL;
}

TNode::TNode(TNODE_TYPE ast_node_type, int stmtNo, int value, TNode* parent) 
{

	_nodeType = ast_node_type;
	_stmtNumber = stmtNo;
	_nodeValueIdx = value;
	_parent = parent;
	_leftSibling = _rightSibling = NULL;
}

int TNode::getStmtNumber() 
{
	return _stmtNumber;
}

int TNode::getNodeValueIdx() 
{
	return _nodeValueIdx;
}

TNODE_TYPE TNode::getNodeType() 
{
	return _nodeType;
}

TNode* TNode::getParent() 
{
	return _parent;
}

void TNode::addChild(TNode* node) 
{
	_children.push_back(node);
}

bool TNode::hasChild() 
{
	if(_children.size() < 1) return false;
	else return true;
}

vector<TNode*>* TNode::getChildren() 
{
	return &_children;
}

void TNode::addLeftSibling(TNode* node) 
{
	_leftSibling = node;	
}

void TNode::addRightSibling(TNode* node) 
{
	_rightSibling = node;
}

bool TNode::hasLeftSibling() 
{
	if(_leftSibling != NULL) return true;
	else return false;
}

bool TNode::hasRightSibling() 
{
	if(_rightSibling != NULL) return true;
	else return false;
}

TNode* TNode::getLeftSibling() 
{
	return _leftSibling;
}

TNode* TNode::getRightSibling() 
{
	return _rightSibling;
}

void TNode::setParent(TNode* parent) 
{
	_parent = parent;
}


std::ostream& operator<<(std::ostream &strm, const TNode &node) 
{
	TNODE_TYPE expr[] = {Plus, Variable, Constant}; // @Todo add times, subtract, etc

	auto res = find(begin(expr), end(expr), node._nodeType);
	bool nodeIsInExpr = (res != end(expr));

	if (nodeIsInExpr) 
	{
		if (node._nodeType == Variable || node._nodeType == Constant) 
		{
			string type = node._nodeType == Variable ? "v" : "c";
			string value = to_string(static_cast<long long>(node._nodeValueIdx));
			return strm << type << value;
		}

		string oper;
		switch (node._nodeType) 
		{
			case (Plus):
				oper = "+";
				break;
			default:
				break;
		}
		
		TNode* leftChild = node._children[0];
		TNode* rightChild = node._children[1];
		return strm << *leftChild <<  " " << *rightChild << " " << oper;
	}
	else 
	{
		string type;
		switch (node._nodeType) 
		{
		case (While):
			type = "while";
			break;
		case (If):
			type = "if";
			break;
		case (Assign):
			type = "assign";
			break;
		default:
			break;

		}
		return strm << "#" << node._stmtNumber << ": " << type;

	}

}