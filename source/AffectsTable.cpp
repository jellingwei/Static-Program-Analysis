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
CNode* getMandatoryNextNode(CNode* node, CFG* cfg) {
	PKB pkb = PKB::getInstance();

	CNODE_TYPE type = node->getNodeType();
	vector<CNode*>* possibleNextNodes = node->getAfter();
	
	for (auto iter = possibleNextNodes->begin(); iter != possibleNextNodes->end(); ++iter) {
		if (cfg->isInsideNode(node, *iter)) {
			continue;
		}
		return *iter;	
	}

	return NULL;
}

// assumes that each node has only 1 non-inside node directly connected  Before it
CNode* getMandatoryPrevNode(CNode* node, CFG* cfg) {
	PKB pkb = PKB::getInstance();

	CNODE_TYPE type = node->getNodeType();
	vector<CNode*>* possibleNextNodes = node->getBefore();
	
	for (auto iter = possibleNextNodes->begin(); iter != possibleNextNodes->end(); ++iter) {
		if (cfg->isInsideNode(node, *iter)) {
			continue;
		}
		return *iter;	
	}

	return NULL;
}

// assumes that each node has only 1 inside node directly connected to it
CNode* getInsideNextNode(CNode* node, CFG* cfg) {
	PKB pkb = PKB::getInstance();

	CNODE_TYPE type = node->getNodeType();
	vector<CNode*>* possibleNextNodes = node->getAfter();
	
	for (auto iter = possibleNextNodes->begin(); iter != possibleNextNodes->end(); ++iter) {
		if (cfg->isInsideNode(node, *iter)) {
			return *iter;
		}	
	}

	return NULL;
}


//	variablesToMatch &= ~nodeModVariables;

	//if (isResultsChanged && isTransitiveClosure) {
	//	variablesToMatch |= nodeModVariables;  or variablesToMatch |= nodeUsedVariables;
	//}




/**
 * For use in the priority queue when traversing the CFG for Affects
 */
class CompareAffects {
    public:
		bool operator() (pair<CNode*, boost::dynamic_bitset<> >& pair1, pair<CNode*, boost::dynamic_bitset<> >& pair2) { 
			return pair1.first->getProcLineNumber() < pair2.first->getProcLineNumber(); 
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

	frontier.push(make_pair<CNode*, boost::dynamic_bitset<> >(getMandatoryNextNode(node, pkb.cfgTable.at(0)), variablesToMatch));
	set<pair<CNode*, boost::dynamic_bitset<> >> visited;

	while (!frontier.empty()) {
		node = frontier.top().first;
		
		variablesToMatch = frontier.top().second;
		
		visited.insert(frontier.top());
		frontier.pop();

		if (node->getNodeType() == Assign_C || node->getNodeType() == Call_C) {
			boost::dynamic_bitset<> variablesModified = pkb.getUseVarInBitvectorForStmt(node->getProcLineNumber());
			boost::dynamic_bitset<> variablesUsed = pkb.getUseVarInBitvectorForStmt(node->getProcLineNumber());
		
			// test for results
			bool isResultsModified = false;
			if (!(variablesToMatch & variablesUsed).none()) {
				result.push_back(node->getProcLineNumber());
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

		CNode* nextNode = getMandatoryNextNode(node, pkb.cfgTable.at(0));
		pair<CNode*, boost::dynamic_bitset<>> nodePair = make_pair<CNode*, boost::dynamic_bitset<> >(nextNode, variablesToMatch);
		if (nextNode && visited.count(nodePair) == 0 ) {
			frontier.push(make_pair<CNode*, boost::dynamic_bitset<> >(nextNode, variablesToMatch));
		}

		CNode* possibleNode = getInsideNextNode(node, pkb.cfgTable.at(0));
		nodePair = make_pair<CNode*, boost::dynamic_bitset<> >(possibleNode, variablesToMatch);
		if (possibleNode && visited.count(nodePair) == 0 ) {
			frontier.push(make_pair<CNode*, boost::dynamic_bitset<> >(possibleNode, variablesToMatch));
		}
	}


	return result;
}


vector<int> AffectsTable::getProgLinesAffecting(int progLine2, bool transitiveClosure) {
	
	return vector<int>();
}



pair<vector<int>, vector<int>> AffectsTable::getAllAffectsPairs(bool transitiveClosure) {
	
	pair<vector<int>, vector<int>> results;

	return results;
}
