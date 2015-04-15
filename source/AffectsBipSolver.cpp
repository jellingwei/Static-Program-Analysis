#pragma once

#include<stdio.h>
#include <iostream>
#include <string>
#include <vector>
#include <unordered_map>
#include <algorithm>
#include <queue>
#include <unordered_set>
#include <map>

#include "boost/dynamic_bitset.hpp"

#include "AffectsBipSolver.h"
#include "PKB.h"


struct VariableTestedCompare {
  bool operator() (const NEXTBIP_STATE& lhs, const NEXTBIP_STATE& rhs) const {
	  
	  if (lhs.first->getProcLineNumber() != rhs.first->getProcLineNumber()) {
		  // compare by proc lines first
		  return lhs.first->getProcLineNumber() < rhs.first->getProcLineNumber();
	  } else {
		  // compare the stacks
		  return lhs.second < rhs.second;
	  }
  }
};

/**
 * For use in the priority queue when traversing the CFG for Affects
 */
class CompareAffectsBip {
    public:
		bool operator() (pair<NEXTBIP_STATE, boost::dynamic_bitset<> >& pair1, pair<NEXTBIP_STATE, boost::dynamic_bitset<> >& pair2) { 
			CNode* nodeInState1 = pair1.first.first;
			CNode* nodeInState2 = pair2.first.first;
			return nodeInState1->getProcLineNumber() < nodeInState2->getProcLineNumber(); 
		}
};

class CompareAffectsBipReverse {
    public:
		bool operator() (pair<NEXTBIP_STATE, boost::dynamic_bitset<> >& pair1, pair<NEXTBIP_STATE, boost::dynamic_bitset<> >& pair2) { 
			CNode* nodeInState1 = pair1.first.first;
			CNode* nodeInState2 = pair2.first.first;
			return nodeInState1->getProcLineNumber() > nodeInState2->getProcLineNumber(); 
		}
};


BOOLEAN_ isVisitedBefore(map<NEXTBIP_STATE, boost::dynamic_bitset<>, VariableTestedCompare > variableTested, NEXTBIP_STATE nextState, 
	                     pair<NEXTBIP_STATE, boost::dynamic_bitset<> > nodePair, set<pair<NEXTBIP_STATE, boost::dynamic_bitset<> >> visited, 
						 boost::dynamic_bitset<> variablesToMatch) {

	bool isVisitedBefore = false;
	
	if (variableTested.count(nextState) != 0) {
		if (variablesToMatch.is_subset_of(variableTested.at(nextState) ) ) {
			isVisitedBefore = true;
		}
	}
	
	isVisitedBefore |= visited.count(nodePair) != 0 ;
	return isVisitedBefore;
}


