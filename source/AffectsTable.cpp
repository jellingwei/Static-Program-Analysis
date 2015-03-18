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

#include "AffectsTable.h"
#include "PKB.h"


bool AffectsTable::isAffects(int progLine1, int progLine2, bool transitiveClosure) {
	return false;
}


bool isNodeCandidateForSkippingAhead() {
	// @todo
	return false;
}

vector<CNode*> getNodesToSkipTo() {
	//@todo
	return vector<CNode*>();
}

// assumes that each node has only 1 non-inside node directly connected After it
CNode* getMandatoryNextNode(CNode* node, CFG* cfg, boost::dynamic_bitset<> variablesToMatch) {
	PKB pkb = PKB::getInstance();

	CNODE_TYPE type = node->getNodeType();
	vector<CNode*>* possibleNextNodes = node->getAfter();

	// handle special case for if statement
	if (node->getNodeType() == If_C && node->getVariablesInside2().size() > 0 && (node->getVariablesInside2() & variablesToMatch).none()) {
		// the else stmtList does not contain the variable,
		// skip to the statement Following the node in the AST

		vector<int> follows = pkb.getStmtFollowedFrom(node->getProcLineNumber());
		if (follows.size() > 0 ) {
			int stmtNumFollowingNode = follows.at(0);
			return pkb.cfgNodeTable.at(stmtNumFollowingNode);
		} else {
			return NULL;
		}
	}
	
	for (auto iter = possibleNextNodes->begin(); iter != possibleNextNodes->end(); ++iter) {
		if (cfg->isInsideNode(node, *iter) || (*iter)->getNodeType() == Proc_C || (*iter)->getNodeType() == EndProc_C) {
			continue;
		}
		
		return *iter;	
	}

	return NULL;
}

// assumes that each node has only 1 non-inside node directly connected  Before it
CNode* getMandatoryPrevNode(CNode* node, CFG* cfg, boost::dynamic_bitset<> variablesToMatch) {
	PKB pkb = PKB::getInstance();

	CNODE_TYPE type = node->getNodeType();
	vector<CNode*>* possiblePrevNodes = node->getBefore();

	// handle special case for end if statement
	if (node->getNodeType() == EndIf_C && node->getVariablesInside2().size() > 0 && (node->getVariablesInside2() & variablesToMatch).none()) {
		// the else stmtList does not contain the variable,
		// skip to the statement Following the node in the AST
		vector<int> follows = pkb.getStmtFollowedTo(node->getProcLineNumber());
		if (follows.size() > 0) {
			int stmtNumFollowedByNode = follows.at(0);
			return pkb.cfgNodeTable.at(stmtNumFollowedByNode);
		} else {
			return NULL;
		}
	}
	
	for (auto iter = possiblePrevNodes->begin(); iter != possiblePrevNodes->end(); ++iter) {
		if (cfg->isInsideNode(node, *iter) || (*iter)->getNodeType() == Proc_C || (*iter)->getNodeType() == EndProc_C) {
			continue;
		}

		return *iter;	
	}

	return NULL;
}

// assumes that each node has only 1 inside node directly connected to it
CNode* getInsideNextNode(CNode* node, CFG* cfg, boost::dynamic_bitset<> variablesToMatch) {
	PKB pkb = PKB::getInstance();

	CNODE_TYPE type = node->getNodeType();
	vector<CNode*>* possibleNextNodes = node->getAfter();

	boost::dynamic_bitset<> variablesToCompareForNode;
	
	variablesToCompareForNode = node->getVariablesInside();
	
	
	
	if (node->getVariablesInside().size() > 0 && (node->getVariablesInside() & variablesToMatch).none()) {
		// this branch will not match variablesToMatch, therefore do an early return
		return NULL;
	}
	
	for (auto iter = possibleNextNodes->begin(); iter != possibleNextNodes->end(); ++iter) {
		if (cfg->isInsideNode(node, *iter) && (*iter)->getNodeType() != Proc_C && (*iter)->getNodeType() != EndProc_C) {
			return *iter;
		}	
	}

	return NULL;
}

// assumes that each node has only 1 inside node directly connected to it
CNode* getInsidePrevNode(CNode* node, CFG* cfg, boost::dynamic_bitset<> variablesToMatch) {
	PKB pkb = PKB::getInstance();

	CNODE_TYPE type = node->getNodeType();
	vector<CNode*>* possiblePrevNodes = node->getBefore();

	boost::dynamic_bitset<> variablesToCompareForNode;
	if (type == EndIf_C) {
		variablesToCompareForNode = node->getVariablesInside();
	} else if (type == While_C) {
		variablesToCompareForNode = pkb.getModVarInBitvectorForStmt(node->getProcLineNumber());
	}
	
	if (variablesToCompareForNode.size() > 0 && (variablesToCompareForNode & variablesToMatch).none()) {
		// this branch will not match variablesToMatch, therefore do an early return
		return NULL;
	}
	
	for (auto iter = possiblePrevNodes->begin(); iter != possiblePrevNodes->end(); ++iter) {
		if (cfg->isInsideNode(node, *iter) && (*iter)->getNodeType() != Proc_C && (*iter)->getNodeType() != EndProc_C) {
			return *iter;
		}	
	}

	return NULL;
}



/**
 * For use in the priority queue when traversing the CFG for Affects
 */
