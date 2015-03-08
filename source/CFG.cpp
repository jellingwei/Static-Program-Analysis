#include <stdio.h>
#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <cstring>
#include <regex>

using namespace std;

#include "CFG.h"

CFG::CFG(TNode* procNode) {
	_procNode = new CNode(Proc_C, 0, NULL, procNode);

	_endNode = new CNode(EndProc_C, 0, NULL, NULL);
	_endNode->setEnd();
}

/**
* @param cfgNodeType  the node type which can be Assign, While, If, Call, Proc, EndProc, EndIf
* @param procLineNum  the program line of the CNode
* @param header  the container statement for if/while statements
* @param ast  the TNode in AST that corresponds to the CNode in CFG
* @return a CNode for the given design entity together with its program line number,
* @exception exception if cfgNodeType is invalid, procLineNum is negative or 0, header or ast is NULL.
*/
CNode* CFG::createCNode(CNODE_TYPE cfgNodeType, int procLineNum, CNode* header, TNode* ast)
{
	CNode* temp = new CNode(cfgNodeType, procLineNum, header, ast);
	allNodes.push_back(temp);
	return temp;
}

/**
* @return the root node of the CFG.
*/
CNode* CFG::getProcRoot() {
	return _procNode;
}

/**
* @return the end node of the cfg
*/
CNode* CFG::getProcEnd() {
	return _endNode;
}

/**
* @param link  the type of connection between CNodes which can be Before, After, Inside, Inside2
* @param currNode  the node that connects to toNode
* @param toNode  the node that receives a connection from currNode
* @return TRUE if the link between the currNode to toNode is created successfully. 
*		  FALSE if the link between the currNode to toNode is not created successfully.
* @exception exception if link is invalid, or currNode and toNode is NULL.
*/
bool CFG::createLink(CLINK_TYPE link, CNode* currNode, CNode* toNode)
{
	switch(link) {
		case Before: {
			CNode& temp = *currNode;
			temp.addBefore(toNode);
			CNode& temp2 = *toNode;
			temp2.addAfter(currNode);
			return true; }
		
		case After: {
			CNode& temp = *currNode;
			temp.addAfter(toNode);
			CNode& temp2 = *toNode;
			temp2.addBefore(currNode);
			return true; }
	
		case Inside: {
			CNode& temp = *currNode;
			temp.addInside(toNode);
			toNode->setHeader(&temp);
			return true; }

		case Inside2: {
			CNode& temp = *currNode;
			temp.addInside2(toNode);
			toNode->setHeader(&temp);
			return true; }

		default:
			return false; 
	}
}

/**
* Check if there exists a stmtLst inside the container statement.
* @param header  the container statement for if/while statements
* @return TRUE if header has a stmtLst. FALSE if header does not have a stmtLst. 
*/
bool CFG::hasInside(CNode* header){
	return header->hasInside();
}

/**
* Get the total number of CNodes inside the container statement.
* @param header  the container statement for if/while statements
* @return the total number of CNodes inside the container statement header.
* @exception exception if header is NULL.
*/
int CFG::getInsideSize(CNode* header) {
	if(header==NULL) {
		throw exception("CFG error: CNode* not referenced");
	}

	CNode& temp = *header;
	vector<CNode*> *pq = temp.getInside();
	return pq->size();
}

/**
* Get a list of CNodes inside the container statement.
* @param header  the container statement for if/while statements
* @return a list of all the CNodes inside the container statement header.
* @exception exception if header is NULL.
*/
vector<CNode*>* CFG::getInsideNodes(CNode* header) {
	if(header==NULL) {
		throw exception("CFG error: CNode* not referenced");
	}

	return header->getInside();
}

/**
* Check if a particular CNode belongs to the container statement.
* @param header  the container statement for if/while statements
* @param inside  a CNode that may or may not belong to a container statement
* @return TRUE if the container statement, header, contains the CNode, inside. 
*		  FALSE if the container statement, header, does not contain the CNode, inside.
*/
bool CFG::isInsideNode(CNode* header, CNode* inside) {
	if(header==NULL) {
		throw exception("CFG error: CNode* not referenced");
	}

	CNode &ins = *inside;
	if(ins.CNode::getHeader() == header) 
		return true;
	else return false;
}

/**
* Check if there exists a stmtLst inside the 'else' section of the if statement.
* @param header  the container statement for if statement
* @return TRUE if header has a stmtLst. FALSE if header does not have a stmtLst. 
*/
bool CFG::hasInsideElse(CNode* header){
	return header->hasInside2();
}

/**
* Get the total number of CNodes inside the 'else' section of the if statement.
* @param header  the container statement for if statement
* @return the total number of CNodes inside the 'else' section of the if statement header.
* @exception exception if header is NULL.
*/
int CFG::getInsideElseSize(CNode* header) {
	if(header==NULL) {
		throw exception("CFG error: CNode* not referenced");
	}

	CNode& temp = *header;
	vector<CNode*> *pq = temp.getInside2();
	return pq->size();
}

/**
* Get a list of all the CNodes inside the 'else' section of the if statement.
* @param header  the container statement for if statement
* @return a list of all the CNodes inside the 'else' section of the if statement header.
* @exception exception if header is NULL.
*/
vector<CNode*>* CFG::getInsideElseNodes(CNode* header) {
	if(header==NULL) {
		throw exception("CFG error: CNode* not referenced");
	}

	return header->getInside2();
}

/**
* Check if a particular CNode belongs to the 'else' section of the if statement.
* @param header  the container statement for if statement
* @param inside  a CNode that may or may not belong to 'else' section of the if statement
* @return TRUE if the if statement, header, contains the CNode, inside. 
*		  FALSE if the if statement, header, does not contain the CNode, inside.
*/
bool CFG::isInsideElseNode(CNode* header, CNode* inside) {
	if(header==NULL) {
		throw exception("CFG error: CNode* not referenced");
	}

	CNode &ins = *inside;
	if(ins.CNode::getHeader() == header) 
		return true;
	else return false;
}

