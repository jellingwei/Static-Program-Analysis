#pragma once

#include<stdio.h>
#include <iostream>
#include <string>
#include <vector>
#include <unordered_map>

using namespace std;

class ProcTable {
public:
	int insertProc(string procName);
	int getProcTableSize();
	string getProcName(int index);
	int getProcIndex(string varName);
	vector<int> getAllProcIndex();

private: 
	vector<string> procMap; 
	unordered_map<string, int> indexMap; // key is variable name
};