void populateFrontierWithNextBipAfter(priority_queue<pair<NEXTBIP_STATE, boost::dynamic_bitset<> >, vector<pair<NEXTBIP_STATE, boost::dynamic_bitset<> > >, CompareAffectsBip>& frontier, 
								      CNode* node, set<pair<NEXTBIP_STATE, boost::dynamic_bitset<> >> visited, NEXTBIP_STATE curState, 
									  boost::dynamic_bitset<> variablesToMatch) {
	try {
		PKB pkb = PKB::getInstance();

		set<NEXTBIP_STATE> nextNodeNums = pkb.getNextBipAfterWithState(node, curState.second);
		queue<NEXTBIP_STATE> nodesToAdd;
		for (auto nextbipIter = nextNodeNums.begin(); nextbipIter != nextNodeNums.end(); ++nextbipIter) {
			nodesToAdd.push(*nextbipIter);
		}

		while (!nodesToAdd.empty()) {
			NEXTBIP_STATE nextState = nodesToAdd.front();
			nodesToAdd.pop();

			CNode* nextNode = nextState.first;
			stack<CNode*> nextStateStack = nextState.second;

			if (nextNode && nextNode->getNodeType() != Proc_C && nextNode->getNodeType() != EndProc_C && 
				nextNode->getNodeType() != EndIf_C) {

				pair<NEXTBIP_STATE, boost::dynamic_bitset<>> nodePair = 
				make_pair<NEXTBIP_STATE, boost::dynamic_bitset<> >(NEXTBIP_STATE(nextNode, nextStateStack), variablesToMatch);
			
				frontier.push(nodePair);
			} else {
				deque<NEXTBIP_STATE> candidateResultNodes;
				candidateResultNodes.push_back(nextState);

				while (!candidateResultNodes.empty()) {
					NEXTBIP_STATE state = candidateResultNodes.back();
					CNode* node = state.first;

					candidateResultNodes.pop_back();

					if (node->getNodeType() != EndIf_C && node->getNodeType() != EndProc_C  && node->getNodeType() != Proc_C) {
						pair<NEXTBIP_STATE, boost::dynamic_bitset<>> nodePair = 
							make_pair<NEXTBIP_STATE, boost::dynamic_bitset<> >(NEXTBIP_STATE(node, state.second), variablesToMatch);
						frontier.push(nodePair);
					} else {
						set<NEXTBIP_STATE> expandedNodes = pkb.getNextBipAfterWithState(node, state.second);
						candidateResultNodes.insert(candidateResultNodes.end(), expandedNodes.begin(), expandedNodes.end());
					}
				}

			}
		}
	} catch (exception e) {
		cout << "in populateFrontierWithNextBipAfter: " << endl;
		cout << e.what() << endl;
	}
}

void populateFrontierWithNextAfter(priority_queue<pair<NEXTBIP_STATE, boost::dynamic_bitset<> >, vector<pair<NEXTBIP_STATE, boost::dynamic_bitset<> > >, CompareAffectsBip>& frontier, 
								      CNode* node, set<pair<NEXTBIP_STATE, boost::dynamic_bitset<> >> visited, NEXTBIP_STATE curState, 
									  boost::dynamic_bitset<> variablesToMatch) {
	PKB pkb = PKB::getInstance();
	PROGLINE_LIST nextNodeNums = pkb.getNextAfter(node->getProcLineNumber());
		
	for (auto nodeIter = nextNodeNums.begin(); nodeIter != nextNodeNums.end(); ++nodeIter) {
		CNode* nextNode = pkb.cfgNodeTable.at(*nodeIter);
		pair<NEXTBIP_STATE, boost::dynamic_bitset<>> nodePair = make_pair<NEXTBIP_STATE, boost::dynamic_bitset<> >(NEXTBIP_STATE(nextNode, curState.second), variablesToMatch);
			
		if (nextNode ) {
			frontier.push(nodePair);
		}
	}
}

void populateFrontierWithNextBipBefore(priority_queue<pair<NEXTBIP_STATE, boost::dynamic_bitset<> >, vector<pair<NEXTBIP_STATE, boost::dynamic_bitset<> > >, CompareAffectsBip>& frontier, 
	                                   CNode* node, set<pair<NEXTBIP_STATE, boost::dynamic_bitset<> >> visited, 
									   map<NEXTBIP_STATE, boost::dynamic_bitset<>, VariableTestedCompare > variableTested,
									   NEXTBIP_STATE& curState, 
									   boost::dynamic_bitset<> variablesToMatch) {
	PKB pkb = PKB::getInstance();
	set<NEXTBIP_STATE> prevNodeNums = pkb.getNextBipBeforeWithState(node, curState.second);
	queue<NEXTBIP_STATE> nodesToAdd;
	for (auto nextbipIter = prevNodeNums.begin(); nextbipIter != prevNodeNums.end(); ++nextbipIter) {
		nodesToAdd.push(*nextbipIter);
	}
	
	while (!nodesToAdd.empty()) {
		NEXTBIP_STATE nextState = nodesToAdd.front();
		nodesToAdd.pop();

		CNode* nextNode = nextState.first;
		stack<CNode*> nextStateStack = nextState.second;

		pair<NEXTBIP_STATE, boost::dynamic_bitset<>> nodePair = 
			make_pair<NEXTBIP_STATE, boost::dynamic_bitset<> >(NEXTBIP_STATE(nextNode, nextStateStack), variablesToMatch);

		if (nextNode) {
			frontier.push(nodePair);
		} 
	}
}


