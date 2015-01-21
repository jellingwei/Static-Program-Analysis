#pragma once

#include<stdio.h>
#include <iostream>
#include <string>
#include <vector>
#include <unordered_map>

#include "VarTable.h"

using namespace std;
using namespace stdext;


/**
* Returns the index of the variable.
*/
int VarTable::insertVar(string varName, int stmtNum) 
{
	if (stmtNum <= 0) 
	{
		throw exception("varTable error: Negative statement number");
	}

	if (varName.empty()) 
	{
		throw exception("varTable error: Empty variable name");
	}

	if (indexMap.count(varName) > 0) 
	{
		return indexMap.at(varName);
	} else 
	{
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

/**
* Returns the size of the varTable.
*/
int VarTable::getVarTableSize() 
{
	return varMap.size();
}

/**
* Returns the name of a variable in varTable using the index as the key.
*/
string VarTable::getVarName(int index) 
{
	return (varMap.size() > 0) ? varMap.at(index) : ""; 
}

/**
* Returns the index of a variable in varTable using the variable name as the key.
*/
int VarTable::getVarIndex(string varName) 
{
	if (varName.empty()) 
	{
		throw exception("varTable error: Empty variable name");
	}

	return (indexMap.count(varName) > 0) ? indexMap.at(varName) : -1;
}

/**
* Returns the statement number the variable is in the varTable.
*/
int VarTable::getStmtNum(string varName) 
{
	if (varName.empty()) 
	{
		throw exception("varTable error: Empty variable name");
	}

	return (stmtMap.count(varName) > 0) ? stmtMap.at(varName) : -1;
}

vector<int> VarTable::getAllVarIndex() 
{
	vector<int> allKeys;

	for (auto iter = varMap.begin(); iter != varMap.end(); ++iter) 
	{
		allKeys.push_back(iter->first);
	}

	return allKeys;
}