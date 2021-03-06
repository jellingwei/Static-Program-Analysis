#include <stdio.h>
#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <deque>

#include "PKB.h"
#include "NextSolver.h"

using namespace std;


bool optimiseNext = true;

NextSolver::NextSolver() {
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

PROGLINE_LIST NextSolver::getNextAfter(PROG_LINE_ progLine1, TRANS_CLOSURE transitiveClosure) {
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

PROGLINE_LIST NextSolver::getNextBefore(PROG_LINE_ progLine2, TRANS_CLOSURE transitiveClosure) {
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
				// for EndIf nodes, need to get both branches
				vector<CNode*> nodesToTraverseBack;
				CNode* prevNodeAfterEndIf = node->getBefore()->at(0);
				CNode* prevNodeAfterEndIf2 = node->getBefore()->at(1);

				nodesToTraverseBack.push_back(prevNodeAfterEndIf); 
				nodesToTraverseBack.push_back(prevNodeAfterEndIf2);
					
				while (!nodesToTraverseBack.empty()) {
					CNode* nodeToExpand = nodesToTraverseBack.back();
					nodesToTraverseBack.pop_back();
						
					updateStateOfBfs(visited, nodeToExpand, frontier, result);	

					if (nodeToExpand->getNodeType() == EndIf_C) {
						prevNodeAfterEndIf = nodeToExpand->getBefore()->at(0);
						prevNodeAfterEndIf2 = nodeToExpand->getBefore()->at(1);

						nodesToTraverseBack.push_back(prevNodeAfterEndIf);
						nodesToTraverseBack.push_back(prevNodeAfterEndIf2);
					}
				}

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



BOOLEAN_ NextSolver::isNext(PROG_LINE_ progLine1, PROG_LINE_ progLine2, TRANS_CLOSURE transitiveClosure) {
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


PROGLINE_LIST NextSolver::getLhs() {
	return lhs;
}

PROGLINE_LIST NextSolver::getRhs() {
	return rhs;
}


PROG_LINE_ NextSolver::getFirstProgLineInContainer(CONTAINER_STATEMENT container) {
	if (firstProgLineInElse.count(container) != 0) {
		return firstProgLineInElse[container];
	} else {
		return -1;
	}
}

PROG_LINE_ NextSolver::getLastProgLineInContainer(CONTAINER_STATEMENT container) {
	if (lastProgLineInContainer.count(container) != 0) {
		return lastProgLineInContainer[container];
	} else {
		return -1;
	}
}

void NextSolver::setFirstProgLineInElse(CONTAINER_STATEMENT container, PROG_LINE_ firstline) {
	firstProgLineInElse.insert(pair<int, int>(container, firstline));
}
void NextSolver::setLastProgLineInContainer(CONTAINER_STATEMENT container, PROG_LINE_ lastline) {
	lastProgLineInContainer.insert(pair<int, int>(container, lastline));
}

PROG_LINE_ NextSolver::getFirstProgLineInProc(PROC_INDEX procIndex) {
	return firstProgLineInProc[procIndex];
}
PROG_LINE_ NextSolver::getLastProgLineInProc(PROC_INDEX procIndex) {
	return lastProgLineInProc[procIndex];
}

void NextSolver::setFirstProgLineInProc(PROC_INDEX procIndex, PROG_LINE_ firstlines) {
	firstProgLineInProc.push_back(firstlines);
}
void NextSolver::setLastProgLineInProc(PROC_INDEX procIndex, PROG_LINE_ lastlines) {
	lastProgLineInProc.push_back(lastlines);

}

BOOLEAN_ NextSolver::setProgLineInWhile(PROG_LINE_ progline) {
	isProgLineInWhile = boost::dynamic_bitset<>(PKB::getInstance().getStmtTableSize() + 1);
	isProgLineInWhile.set(progline);
	return true;
}

BOOLEAN_ NextSolver::isValid() {
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

BOOLEAN_ NextSolver::setLhs(PROG_LINE_ newLine) {
	lhs.push_back(newLine);
	return true;
}
BOOLEAN_ NextSolver::setRhs(PROG_LINE_ newLine) {
	rhs.push_back(newLine);
	return true;
}


BOOLEAN_ NextSolver::isLhsValid(PROG_LINE_ lhs) {
	return find(this->lhs.begin(), this->lhs.end(), lhs) != this->lhs.end();
}

BOOLEAN_ NextSolver::isRhsValid(PROG_LINE_ rhs) {
	return find(this->rhs.begin(), this->rhs.end(), rhs) != this->rhs.end();
}	