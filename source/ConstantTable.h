#pragma once

#include<stdio.h>
#include <iostream>
#include <string>
#include <vector>
#include <unordered_map>

#include "common.h"

using namespace std;

/**
	@brief Refer to the PKB for the documentation.
*/

class ConstantTable {
public:
	STATUS insertConstant(VALUE constant, STATEMENT stmtNum);
	INTEGER getSize();
	STATEMENT_LIST getStmtNum(VALUE constant);
	BOOLEAN_ isConstant(INTEGER number);
	CONSTANT_LIST getAllConstant();

private: 
	unordered_map<int, int> constantMap; // key is index
	unordered_map<int, int> indexMap; // key is constant
	unordered_map<int, vector<int>> stmtMap; // key is constant to get statement number

};