/**
* Check if the current CNode is linked to any predecessor CNode. 
* @param curr  the current CNode
* @return TRUE if the current CNode curr is linked to at least 1 predecessor CNode.
*		  FALSE if the current CNode is not linked to any predecessor CNode or if curr is NULL.
*/
bool CFG::hasBefore(CNode* curr){
	return curr->hasBefore();
}

/**
* Get the total number of predecessor CNodes the current CNode is linked to.
* @param curr  the current CNode
* @return the total number of predecessor CNodes the current CNode is linked to.
* @exception exception if curr is NULL.
*/
int CFG::getBeforeSize(CNode* curr) {
	if(curr==NULL) {
		throw exception("CFG error: CNode* not referenced");
	}

	CNode& temp = *curr;
	vector<CNode*> *pq = temp.getBefore();
	return pq->size();
}

/**
* Get a list of all the predecessor CNodes the current CNode is linked to.
* @param curr  the current CNode
* @return a list of all the predecessor CNodes the current CNode is linked to.
* @exception exception if curr is NULL.
*/
vector<CNode*>* CFG::getBeforeNodes(CNode* curr) {
	if(curr==NULL) {
		throw exception("CFG error: CNode* not referenced");
	}

	return curr->getBefore();
}

/**
* Check if the current CNode is linked to the predecessor CNode.
* @param curr  the current CNode
* @param before  a predecessor CNode
* @return TRUE if the current CNode, curr, is linked to the predecessor CNode, before.
*		  FALSE if the current CNode, curr, is not linked to the predecessor CNode, before, or if curr or before is NULL.
*/
bool CFG::isBeforeNode(CNode* curr, CNode* before) {
	if(curr==NULL) {
		throw exception("CFG error: CNode* not referenced");
	}

	vector<CNode*> &beforeList = *curr->getBefore();
	return std::find(beforeList.begin(), beforeList.end(), before)!= beforeList.end();
}

/**
* Check if the current CNode is linked to the any successor CNode.
* @param curr  the current CNode
* @return TRUE if the current CNode, curr, is linked to at least 1 successor CNode.
*		  FALSE if the current CNode,curr, is not linked to any successor CNode or if curr is NULL.
*/
bool CFG::hasAfter(CNode* curr){
	return curr->hasAfter();
}

/**
* Get the total number of successor CNodes the current CNode is linked to.
* @param curr  the current CNode
* @return the total number of successor CNodes the current CNode is linked to.
* @exception exception if curr is NULL.
*/
int CFG::getAfterSize(CNode* curr) {
	if(curr==NULL) {
		throw exception("CFG error: CNode* not referenced");
	}

	CNode& temp = *curr;
	vector<CNode*> *pq = temp.getAfter();
	return pq->size();
}

/**
* Get a list of all the successor CNodes the current CNode is linked to.
* @param curr  the current CNode
* @return a list of all the successor CNodes the current CNode is linked to.
* @exception exception if curr is NULL.
*/
vector<CNode*>* CFG::getAfterNodes(CNode* curr) {
	if(curr==NULL) {
		throw exception("CFG error: CNode* not referenced");
	}

	return curr->getAfter();
}

/**
* Check if the current CNode is linked to the successor CNode.
* @param curr  the current CNode
* @param after  a successor CNode
* @return TRUE if the current CNode, curr, is linked to the successor CNode, after.
*		  FALSE if the current CNode, curr, is not linked to the successor CNode, after, or if curr or after is NULL.
*/
bool CFG::isAfterNode(CNode* curr, CNode* after) {
	if(curr==NULL) {
		throw exception("CFG error: CNode* not referenced");
	}

	vector<CNode*> &afterList = *curr->getAfter();
	return std::find(afterList.begin(), afterList.end(), after)!= afterList.end();
}

/**
* Check if a particular CNode exists.
* @param node  a CNode
* @return TRUE if the CNode, node, exists.
*		  FALSE if the CNode, node, does not exist, or if node is NULL.
*/
bool CFG::isExists(CNode* node) { 
	if(node==NULL) {
		throw exception("CFG error: CNode* not referenced");
	}

	vector<CNode*>::iterator it;
	it = find(allNodes.begin(), allNodes.end(), node);
	if(it != allNodes.end())
		return true;
	else return false;
}

/**
* Get the TNode in AST that corresponds to the CNode in CFG. 
* @param node  a CNode
* @return the corresponding TNode in AST that matches the CNode in CFG.
*/
TNode* CFG::getASTref(CNode* node) {
	return node->getASTref();
}

/**
* Set the last CNode in CFG.
* @param CFGlast  the last CNode in CFG
*/
void CFG::setEndNode(CNode* CFGlast) {
	CFGlast->addAfter(_endNode);
}

/**
* Convert the type of TNode in AST to the corresponding type of CNode in CFG for Assign/While/If/Procedure/Call statements.
* @param type  the type of TNode which can be Assign/While/If/Procedure/Call statements
*/
CNODE_TYPE CFG::convertTNodeTypeToCNodeType(TNODE_TYPE type) {
	switch(type) {
	case Assign:
		return Assign_C;
		break;
	case While:
		return While_C;
		break;
	case If:
		return If_C;
		break;
	case Procedure:
		return Proc_C;
		break;
	case Call:
		return Call_C;
		break;
	default:
		throw logic_error("CFG::convertTNodeTypeToCNodeType does not support the TNODE_TYPE passed in. ");
	}
}