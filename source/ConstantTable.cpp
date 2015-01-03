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
* Returns the index of the constant.
*/
bool ConstantTable::insertConstant(int constant, int stmtNum) {
	if (stmtNum <= 0 || constant < 0) {
		throw exception("constantTable error: Negative statement number or constant number");
	}
	
	int size = stmtMap.size();		
	int index = size+1;

	pair<int,int> pairIndexVar (index, constant);
	constantMap.insert(pairIndexVar);

	pair<int,int> pairVarIndex (constant, index);
	indexMap.insert(pairVarIndex);

	vector<int> stmtVector;
	if (stmtMap.count(constant) > 0) {
		stmtVector = stmtMap.at(constant);
		stmtMap.erase(constant);
	}

	stmtVector.push_back(stmtNum);

	pair<int, vector<int>> pairVarStmt (constant, stmtVector);
		
	stmtMap.insert(pairVarStmt);
	
	return index;
	
}

/**
* Returns the size of the constantTable.
*/
int ConstantTable::getSize() {
	return stmtMap.size();
}

/**
* Returns the constant in ConstantTable using the index as the key.
*/
int ConstantTable::getConstant(int index) {
	if (constantMap.count(index) > 0) {
		return constantMap.at(index);
	} else {
		return -1;
	}
}

/**
* Returns the index of a constant in ConstantTable using the constant as the key.
*/
int ConstantTable::getConstantIndex(int constant) {
	if (constant <= 0) {
		throw exception("constantTable error: Negative constant input");
	}

	if (indexMap.count(constant) > 0) {
		return indexMap.at(constant);
	}else {
		return -1;
	}
}

/**
* Returns the statement number the constant is in the ConstantTable.
*/
vector<int> ConstantTable::getStmtNum(int constant) {
	if (constant <= 0) {
		throw exception("constantTable error: Negative constant input");
	}

	if (stmtMap.count(constant) > 0) {
		return stmtMap.at(constant);
	}else {
		return vector<int>();
	}
}

/**
* Check if it is a constant in the ConstantTable.
*/
bool ConstantTable::isConstant(int number) {
	if (number <= 0) {
		throw exception("constantTable error: Negative number input");
	}

	if (indexMap.count(number) > 0) {
		return true;
	}else {
		return false;
	}
}

/**
* Returns the list of all the constant in the ConstantTable.
*/
vector<int> ConstantTable::getAllConstant() {
	vector<int> allValues;

	for (auto iter = stmtMap.begin(); iter != stmtMap.end(); ++iter) {
		allValues.push_back(iter->first);
	}

	return allValues;
}