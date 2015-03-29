#pragma once

#include<stdio.h>
#include <iostream>
#include <string>
#include <vector>
#include <unordered_map>

#include "ConstantTable.h"

using namespace std;
using namespace stdext;


STATUS ConstantTable::insertConstant(VALUE constant, STATEMENT stmtNum) 
{
	if (stmtNum <= 0) {
		throw invalid_argument("constantTable error: Negative statement number");
	}
	if (constant < 0) {
		throw invalid_argument("constantTable error: Negative constant");
	}


	int index = stmtMap.size() + 1;	

	pair<int,int> pairIndexConstant (index, constant);
	constantMap.insert(pairIndexConstant);

	pair<int,int> pairConstantIndex (constant, index);
	indexMap.insert(pairConstantIndex);


	vector<int> stmtVector;
	if (stmtMap.count(constant) > 0) {
		// constant table already has a mapping for the constant
		stmtVector = stmtMap.at(constant);
		stmtMap.erase(constant);
	}

	stmtVector.push_back(stmtNum);
	pair<int, vector<int>> pairVarStmt (constant, stmtVector);
	stmtMap.insert(pairVarStmt);
	
	return true; 
}


INTEGER ConstantTable::getSize() 
{
	return stmtMap.size();
}

STATEMENT_LIST ConstantTable::getStmtNum(VALUE constant) 
{
	if (constant < 0) {
		throw exception("constantTable error: Negative constant input");
	}

	return (stmtMap.count(constant) > 0) ? stmtMap.at(constant) : vector<int>();
}


BOOLEAN_ ConstantTable::isConstant(INTEGER number) 
{

	if (indexMap.count(number) > 0) {
		return true;
	}
		
	return false;
}

CONSTANT_LIST ConstantTable::getAllConstant() 
{
	vector<int> allValues;

	for (auto iter = stmtMap.begin(); iter != stmtMap.end(); ++iter) {
		allValues.push_back(iter->first);
	}

	return allValues;
}