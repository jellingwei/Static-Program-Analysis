#pragma once

#include<stdio.h>
#include <iostream>
#include <string>
#include <vector>
#include <unordered_map>
#include <algorithm>

#include "PKB.h"
#include "ParentTable.h"
#include "ModifiesTable.h"

using namespace std;
using namespace stdext;

/**
* Return TRUE if the ModifiesTable is updated accordingly. Otherwise, return FALSE. 
* If stmtNum and varIndex are already present in the ModifiesTable and are previously set, the ModifiesTable will not be updated.
* @exception if stmtNum is negative or 0, or varIndex is negative.
*/
bool ModifiesTable::setModifies(int stmtNum, int varIndex) 
{
	if (stmtNum <= 0 || varIndex <= 0) {
		throw exception("ModifiesTable error: Negative statement number or varIndex");
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
		if (result != end(stmtList)) 	{
			
		} else {
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

/**
* Return TRUE if the Modifies relationship holds between statement number
* and the variable index.
* If stmtNum is negative or 0, or varIndex is negative, return FALSE.
*/
bool ModifiesTable::isModifies(int stmtNum, int varIndex) 
{
	if (stmtNum <= 0 || varIndex <= 0) {
		return false;
	}

	if (varIndexMap.count(stmtNum) == 0) {
		return false;
	}

	vector<int> varIndexList = varIndexMap.at(stmtNum);

	auto result = find(begin(varIndexList), end(varIndexList), varIndex);
	// TRUE if varindex is new in the table
	return result != end(varIndexList);
}

/**
* Return the list of all the statements that modifies the variable index. 
* If there is no answer, or if varIndex is invalid, return an empty list.
*/
vector<int> ModifiesTable::getModStmtNum(int varIndex) 
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

/**
* Return the list of all the variables that is modified by the statement number. 
* If there is no answer, or if stmtNum is negative or 0, return an empty list.
*/
vector<int> ModifiesTable::getModVarForStmt(int stmtNum) 
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

/**
* Return a pair of lists of statements and variable indexes, which correspond to the entire set of pairs of statements 
* and variables satisfying Modifies(stmtNum, varIndex). The first value of the pair(stmtNum) is contained in the first 
* list in the pair, and the second value of the pair(varIndex) is contained in the second list in the pair. 
* If there is no such pair, a pair containing 2 empty lists is returned.
*/
pair<vector<int>, vector<int>> ModifiesTable::getAllModPair() 
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

bool ModifiesTable::setModifiesProc(int procIndex, int varIndex) 
{
	if (procIndex < 0 ) {
		throw logic_error("ModifiesTable error: Negative procedure index");
	}
	if (varIndex <= 0) {
		throw logic_error("ModifiesTable error: Negative varIndex");
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

	//check if procIndexMap key varIndex contains the procedure index
	if (procIndexMap.count(varIndex) > 0) {
		procIndexList = procIndexMap.at(varIndex);

		auto result = find(begin(procIndexList), end(procIndexList), procIndex);
		if (result != end(procIndexList)) 	{
			
		} else {
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

bool ModifiesTable::isModifiesProc(int procIndex, int varIndex) 
{
	if (procIndex < 0 || varIndex <= 0) {
		return false;
	}

	if (varIndexMap.count(procIndex) == 0) {
		return false;
	}

	vector<int> varIndexList = varIndexMap.at(procIndex);

	auto result = find(begin(varIndexList), end(varIndexList), varIndex);
	// TRUE if varindex is new in the table
	return result != end(varIndexList);
}

vector<int> ModifiesTable::getModProcIndex(int varIndex) 
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

vector<int> ModifiesTable::getModVarForProc(int procIndex) 
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

pair<vector<int>, vector<int>> ModifiesTable::getAllModProcPair() 
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





