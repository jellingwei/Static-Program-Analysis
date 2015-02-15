#pragma once

#include<stdio.h>
#include <iostream>
#include <string>
#include <vector>
#include <stack>
#include <unordered_set>
#include <numeric>
#include <algorithm>
#include <cassert>

#include "DesignExtractor.h"
#include "TNode.h"
#include "PKB.h"


using namespace std;


DesignExtractor::DesignExtractor() {
	PKB pkb = PKB::getInstance();
	// assert that the parser did not do a bad job
	assert(pkb.getRoot()->getChildren()->size() == pkb.getProcTableSize());

}

vector<int> dfsForProcedures(int startProc, vector<int>* allProcs, unordered_set<int>* visited) {
	PKB pkb = PKB::getInstance();

	vector<int> result;
	
	deque<int> frontier;
	frontier.push_back(startProc);

	while (!frontier.empty()) {
		int curProc = frontier.back(); frontier.pop_back();
		visited->insert(curProc);
		result.push_back(curProc);

		// update list of procedures to only contain unvisited procs
		auto position = find(allProcs->begin(), allProcs->end(), curProc);
		if (position != allProcs->end()) {
			allProcs->erase(position);
		}

		// add new procs to frontier
		vector<int> intermediateProcs = pkb.getProcsCalledBy(curProc, false);
		for (auto iter = intermediateProcs.begin(); iter != intermediateProcs.end(); ++iter) {
			if (visited->count(*iter) != 0 || find(frontier.begin(), frontier.end(), *iter) != frontier.end()) {
				// skip if already visited, or if already in the frontier
				continue;
			}
			frontier.push_back(*iter);
		}
	}

	reverse(result.begin(), result.end());
	return result;
}

/**
 * Return the called procedures in topological order
 */
vector<int> getCallsInTopologicalOrder() {
	PKB pkb = PKB::getInstance();
	vector<int> result;
	
	int startProc = 0;

	vector<int> allProcs(pkb.getProcTableSize()); // used to check which procs hasn't been included
	std::iota (begin(allProcs), end(allProcs), 0);  // assumes that procedures are numbered from 0 to size-1
	unordered_set<int> visited;

	result = dfsForProcedures(startProc, &allProcs, &visited);
	
	if (!allProcs.empty()) {
		startProc = allProcs.front();
		vector<int> newResult = dfsForProcedures(startProc, &allProcs, &visited);
		result.insert(result.end(), newResult.begin(), newResult.end()); // append newResult behind result
	}

	return result;
}

class CallComparator {
public:
	CallComparator(vector<int> topoOrder) {
		this->topoOrder = topoOrder; 
	}

	bool operator() (TNode* i, TNode* j) { 
		assert(i->getNodeType() == Call);
		assert(j->getNodeType() == Call);

		PKB pkb = PKB::getInstance();
		int iProc = pkb.stmtToProcMap.at(i->getStmtNumber());
		int jProc = pkb.stmtToProcMap.at(j->getStmtNumber());

		int iPosition = find(topoOrder.begin(), topoOrder.end(), iProc) - topoOrder.begin();
		int jPosition = find(topoOrder.begin(), topoOrder.end(), jProc) - topoOrder.begin();
		return iPosition < jPosition;
	}
private:
	vector<int> topoOrder;
};


vector<TNode*> DesignExtractor::obtainCallStatementsInTopologicalOrder() {
	vector<int> topologicalOrder = getCallsInTopologicalOrder();

	PKB pkb = PKB::getInstance();

	vector<int> allCallStatementsNum = pkb.getStmtNumForType(CALL);
	
	// obtain every call statement node
	vector<TNode*> callStatementNodes;
	for (auto iter = allCallStatementsNum.begin(); iter != allCallStatementsNum.end(); ++iter) {
		TNode* node = pkb.nodeTable.at(*iter);
		callStatementNodes.push_back(node);
	}

	// sort all call statements 
	CallComparator compare(topologicalOrder);
	sort(callStatementNodes.begin(), callStatementNodes.end(), compare);
	
	return callStatementNodes;
}

/**
 * For the input call statements nodes, set Modifies for them as well as all their ancestors
 * This function writes into the PKB
 * @param callStmt a vector of sorted TNodes in order of setting Modifies on them
 */
void DesignExtractor::setModifiesForCallStatements(vector<TNode*> callStmt) {
	PKB pkb = PKB::getInstance();

	for (auto stmt = callStmt.begin(); stmt != callStmt.end(); ++stmt) {
		int procCalled = (*stmt)->getNodeValueIdx();
		int stmtNumber = (*stmt)->getStmtNumber();
		int currentProc = pkb.stmtToProcMap.at(stmtNumber);

		vector<int> varModifiedByProcCalled = pkb.getModVarForProc(procCalled);

		// set modifies for the (current statement, all variables modified by the function called)
		for (auto iter = varModifiedByProcCalled.begin(); iter != varModifiedByProcCalled.end(); ++iter) {
			pkb.setModifies(stmtNumber, *iter);
		}
		
		// for ancestors, set (parent, all variabled modified by the function called)
		while (pkb.getParent(stmtNumber).size()) 
		{
			stmtNumber = pkb.getParent(stmtNumber).at(0);
			if (stmtNumber <= 0) {
				continue;
			}
			for (auto iter = varModifiedByProcCalled.begin(); iter != varModifiedByProcCalled.end(); ++iter) {
				pkb.setModifies(stmtNumber, *iter);
			}			
		}

		// for the current proc, set modifies
		//for (auto iter = varModifiedByProcCalled.begin(); iter != varModifiedByProcCalled.end(); ++iter) {
		//	pkb.setModifiesProc(currentProc, *iter);
		//}
		
	}

}

