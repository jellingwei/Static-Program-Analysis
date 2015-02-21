#include <stdio.h>
#include <iostream>
#include <string>
#include <vector>
#include <algorithm>

#include "CNode.h"

CNode::CNode() 
{}

CNode::CNode(CNODE_TYPE cfg_node_type, int procLineNo, CNode* header, TNode* ast) 
{
	_nodeType = cfg_node_type;
	_procLineNumber = procLineNo;
	_header = header;
	_ASTref = ast;
}

int CNode::getProcLineNumber() 
{
	return _procLineNumber;
}

CNODE_TYPE CNode::getNodeType() 
{
	return _nodeType;
}

vector<CNode*>* CNode::getBefore()
{
	return &_before;
}

void CNode::addBefore(CNode* node)
{
	_before.push_back(node);
}

bool CNode::hasBefore() 
{
	if(_before.size() < 1) return false;
	else return true;
}

vector<CNode*>* CNode::getAfter()
{
	return &_after;
}

void CNode::addAfter(CNode* node)
{
	_after.push_back(node);
}

bool CNode::hasAfter() 
{
	if(_after.size() < 1) return false;
	else return true;
}

void CNode::setHeader(CNode* header) 
{
	_header = header;
}

CNode* CNode::getHeader() 
{
	return _header;
}

vector<CNode*>* CNode::getInside()
{
	return &_inside;
}

void CNode::addInside(CNode* node)
{
	_inside.push_back(node);
}

bool CNode::hasInside()
{
	if(_inside.size() < 1) return false;
	else return true;
}

vector<CNode*>* CNode::getInside2()
{
	return &_inside;
}

void CNode::addInside2(CNode* node)
{
	_inside.push_back(node);
}

bool CNode::hasInside2()
{
	if(_inside.size() < 1) return false;
	else return true;
}

TNode* CNode::getASTref()
{
	return _ASTref;
}

void CNode::setEnd()
{
	isEnd = true;
}

bool CNode::getEnd()
{
	return isEnd;
}