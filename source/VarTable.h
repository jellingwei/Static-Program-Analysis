#pragma once

#include<stdio.h>
#include <iostream>
#include <string>
#include <vector>
#include <unordered_map>

using namespace std;

class VarTable {
public:
	int insertVar(string varName, int stmtNum);
	int getVarTableSize();
	string getVarName(int index);
	int getVarIndex(string varName);
	vector<int> getAllVarIndex();

private: 
	unordered_map<int, string> varMap; // key is index
	unordered_map<string, int> indexMap; // key is variable name
	unordered_map<string, int> stmtMap; // key is variable name to get statement number

};
