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

using namespace std;


#include "PKB.h"

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


vector<int> DesignExtractor::getCallsInTopologicalOrder() {
	PKB pkb = PKB::getInstance();
	vector<int> result;
	
	// Assuming that the 0th procedure is the first procedure encountered
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
	CallComparator(vector<int> topoOrder) {this->topoOrder = topoOrder; }

	bool operator() (TNode* i, TNode* j) { 
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


vector<TNode*> DesignExtractor::obtainCallStatementsInTopologicalOrder(vector<int> topologicalOrder) {
	// sort all call statements 
	PKB pkb = PKB::getInstance();

	vector<int> allCallStatementsNum = pkb.getStmtNumForType("call");
	vector<TNode*> allCallStatementNodes;

	for (auto iter = allCallStatementsNum.begin(); iter != allCallStatementsNum.end(); ++iter) {
		TNode* node = pkb.nodeTable.at(*iter);
		allCallStatementNodes.push_back(node);
	}

	cout << "call statements: " << endl;
	for (auto iter = allCallStatementNodes.begin(); iter != allCallStatementNodes.end(); ++iter) {
		cout << " " << **iter;
	} cout << endl;

	CallComparator compare(topologicalOrder);

	sort(allCallStatementNodes.begin(), allCallStatementNodes.end(), compare); 
	cout << "sorted call statements: " << endl;
	for (auto iter = allCallStatementNodes.begin(); iter != allCallStatementNodes.end(); ++iter) {
		cout << " " << **iter;
	} cout << endl;
	
	return allCallStatementNodes;
}