#pragma once

#include<stdio.h>
#include <iostream>
#include <string>
#include <vector>
#include <unordered_map>

using namespace std;

class UsesTable {
public:
	UsesTable(int numVariables);
	// for statement numbers
	bool setUses(int stmtNum, int varIndex);
	bool isUses(int stmtNum, int varIndex);
	vector<int> getUsesStmtNum(int varIndex);
	vector<int> getUsesVarForStmt(int stmtNum);
	pair<vector<int>, vector<int>> getAllUsesPair();
	vector<int> getLhs();
	vector<int> getRhs();

	// for procedures 
	bool setUsesProc(int procIndex, int varIndex);
	bool isUsesProc(int procIndex, int varIndex);
	vector<int> getUsesProcIndex(int varIndex);
	vector<int> getUsesVarForProc(int procIndex);
	pair<vector<int>, vector<int>> getAllUsesProcPair();

private: 
	//unordered_map<int, vector<int>> varIndexMap; // key is stmtNum 
	unordered_map<int, vector<bool>> varIndexMap;
	unordered_map<int, vector<int>> stmtNumMap; // key is variable index

	unordered_map<int, vector<int>> procVarIndexMap; // key is procedure index 
	unordered_map<int, vector<int>> procIndexMap; // key is variable index

	int numVariables;
};