/**
 * For the input call statements nodes, set Uses for them as well as all their ancestors.
 * This function writes into the PKB
 * @param callStmt a vector of sorted TNodes in order of setting Uses on them
 */
void DesignExtractor::setUsesForCallStatements(vector<TNode*> callStmt) {

	PKB pkb = PKB::getInstance();

	for (auto stmt = callStmt.begin(); stmt != callStmt.end(); ++stmt) {
		int procCalled = (*stmt)->getNodeValueIdx();
		int stmtNumber = (*stmt)->getStmtNumber();
		int currentProc = pkb.stmtToProcMap.at(stmtNumber);

		vector<int> varUsedByProcCalled = pkb.getUsesVarForProc(procCalled);

		// set uses for the (current statement, all variables modified by the function called)
		for (auto iter = varUsedByProcCalled.begin(); iter != varUsedByProcCalled.end(); ++iter) {
			pkb.setUses(stmtNumber, *iter);
		}
		
		// for ancestors, set (parent, all variabled modified by the function called)
		while (pkb.getParent(stmtNumber).size()) 
		{
			stmtNumber = pkb.getParent(stmtNumber).at(0);
			if (stmtNumber <= 0) {
				continue;
			}
			for (auto iter = varUsedByProcCalled.begin(); iter != varUsedByProcCalled.end(); ++iter) {
				pkb.setUses(stmtNumber, *iter);
			}			
		}

		//for (auto iter = varUsedByProcCalled.begin(); iter != varUsedByProcCalled.end(); ++iter) {
		//	pkb.setUsesProc(currentProc, *iter);
		//}
		
	}

}

/**
 * Build Control Flow Graph
 */
bool DesignExtractor::constructCfg() {
	throw exception("Not implemented yet");
}

void DesignExtractor::constructStatisticsTable() {
	throw exception("Not implemented yet");
}

void DesignExtractor::setModifiesForAssignmentStatements() {

	PKB pkb = PKB::getInstance();

	vector<int> assignStmts = pkb.getStmtNumForType(ASSIGN);
	for (auto iter = assignStmts.begin(); iter != assignStmts.end(); ++iter) {
		int stmtNumber = *iter;

		// find variable modified
		TNode* node = pkb.nodeTable.at(stmtNumber);
		assert(node->getChildren()->size() == 2);

		int varIndex = node->getChildren()->at(0)->getNodeValueIdx();
		pkb.setModifies(stmtNumber, varIndex);

		// for ancestors, set (parent, all variable)
		while (pkb.getParent(stmtNumber).size()) 
		{
			stmtNumber = pkb.getParent(stmtNumber).at(0);
			if (stmtNumber <= 0) {
				continue;
			}
			pkb.setModifies(stmtNumber, varIndex);	
		}
	}
}

vector<int> obtainVarUsedInExpression(TNode* node) {
	vector<int> varUsed;

	deque<TNode*> frontier;
	frontier.push_back(node->getChildren()->at(1));
	TNode* curNode;
	while (!frontier.empty()) {
		curNode = frontier.back(); frontier.pop_back();

		if (curNode->hasChild()) {
			vector<TNode*>* children = curNode->getChildren();

			for (auto iter = children->begin(); iter != children->end(); ++iter) {
				frontier.push_back(*iter);
			}
		}

		if (curNode->getNodeType() == Variable) {
			varUsed.push_back(curNode->getNodeValueIdx());
		}
	}
	return varUsed;
}

void DesignExtractor::setUsesForAssignmentStatements() {

	PKB pkb = PKB::getInstance();

	vector<int> assignStmts = pkb.getStmtNumForType(ASSIGN);
	for (auto iter = assignStmts.begin(); iter != assignStmts.end(); ++iter) {
		int stmtNumber = *iter;

		// find variable used
		TNode* node = pkb.nodeTable.at(stmtNumber);
		vector<int> varIndexesUsed = obtainVarUsedInExpression(node);
		for (auto varIter = varIndexesUsed.begin(); varIter != varIndexesUsed.end(); ++varIter) {
			pkb.setUses(stmtNumber, *varIter);
		}

		// for ancestors, set (parent, all variable)
		while (pkb.getParent(stmtNumber).size()) 
		{
			stmtNumber = pkb.getParent(stmtNumber).at(0);
			if (stmtNumber <= 0) {
				continue;
			}
			for (auto varIter = varIndexesUsed.begin(); varIter != varIndexesUsed.end(); ++varIter) {
				pkb.setUses(stmtNumber, *varIter);
			}		
		}
	}
}

void DesignExtractor::setUsesForContainerStatements() {
	PKB pkb = PKB::getInstance();

	vector<int> containerStmts = pkb.getStmtNumForType(IF);
	vector<int> whileStmts = pkb.getStmtNumForType(WHILE);

	containerStmts.insert(containerStmts.end(), whileStmts.begin(), whileStmts.end());

	for (auto iter = containerStmts.begin(); iter != containerStmts.end(); ++iter) {
		int stmtNumber = *iter;

		// find variable used
		TNode* node = pkb.nodeTable.at(stmtNumber);
		assert(node->getChildren()->size() == 2 || node->getChildren()->size() == 3);

		int varIndex = node->getChildren()->at(0)->getNodeValueIdx();
		pkb.setUses(stmtNumber, varIndex);

		// for ancestors, set (parent, all variable)
		while (pkb.getParent(stmtNumber).size()) 
		{
			stmtNumber = pkb.getParent(stmtNumber).at(0);
			if (stmtNumber <= 0) {
				continue;
			}
			
			pkb.setUses(stmtNumber, varIndex);
		}
	}
}