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

PROGLINE_LIST NextTable::getNextAfter(PROG_LINE_ progLine1, TRANS_CLOSURE transitiveClosure) {
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

PROGLINE_LIST NextTable::getNextBefore(PROG_LINE_ progLine2, TRANS_CLOSURE transitiveClosure) {
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



BOOLEAN_ NextTable::isNext(PROG_LINE_ progLine1, PROG_LINE_ progLine2, TRANS_CLOSURE transitiveClosure) {
	PKB pkb = PKB::getInstance();
	
	if (pkb.cfgNodeTable.count(progLine1) == 0) { // progline does not have a cfg node
		return false;
	}

	// early return if not in the same procedure
	if (PKB::getInstance().stmtToProcMap[progLine1] != PKB::getInstance().stmtToProcMap[progLine2]) {
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


PROGLINE_LIST NextTable::getLhs() {
	/*PKB pkb = PKB::getInstance();

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
	*/

	PKB pkb = PKB::getInstance();
	
	vector<int> result;
	for (int i = 1; i <= pkb.getStmtTableSize(); i++) {

		CNode* node = pkb.cfgNodeTable.at(i);

		if (node->getNodeType() == While_C || node->getNodeType() == If_C) {
			result.push_back(node->getProcLineNumber());
			continue;
		}

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

PROGLINE_LIST NextTable::getRhs() {
	PKB pkb = PKB::getInstance();

	vector<int> result;
	vector<int> assignStmts = pkb.getStmtNumForType(ASSIGN);
	for (int i = 1; i <= pkb.getStmtTableSize(); i++) {
		CNode* node = pkb.cfgNodeTable.at(i);

		vector<CNode*>* before = node->getBefore();
		bool isFirstNode = (before->size() == 1 && before->at(0)->getNodeType() == Proc_C);
		
		if (!isFirstNode) {
			result.push_back(i);
		}
	}

	return result;
}

CNode* NextTable::getCNodeForProgLine(PROG_LINE_ progLine) {
	throw exception("not implemented yet");
}

PROG_LINE_ NextTable::getFirstProgLineInContainer(CONTAINER_STATEMENT container) {
	if (firstProgLineInElse.count(container) != 0) {
		return firstProgLineInElse[container];
	} else {
		return -1;
	}
}

PROG_LINE_ NextTable::getLastProgLineInContainer(CONTAINER_STATEMENT container) {
	if (lastProgLineInContainer.count(container) != 0) {
		return lastProgLineInContainer[container];
	} else {
		return -1;
	}
}

void NextTable::setFirstProgLineInElse(CONTAINER_STATEMENT container, PROG_LINE_ firstline) {
	firstProgLineInElse.insert(pair<int, int>(container, firstline));
}
void NextTable::setLastProgLineInContainer(CONTAINER_STATEMENT container, PROG_LINE_ lastline) {
	lastProgLineInContainer.insert(pair<int, int>(container, lastline));
}

PROG_LINE_ NextTable::getFirstProgLineInProc(PROC_INDEX procIndex) {
	return firstProgLineInProc[procIndex];
}
PROG_LINE_ NextTable::getLastProgLineInProc(PROC_INDEX procIndex) {
	return lastProgLineInProc[procIndex];
}

void NextTable::setFirstProgLineInProc(PROC_INDEX procIndex, PROG_LINE_ firstlines) {
	firstProgLineInProc.push_back(firstlines);
}
void NextTable::setLastProgLineInProc(PROC_INDEX procIndex, PROG_LINE_ lastlines) {
	lastProgLineInProc.push_back(lastlines);

}

BOOLEAN_ NextTable::setProgLineInWhile(PROG_LINE_ progline) {
	isProgLineInWhile = boost::dynamic_bitset<>(PKB::getInstance().getStmtTableSize() + 1);
	isProgLineInWhile.set(progline);
	return true;
}

BOOLEAN_ NextTable::isValid() {
	vector<int> procIndexes = PKB::getInstance().getAllProcIndex();

	for (auto iter = procIndexes.begin(); iter != procIndexes.end(); ++iter) {
		if (getFirstProgLineInProc(*iter) != getLastProgLineInProc(*iter)) {
			// if the first and last lines in a proc are not the same,
			// then there are more than one line in the proc, and Next is valid
			return true;
		}
	}

	return false;
}