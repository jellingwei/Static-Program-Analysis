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

void updateStateOfBfs(set<int>& visited, CNode* nextNode, deque<CNode*>& frontier, vector<int>& result) {
	if (nextNode->getNodeType() == Proc_C || nextNode->getNodeType() == EndProc_C) {
		return;
	}
	if (visited.count(nextNode->getProcLineNumber()) == 0) {
		frontier.push_back(nextNode);
		if (nextNode->getNodeType() != EndIf_C){
			result.push_back(nextNode->getProcLineNumber());
		}
		visited.insert(nextNode->getProcLineNumber());
	}
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
			CNode* node = *iter;

			// if the previous node is a dummy end_if node,
			// skip it and go to its "after" nodes directly
			while (node->getNodeType() == EndIf_C) {
				assert(node->getAfter()->size() <= 1);

				if (node->getAfter()->size() == 0) { // if is the last progline
					continue;
				}
				CNode* nextNodeAfterEndIf = node->getAfter()->at(0);
				node = nextNodeAfterEndIf;
			}

			updateStateOfBfs(visited, node, frontier, result);			
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
			CNode* node = *iter;
			if (node->getNodeType() == Proc_C || node->getNodeType() == EndProc_C) {
				continue;
			}

			// if the previous node is a dummy end_if node,
			// skip it and go to its "before" nodes directly
			if (node->getNodeType() == EndIf_C) {
				
				// jump to the next node immediately as an EndIf_C node should never be considered
				assert(node->getBefore()->size() == 2);

				CNode* nextNodeBeforeEndIf = node->getBefore()->at(0);
				CNode* nextNodeBeforeEndIf1 = node->getBefore()->at(1);

				updateStateOfBfs(visited, nextNodeBeforeEndIf, frontier, result);
				updateStateOfBfs(visited, nextNodeBeforeEndIf1, frontier, result);
				

			} else  {
				updateStateOfBfs(visited, node, frontier, result);
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

	// early return if not in the same procedure
	if (PKB::getInstance().stmtToProcMap[progLine1] != PKB::getInstance().stmtToProcMap[progLine2]) {
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
			while (node->getNodeType() == EndIf_C) { 
				// jump to the next node immediately as an EndIf_C node should never be considered
				assert(node->getAfter()->size() == 1 || node->getAfter()->size() == 0);

				if (node->getAfter()->size() == 0) { // if is the last stmt in the stmtlist
					continue;
				}

				CNode* nextNodeAfterEndIf = node->getAfter()->at(0);
				node = nextNodeAfterEndIf;
			} 

			updateStateOfBfs(visited, node, frontier, result);	

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


vector<int> NextTable::getLhs() {
	//@todo change after asking jin what is proglines. might be able to do it faster depending on which definition

	PKB pkb = PKB::getInstance();
	vector<int> result;

	for (auto iter = pkb.cfgNodeTable.begin(); iter != pkb.cfgNodeTable.end(); ++iter) {
		CNode* node = iter->second;

		vector<CNode*>* after = node->getAfter();
		bool isLastNode;

		// first handle special case for dummy node (End of if statement)
		while (after->size() == 1 && after->at(0)->getNodeType() == EndIf_C) {
			after = after->at(0)->getAfter();	
		}

		isLastNode = (after->size() == 1 && after->at(0)->getNodeType() == EndProc_C);
		
		if (!isLastNode) {
			result.push_back(node->getProcLineNumber());
		}
	}

	return result;
}

vector<int> NextTable::getRhs() {
	//@todo change after asking jin what is proglines. might be able to do it faster depending on which definition

	PKB pkb = PKB::getInstance();
	vector<int> result;

	for (auto iter = pkb.cfgNodeTable.begin(); iter != pkb.cfgNodeTable.end(); ++iter) {
		CNode* node = iter->second;

		vector<CNode*>* before = node->getBefore();
		bool isFirstNode = (before->size() == 1 && before->at(0)->getNodeType() == Proc_C);
		
		if (!isFirstNode) {
			result.push_back(node->getProcLineNumber());
		}
	}

	return result;
}

CNode* NextTable::getCNodeForProgLine(int progLine) {
	throw exception("not implemented yet");
}