void populateFrontierWithNextBefore(priority_queue<pair<NEXTBIP_STATE, boost::dynamic_bitset<> >, vector<pair<NEXTBIP_STATE, boost::dynamic_bitset<> > >, CompareAffectsBip>& frontier, 
	                                   CNode* node, set<pair<NEXTBIP_STATE, boost::dynamic_bitset<> >> visited, 
									   map<NEXTBIP_STATE, boost::dynamic_bitset<>, VariableTestedCompare > variableTested,
									   NEXTBIP_STATE& curState, 
									   boost::dynamic_bitset<> variablesToMatch) {
	PKB pkb = PKB::getInstance();
	PROGLINE_LIST nextNodeNums = pkb.getNextBefore(node->getProcLineNumber());
		
	for (auto nodeIter = nextNodeNums.begin(); nodeIter != nextNodeNums.end(); ++nodeIter) {
		CNode* nextNode = pkb.cfgNodeTable.at(*nodeIter);
		pair<NEXTBIP_STATE, boost::dynamic_bitset<>> nodePair = make_pair<NEXTBIP_STATE, boost::dynamic_bitset<> >(NEXTBIP_STATE(nextNode, curState.second), variablesToMatch);
			
		if (nextNode ) {
			frontier.push(nodePair);
		}
	}
}

void handleLastLine(CNode* node, priority_queue<pair<NEXTBIP_STATE, boost::dynamic_bitset<> >, vector<pair<NEXTBIP_STATE, boost::dynamic_bitset<> > >, CompareAffectsBip>& frontier, 
	                set<pair<NEXTBIP_STATE, boost::dynamic_bitset<> >> visited, NEXTBIP_STATE curState, boost::dynamic_bitset<> variablesToMatch) {

	if (node->getNodeType() == Call_C || curState.second.size() == 0) {
		// expand as per normal, for call statements or if no traversal history
		populateFrontierWithNextBipAfter(frontier, node, visited, curState, variablesToMatch);
	}
	else		
	// pop from the stack and add the progline into the frontier, if stack has traversal history
	if (curState.second.size() > 0) {
		CNode* candidateNextNode = curState.second.top();
		curState.second.pop();

		// test for special dummy 'termination' node
		if (candidateNextNode->getNodeType() == EndProc_C && candidateNextNode->getProcLineNumber() == -1) {
			return;
		}

		while (candidateNextNode->getNodeType() == EndIf_C || candidateNextNode->getNodeType() == EndProc_C) {
			if (candidateNextNode->getNodeType() == EndIf_C) {
				// get next node
				candidateNextNode = candidateNextNode->getAfter()->at(0);
			} else {
				// take node from the stack again
				candidateNextNode = curState.second.top();
				curState.second.pop();
			}
		}

		NEXTBIP_STATE nextState = NEXTBIP_STATE(candidateNextNode, curState.second);
		pair<NEXTBIP_STATE, boost::dynamic_bitset<> > nodePair = 
			make_pair<NEXTBIP_STATE, boost::dynamic_bitset<> >(nextState, variablesToMatch);
			
		if (candidateNextNode && visited.count(nodePair) == 0 ) {
			frontier.push(nodePair);
		}
	}

}

