#pragma once

#include<stdio.h>
#include <iostream>
#include <string>
#include <vector>
#include <unordered_map>

#include <boost/dynamic_bitset.hpp>

using namespace std;

class ModifiesTable {
public:
	void init(int numVariables);
	//for statement numbers
	bool setModifies(int stmtNum, int varIndex);
	bool isModifies(int stmtNum, int varIndex);
	vector<int> getModStmtNum(int varIndex);
	vector<int> getModVarForStmt(int stmtNum);
	pair<vector<int>, vector<int>> getAllModPair();
	vector<int> getLhs();
	vector<int> getRhs();

	//for procedures
	bool setModifiesProc(int procIndex, int varIndex);
	bool isModifiesProc(int procIndex, int varIndex);
	vector<int> getModProcIndex(int varIndex);
	vector<int> getModVarForProc(int procIndex);
	pair<vector<int>, vector<int>> getAllModProcPair();

private: 
	unordered_map<int, boost::dynamic_bitset<>> varIndexMap; // key is stmtNum 
	unordered_map<int, vector<int>> stmtNumMap; // key is variable index

	unordered_map<int, vector<int>> procVarIndexMap; // key is procedure index 
	unordered_map<int, vector<int>> procIndexMap; // key is variable index

	int numVariables;
};
