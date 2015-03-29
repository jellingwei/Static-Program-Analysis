#pragma once

#include<stdio.h>
#include <iostream>
#include <string>
#include <vector>
#include <unordered_map>

#include "VarTable.h"

using namespace std;
using namespace stdext;

VAR_INDEX VarTable::insertVar(VARNAME varName, int stmtNum) 
{
	if (stmtNum <= 0) {
		throw exception("varTable error: Negative statement number");
	}

	if (varName.empty()) {
		throw exception("varTable error: Empty variable name");
	}

	if (indexMap.count(varName) > 0) {
		return indexMap.at(varName);
	} else {
		int index = indexMap.size() + 1;

		pair<int,string> pairIndexVar (index, varName);
		varMap.insert(pairIndexVar);

		pair<string,int> pairVarIndex (varName, index);
		indexMap.insert(pairVarIndex);

		pair<string,int> pairVarStmt (varName, stmtNum);
		stmtMap.insert(pairVarStmt);

		return index;
	}
}

INTEGER VarTable::getVarTableSize() 
{
	return varMap.size();
}

VARNAME VarTable::getVarName(VAR_INDEX index) 
{
	return (varMap.count(index) > 0) ? varMap.at(index) : ""; 
}

VAR_INDEX VarTable::getVarIndex(VARNAME varName) 
{
	if (varName.empty()) {
		throw exception("varTable error: Empty variable name");
	}

	return (indexMap.count(varName) > 0) ? indexMap.at(varName) : -1;
}

VARINDEX_LIST VarTable::getAllVarIndex() 
{
	vector<int> allKeys;

	for (auto iter = varMap.begin(); iter != varMap.end(); ++iter) {
		allKeys.push_back(iter->first);
	}

	return allKeys;
}