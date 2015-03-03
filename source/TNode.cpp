#include <stdio.h>
#include <iostream>
#include <string>
#include <vector>
#include <algorithm>

#include "TNode.h"

TNode::TNode() 
{}

/**
 * @param astNodeType the type of Tnode which can be Procedure/Assign/Plus/Variable/StmtLst/While/If/Constant/Minus/Times/Program/Call
 * @param stmtNum the statement number of the TNode
 * @param value a number which can be the INDEX of the variable or the value of the CONSTANT
 * @return a TNode with its node type, the corresponding statement number and the corresponding value. 
 * If astNodeType is Variable, then value is the INDEX of the variable in the VarTable. 
 * If astNodeType is Constant, then value is the value of the CONSTANT. 
 * If astNodeType is neither Variable nor Constant, then value is not a meaningful value.
 * @exception if stmtNum is negative or 0.
 */
TNode::TNode(TNODE_TYPE astNodeType, int stmtNum, int value) 
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

	_nodeType = astNodeType;
	_stmtNumber = stmtNum;
	_nodeValueIdx = value;
	_leftSibling = _rightSibling = NULL;
	_parent = NULL;
}

/**
 * @param astNodeType the type of Tnode which can Procedure/Assign/Plus/Variable/StmtLst/While/If/Constant/Minus/Times/Program/Call
 * @param stmtNum the statement number of the TNode
 * @param value a number which can be the INDEX of the variable or the value of the CONSTANT
 * @param parent a TNode indicating the parent of this particular TNode
 * @return a TNode with its node type, the corresponding statement number, value and the parent TNode. 
 * If astNodeType is Variable, then value is the INDEX of the variable in the VarTable. 
 * If astNodeType is Constant, then value is the value of the CONSTANT. 
 * If astNodeType is neither Variable nor Constant, then value is not a meaningful value.
 * @exception if stmtNum is negative or 0.
 */
TNode::TNode(TNODE_TYPE astNodeType, int stmtNum, int value, TNode* parent) 
{

	_nodeType = astNodeType;
	_stmtNumber = stmtNum;
	_nodeValueIdx = value;
	_parent = parent;
	_leftSibling = _rightSibling = NULL;
}

/**
 * @return the statement number of the TNode.
 */
int TNode::getStmtNumber() 
{
	return _stmtNumber;
}

/**
 * @return the value of the node. 
 * If the node is a Variable node, then value is the INDEX of the variable in the VarTable. 
 * If the node is a Constant node, then value is a CONSTANT. 
 * If the node is neither a Variable node or Constant node, then value is not a meaningful value.
 */
int TNode::getNodeValueIdx() 
{
	return _nodeValueIdx;
}


/**
 * @return the type of the TNode.
 */
TNODE_TYPE TNode::getNodeType() 
{
	return _nodeType;
}

/**
 * @return the parent TNode.
 */
TNode* TNode::getParent() 
{
	return _parent;
}

/**
 * Add node to the existing list of child nodes.	
 * @param node a TNode
 * @exception exception if node is NULL.
 */
void TNode::addChild(TNode* node) 
{
	_children.push_back(node);
}

/**
 * @return TRUE if the current node has any child node. FALSE if the current node does not have any child node.
 */
bool TNode::hasChild() 
{
	if(_children.size() < 1) return false;
	else return true;
}

/**
 * @return a list of all the children nodes of the current node.	 	
 * If the current node does not have any children nodes, return an empty list.
 */
vector<TNode*>* TNode::getChildren() 
{
	return &_children;
}

/**
 * Add node as the left sibling of the current node.
 * @param node a TNode
 * @exception exception if node is NULL.
 */
void TNode::addLeftSibling(TNode* node) 
{
	_leftSibling = node;	
}

/**
 * Add node as the right sibling of the current node.
 * @param node a TNode
 * @exception exception if node is NULL.
 */
void TNode::addRightSibling(TNode* node) 
{
	_rightSibling = node;
}

/**
 * Check if the current node has any left sibling.
 * @return TRUE if the current node has a left sibling. FALSE if the current node does not have any left sibling.
 */
bool TNode::hasLeftSibling() 
{
	if(_leftSibling != NULL) return true;
	else return false;
}

/**
 * Check if the current node has any right sibling.
 * @return TRUE if the current node has a right sibling. FALSE if the current node does not have any right sibling.
 */
bool TNode::hasRightSibling() 
{
	if(_rightSibling != NULL) return true;
	else return false;
}

/**
 * @return the left sibling of the current node.
 */
TNode* TNode::getLeftSibling() 
{
	return _leftSibling;
}

/**
 * @return the right sibling of the current node.
 */
TNode* TNode::getRightSibling() 
{
	return _rightSibling;
}

/**
 * Set the parent node for current node.
 * @param parent a parent TNode for the current nodes
 * @exception exception if parent is NULL.
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
			case (Times):
				oper = "*";
				break;
			case (Minus):
				oper = "-";
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