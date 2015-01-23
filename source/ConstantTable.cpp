#pragma once

#include<stdio.h>
#include <iostream>
#include <string>
#include <vector>
#include <unordered_map>

#include "ConstantTable.h"

using namespace std;
using namespace stdext;


/**
 * If constant is not in the ConstantTable, inserts it into the ConstantTable with the
 * given statement number stmtNum and return its index. Otherwise, return its STATUS
 * and the table remains unchanged.
 * @exception if constant is empty or stmtNum is negative or 0.
 */
bool ConstantTable::insertConstant(int constant, int stmtNum) 
{
	if (stmtNum <= 0) 
	{
		throw invalid_argument("constantTable error: Negative statement number");
	}
	if (constant < 0) 
	{
		throw invalid_argument("constantTable error: Negative constant");
	}


	int index = stmtMap.size() + 1;	

	pair<int,int> pairIndexConstant (index, constant);
	constantMap.insert(pairIndexConstant);

	pair<int,int> pairConstantIndex (constant, index);
	indexMap.insert(pairConstantIndex);


	vector<int> stmtVector;
	if (stmtMap.count(constant) > 0) 
	{
		// constant table already has a mapping for the constant
		stmtVector = stmtMap.at(constant);
		stmtMap.erase(constant);
	}

	stmtVector.push_back(stmtNum);
	pair<int, vector<int>> pairVarStmt (constant, stmtVector);
	stmtMap.insert(pairVarStmt);
	
	return true; 
}

/**
 * Return the total number of constants in the the ConstantTable.
 */
int ConstantTable::getSize() 
{
	return stmtMap.size();
}

/**
 * Return the constant in the ConstantTable with the given index.
 * If index is out of range, return an empty string.
 */
int ConstantTable::getConstant(int index) 
{
	return (constantMap.count(index) > 0) ? constantMap.at(index) : -1;
}

/**
* Returns the index of a constant in ConstantTable using the constant as the key.
*/
int ConstantTable::getConstantIndex(int constant) 
{
	if (constant < 0) 
	{
		throw exception("constantTable error: Negative constant input");
	}

	return (indexMap.count(constant) > 0) ? indexMap.at(constant) : -1;
}

/**
* Returns the statement number the constant is in the ConstantTable.
*/
vector<int> ConstantTable::getStmtNum(int constant) 
{
	if (constant < 0) 
	{
		throw exception("constantTable error: Negative constant input");
	}

	return (stmtMap.count(constant) > 0) ? stmtMap.at(constant) : vector<int>();
}

/**
* Check if it is a constant in the ConstantTable.
*/
bool ConstantTable::isConstant(int number) 
{
	if (number < 0) 
	{
		throw exception("constantTable error: Negative number input");
	}

	if (indexMap.count(number) > 0) 
	{
		return true;
	}
		
	return false;
}

/**
* Returns the list of all the constant in the ConstantTable.
*/
vector<int> ConstantTable::getAllConstant() 
{
	vector<int> allValues;

	for (auto iter = stmtMap.begin(); iter != stmtMap.end(); ++iter) 
	{
		allValues.push_back(iter->first);
	}

	return allValues;
}