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



NextBipTable::NextBipTable() {
}

vector<CNode*> getNextNodesOfNodes(vector<CNode*> nodes);

void updateStateOfBfs(set<NEXTBIP_STATE>* visited, NEXTBIP_STATE nextState, deque<NEXTBIP_STATE >& frontier, set<int>& result) {
	CNode* node = nextState.first;

	if (visited->count(nextState) == 0) {
		frontier.push_back(nextState);
		if (node->getNodeType() != EndIf_C && node->getNodeType() != EndProc_C) {
			result.insert(node->getProcLineNumber());
		}
		visited->insert(nextState);
	}
}

CNode* getNodeCalledByProgLine(CNode* nextNode) {
	assert(nextNode->getNodeType() == Call_C);

	PKB pkb = PKB::getInstance();

	TNode* astNode = nextNode->getASTref();
	int procIndex = astNode->getNodeValueIdx();
	int procLineToGoTo = pkb.getFirstProgLineInProc(procIndex);

	assert(pkb.cfgNodeTable.count(procLineToGoTo) != 0);
	CNode* nodeOfProc = pkb.cfgNodeTable.at(procLineToGoTo);

	return nodeOfProc;
}

vector<CNode*> getLastNodeInProcCalledByProgLine(CNode* nextNode) {
	assert(nextNode->getNodeType() == Call_C);

	PKB pkb = PKB::getInstance();

	TNode* astNode = nextNode->getASTref();
	int procIndex = astNode->getNodeValueIdx();
	
	CNode* nodeOfProc = pkb.cfgTable.at(procIndex)->getProcEnd();

	assert( nodeOfProc->getBefore()->size() == 1);

	vector<CNode*> previousNodes;
	vector<CNode*>* candidateNodes = nodeOfProc->getBefore();

	for (auto iter = candidateNodes->begin(); iter != candidateNodes->end(); ++iter) {
		CNode* node = *iter;
		if (node->getNodeType() != Call_C) {
			previousNodes.push_back(node);
		} else {
			vector<CNode*> nodesFromNestedCallProcedure = getLastNodeInProcCalledByProgLine(node);
			previousNodes.insert(previousNodes.end(), nodesFromNestedCallProcedure.begin(), nodesFromNestedCallProcedure.end());
		}
	}

	return previousNodes;
	
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

/**
 * Get next nodes, accounting for nested consecutive EndProcs
 */
vector<CNode*>* getAfterNode(CNode* node) {
	vector<CNode*>* candidateList = node->getAfter();
	vector<CNode*>* resultList = new vector<CNode*>();
	for (unsigned int i = 0; i < candidateList->size(); i++) {
		CNode* candidateNode = candidateList->at(i);
		while (candidateNode->getNodeType() == EndIf_C) {
			assert(candidateNode->getAfter()->size() <= 1);

			if (candidateNode->getAfter()->size() == 0) { // if is the last progline
				continue;
			}
			CNode* nextNodeAfterEndIf = candidateNode->getAfter()->at(0);
			candidateNode = nextNodeAfterEndIf;
		}

		if (candidateNode->getNodeType() == EndProc_C) {
			vector<CNode*> callStmts = getNextNodesOfNodes(getCallStatementsToProc(candidateNode->getASTref()->getNodeValueIdx()));
			resultList->insert(resultList->end(), callStmts.begin(), callStmts.end());

		} else {
			// default case: add to result
			resultList->push_back(candidateNode);
		}
	}

	return resultList;
}

vector<CNode*> getNextNodesOfNodes(vector<CNode*> nodes) {
	vector<CNode*> result;
	for (auto iter = nodes.begin(); iter != nodes.end(); ++iter) {
		
		vector<CNode*>* nextNodes = getAfterNode(*iter);

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
				
				vector<CNode*> possibleNodes = getNextNodesOfNodes(getCallStatementsToProc(curProcIndex));

				for (auto iter = possibleNodes.begin(); iter != possibleNodes.end(); ++iter) {
					updateStateOfBfs(visited, NEXTBIP_STATE(*iter, afterCall), frontier, result);
				}
			}
		}
		else if (curNode->getNodeType() == Call_C) {
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
				if (node->getNodeType() == EndProc_C) {
					// pop the top of the stack, and go to that node which was on the top

					if (afterCall.size() > 0) {
						CNode* nodeAfterCall = afterCall.top(); afterCall.pop();
						updateStateOfBfs(visited, NEXTBIP_STATE(nodeAfterCall, afterCall), frontier, result);
					} else {
						// if there is no history of which line called this procedure, need to find all possible execution paths in SIMPLE
						int curProcIndex = node->getASTref()->getNodeValueIdx();
				
						vector<CNode*> possibleNodes = getNextNodesOfNodes(getCallStatementsToProc(curProcIndex));

						for (auto iter = possibleNodes.begin(); iter != possibleNodes.end(); ++iter) {
							updateStateOfBfs(visited, NEXTBIP_STATE(*iter, afterCall), frontier, result);
						}
					}
				} else {
					updateStateOfBfs(visited, NEXTBIP_STATE(node, afterCall), frontier, result);			
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

void expandProc(CNode* curNode, stack<CNode*> afterCall, set<NEXTBIP_STATE>* visited, deque<NEXTBIP_STATE>& frontier, set<int>& result) {
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
		 
		for (auto iter = nextNodes->begin(); iter != nextNodes->end(); ++iter) {
			CNode* node = *iter;

			// call stmt: expand to the last nodes of the procedure called
			if (node->getNodeType() == Call_C) {

				vector<CNode*> lastNodes = getLastNodeInProcCalledByProgLine(node);

				for (auto lastNodeIter = lastNodes.begin(); lastNodeIter != lastNodes.end(); ++lastNodeIter) {
					updateStateOfBfs(visited, NEXTBIP_STATE(*lastNodeIter, afterCall), frontier, result);	
				}

			} else if (node->getNodeType() != EndIf_C) {
				// normal case: on a Start of Proc, pop the stack/ expand to all call statements calling the proc
				if (node->getNodeType() == Proc_C) {
					expandProc(node, afterCall, visited, frontier, result);
				} else {
					// otherwise just traverse
					updateStateOfBfs(visited, NEXTBIP_STATE(node, afterCall), frontier, result);	
				}

			} else {
				// for EndIf nodes, need to get both branches
				vector<CNode*> nodesToTraverseBack;
				CNode* prevNodeAfterEndIf = node->getBefore()->at(0);
				CNode* prevNodeAfterEndIf2 = node->getBefore()->at(1);

				nodesToTraverseBack.push_back(prevNodeAfterEndIf); 
				nodesToTraverseBack.push_back(prevNodeAfterEndIf2);
				
				// updates results and frontier, accounting of EndIf nodes possibly leading to multiple nodes
				while (!nodesToTraverseBack.empty()) {
					CNode* nodeToExpand = nodesToTraverseBack.back();
					nodesToTraverseBack.pop_back();
						
					if (nodeToExpand->getNodeType() == Proc_C) {
						expandProc(nodeToExpand, afterCall, visited, frontier, result);
						continue;
					} 

					updateStateOfBfs(visited, NEXTBIP_STATE(nodeToExpand, afterCall), frontier, result);	

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
	// unoptimised
	vector<int> afterProgLine1 = getNextBipAfter(progLine1, transitiveClosure);

	return find(afterProgLine1.begin(), afterProgLine1.end(), progLine2) != afterProgLine1.end();
}


PROGLINE_LIST NextBipTable::getLhs() {	
	PKB pkb = PKB::getInstance();
	vector<int> result;

	for (size_t i = 1; i <= pkb.getStmtTableSize(); i++) {
		vector<int> rhs = getNextBipAfter(i);
		if (rhs.size() > 0) {
			result.push_back(i);
		}
	}

	return result;
}

PROGLINE_LIST NextBipTable::getRhs() {
	PKB pkb = PKB::getInstance();
	vector<int> result;

	for (size_t i = 1; i <= pkb.getStmtTableSize(); i++) {
		vector<int> lhs = getNextBipBefore(i);
		if (lhs.size() > 0) {
			result.push_back(i);
		}
	}

	return result;
}
