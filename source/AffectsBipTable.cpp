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

#include "AffectsBipTable.h"
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
	PKB pkb = PKB::getInstance();
	PROGLINE_LIST nextNodeNums = pkb.getNextBipAfter(node->getProcLineNumber());
		
	for (auto nodeIter = nextNodeNums.begin(); nodeIter != nextNodeNums.end(); ++nodeIter) {
		CNode* nextNode = pkb.cfgNodeTable.at(*nodeIter);
		pair<NEXTBIP_STATE, boost::dynamic_bitset<>> nodePair = make_pair<NEXTBIP_STATE, boost::dynamic_bitset<> >(NEXTBIP_STATE(nextNode, curState.second), variablesToMatch);
			
		if (nextNode && visited.count(nodePair) == 0 ) {
			frontier.push(nodePair);
		}
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
			
		if (nextNode && visited.count(nodePair) == 0 ) {
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
	PROGLINE_LIST prevNodeNums = pkb.getNextBipBefore(node->getProcLineNumber());
		
	for (auto nodeIter = prevNodeNums.begin(); nodeIter != prevNodeNums.end(); ++nodeIter) {

		CNode* nextNode = pkb.cfgNodeTable.at(*nodeIter);
		stack<CNode*> nextStateStack = curState.second;

		// if call_c, add call_c node into the stack
		if (nextNode->getNodeType() == Call_C) {
			nextStateStack.push(nextNode);
		}

		pair<NEXTBIP_STATE, boost::dynamic_bitset<>> nodePair = make_pair<NEXTBIP_STATE, boost::dynamic_bitset<> >(NEXTBIP_STATE(nextNode, nextStateStack), variablesToMatch);
		
		bool isNodeVisitedBefore = isVisitedBefore(variableTested, NEXTBIP_STATE(nextNode, nextStateStack), 
			                                       nodePair, visited, variablesToMatch);
		if (nextNode && visited.count(nodePair) == 0 && !isNodeVisitedBefore) {
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
	                set<pair<NEXTBIP_STATE, boost::dynamic_bitset<> >> visited, NEXTBIP_STATE curState, boost::dynamic_bitset<> variablesToMatch, map<NEXTBIP_STATE, boost::dynamic_bitset<> , VariableTestedCompare> variableTested) {

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
				candidateNextNode = curState.second.top();
				curState.second.pop();

				nodesToTraverseBack.push_back(candidateNextNode);
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
	
	vector<int> currentLine = pkb.getNextBipAfter(progLine1);
	if (currentLine.size() == 0) {
		// return early if there is nothing next
		return result;
	}

	PROG_LINE_ nextLine = currentLine.at(0);
	node = pkb.cfgNodeTable.at(nextLine);

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
		if (visited.count(frontier.top()) != 0) {
			frontier.pop();
			continue;
		}
		visited.insert(frontier.top());

		NEXTBIP_STATE curState = frontier.top().first; variablesToMatch = frontier.top().second;
		CNode* node = curState.first;

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
		
		assert(node->getNodeType() != EndIf_C && node->getNodeType() != EndProc_C ); // don't store dummy nodes in frontier
																					 // otherwise cannot make use of NextBip API
		frontier.pop();

		// maintain the stack of nodes after call, partially duplicated from NextBip
		if (curState.second.size() > 0 && 
		  curState.second.top()->getProcLineNumber() == curState.first->getProcLineNumber()) {
			//@todo think about this more carefully
			// pop the top off the stack when the line indicated is reached
			curState.second.pop();
		}

		// special handling of different node types 
		if (node->getNodeType() == Call_C) {
			// add the next node to the stack
			CNode* callNode = pkb.cfgNodeTable.at(node->getProcLineNumber());

			assert(callNode->getAfter()->size() == 1);
			CNode* afterCallNode = callNode->getAfter()->at(0); 

			// skip dummy nodes,
			stack<CNode*> tempStack = curState.second; // use this to not make changes to the state
			while (afterCallNode != NULL && (afterCallNode->getNodeType() == EndIf_C || afterCallNode->getNodeType() == EndProc_C)) {
				if (afterCallNode->getNodeType() == EndIf_C) {
					// get next node
					afterCallNode = afterCallNode->getAfter()->at(0);
				} else {
					// if EndProc C
					// take node from the stack again
					afterCallNode = NULL;
					if (tempStack.size() > 0) {
						afterCallNode = tempStack.top();
						tempStack.pop();
					} 
				}
			}
			
			if (afterCallNode) {
				curState.second.push(afterCallNode);
			} else {
				curState.second.push(terminateNode);
			}

		} else if (node->getNodeType() == Assign_C) {
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


		// update frontier with new nodes
		assert(node->getAfter()->size() > 0);
		bool isLastLine = false;
		vector<CNode*>* afterNodes =  node->getAfter();
		for (auto afterNodeIter = afterNodes->begin(); afterNodeIter != afterNodes->end(); ++afterNodeIter) {
			isLastLine = (*afterNodeIter)->getNodeType() == EndProc_C;
		}
		if (!isLastLine) {
			populateFrontierWithNextBipAfter(frontier, node, visited, curState, variablesToMatch);
		} else {
			// special handling for last node.. 
			populateFrontierWithNextAfter(frontier, node, visited, curState, variablesToMatch);
			handleLastLine(node, frontier, visited, curState, variablesToMatch);
		}
		
	}

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
	vector<int> currentLine = pkb.getNextBipBefore(progLine2);
	if (currentLine.size() == 0) {
		// return early if there is nothing next
		return result;
	}

	PROG_LINE_ nextLine = currentLine.at(0);
	node = pkb.cfgNodeTable.at(nextLine);

	boost::dynamic_bitset<> variablesToMatch = pkb.getUseVarInBitvectorForStmt(progLine2);
	//@todo, refactor
	priority_queue<pair<NEXTBIP_STATE, boost::dynamic_bitset<> >, vector<pair<NEXTBIP_STATE, boost::dynamic_bitset<> > >, CompareAffectsBip> frontier;

	// for AffectsBip, do DFS over NEXTBIP_STATE rather than over CNodes
	frontier.push(pair<NEXTBIP_STATE, boost::dynamic_bitset<> >(NEXTBIP_STATE(node, stack<CNode*>()), variablesToMatch));

	set<int> addedToAnswer;
	set<pair<NEXTBIP_STATE, boost::dynamic_bitset<> >> visited;	

	map<NEXTBIP_STATE, boost::dynamic_bitset<>, VariableTestedCompare > variableTested;

	while (!frontier.empty()) {
		if (visited.count(frontier.top()) != 0) {
			frontier.pop();
			continue;
		}
		visited.insert(frontier.top());

		NEXTBIP_STATE curState = frontier.top().first; variablesToMatch = frontier.top().second;
		CNode* node = curState.first;

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

		
		assert(node->getNodeType() != EndIf_C && node->getNodeType() != EndProc_C ); // don't store dummy nodes in frontier
																					 // otherwise cannot make use of NextBip API

		frontier.pop();

		// maintain the stack of nodes after call, partially duplicated from NextBip
		if (curState.second.size() > 0 && 
		  curState.second.top()->getProcLineNumber() == curState.first->getProcLineNumber()) {
			//@todo think about this more carefully
			// pop the top off the stack when the line indicated is reached
			curState.second.pop();
		}

		// special handling of different node types 
		if (node->getNodeType() == Assign_C) {
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

		// update frontier with new nodes
		PROGLINE_LIST nextNodeNums;
		assert(node->getAfter()->size() > 0);
		bool isFirstLine = node->getAfter()->at(0)->getNodeType() == Proc_C;

		if (!isFirstLine) {
			populateFrontierWithNextBipBefore(frontier, node, visited, variableTested, curState, variablesToMatch);
		} else {
			// special handling for first node.. 
			handleFirstLine(node, frontier, visited, curState, variablesToMatch, variableTested);
		}
		
	}

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

	for (auto stmt = assignments.begin(); stmt != assignments.end(); ++ stmt) {
		vector<int> lhs = getProgLinesAffectsBipBefore(*stmt, false);
		if (lhs.size() > 0) {
			results.push_back(*stmt);
		}
	}

	return results;
}



BOOLEAN_ AffectsBipTable::isAffectsBip(PROG_LINE_ progLine1, PROG_LINE_ progLine2, TRANS_CLOSURE transitiveClosure) {
	if (PKB::getInstance().stmtToProcMap[progLine1] != PKB::getInstance().stmtToProcMap[progLine2]) {
		return false;
	}

	PROGLINE_LIST rhs = getProgLinesAffectsBipAfter(progLine1, false);

	return find(rhs.begin(), rhs.end(), progLine2) != rhs.end();
}