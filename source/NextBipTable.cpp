#include <stdio.h>
#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <deque>
#include <stack>

#include "PKB.h"
#include "NextBipTable.h"

using namespace std;

typedef pair<CNode*, stack<CNode*> > NEXTBIP_STATE;


NextBipTable::NextBipTable() {
}


void updateStateOfBfs(set<NEXTBIP_STATE>* visited, NEXTBIP_STATE nextState, deque<NEXTBIP_STATE >& frontier, set<int>& result) {
	CNode* node = nextState.first;
	if (node->getNodeType() == Proc_C) {
		return;
	}

	if (visited->count(nextState) == 0) {
		frontier.push_back(nextState);
		if (node->getNodeType() != EndIf_C && node->getNodeType() != EndProc_C){
			result.insert(node->getProcLineNumber());
		}
		visited->insert(nextState);
	}
}

CNode* getNodeCalledByProgLine(CNode* nextNode) {
	PKB pkb = PKB::getInstance();

	TNode* astNode = pkb.getNodeForStmt(nextNode->getProcLineNumber());
	int procIndex = astNode->getNodeValueIdx();
	int procLineToGoTo = pkb.getFirstProgLineInProc(procIndex);

	assert(pkb.cfgNodeTable.count(procLineToGoTo) != 0);
	CNode* nodeOfProc = pkb.cfgNodeTable.at(procLineToGoTo);

	return nodeOfProc;
}

PROGLINE_LIST NextBipTable::getNextBipAfter(PROG_LINE_ progLine1, TRANS_CLOSURE transitiveClosure) {
	PKB pkb = PKB::getInstance();
	if (pkb.cfgNodeTable.count(progLine1) == 0) { // progline does not have a cfg node
		return vector<int>();
	}
	CNode* curNode = pkb.cfgNodeTable.at(progLine1);
	stack<CNode*> afterCall;
	set<int> result;

	deque<NEXTBIP_STATE > frontier; 
	set<NEXTBIP_STATE>* visited = new set<NEXTBIP_STATE>();

	frontier.push_back(NEXTBIP_STATE(curNode, stack<CNode*>() ) );

	while (!frontier.empty()) {
		NEXTBIP_STATE curState = frontier.back();
		curNode = curState.first; 
		afterCall = curState.second;
		frontier.pop_back();

		vector<CNode*>* nextNodes = curNode->getAfter();

		if (curNode->getNodeType() == EndProc_C) {
			// pop the top of the stack, and go to that node which was on the top

			if (afterCall.size() > 0) {
				CNode* nodeAfterCall = afterCall.top(); afterCall.pop();
				updateStateOfBfs(visited, NEXTBIP_STATE(nodeAfterCall, afterCall), frontier, result);
			} else {

			}
		} else if (curNode->getNodeType() == Call_C) {
			// for call statement
			// update afterCall to include the progline after the call
			assert(nextNodes->size() == 1);
			afterCall.push(nextNodes->at(0)); // for call-statements, only one possible next node so no need to iterate through the list

			// get the first node of the procedure
			CNode* nodeOfProc = getNodeCalledByProgLine(curNode);

			updateStateOfBfs(visited, NEXTBIP_STATE(nodeOfProc, afterCall), frontier, result);
		} else {
			// for non-call statement, follow the same thing as Next
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

				updateStateOfBfs(visited, NEXTBIP_STATE(node, afterCall), frontier, result);			
			}
		}

		// break after the first iteration if we are not finding transitive closure
		if (!transitiveClosure) {
			break;
		}
	}

	vector<int> resultAsVector;
	resultAsVector.insert(resultAsVector.end(), result.begin(), result.end());
	return resultAsVector;
}

PROGLINE_LIST NextBipTable::getNextBipBefore(PROG_LINE_ progLine2, TRANS_CLOSURE transitiveClosure) {
	/*PKB pkb = PKB::getInstance();
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

	return result;*/
	return vector<int>();
}



BOOLEAN_ NextBipTable::isNextBip(PROG_LINE_ progLine1, PROG_LINE_ progLine2, TRANS_CLOSURE transitiveClosure) {
	
	return false;
}


PROGLINE_LIST NextBipTable::getLhs() {	
	return vector<int>();
}

PROGLINE_LIST NextBipTable::getRhs() {
	return vector<int>();
}
