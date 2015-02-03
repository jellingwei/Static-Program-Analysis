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

// @todo Fix all Transitive Closure Issues

bool CallsTable::setCalls(int procIndex1, int procIndex2) 
{
	if (procIndex1 < 0) {
		throw exception("CallsTable error: Invalid procIndex1");
	}

	if (procIndex2 < 0) {
		throw exception("CallsTable error: Invalid procIndex2");
	}

	vector<int> newProc1List;
	vector<int> ProcIndex1List;

	// check if procIndex1Map key procIndex2 contains the procIndex1
	if (procIndex1Map.count(procIndex2) > 0) {
		ProcIndex1List = procIndex1Map.at(procIndex2);

		auto result = find(begin(ProcIndex1List), end(ProcIndex1List), procIndex1);
		if (result != end(ProcIndex1List)) { // procIndex1 can be found already
			
		} else {
			ProcIndex1List.push_back(procIndex1);
			procIndex1Map.erase(procIndex2);
			procIndex1Map.insert(pair<int, vector<int>> (procIndex2, ProcIndex1List));
		}

	} else {
		newProc1List.push_back(procIndex1);
		procIndex1Map.insert(pair<int, vector<int>> (procIndex2, newProc1List));
	}

	vector<int> newProc2List;
	vector<int> ProcIndex2List;

	// check if procIndex2Map key procIndex1 contains the procIndex2
	if (procIndex2Map.count(procIndex1) > 0) {
		ProcIndex2List = procIndex2Map.at(procIndex1);

		auto result = find(begin(ProcIndex2List), end(ProcIndex2List), procIndex2);
		if (result != end(ProcIndex2List)) 	{ // procIndex2 can be found already
			
		} else {
			ProcIndex2List.push_back(procIndex2);
			procIndex2Map.erase(procIndex1);
			procIndex2Map.insert(pair<int, vector<int>> (procIndex1, ProcIndex2List));
		}

	} else {
		newProc2List.push_back(procIndex2);
		procIndex2Map.insert(pair<int, vector<int>> (procIndex1, newProc2List));
	}
	return true;
}


bool CallsTable::isCalls(int procIndex1, int procIndex2, bool transitiveClosure) 
{
	if (procIndex1 < 0 || procIndex2 < 0) {
		return false;
	}

	if (!transitiveClosure) {
		if ((procIndex1Map.count(procIndex2) == 0) || (procIndex2Map.count(procIndex1) == 0)) {
			return false;
		}
		vector<int> calledProcs = getProcsCalledBy(procIndex1, false);

		return find(calledProcs.begin(), calledProcs.end(), procIndex2) != calledProcs.end();
	} else {
		bool isTransitiveClosure = true;
		vector<int> allCalledProcs = getProcsCalledBy(procIndex1, isTransitiveClosure);
		
		// return true if procIndex2 can be found in procs called by proc1
		return find(allCalledProcs.begin(), allCalledProcs.end(), procIndex2) != allCalledProcs.end();
	}
}

vector<int> CallsTable::getProcsCalling(int procIndex2, bool transitiveClosure) 
{
	if (procIndex2 < 0 || procIndex1Map.count(procIndex2) == 0) {
		return vector<int>();
	}

	if (!transitiveClosure) {
		return procIndex1Map.at(procIndex2);
	}

	vector<int> intermediateValues = procIndex2Map.at(procIndex2);

	if (intermediateValues.empty()) {
		return vector<int>();
	}
	
	queue<int> frontier(std::deque<int>(intermediateValues.begin(), intermediateValues.end()));
	//@todo optimise by not storing as set first
	unordered_set<int> result(intermediateValues.begin(), intermediateValues.end()); // store as set first to prevent duplicates
		
	while (frontier.empty()) {
		int curProc = frontier.front(); frontier.pop();
		intermediateValues = getProcsCalling(curProc, false);
		for (auto iter = intermediateValues.begin(); iter != intermediateValues.end(); ++iter) {
			frontier.push(*iter);
			result.insert(*iter);
		}
	}
		
	vector<int> resultList(result.begin(), result.end());
	return resultList;
}


vector<int> CallsTable::getProcsCalledBy(int procIndex1, bool transitiveClosure) 
{
	if (procIndex1 < 0 || procIndex2Map.count(procIndex1) == 0) {
		return vector<int>();
	}

	if (!transitiveClosure) {
		return procIndex2Map.at(procIndex1);
	}
		
	vector<int> intermediateValues = procIndex2Map.at(procIndex1);

	if (intermediateValues.empty()) {
		return vector<int>();
	}
	queue<int> frontier(std::deque<int>(intermediateValues.begin(), intermediateValues.end()));
	//@todo optimise by not storing as set first
	unordered_set<int> result(intermediateValues.begin(), intermediateValues.end()); // store as set first to prevent duplicates
		
	while (frontier.empty()) {
		int curProc = frontier.front(); frontier.pop();
		intermediateValues = getProcsCalledBy(curProc, false);
		for (auto iter = intermediateValues.begin(); iter != intermediateValues.end(); ++iter) {
			frontier.push(*iter);
			result.insert(*iter);
		}
	}
		
	vector<int> resultList(result.begin(), result.end());
	return resultList;
}


pair<vector<int>, vector<int>> CallsTable::getAllCallsPairs(bool transitiveClosure) 
{
	pair<vector<int>, vector<int>> results;

	return results;
}

