#include <stdio.h>
#include <iostream>
#include <string>
#include <vector>
#include "TNode.h"

TNode::TNode() {}

TNode::TNode(TNODE_TYPE ast_node_type, int stmtNo, int value) {
	/*if(stmtNo <= 0) {
		throw exception("TNode error: Negative statment number");
	} else if(value <= 0) {
		throw exception("TNode error: Negative value");
	} else if(ast_node_type!=Procedure || ast_node_type!=Assign || ast_node_type!=Plus || ast_node_type!=Variable || ast_node_type!=StmtLst || ast_node_type!=While || ast_node_type!=Constant) {
		throw exception("TNode error: Invalid TNode_Type");
	}*/

	_nodeType = ast_node_type;
	_stmtNumber = stmtNo;
	_nodeValueIdx = value;
	_leftSibling = _rightSibling = NULL;
}

TNode::TNode(TNODE_TYPE ast_node_type, int stmtNo, int value, TNode* parent) {

	_nodeType = ast_node_type;
	_stmtNumber = stmtNo;
	_nodeValueIdx = value;
	_parent = parent;
	_leftSibling = _rightSibling = NULL;
}

int TNode::getStmtNumber() {
	return _stmtNumber;
}

int TNode::getNodeValueIdx() {
	return _nodeValueIdx;
}

TNODE_TYPE TNode::getNodeType() {
	return _nodeType;
}

TNode* TNode::getParent() {
	return _parent;
}

void TNode::addChild(TNode* node) {
	_children.push_back(node);
}

bool TNode::hasChild() {
	if(_children.size() < 1) return false;
	else return true;
}

vector<TNode*>* TNode::getChildren() {
	return &_children;
}

void TNode::addLeftSibling(TNode* node) {
	_leftSibling = node;	
}

void TNode::addRightSibling(TNode* node) {
	_rightSibling = node;
}

bool TNode::hasLeftSibling() {
	if(_leftSibling != NULL) return true;
	else return false;
}

bool TNode::hasRightSibling() {
	if(_rightSibling != NULL) return true;
	else return false;
}

TNode* TNode::getLeftSibling() {
	return _leftSibling;
}

TNode* TNode::getRightSibling() {
	return _rightSibling;
}

void TNode::setParent(TNode* parent) {
	_parent = parent;
}