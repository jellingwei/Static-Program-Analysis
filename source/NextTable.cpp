#include <stdio.h>
#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <deque>

#include "PKB.h"
#include "NextTable.h"

using namespace std;


NextTable::NextTable() {
	
	
}

vector<int> NextTable::getNextAfter(int progLine1, bool transitiveClosure) {
	PKB pkb = PKB::getInstance();
	if (pkb.cfgNodeTable.count(progLine1) == 0) { // progline does not have a cfg node
		return vector<int>();
	}
	CNode* curNode = pkb.cfgNodeTable.at(progLine1);
	vector<int> result;

	deque<CNode*> frontier; 
	set<int> visited;

	frontier.push_back(curNode);

	while (!frontier.empty()) {
		curNode = frontier.back(); frontier.pop_back();

		vector<CNode*>* nextNodes = curNode->getAfter();
		for (auto iter = nextNodes->begin(); iter != nextNodes->end(); ++iter) {
			if (visited.count((*iter)->getProcLineNumber()) == 0 || (*iter)->getNodeType() == EndIf_C) {
				frontier.push_back(*iter);
				if ((*iter)->getNodeType() != EndIf_C){
					result.push_back((*iter)->getProcLineNumber());
				}
				visited.insert((*iter)->getProcLineNumber());
			}
		}

		// break after the first iteration if we are not finding transitive closure
		if (!transitiveClosure) {
			break;
		}
	}

	return result;
}

vector<int> NextTable::getNextBefore(int progLine2, bool transitiveClosure) {
	PKB pkb = PKB::getInstance();
	if (pkb.cfgNodeTable.count(progLine2) == 0) { // progline does not have a cfg node
		return vector<int>();
	}
	CNode* curNode = pkb.cfgNodeTable.at(progLine2);
	vector<int> result;

	deque<CNode*> frontier; 
	set<int> visited;
	frontier.push_back(curNode);

	while (!frontier.empty()) {
		curNode = frontier.back(); frontier.pop_back();

		vector<CNode*>* nextNodes = curNode->getBefore();
		for (auto iter = nextNodes->begin(); iter != nextNodes->end(); ++iter) {
			if (visited.count((*iter)->getProcLineNumber()) == 0 || (*iter)->getNodeType() == EndIf_C) {
				frontier.push_back(*iter);
				if ((*iter)->getNodeType() != EndIf_C){
					result.push_back((*iter)->getProcLineNumber());
				}
				visited.insert((*iter)->getProcLineNumber());
			}
		}

		// break after the first iteration if we are not finding transitive closure
		if (!transitiveClosure) {
			break;
		}
	}

	return result;
}

bool NextTable::isNext(int progLine1, int progLine2, bool transitiveClosure) {
	PKB pkb = PKB::getInstance();
	
	if (pkb.cfgNodeTable.count(progLine1) == 0) { // progline does not have a cfg node
		return false;
	}
	CNode* curNode = pkb.cfgNodeTable.at(progLine1);
	
	vector<int> result;

	deque<CNode*> frontier; 
	set<int> visited;
	frontier.push_back(curNode);
	
	while (!frontier.empty()) {
		curNode = frontier.back(); frontier.pop_back();

		vector<CNode*>* nextNodes = curNode->getAfter();
		
		for (auto iter = nextNodes->begin(); iter != nextNodes->end(); ++iter) {
			CNode* node = *iter;
			if (visited.count(node->getProcLineNumber()) == 0 || (*iter)->getNodeType() == EndIf_C) {
				frontier.push_back(node);
				if ((*iter)->getNodeType() != EndIf_C){
					result.push_back((*iter)->getProcLineNumber());
				}
				visited.insert(node->getProcLineNumber());
		
			}

			if (node->getProcLineNumber() == progLine2) {
				return true;
			}
		}

		// break after the first iteration if we are not finding transitive closure
		if (!transitiveClosure) {
			break;
		}
	}
	

	return false;
}