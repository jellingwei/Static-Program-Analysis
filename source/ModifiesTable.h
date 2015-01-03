#pragma once

#include<stdio.h>
#include <iostream>
#include <string>
#include <vector>
#include <unordered_map>

using namespace std;

class ModifiesTable {
public:
	bool setModifies(int stmtNum, int varIndex);
	bool isModifies(int stmtNum, int varIndex);
	vector<int> getModStmtNum(int varIndex);
	vector<int> getModVarForStmt(int stmtNum);
	pair<vector<int>, vector<int>> getAllModPair();

private: 
	unordered_map<int, vector<int>> varIndexMap; // key is stmtNum 
	unordered_map<int, vector<int>> stmtNumMap; // key is variable index
};