void handleFirstLine(CNode* node, priority_queue<pair<NEXTBIP_STATE, boost::dynamic_bitset<> >, vector<pair<NEXTBIP_STATE, boost::dynamic_bitset<> > >, CompareAffectsBip>& frontier, 
	                set<pair<NEXTBIP_STATE, boost::dynamic_bitset<> >> visited, NEXTBIP_STATE curState, boost::dynamic_bitset<> variablesToMatch, map<NEXTBIP_STATE, boost::dynamic_bitset<> , VariableTestedCompare> variableTested, CNode* terminateNode) {

	if (curState.second.size() == 0) {
		// expand as per normal, if no traversal history
		populateFrontierWithNextBipBefore(frontier, node, visited, variableTested, curState, variablesToMatch);
	}
	else { // pop from the stack and add the progline into the frontier, if stack has traversal history
		CNode* candidateNextNode = curState.second.top();
		curState.second.pop();

		vector<CNode*> nodesToTraverseBack;
		nodesToTraverseBack.push_back(candidateNextNode); 
		
		while (!nodesToTraverseBack.empty()) {
			candidateNextNode = nodesToTraverseBack.back();
			nodesToTraverseBack.pop_back();

			if (candidateNextNode->getNodeType() == EndIf_C) {
				// get both prev nodes
				candidateNextNode = candidateNextNode->getBefore()->at(0);
				nodesToTraverseBack.push_back(candidateNextNode);

				candidateNextNode = candidateNextNode->getBefore()->at(1);
				nodesToTraverseBack.push_back(candidateNextNode);

			} else if (candidateNextNode->getNodeType() == EndProc_C) {
				// take node from the stack again
				if (curState.second.size() > 0) {
					candidateNextNode = curState.second.top();
					curState.second.pop();

					nodesToTraverseBack.push_back(candidateNextNode);
				} else {
					nodesToTraverseBack.push_back(terminateNode);
				}
			} else {
				NEXTBIP_STATE nextState = NEXTBIP_STATE(candidateNextNode, curState.second);
				pair<NEXTBIP_STATE, boost::dynamic_bitset<> > nodePair = 
					make_pair<NEXTBIP_STATE, boost::dynamic_bitset<> >(nextState, variablesToMatch);
			
				if (candidateNextNode && visited.count(nodePair) == 0 ) {
					frontier.push(nodePair);
				}
			}
		}

		
	}
}

