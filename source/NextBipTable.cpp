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
	//if (node->getNodeType() == Proc_C) {    // what is this for?
	//	return;
	//}

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

	TNode* astNode = nextNode->getASTref();
	int procIndex = astNode->getNodeValueIdx();
	int procLineToGoTo = pkb.getFirstProgLineInProc(procIndex);

	assert(pkb.cfgNodeTable.count(procLineToGoTo) != 0);
	CNode* nodeOfProc = pkb.cfgNodeTable.at(procLineToGoTo);

	return nodeOfProc;
}

CNode* getLastNodeInProcCalledByProgLine(CNode* nextNode) {
	PKB pkb = PKB::getInstance();

	TNode* astNode = nextNode->getASTref();
	int procIndex = astNode->getNodeValueIdx();
	
	CNode* nodeOfProc = pkb.cfgTable.at(procIndex)->getProcEnd();
	return nodeOfProc;
}

vector<CNode*> getCallStatementsToProc(int procIndex) {
	//todo: do this function as a precomputation
	PKB pkb = PKB::getInstance();
	
	vector<int> possibleCallStmts = pkb.getStmtNumForType(CALL);
	vector<CNode*> callStmtsToCurrentProc;

	// filter out the calls to other procs
	for (auto iter = possibleCallStmts.begin(); iter != possibleCallStmts.end(); ++iter) {
		TNode* astNode = pkb.getNodeForStmt(*iter);
		if (astNode->getNodeValueIdx() == procIndex) {
			CNode* cNode = pkb.cfgNodeTable.at(*iter);
			callStmtsToCurrentProc.push_back(cNode);
		}
	}

	return callStmtsToCurrentProc;
}

vector<CNode*> getNextNodesOfNodes(vector<CNode*> nodes) {
	vector<CNode*> result;
	for (auto iter = nodes.begin(); iter != nodes.end(); ++iter) {
		vector<CNode*>* nextNodes = (*iter)->getAfter();

		result.insert(result.end(), nextNodes->begin(), nextNodes->end());
	}

	return result;
}

vector<CNode*> getPrevNodesOfNodes(vector<CNode*> nodes) {
	vector<CNode*> result;
	for (auto iter = nodes.begin(); iter != nodes.end(); ++iter) {
		vector<CNode*>* nextNodes = (*iter)->getBefore();

		result.insert(result.end(), nextNodes->begin(), nextNodes->end());
	}

	return result;
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
				// if there is no history of which line called this procedure, need to find all possible execution paths in SIMPLE
				int curProcIndex = curNode->getASTref()->getNodeValueIdx();
				
				vector<CNode*> possibleNodes = getCallStatementsToProc(curProcIndex);

				for (auto iter = possibleNodes.begin(); iter != possibleNodes.end(); ++iter) {
					updateStateOfBfs(visited, NEXTBIP_STATE(*iter, afterCall), frontier, result);
				}
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

void expandProc(CNode* curNode, stack<CNode*> afterCall, set<NEXTBIP_STATE>* visited, deque<NEXTBIP_STATE> frontier, set<int>& result) {
	// pop the top of the stack, and go to that node which was on the top
	if (afterCall.size() > 0) {
		CNode* nodeAfterCall = afterCall.top(); afterCall.pop();
		updateStateOfBfs(visited, NEXTBIP_STATE(nodeAfterCall, afterCall), frontier, result);
	} else {
		// if there is no history of which line called this procedure, need to find all possible execution paths in SIMPLE
		int curProcIndex = curNode->getASTref()->getNodeValueIdx();
			
		vector<CNode*> possibleNodes = getCallStatementsToProc(curProcIndex);
	
		for (auto iter = possibleNodes.begin(); iter != possibleNodes.end(); ++iter) {
			updateStateOfBfs(visited, NEXTBIP_STATE(*iter, afterCall), frontier, result);
		}
	}
	
}

PROGLINE_LIST NextBipTable::getNextBipBefore(PROG_LINE_ progLine2, TRANS_CLOSURE transitiveClosure) {
	PKB pkb = PKB::getInstance();
	if (pkb.cfgNodeTable.count(progLine2) == 0) { // progline does not have a cfg node
		return vector<int>();
	}
	CNode* curNode = pkb.cfgNodeTable.at(progLine2);
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

		vector<CNode*>* nextNodes = curNode->getBefore();
		 
			// for non-call statement, follow the same thing as Next
		for (auto iter = nextNodes->begin(); iter != nextNodes->end(); ++iter) {
			CNode* node = *iter;

			//
			if (node->getNodeType() != EndIf_C) {
				if (node->getNodeType() == Proc_C) {
					expandProc(node, afterCall, visited, frontier, result);
				} else {
					updateStateOfBfs(visited, NEXTBIP_STATE(node, afterCall), frontier, result);	
				}

			} else {
				// 
				vector<CNode*> nodesToTraverseBack;
				CNode* prevNodeAfterEndIf = node->getBefore()->at(0);
				CNode* prevNodeAfterEndIf2 = node->getBefore()->at(1);

				nodesToTraverseBack.push_back(prevNodeAfterEndIf); 
				nodesToTraverseBack.push_back(prevNodeAfterEndIf2);
					
				while (!nodesToTraverseBack.empty()) {
					CNode* nodeToExpand = nodesToTraverseBack.back();
					nodesToTraverseBack.pop_back();
						
					if (nodeToExpand->getNodeType() == Proc_C) {
						expandProc(nodeToExpand, afterCall, visited, frontier, result);
					} else {
						updateStateOfBfs(visited, NEXTBIP_STATE(nodeToExpand, afterCall), frontier, result);	
					}

					if (nodeToExpand->getNodeType() == EndIf_C) {
						prevNodeAfterEndIf = node->getBefore()->at(0);
						prevNodeAfterEndIf2 = node->getBefore()->at(1);

						nodesToTraverseBack.push_back(prevNodeAfterEndIf);
						nodesToTraverseBack.push_back(prevNodeAfterEndIf2);
					}
				}

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



BOOLEAN_ NextBipTable::isNextBip(PROG_LINE_ progLine1, PROG_LINE_ progLine2, TRANS_CLOSURE transitiveClosure) {
	
	return false;
}


PROGLINE_LIST NextBipTable::getLhs() {	
	return vector<int>();
}

PROGLINE_LIST NextBipTable::getRhs() {
	return vector<int>();
}
