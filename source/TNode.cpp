#include <stdio.h>
#include <iostream>
#include <string>
#include <vector>
#include <algorithm>

#include "TNode.h"

TNode::TNode() 
{}

/**
 * Return a TNode with parameters as follows, node type, the corresponding statement number and the corresponding value. 
 * If ast_node_type is Variable, then value is the INDEX of the variable in the VarTable. 
 * If ast_node_type is Constant, then value is the value of the CONSTANT. 
 * If ast_node_type is neither Variable nor Constant, then value is not a meaningful value.
 * @exception if stmtNo is negative or 0.
 */
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
	_parent = NULL;
}

/**
 * Return a TNode with parameters as follows, node type, the corresponding statement number and the corresponding parent TNode. 
 * If ast_node_type is Variable, then value is the INDEX of the variable in the VarTable. 
 * If ast_node_type is Constant, then value is the value of the CONSTANT. 
 * If ast_node_type is neither Variable nor Constant, then value is not a meaningful value.
 * @exception if stmtNo is negative or 0.
 */
TNode::TNode(TNODE_TYPE ast_node_type, int stmtNo, int value, TNode* parent) 
{

	_nodeType = ast_node_type;
	_stmtNumber = stmtNo;
	_nodeValueIdx = value;
	_parent = parent;
	_leftSibling = _rightSibling = NULL;
}

/**
 * Return the statement number.
 */
int TNode::getStmtNumber() 
{
	return _stmtNumber;
}

/**
 * Return the value of the node. 
 * If the node is a Variable node, then value is the INDEX of the variable in the VarTable. 
 * If the node is a Constant node, then value is a CONSTANT. 
 * If the node is neither a Variable node or Constant node, then value is not a meaningful value.
 */
int TNode::getNodeValueIdx() 
{
	return _nodeValueIdx;
}


/**
 * Return the type of the TNode.
 */
TNODE_TYPE TNode::getNodeType() 
{
	return _nodeType;
}

/**
 * Return the parent TNode.
 */
TNode* TNode::getParent() 
{
	return _parent;
}

/**
 * Add node to the existing list of child nodes.	
 * @exception if node is NULL.
 */
void TNode::addChild(TNode* node) 
{
	_children.push_back(node);
}

/**
 * Return TRUE if the current node has any child node. Otherwise, return FALSE.
 */
bool TNode::hasChild() 
{
	if(_children.size() < 1) return false;
	else return true;
}

/**
 * Return a list of children nodes from the current node.	 	
 * If there is no answer, return an empty list.
 */
vector<TNode*>* TNode::getChildren() 
{
	return &_children;
}

/**
 * Add node as the left sibling of the current node.
 * @exception if node is NULL.
 */
void TNode::addLeftSibling(TNode* node) 
{
	_leftSibling = node;	
}

/**
 * Add node as the right sibling of the current node.
 * @exception if node is NULL.
 */
void TNode::addRightSibling(TNode* node) 
{
	_rightSibling = node;
}

/**
 * Return TRUE if the current node has a left sibling. Otherwise, return FALSE.
 */
bool TNode::hasLeftSibling() 
{
	if(_leftSibling != NULL) return true;
	else return false;
}

/**
 * Return TRUE if the current node has a right sibling. Otherwise, return FALSE.
 */
bool TNode::hasRightSibling() 
{
	if(_rightSibling != NULL) return true;
	else return false;
}

/**
 * Return the left sibling of the current node.
 */
TNode* TNode::getLeftSibling() 
{
	return _leftSibling;
}

/**
 * Return the right sibling of the current node.
 */
TNode* TNode::getRightSibling() 
{
	return _rightSibling;
}

/**
 * Set the parent node for current node.
 * @exception if parent is NULL.
 */
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