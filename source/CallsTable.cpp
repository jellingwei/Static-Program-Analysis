#pragma once

#include<stdio.h>
#include <iostream>
#include <string>
#include <vector>
#include <unordered_map>
#include <algorithm>
#include <queue>
#include <unordered_set>

#include "CallsTable.h"
#include "PKB.h"


using namespace std;
using namespace stdext;

bool CallsTable::setCalls(int procIndex1, int procIndex2) 
{
	if (procIndex1 < 0) {
		throw exception("CallsTable error: Invalid procIndex1");
	}

	if (procIndex2 < 0) {
		throw exception("CallsTable error: Invalid procIndex2");
	}

	vector<int> newProc1List;
	vector<int> procIndex1List;

	// check if procIndex1Map key procIndex2 contains the procIndex1
	if (procIndex1Map.count(procIndex2) > 0) {
		procIndex1List = procIndex1Map.at(procIndex2);

		auto result = find(begin(procIndex1List), end(procIndex1List), procIndex1);
		if (result != end(procIndex1List)) { // procIndex1 can be found already
			
		} else {
			procIndex1List.push_back(procIndex1);
			procIndex1Map.erase(procIndex2);
			procIndex1Map.insert(pair<int, vector<int>> (procIndex2, procIndex1List));
		}

	} else {
		newProc1List.push_back(procIndex1);
		procIndex1Map.insert(pair<int, vector<int>> (procIndex2, newProc1List));
	}

	vector<int> newProc2List;
	vector<int> procIndex2List;

	// check if procIndex2Map key procIndex1 contains the procIndex2
	if (procIndex2Map.count(procIndex1) > 0) {
		procIndex2List = procIndex2Map.at(procIndex1);

		auto result = find(begin(procIndex2List), end(procIndex2List), procIndex2);
		if (result != end(procIndex2List)) 	{ // procIndex2 can be found already
			
		} else {
			procIndex2List.push_back(procIndex2);
			procIndex2Map.erase(procIndex1);
			procIndex2Map.insert(pair<int, vector<int>> (procIndex1, procIndex2List));
		}

	} else {
		newProc2List.push_back(procIndex2);
		procIndex2Map.insert(pair<int, vector<int>> (procIndex1, newProc2List));
	}
	return true;
}


bool CallsTable::isCalls(int procIndex1, int procIndex2, bool transitiveClosure) {
	if (procIndex1 < 0 || procIndex2 < 0) {
		return false;
	}
	bool earlyTermination = true;

	if (!transitiveClosure) {
		if ((procIndex1Map.count(procIndex2) == 0) || (procIndex2Map.count(procIndex1) == 0)) {
			return false;
		}
		
		vector<int> calledProcs = getProcsCalledBy(procIndex1, false, true, procIndex2);

		return find(calledProcs.begin(), calledProcs.end(), procIndex2) != calledProcs.end();
	} else {
		bool isTransitiveClosure = true;
		vector<int> allCalledProcs = getProcsCalledBy(procIndex1, isTransitiveClosure, true, procIndex2);
		
		// return true if procIndex2 can be found in procs called by proc1
		return find(allCalledProcs.begin(), allCalledProcs.end(), procIndex2) != allCalledProcs.end();
	}
}

vector<int> CallsTable::getProcsCalling(int procIndex2, bool transitiveClosure, bool terminateOnFinding, int lineToFind ) 
{
	if (procIndex2 < 0 || procIndex1Map.count(procIndex2) == 0) {
		return vector<int>();
	}

	if (!transitiveClosure) {
		return procIndex1Map.at(procIndex2);
	}

	if (procIndex1Map.count(procIndex2) < 0){
		return vector<int>();
	}

	vector<int> intermediateValues;
	vector<int> result;
	deque<int> frontier;
	unordered_set<int> visited;

	frontier.push_back(procIndex2);
		
	while (!frontier.empty()) {
		int curProc = frontier.back(); frontier.pop_back();
		visited.insert(curProc);

		intermediateValues = getProcsCalling(curProc, false);
		for (auto iter = intermediateValues.begin(); iter != intermediateValues.end(); ++iter) {
			if (visited.count(*iter) != 0 || find(frontier.begin(), frontier.end(), *iter) != frontier.end()) {
				// skip if already visited, or if already in the frontier
				continue;
			}
			frontier.push_back(*iter);
			result.push_back(*iter);

			if (terminateOnFinding && lineToFind == *iter) {
				// early termination
				vector<int> resultList(result.begin(), result.end());
				return resultList;
			}
		}
	}
		
	vector<int> resultList(result.begin(), result.end());
	return resultList;
}


vector<int> CallsTable::getProcsCalledBy(int procIndex1, bool transitiveClosure, bool terminateOnFinding, int lineToFind) 
{
	if (procIndex1 < 0 || procIndex2Map.count(procIndex1) == 0) {
		return vector<int>();
	}

	if (!transitiveClosure) {
		return procIndex2Map.at(procIndex1);
	}

	if (procIndex2Map.count(procIndex1) < 0) {
		return vector<int>();
	}

	vector<int> intermediateValues;
	vector<int> result;
	deque<int> frontier;
	unordered_set<int> visited;

	frontier.push_back(procIndex1);
		
	while (!frontier.empty()) {
		int curProc = frontier.back(); frontier.pop_back();
		visited.insert(curProc);

		intermediateValues = getProcsCalledBy(curProc, false);
		for (auto iter = intermediateValues.begin(); iter != intermediateValues.end(); ++iter) {
			if (visited.count(*iter) != 0 || find(frontier.begin(), frontier.end(), *iter) != frontier.end()) {
				// skip if already visited, or if already in the frontier
				continue;
			}
			frontier.push_back(*iter);
			result.push_back(*iter);

			if (terminateOnFinding && lineToFind == *iter) {
				// early termination
				vector<int> resultList(result.begin(), result.end());
				return resultList;
			}
		}
	}
		
	vector<int> resultList(result.begin(), result.end());
	return resultList;
}

vector<int> CallsTable::getLhs() {
	vector<int> result;
	for (auto iter = procIndex2Map.begin(); iter != procIndex2Map.end(); ++iter) {
		result.push_back(iter->first);
	}
	return result;
}

vector<int> CallsTable::getRhs() {
	vector<int> result;
	for (auto iter = procIndex1Map.begin(); iter != procIndex1Map.end(); ++iter) {
		result.push_back(iter->first);
	}
	return result;
}


pair<vector<int>, vector<int>> CallsTable::getAllCallsPairs(bool transitiveClosure) 
{
	pair<vector<int>, vector<int>> results;

	return results;
}

string CallsTable::getProcNameCalledByStatement(int stmtNum) {
	PKB pkb = PKB::getInstance();
	TNode* node = pkb.getNodeForStmt(stmtNum);

	if (node == NULL || node->getNodeType() != Call) {
		throw logic_error("CallsTable error: stmtNum does not have a node in AST or is not a call statement");
	}
	int procIndex = node->getNodeValueIdx();

	return pkb.getProcName(procIndex);
}