#pragma once

#include<stdio.h>
#include <iostream>
#include <string>
#include <vector>
#include <unordered_map>

#include "common.h"
#include "common_list.h"

using namespace std;

class VarTable {
public:
	VAR_INDEX insertVar(VARNAME varName, STATEMENT stmtNum);
	INTEGER getVarTableSize();
	VARNAME getVarName(VAR_INDEX index);
	VAR_INDEX getVarIndex(VARNAME varName);
	VARINDEX_LIST getAllVarIndex();

private: 
	unordered_map<int, string> varMap; // key is index
	unordered_map<string, int> indexMap; // key is variable name
	unordered_map<string, int> stmtMap; // key is variable name to get statement number

};
