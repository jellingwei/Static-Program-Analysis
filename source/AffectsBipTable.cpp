#pragma once

#include<stdio.h>
#include <iostream>
#include <string>
#include <vector>
#include <unordered_map>
#include <algorithm>
#include <queue>
#include <unordered_set>

#include "boost/dynamic_bitset.hpp"

#include "AffectsBipTable.h"
#include "PKB.h"


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

void populateFrontierWithNextBipAfter(priority_queue<pair<NEXTBIP_STATE, boost::dynamic_bitset<> >, vector<pair<NEXTBIP_STATE, boost::dynamic_bitset<> > >, CompareAffectsBip>& frontier, CNode* node, set<pair<NEXTBIP_STATE, boost::dynamic_bitset<> >> visited, NEXTBIP_STATE curState, boost::dynamic_bitset<> variablesToMatch) {
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

void handleLastLine(CNode* node, priority_queue<pair<NEXTBIP_STATE, boost::dynamic_bitset<> >, vector<pair<NEXTBIP_STATE, boost::dynamic_bitset<> > >, CompareAffectsBip>& frontier, 
	                set<pair<NEXTBIP_STATE, boost::dynamic_bitset<> >> visited, NEXTBIP_STATE curState, boost::dynamic_bitset<> variablesToMatch) {

	if (node->getNodeType() == Call_C) {
		// still need to expand if the last line is a Call stmt
		populateFrontierWithNextBipAfter(frontier, node, visited, curState, variablesToMatch);
	}
			
	// since this is last line, pop from the stack and add the progline into the frontier
	if (curState.second.size() > 0) {
		CNode* candidateNextNode = curState.second.top();
		curState.second.pop();

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

	while (!frontier.empty()) {
		visited.insert(frontier.top());

		NEXTBIP_STATE curState = frontier.top().first; variablesToMatch = frontier.top().second;
		CNode* node = curState.first;

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
			curState.second.push(afterCallNode);

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
		PROGLINE_LIST nextNodeNums;
		assert("node has incorrect number of after nodes in AffectsBip", node->getAfter()->size() > 0);
		bool isLastLine = node->getAfter()->at(0)->getNodeType() == EndProc_C;

		if (!isLastLine) {
			populateFrontierWithNextBipAfter(frontier, node, visited, curState, variablesToMatch);
			continue;
		} 

		handleLastLine(node, frontier, visited, curState, variablesToMatch);
		// special handling for last node.. 
		

		
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


	return results;
}

PROGLINE_LIST AffectsBipTable::getRhs() {
	vector<int> results;

	return results;
}



BOOLEAN_ AffectsBipTable::isAffectsBip(PROG_LINE_ progLine1, PROG_LINE_ progLine2, TRANS_CLOSURE transitiveClosure) {
	if (PKB::getInstance().stmtToProcMap[progLine1] != PKB::getInstance().stmtToProcMap[progLine2]) {
		return false;
	}


	return false;
}