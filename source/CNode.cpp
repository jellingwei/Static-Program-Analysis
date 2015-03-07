#include <stdio.h>
#include <iostream>
#include <string>
#include <vector>
#include <algorithm>

#include "CNode.h"

CNode::CNode() 
{}

/**
 * Set a CNode.
 * @param cfgNodeType the type of CNode which can be Assign/While/If/Call/Proc/EndProc/EndIf
 * @param procLineNum the program line number of the CNode
 * @param header  the container statement for if/while statements
 * @param ast  the TNode in AST that corresponds to the CNode in CFG
 */
CNode::CNode(CNODE_TYPE cfgNodeType, int procLineNum, CNode* header, TNode* ast) 
{
	_nodeType = cfgNodeType;
	_procLineNumber = procLineNum;
	_header = header;
	_ASTref = ast;
}


bool CNode::isDummy() {
	return _nodeType == EndIf_C;
}

/**
 * @return the program line number of the corresponding CNode.
 */
int CNode::getProcLineNumber() 
{
	return _procLineNumber;
}

/**
 * Get the the type of CNode which can be Assign/While/If/Call/Proc/EndProc/EndIf of the corresponding CNode.
 * @return the node type of the corresponding CNode.
 */
CNODE_TYPE CNode::getNodeType() 
{
	return _nodeType;
}

/**
 * @return a list of all the predecessor CNodes of the current CNode.
 */
vector<CNode*>* CNode::getBefore()
{
	return &_before;
}

/**
 * Add in the node to a list of predecessor CNodes.
 * @param node a CNode
 */
void CNode::addBefore(CNode* node)
{
	_before.push_back(node);
}

/**
 * Check if the current CNode has any predecessor CNode. 
 * @return TRUE if the current CNode has at least 1 predecessor CNode. FALSE if the current CNode does not have any predecessor CNode.
 */
bool CNode::hasBefore() 
{
	if(_before.size() < 1) return false;
	else return true;
}

/**
 * @return a list of all the successor CNodes of the current CNode.
 */
vector<CNode*>* CNode::getAfter()
{
	return &_after;
}

/**
 * Add in the node to a list of successor CNodes.
 * @param node a CNode
 */
void CNode::addAfter(CNode* node)
{
	_after.push_back(node);
}

/**
 * Check if the current CNode has any successor CNode. 
 * @return TRUE if the current CNode has at least 1 successor CNode. FALSE if the current CNode does not have any successor CNode.
 */
bool CNode::hasAfter() 
{
	if(_after.size() < 1) return false;
	else return true;
}

/**
 * Set a CNode called header which is the container statement for if/while statements.
 * @param header a CNode the container statement for if/while statements
 */
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

VARIABLES CNode::getVariablesInside2() {
	return _variablesInside2;
}
void CNode::setVariablesInside2(VARIABLES variables) {
	_variablesInside2 = variables;
}

VARIABLES CNode::getVariablesInside() {
	return _variablesInside;
}
void CNode::setVariablesInside(VARIABLES variables) {
	_variablesInside = variables;
}

VAR_TO_STATEMENT_MAP CNode::getReachingDefinitions() {
	return _reachingDefinitions;
}
void CNode::setReachingDefinitions(VAR_TO_STATEMENT_MAP varStmtMap) {
	_reachingDefinitions = varStmtMap;
}

VAR_TO_STATEMENT_MAP CNode::getFirstUseOfVariable() {
	return _firstUseOfVariable;
}
void CNode::setFirstUseOfVariable(VAR_TO_STATEMENT_MAP varStmtMap) {
	_firstUseOfVariable = varStmtMap;
}