PROGLINE_LIST AffectsBipTable::getProgLinesAffectsBipAfter(PROG_LINE_ progLine1, TRANS_CLOSURE transitiveClosure, bool terminateOnOneResult) {
	PKB pkb = PKB::getInstance();
	// verify that progLine1 is a program line and is an assignment statement
	if (pkb.cfgNodeTable.count(progLine1) == 0) {
		return vector<int>();
	}
	CNode* node = pkb.cfgNodeTable.at(progLine1);
	if (node->getNodeType() != Assign_C) {
		return vector<int>();
	}

	vector<int> result;

	node = pkb.cfgNodeTable.at(progLine1);
	bool isTestingFirstLine = true;

	boost::dynamic_bitset<> variablesToMatch = pkb.getModVarInBitvectorForStmt(progLine1);
	//@todo, refactor
	priority_queue<pair<NEXTBIP_STATE, boost::dynamic_bitset<> >, vector<pair<NEXTBIP_STATE, boost::dynamic_bitset<> > >, CompareAffectsBip> frontier;

	// for AffectsBip, do DFS over NEXTBIP_STATE rather than over CNodes
	frontier.push(pair<NEXTBIP_STATE, boost::dynamic_bitset<> >(NEXTBIP_STATE(node, stack<CNode*>()), variablesToMatch));

	set<int> addedToAnswer;
	set<pair<NEXTBIP_STATE, boost::dynamic_bitset<> >> visited;	
	map<NEXTBIP_STATE, boost::dynamic_bitset<>, VariableTestedCompare > variableTested;

	//dummy termination node
	CNode* terminateNode = new CNode(EndProc_C, -1, NULL, NULL);
	while (!frontier.empty()) {
		
		//visited.insert(frontier.top());
		assert(node->getNodeType() != EndIf_C && node->getNodeType() != EndProc_C ); // don't store dummy nodes in frontier
																					 // otherwise cannot make use of NextBip API

		NEXTBIP_STATE curState = frontier.top().first; variablesToMatch = frontier.top().second;
		CNode* node = curState.first;

		if (!isTestingFirstLine) {
			if (variableTested.count(curState) != 0) {
				if (variablesToMatch.is_subset_of(variableTested.at(curState) ) ) {
					frontier.pop();
					continue;
				}
			}
			if (variableTested.count(curState) != 0) {
				variableTested[curState] |=  variablesToMatch;
			} else {
				variableTested[curState] =  variablesToMatch;
			}
		}
		
		frontier.pop();

		
		if (node->getNodeType() == Assign_C && !isTestingFirstLine) {
			boost::dynamic_bitset<> variablesModified = pkb.getModVarInBitvectorForStmt(node->getProcLineNumber());
			boost::dynamic_bitset<> variablesUsed = pkb.getUseVarInBitvectorForStmt(node->getProcLineNumber());
			
			// test for results
			bool isAssignment = node->getNodeType() == Assign_C ;
			bool isResultsModified = false;
			bool isAlreadyAddedToResults = find(addedToAnswer.begin(), addedToAnswer.end(), node->getProcLineNumber()) != addedToAnswer.end();

			if (!((variablesToMatch & variablesUsed).none()) && isAssignment) {
				if (!isAlreadyAddedToResults) {
					result.push_back(node->getProcLineNumber());
					addedToAnswer.insert(node->getProcLineNumber());
				}
				isResultsModified = true;

				// early return. this is used if the presense of one result is sufficient for the query
				if (terminateOnOneResult) {
					return result;
				}
			}

			// reset any re-defined variables
			variablesToMatch &= ~variablesModified;

			// set variables for indirectly affected stmts
			if (transitiveClosure && isResultsModified) {
				variablesToMatch |= variablesModified;
			}

			// if there are no more variables to match, don't explore this path further
			if (variablesToMatch.none()) {
				continue;
			}
		}

		isTestingFirstLine = false;

		// update frontier with new nodes
		populateFrontierWithNextBipAfter(frontier, node, visited, curState, variablesToMatch);
		
		
	}
	delete terminateNode;
	
	return result;
}


