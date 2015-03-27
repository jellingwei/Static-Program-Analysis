#include <stdio.h>
#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <deque>

#include "PKB.h"
#include "NextTable.h"

using namespace std;


bool optimiseNext = true;

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

	if (optimiseNext && isProgLineInWhile.test(progLine1) && transitiveClosure) {
		vector<int> parents = pkb.getParent(progLine1);
		if (parents.size() > 0){
			int parent = parents[0];

			int lastline = getLastProgLineInContainer(parent);

			if (lastline >= progLine2) {
				return true;
			}
		}

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
	PKB pkb = PKB::getInstance();

	vector<int> listOfProglinesToExclude;
	
	vector<int> procs = pkb.getAllProcIndex();
	for (auto iter = procs.begin(); iter != procs.end(); ++iter) {
		int procNum = *iter;

		int lastline = getLastProgLineInProc(procNum);
		CNode* node = pkb.cfgNodeTable.at(lastline);

		if (node->getNodeType() == EndIf_C ) {
			vector<CNode*> frontier; 
			frontier.push_back(node);

			while (!frontier.empty()) {
				node = frontier.back(); 
				frontier.pop_back();
				if (node->getNodeType() == EndIf_C) {
					// add previous nodes 
					vector<CNode*>* prevNodes = node->getBefore();

					for (auto prevNodeIt = prevNodes->begin(); prevNodeIt != prevNodes->end(); ++prevNodeIt) {
						frontier.push_back(*prevNodeIt);
					}
				} else if (node->getNodeType() != While_C) {
					listOfProglinesToExclude.push_back(node->getProcLineNumber() );
				}
			}
		} else {
			if (node->getNodeType() != While_C) {
				listOfProglinesToExclude.push_back(node->getProcLineNumber() );
			}	
		}
	}

	//sort in desc
	sort(listOfProglinesToExclude.begin(), listOfProglinesToExclude.end(), std::greater<int>());

	// include every line except the ones in listOfProglinesToExclude
	vector<int> result;
	for (unsigned int i = 1; i <= pkb.getStmtTableSize(); i++) {
		if (!listOfProglinesToExclude.empty() && i == listOfProglinesToExclude.back()) {
			listOfProglinesToExclude.pop_back();
			continue;
		}

		result.push_back(i);
	}

	return result;
}

vector<int> NextTable::getRhs() {

	PKB pkb = PKB::getInstance();
	
	/*for (auto iter = pkb.cfgNodeTable.begin(); iter != pkb.cfgNodeTable.end(); ++iter) {
		CNode* node = iter->second;

		vector<CNode*>* before = node->getBefore();
		bool isFirstNode = (before->size() == 1 && before->at(0)->getNodeType() == Proc_C);
		
		if (!isFirstNode) {
			result.push_back(node->getProcLineNumber());
		}
	}*/

	vector<int> listOfProglinesToExclude;

	vector<int> procs = pkb.getAllProcIndex();
	for (auto iter = procs.begin(); iter != procs.end(); ++iter) {
		int procNum = *iter;

		int firstline = getFirstProgLineInProc(procNum);
		CNode* node = pkb.cfgNodeTable.at(firstline);

		if (node->getNodeType() != While_C) {
			listOfProglinesToExclude.push_back(node->getProcLineNumber());
		}
	}

	//sort in desc
	sort(listOfProglinesToExclude.begin(), listOfProglinesToExclude.end(), std::greater<int>());

	// include every line except the ones in listOfProglinesToExclude
	vector<int> result;
	for (unsigned int i = 1; i <= pkb.getStmtTableSize(); i++) {
		if (!listOfProglinesToExclude.empty() && i == listOfProglinesToExclude.back()) {
			listOfProglinesToExclude.pop_back();
			continue;
		}

		result.push_back(i);
	}



	return result;
}

CNode* NextTable::getCNodeForProgLine(int progLine) {
	throw exception("not implemented yet");
}

int NextTable::getFirstProgLineInContainer(int container) {
	if (firstProgLineInElse.count(container) != 0) {
		return firstProgLineInElse[container];
	} else {
		return -1;
	}
}

int NextTable::getLastProgLineInContainer(int container) {
	if (lastProgLineInContainer.count(container) != 0) {
		return lastProgLineInContainer[container];
	} else {
		return -1;
	}
}

void NextTable::setFirstProgLineInElse(int container, int firstline) {
	firstProgLineInElse.insert(pair<int, int>(container, firstline));
}
void NextTable::setLastProgLineInContainer(int container, int lastline) {
	lastProgLineInContainer.insert(pair<int, int>(container, lastline));
}

int NextTable::getFirstProgLineInProc(int procIndex) {
	return firstProgLineInProc[procIndex];
}
int NextTable::getLastProgLineInProc(int procIndex) {
	return lastProgLineInProc[procIndex];
}

void NextTable::setFirstProgLineInProc(int procIndex, int firstlines) {
	firstProgLineInProc.push_back(firstlines);
}
void NextTable::setLastProgLineInProc(int procIndex, int lastlines) {
	lastProgLineInProc.push_back(lastlines);

}

bool NextTable::setProgLineInWhile(int progline) {
	isProgLineInWhile = boost::dynamic_bitset<>(PKB::getInstance().getStmtTableSize() + 1);
	isProgLineInWhile.set(progline);
	return true;
}