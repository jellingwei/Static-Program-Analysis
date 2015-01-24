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

/**
* Return TRUE if the UsesTable is updated accordingly. Otherwise, return FALSE. 
* If stmtNum and varIndex are already present in the UsesTable and are previously set, the UsesTable will not be updated.
* @exception if stmtNum is negative or 0, or varIndex is negative.
*/
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

/**
* Return TRUE if the Uses relationship holds between statement number
* and the variable index.
* If stmtNum is negative or 0, or varIndex is negative, return FALSE.
*/
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

/**
* Return the list of all the statements that uses the variable index. 
* If there is no answer, or if varIndex is invalid, return an empty list.
*/
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

/**
* Return the list of all the variables that is used by the statement number. 
* If there is no answer, or if stmtNum is negative or 0, return an empty list.
*/
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

/**
* Return a pair of lists of statements and variable indexes, which correspond to the entire set of pairs of statements 
* and variables satisfying Uses(stmtNum, varIndex). The first value of the pair(stmtNum) is contained in the first 
* list in the pair, and the second value of the pair(varIndex) is contained in the second list in the pair. 
* If there is no such pair, a pair containing 2 empty lists is returned.
*/
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