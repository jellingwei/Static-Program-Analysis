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
* Returns true if UsesTable updated.
*/
bool UsesTable::setUses(int stmtNum, int varIndex) 
{
	if (stmtNum <= 0 || varIndex <= 0) 
	{
		throw exception("UsesTable error: Negative statement number or varIndex");
	}

	vector<int> newVarList;
	vector<int> varIndexList;

	//check if varIndexMap key stmtNum contains the variable
	if (varIndexMap.count(stmtNum) > 0) 
	{
		varIndexList = varIndexMap.at(stmtNum);

		auto result = find(begin(varIndexList), end(varIndexList), varIndex);
		if (result != end(varIndexList)) 
		{
		
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
	if (stmtNumMap.count(varIndex) > 0) 
	{
		stmtList = stmtNumMap.at(varIndex);

		auto result = find(begin(stmtList), end(stmtList), stmtNum);
		if (result == end(stmtList)) 
		{
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
* Returns true if the statement number Uses the variable.
*/
bool UsesTable::isUses(int stmtNum, int varIndex) 
{
	if (stmtNum <= 0 || varIndex <= 0) 
	{
	//	throw exception("UsesTable error: Negative statement number or varIndex");
		return false;
	}

	vector<int> varIndexList;

	if (varIndexMap.count(stmtNum) > 0) 
	{
		varIndexList = varIndexMap.at(stmtNum);
	} else {
		return false;
	}

	auto result = find(begin(varIndexList), end(varIndexList), varIndex);
	if (result != end(varIndexList)) 
	{
		return true;  
	} 

	return false;
}

/**
* Returns a list of statement number that Uses the variable.
*/
vector<int> UsesTable::getUsesStmtNum(int varIndex) 
{
	if (varIndex <= 0) 
	{
		//throw exception("UsesTable error: Negative varIndex");
		return vector<int>();
	}

	if (stmtNumMap.count(varIndex) == 0) 
	{
		vector<int> result;
		return result;
	}

	vector<int> stmtList = stmtNumMap.at(varIndex);

	return stmtList;
}

/**
* Returns a list of variables used by the statement number.
*/
vector<int> UsesTable::getUsesVarForStmt(int stmtNum) 
{
	if (stmtNum <= 0) 
	{
		//throw exception("UsesTable error: Negative statement number");
		return vector<int>();
	}

	if (varIndexMap.count(stmtNum) == 0) 
	{
		vector<int> result;
		return result;
	}
	vector<int> varIndexList = varIndexMap.at(stmtNum);

	return varIndexList;
}

/**
* Returns a pair of vectors, where Uses(stmt#, var index) holds.  
* The first vector returned consists of stmt numbers, the second vector consists of variable indexes.
*/
pair<vector<int>, vector<int>> UsesTable::getAllUsesPair() 
{
	pair<vector<int>, vector<int>> result;
	for (auto iter = varIndexMap.begin(); iter != varIndexMap.end(); ++iter) 
	{
		for (vector<int>::iterator varListIter = iter->second.begin(); varListIter != iter->second.end(); ++varListIter) 
		{
			result.first.push_back(iter->first);
			result.second.push_back(*varListIter);
		}
		
	}

	return result;
}