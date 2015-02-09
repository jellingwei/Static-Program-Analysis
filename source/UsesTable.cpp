#pragma once

#include<stdio.h>
#include <iostream>
#include <string>
#include <vector>
#include <unordered_map>
#include <algorithm>

#include "UsesTable.h"
#include "ParentTable.h"
#include "PKB.h"

using namespace std;
using namespace stdext;


bool UsesTable::setUses(int stmtNum, int varIndex) 
{
	if (stmtNum <= 0 || varIndex <= 0) {
		throw exception("UsesTable error: Negative statement number or varIndex");
	}

	vector<int> newVarList;
	vector<int> varIndexList;

	//check if varIndexMap key stmtNum contains the variable
	if (varIndexMap.count(stmtNum) > 0) {
		varIndexList = varIndexMap.at(stmtNum);

		auto result = find(begin(varIndexList), end(varIndexList), varIndex);
		
		if (result != end(varIndexList)) { // varIndex can be found already
		
		} else {
			varIndexList.push_back(varIndex);
			varIndexMap.erase(stmtNum);
			varIndexMap.insert(pair<int, vector<int>> (stmtNum, varIndexList));
		}

	} else {
		newVarList.push_back(varIndex);
		varIndexMap.insert(pair<int, vector<int>> (stmtNum, newVarList));
		
	}

	vector<int> newStmtList;
	vector<int> stmtList;

	//check if stmtNumMap key varIndex contains the stmtNum
	if (stmtNumMap.count(varIndex) > 0) {
		stmtList = stmtNumMap.at(varIndex);

		auto result = find(begin(stmtList), end(stmtList), stmtNum);
		if (result == end(stmtList)) {
			stmtList.push_back(stmtNum);
			stmtNumMap.erase(varIndex);
			stmtNumMap.insert(pair<int, vector<int>> (varIndex, stmtList));
			
		}

	} else {
		newStmtList.push_back(stmtNum);
		stmtNumMap.insert(pair<int, vector<int>> (varIndex, newStmtList));
	}
	return true;	
}


bool UsesTable::isUses(int stmtNum, int varIndex) 
{
	if (stmtNum <= 0 || varIndex <= 0) {
		return false;
	}

	vector<int> varIndexList;

	if (varIndexMap.count(stmtNum) > 0) {
		varIndexList = varIndexMap.at(stmtNum);
	} else {
		return false;
	}

	auto result = find(begin(varIndexList), end(varIndexList), varIndex);
	// TRUE if varindex is new in the table
	return result != end(varIndexList);
}


vector<int> UsesTable::getUsesStmtNum(int varIndex) 
{
	if (varIndex <= 0) {
		return vector<int>();
	}

	if (stmtNumMap.count(varIndex) == 0) {
		vector<int> result;
		return result;
	}

	vector<int> stmtList = stmtNumMap.at(varIndex);
	return stmtList;
}


vector<int> UsesTable::getUsesVarForStmt(int stmtNum) 
{
	if (stmtNum <= 0) {
		return vector<int>();
	}

	if (varIndexMap.count(stmtNum) == 0) {
		vector<int> result;
		return result;
	}

	vector<int> varIndexList = varIndexMap.at(stmtNum);
	return varIndexList;
}


pair<vector<int>, vector<int>> UsesTable::getAllUsesPair() 
{
	pair<vector<int>, vector<int>> result;
	for (auto iter = varIndexMap.begin(); iter != varIndexMap.end(); ++iter) {
		for (vector<int>::iterator varListIter = iter->second.begin(); varListIter != iter->second.end(); ++varListIter) {
			result.first.push_back(iter->first);
			result.second.push_back(*varListIter);
		}
		
	}

	return result;
}

// for procedures

bool UsesTable::setUsesProc(int procIndex, int varIndex) 
{
	if (procIndex < 0 || varIndex <= 0) {
		throw exception("UsesTable error: Negative procedure index or varIndex");
	}

	vector<int> newVarList;
	vector<int> varIndexList;

	//check if varIndexMap key stmtNum contains the variable
	if (procVarIndexMap.count(procIndex) > 0) {
		varIndexList = procVarIndexMap.at(procIndex);

		auto result = find(begin(varIndexList), end(varIndexList), varIndex);
		
		if (result != end(varIndexList)) { // varIndex can be found already
		
		} else {
			varIndexList.push_back(varIndex);
			procVarIndexMap.erase(procIndex);
			procVarIndexMap.insert(pair<int, vector<int>> (procIndex, varIndexList));
		}

	} else {
		newVarList.push_back(varIndex);
		procVarIndexMap.insert(pair<int, vector<int>> (procIndex, newVarList));
		
	}

	vector<int> newProcIndexList;
	vector<int> procIndexList;

	//check if stmtNumMap key varIndex contains the stmtNum
	if (procIndexMap.count(varIndex) > 0) {
		procIndexList = procIndexMap.at(varIndex);

		auto result = find(begin(procIndexList), end(procIndexList), procIndex);
		if (result == end(procIndexList)) {
			procIndexList.push_back(procIndex);
			procIndexMap.erase(varIndex);
			procIndexMap.insert(pair<int, vector<int>> (varIndex, procIndexList));
			
		}

	} else {
		newProcIndexList.push_back(procIndex);
		procIndexMap.insert(pair<int, vector<int>> (varIndex, newProcIndexList));
	}
	return true;	
}


bool UsesTable::isUsesProc(int procIndex, int varIndex) 
{
	if (procIndex < 0 || varIndex <= 0) {
		return false;
	}

	vector<int> varIndexList;

	if (procVarIndexMap.count(procIndex) > 0) {
		varIndexList = procVarIndexMap.at(procIndex);
	} else {
		return false;
	}

	auto result = find(begin(varIndexList), end(varIndexList), varIndex);
	// TRUE if varindex is new in the table
	return result != end(varIndexList);
}



vector<int> UsesTable::getUsesProcIndex(int varIndex) 
{
	if (varIndex <= 0) {
		return vector<int>();
	}

	if (procIndexMap.count(varIndex) == 0) {
		vector<int> result;
		return result;
	}

	vector<int> procIndexList = procIndexMap.at(varIndex);
	return procIndexList;
}



vector<int> UsesTable::getUsesVarForProc(int procIndex) 
{
	if (procIndex < 0) {
		return vector<int>();
	}

	if (procVarIndexMap.count(procIndex) == 0) {
		vector<int> result;
		return result;
	}

	vector<int> varIndexList = procVarIndexMap.at(procIndex);
	return varIndexList;
}



pair<vector<int>, vector<int>> UsesTable::getAllUsesProcPair() 
{
	pair<vector<int>, vector<int>> result;
	for (auto iter = procVarIndexMap.begin(); iter != procVarIndexMap.end(); ++iter) {
		for (vector<int>::iterator varListIter = iter->second.begin(); varListIter != iter->second.end(); ++varListIter) {
			result.first.push_back(iter->first);
			result.second.push_back(*varListIter);
		}
		
	}

	return result;
}