PROGLINE_LIST AffectsBipTable::getProgLinesAffectsBipBefore(PROG_LINE_ progLine2, bool transitiveClosure, bool terminateOnOneResult) {
	PKB pkb = PKB::getInstance();
	// verify that progLine2 is a program line and is an assignment statement
	if (pkb.cfgNodeTable.count(progLine2) == 0) {
		return vector<int>();
	}
	CNode* node = pkb.cfgNodeTable.at(progLine2);
	if (node->getNodeType() != Assign_C) {
		return vector<int>();
	}

	vector<int> result;

	node = pkb.cfgNodeTable.at(progLine2);
	bool isTestingFirstNode = true;

	boost::dynamic_bitset<> variablesToMatch = pkb.getUseVarInBitvectorForStmt(progLine2);
	//@todo, refactor
	priority_queue<pair<NEXTBIP_STATE, boost::dynamic_bitset<> >, vector<pair<NEXTBIP_STATE, boost::dynamic_bitset<> > >, CompareAffectsBip> frontier;

	// for AffectsBip, do DFS over NEXTBIP_STATE rather than over CNodes
	frontier.push(pair<NEXTBIP_STATE, boost::dynamic_bitset<> >(NEXTBIP_STATE(node, stack<CNode*>()), variablesToMatch));

	set<int> addedToAnswer;
	set<pair<NEXTBIP_STATE, boost::dynamic_bitset<> >> visited;	

	map<NEXTBIP_STATE, boost::dynamic_bitset<>, VariableTestedCompare > variableTested;
	CNode* terminateNode = new CNode(EndProc_C, -1, NULL, NULL); // dummy node for terminating in some cases

	while (!frontier.empty()) {
		
		NEXTBIP_STATE curState = frontier.top().first; variablesToMatch = frontier.top().second;
		CNode* node = curState.first;

		if (!isTestingFirstNode) {
			if (variableTested.count(curState) != 0) {
				if (variablesToMatch.is_subset_of(variableTested.at(curState) ) ) {
					frontier.pop();
					continue;
				}
			}
			if (variableTested.count(frontier.top().first) != 0) {
				variableTested[frontier.top().first] |=  variablesToMatch;
			} else {
				variableTested[frontier.top().first] =  variablesToMatch;
			}
		}

		assert(node->getNodeType() != EndIf_C && node->getNodeType() != EndProc_C ); // don't store dummy nodes in frontier
																					 // otherwise cannot make use of NextBip API
		frontier.pop();

		// special handling of different node types 
		if (node->getNodeType() == Assign_C && !isTestingFirstNode) {
			boost::dynamic_bitset<> variablesModified = pkb.getModVarInBitvectorForStmt(node->getProcLineNumber());
			boost::dynamic_bitset<> variablesUsed = pkb.getUseVarInBitvectorForStmt(node->getProcLineNumber());
		
			// test for results
			bool lineIsAssignment = node->getNodeType() == Assign_C;
			bool isResultsModified = false;
			bool isAlreadyAddedToResults = find(addedToAnswer.begin(), addedToAnswer.end(), node->getProcLineNumber()) != addedToAnswer.end();
			if (!((variablesToMatch & variablesModified).none()) && lineIsAssignment) {
				if (!isAlreadyAddedToResults) {
					result.push_back(node->getProcLineNumber());
					addedToAnswer.insert(node->getProcLineNumber());
				}
				isResultsModified = true;

				// early return. this is used if the presense of one result is sufficient for the query
				if (terminateOnOneResult) {
					return result;
				}
			}
			// reset any re-defined variables
			variablesToMatch &= ~variablesModified;

			// set variables for indrectly affected stmts
			if (transitiveClosure && isResultsModified) {
				variablesToMatch |= variablesUsed;
			}

			// if there are no more variables to match, don't explore this path further
			if (variablesToMatch.none()) {
				continue;
			}
		} 

		isTestingFirstNode = false;

		// update frontier with new nodes
		assert(node->getAfter()->size() > 0);
		

		populateFrontierWithNextBipBefore(frontier, node, visited, variableTested, curState, variablesToMatch);
		
		
	}
	delete terminateNode;

	return result;
}



pair<PROGLINE_LIST, PROGLINE_LIST> AffectsBipTable::getAllAffectsBipPairs(TRANS_CLOSURE transitiveClosure) {
	
	pair<vector<int>, vector<int>> results;

	return results;
}



PROGLINE_LIST AffectsBipTable::getLhs() {
	PKB pkb = PKB::getInstance();
	vector<int> assignments = pkb.getStmtNumForType(ASSIGN);
	vector<int> results;

	for (auto stmt = assignments.begin(); stmt != assignments.end(); ++ stmt) {
		PROGLINE_LIST rhs = getProgLinesAffectsBipAfter(*stmt, false);
		if (rhs.size() > 0) {
			results.push_back(*stmt);
		}
	}

	return results;
}

PROGLINE_LIST AffectsBipTable::getRhs() {
	vector<int> assignments = PKB::getInstance().getStmtNumForType(ASSIGN);
	vector<int> results;

	try {
	for (auto stmt = assignments.begin(); stmt != assignments.end(); ++ stmt) {
		vector<int> lhs = getProgLinesAffectsBipBefore(*stmt, false);
		
		if (lhs.size() > 0) {
			results.push_back(*stmt);
		}
	}
	} catch (exception e) {
		cout << "AffectsBip getRhs: " << e.what() << endl;
	}

	return results;
}



BOOLEAN_ AffectsBipTable::isAffectsBip(PROG_LINE_ progLine1, PROG_LINE_ progLine2, TRANS_CLOSURE transitiveClosure) {
	PROGLINE_LIST rhs = getProgLinesAffectsBipAfter(progLine1, transitiveClosure);

	return find(rhs.begin(), rhs.end(), progLine2) != rhs.end();
}