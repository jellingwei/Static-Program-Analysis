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
 * If varName is not in the VarTable, inserts it into the VarTable with the
 * given statement number stmtNum and return its index. Otherwise, return its INDEX
 * and the table remains unchanged.
 * @exception if varName is empty or stmtNum is negative or 0.
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
 * Return the total number of variables in the the VarTable.
 */
int VarTable::getVarTableSize() 
{
	return varMap.size();
}

/**
 * Return the name of the variable in the VarTable with the given index.
 * If index is out of range, return an empty string.
 */
string VarTable::getVarName(int index) 
{
	
	return (varMap.count(index) > 0) ? varMap.at(index) : ""; 
}

/**
 * If varName is in the VarTable, return its index. Otherwise, return -1 to
 * indicate there is no such variable in the the VarTable. If varName is empty or null, -1 is
 * returned.
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
 * Return the statement number of the variable in the VarTable with the given
 * varName. Otherwise, return -1 to indicate there is no such statement number.
 * If there is no answer or if varName is empty or null, return an empty list.
 */

int VarTable::getStmtNum(string varName) 
{
	if (varName.empty()) 
	{
		throw exception("varTable error: Empty variable name");
	}

	return (stmtMap.count(varName) > 0) ? stmtMap.at(varName) : -1;
}

/**
 * Return all the index of the variables in the the VarTable.
 * If there is no answer, return an empty list.
 */
vector<int> VarTable::getAllVarIndex() 
{
	vector<int> allKeys;

	for (auto iter = varMap.begin(); iter != varMap.end(); ++iter) 
	{
		allKeys.push_back(iter->first);
	}

	return allKeys;
}