class CompareAffects {
    public:
		bool operator() (pair<CNode*, boost::dynamic_bitset<> >& pair1, pair<CNode*, boost::dynamic_bitset<> >& pair2) { 
			return pair1.first->getProcLineNumber() < pair2.first->getProcLineNumber(); 
		}
};

class CompareAffectsReverse {
    public:
		bool operator() (pair<CNode*, boost::dynamic_bitset<> >& pair1, pair<CNode*, boost::dynamic_bitset<> >& pair2) { 
			return pair1.first->getProcLineNumber() > pair2.first->getProcLineNumber(); 
		}
};

vector<int> AffectsTable::getProgLinesAffectedBy(int progLine1, bool transitiveClosure) {
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

	// initialise variablesToMatch and the priority queue
	boost::dynamic_bitset<> variablesToMatch = pkb.getModVarInBitvectorForStmt(progLine1);
	priority_queue<pair<CNode*, boost::dynamic_bitset<> >, vector<pair<CNode*, boost::dynamic_bitset<> > >, CompareAffects> frontier;

	CNode* startNode = getMandatoryNextNode(node, pkb.cfgTable.at(0), variablesToMatch);
	if (startNode) {
		frontier.push(make_pair<CNode*, boost::dynamic_bitset<> >(startNode,variablesToMatch));
	}
	set<pair<CNode*, boost::dynamic_bitset<> >> visited;
	set<int> addedToAnswer;

	while (!frontier.empty()) {
		node = frontier.top().first;
		variablesToMatch = frontier.top().second;
		visited.insert(frontier.top());
		frontier.pop();

		if (node->getNodeType() == Assign_C || node->getNodeType() == Call_C) {
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
			}
			// reset any re-defined variables
			variablesToMatch &= ~variablesModified;

			// set variables for indrectly affected stmts
			if (transitiveClosure && isResultsModified) {
				variablesToMatch |= variablesModified;
			}

			// if there are no more variables to match, don't explore this path further
			if (variablesToMatch.none()) {
				continue;
			}
		} 

		CNode* nextNode = getMandatoryNextNode(node, pkb.cfgTable.at(0), variablesToMatch);
		pair<CNode*, boost::dynamic_bitset<>> nodePair = make_pair<CNode*, boost::dynamic_bitset<> >(nextNode, variablesToMatch);
		
		if (nextNode && visited.count(nodePair) == 0 ) {
			frontier.push(make_pair<CNode*, boost::dynamic_bitset<> >(nextNode, variablesToMatch));
		}

		CNode* possibleNode = getInsideNextNode(node, pkb.cfgTable.at(0), variablesToMatch);
		nodePair = make_pair<CNode*, boost::dynamic_bitset<> >(possibleNode, variablesToMatch);
		
		if (possibleNode && visited.count(nodePair) == 0 ) {
			frontier.push(make_pair<CNode*, boost::dynamic_bitset<> >(possibleNode, variablesToMatch));
		}
	}


	return result;
}


vector<int> AffectsTable::getProgLinesAffecting(int progLine2, bool transitiveClosure) {
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

	// initialise variablesToMatch and the priority queue
	boost::dynamic_bitset<> variablesToMatch = pkb.getUseVarInBitvectorForStmt(progLine2);
	priority_queue<pair<CNode*, boost::dynamic_bitset<> >, vector<pair<CNode*, boost::dynamic_bitset<> > >, CompareAffectsReverse> frontier;

	CNode* startNode = getMandatoryPrevNode(node, pkb.cfgTable.at(0), variablesToMatch);
	if (startNode) {
		frontier.push(make_pair<CNode*, boost::dynamic_bitset<> >(startNode, variablesToMatch));
	}
	set<pair<CNode*, boost::dynamic_bitset<> >> visited;
	set<int> addedToAnswer;

	while (!frontier.empty()) {
		node = frontier.top().first;
		variablesToMatch = frontier.top().second;
		
		visited.insert(frontier.top());
		frontier.pop();

		if (node->getNodeType() == Assign_C || node->getNodeType() == Call_C) {
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

		CNode* prevNode = getMandatoryPrevNode(node, pkb.cfgTable.at(0), variablesToMatch);
		pair<CNode*, boost::dynamic_bitset<>> nodePair = make_pair<CNode*, boost::dynamic_bitset<> >(prevNode, variablesToMatch);
		if (prevNode && visited.count(nodePair) == 0 ) {
			frontier.push(make_pair<CNode*, boost::dynamic_bitset<> >(prevNode, variablesToMatch));
		}

		CNode* possibleNode = getInsidePrevNode(node, pkb.cfgTable.at(0), variablesToMatch);
		nodePair = make_pair<CNode*, boost::dynamic_bitset<> >(possibleNode, variablesToMatch);
		if (possibleNode && visited.count(nodePair) == 0 ) {
			frontier.push(make_pair<CNode*, boost::dynamic_bitset<> >(possibleNode, variablesToMatch));
		}
		
	}

	return result;
}



pair<vector<int>, vector<int>> AffectsTable::getAllAffectsPairs(bool transitiveClosure) {
	
	pair<vector<int>, vector<int>> results;

	return results;
}




vector<int> AffectsTable::getLhs() {
	return lhs;
}
vector<int> AffectsTable::getRhs(){
	return rhs;
}

void AffectsTable::setLhs(vector<int>) {

}
void AffectsTable::setRhs(vector<int>) {

}