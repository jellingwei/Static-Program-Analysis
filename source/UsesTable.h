#pragma once

#include<stdio.h>
#include <iostream>
#include <string>
#include <vector>
#include <unordered_map>

using namespace std;

class UsesTable {
public:
	bool setUses(int stmtNum, int varIndex);
	bool isUses(int stmtNum, int varIndex);
	vector<int> getUsesStmtNum(int varIndex);
	vector<int> getUsesVarForStmt(int stmtNum);
	pair<vector<int>, vector<int>> getAllUsesPair();

private: 
	unordered_map<int, vector<int>> varIndexMap; // key is stmtNum 
	unordered_map<int, vector<int>> stmtNumMap; // key is variable index
};
