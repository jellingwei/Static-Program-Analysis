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

/**
 * @return a CNode, header, which is the container statement for if/while statements.
 */
CNode* CNode::getHeader() 
{
	return _header;
}

/**
 * @return a list of all the CNodes inside the container statement for if/while statements.
 */
vector<CNode*>* CNode::getInside()
{
	return &_inside;
}

/**
 * Add the CNode to the list of CNodes inside the container statement for if/while statements.
 * @param node a CNode
 */
void CNode::addInside(CNode* node)
{
	_inside.push_back(node);
}

/**
 * Check if the current CNode has any CNode inside the container statement for if/while statements. 
 * @return TRUE if the current CNode has at least 1 CNode inside the container statement. 
 *		   FALSE if the current CNode does not have any CNode inside the container statement.
 */
bool CNode::hasInside()
{
	if(_inside.size() < 1) return false;
	else return true;
}

/**
 * @return a list of all the CNodes inside the 'else' of the if/else statement.
 */
vector<CNode*>* CNode::getInside2()
{
	return &_inside;
}

/**
 * Add the CNode to the list of CNodes inside the 'else' of the if/else statement.
 * @param node a CNode
 */
void CNode::addInside2(CNode* node)
{
	_inside.push_back(node);
}

/**
 * Check if the current CNode has any CNode inside the 'else' of the if/else statement.
 * @return TRUE if the current CNode has at least 1 CNode inside the 'else' of the if/else statement. 
 *		   FALSE if the current CNode does not have any CNode inside the 'else' of the if/else statement.
 */
bool CNode::hasInside2()
{
	if(_inside.size() < 1) return false;
	else return true;
}

/**
 * Get the TNode in AST that corresponds to the CNode in CFG.
 * @return the TNode corresponds to the CNode given.
 */
TNode* CNode::getASTref()
{
	return _ASTref;
}

/**
 * Set the node to indicate it is the last node in the CFG of a procedure.
 */
void CNode::setEnd()
{
	isEnd = true;
}

/**
 * Check if the node is the end node in the CFG of a procedure.
 * @return TRUE if the the node is the end node in the CFG of a procedure.
 *		   FALSE if the the node is not the end node in the CFG of a procedure.
 */
bool CNode::getEnd()
{
	return isEnd;
}