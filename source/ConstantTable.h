#pragma once

#include<stdio.h>
#include <iostream>
#include <string>
#include <vector>
#include <unordered_map>

using namespace std;

class ConstantTable {
public:
	bool insertConstant(int constant, int stmtNum);
	int getSize();
	int getConstant(int index);
	int getConstantIndex(int constant);
	vector<int> getStmtNum(int constant);
	bool isConstant(int number);
	vector<int> getAllConstant();

private: 
	unordered_map<int, int> constantMap; // key is index
	unordered_map<int, int> indexMap; // key is constant
	unordered_map<int, vector<int>> stmtMap; // key is constant to get statement number

};
