#pragma once

#include<stdio.h>
#include <iostream>
#include <string>
#include <vector>
#include <unordered_map>
#include <algorithm>

#include "CallsTable.h"
#include "PKB.h"


using namespace std;
using namespace stdext;

// @ToDo Fixed all Transitive Closure Issues

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

	//check if procIndex1Map key procIndex2 contains the procIndex1
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

	//check if procIndex2Map key procIndex1 contains the procIndex2
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
	bool status = true;

	if (procIndex1 < 0 || procIndex2 < 0) {
		status = false;
	}

	if (!transitiveClosure) {
		if ((procIndex1Map.count(procIndex2) == 0) || (procIndex2Map.count(procIndex1) == 0)) {
			status = false;
		}

	} else {
		 
		
	}
}

vector<int> CallsTable::getCalledFrom(int procIndex2, bool transitiveClosure) 
{
	if (procIndex2 < 0) {
		return vector<int>();
	}

	vector<int> result = vector<int>();
	vector<int> intermediateResult;
	int currentProc = procIndex2;
	int i = 0;

	while (procIndex1Map.count(currentProc) > 0) 
	{
		vector<int> list = procIndex1Map.at(currentProc);

		if (transitiveClosure) 
		{
			for (int i = 0; i < list.size()-1; i++) {
				intermediateResult.push_back(list.at(i));
			}
		} else 
		{
			result = list;
			break;
		}
	}
	
	return result;
}


vector<int> CallsTable::getCalledBy(int procIndex1, bool transitiveClosure) 
{
	if (procIndex1 < 0) {
		return vector<int>();
	}

	vector<int> result = vector<int>();
	int currentProc = procIndex1;

	while (procIndex2Map.count(currentProc) > 0) 
	{
		vector<int> list = procIndex2Map.at(currentProc);

		if (transitiveClosure) 
		{
			
		} else 
		{
			result = list;
			break;
		}
	}
	
	return result;
}


pair<vector<int>, vector<int>> CallsTable::getAllCallsPairs(bool transitiveClosure) 
{
	pair<vector<int>, vector<int>> results;

	return results;
}

