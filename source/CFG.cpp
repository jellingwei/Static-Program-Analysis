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
	//allNodes.push_back(_procNode);

	_endNode = new CNode(EndProc_C, 0, NULL, NULL);
	_endNode->setEnd();
}

CNode* CFG::createCNode(CNODE_TYPE cfg_node_type, int procLineNo, CNode* header, TNode* ast)
{
	CNode* temp = new CNode(cfg_node_type, procLineNo, header, ast);
	allNodes.push_back(temp);
	return temp;
}

CNode* CFG::getProcRoot() {
	return _procNode;
}

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

bool CFG::hasInside(CNode* header){
	return header->hasInside();
}

int CFG::getInsideSize(CNode* header) {
	if(header==NULL) {
		throw exception("CFG error: CNode* not referenced");
	}

	CNode& temp = *header;
	vector<CNode*> *pq = temp.getInside();
	return pq->size();
}

vector<CNode*>* CFG::getInsideNodes(CNode* header) {
	if(header==NULL) {
		throw exception("CFG error: CNode* not referenced");
	}

	return header->getInside();
}

bool CFG::isInsideNode(CNode* header, CNode* inside) {
	if(header==NULL) {
		throw exception("CFG error: CNode* not referenced");
	}

	CNode &ins = *inside;
	if(ins.CNode::getHeader() == header) 
		return true;
	else return false;
}

bool CFG::hasInside2(CNode* header){
	return header->hasInside2();
}

int CFG::getInside2Size(CNode* header) {
	if(header==NULL) {
		throw exception("CFG error: CNode* not referenced");
	}

	CNode& temp = *header;
	vector<CNode*> *pq = temp.getInside2();
	return pq->size();
}

vector<CNode*>* CFG::getInside2Nodes(CNode* header) {
	if(header==NULL) {
		throw exception("CFG error: CNode* not referenced");
	}

	return header->getInside2();
}

bool CFG::isInside2Node(CNode* header, CNode* inside) {
	if(header==NULL) {
		throw exception("CFG error: CNode* not referenced");
	}

	CNode &ins = *inside;
	if(ins.CNode::getHeader() == header) 
		return true;
	else return false;
}

bool CFG::hasBefore(CNode* curr){
	return curr->hasBefore();
}

int CFG::getBeforeSize(CNode* curr) {
	if(curr==NULL) {
		throw exception("CFG error: CNode* not referenced");
	}

	CNode& temp = *curr;
	vector<CNode*> *pq = temp.getBefore();
	return pq->size();
}

vector<CNode*>* CFG::getBeforeNodes(CNode* curr) {
	if(curr==NULL) {
		throw exception("CFG error: CNode* not referenced");
	}

	return curr->getBefore();
}

bool CFG::isBeforeNode(CNode* curr, CNode* before) {
	if(curr==NULL) {
		throw exception("CFG error: CNode* not referenced");
	}

	vector<CNode*> &beforeList = *curr->getBefore();
	return std::find(beforeList.begin(), beforeList.end(), before)!= beforeList.end();
}

bool CFG::hasAfter(CNode* curr){
	return curr->hasAfter();
}

int CFG::getAfterSize(CNode* curr) {
	if(curr==NULL) {
		throw exception("CFG error: CNode* not referenced");
	}

	CNode& temp = *curr;
	vector<CNode*> *pq = temp.getAfter();
	return pq->size();
}

vector<CNode*>* CFG::getAfterNodes(CNode* curr) {
	if(curr==NULL) {
		throw exception("CFG error: CNode* not referenced");
	}

	return curr->getAfter();
}

bool CFG::isAfterNode(CNode* curr, CNode* after) {
	if(curr==NULL) {
		throw exception("CFG error: CNode* not referenced");
	}

	vector<CNode*> &afterList = *curr->getAfter();
	return std::find(afterList.begin(), afterList.end(), after)!= afterList.end();
}

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

TNode* CFG::getASTref(CNode* node) {
	return node->getASTref();
}

void CFG::setEndNode(CNode* CFGlast) {
	CFGlast->addAfter(_endNode);
}

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
	default:
		throw logic_error("CFG::convertTNodeTypeToCNodeType does not support the TNODE_TYPE passed in. ");
	}
}