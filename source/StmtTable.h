#pragma once

#include <stdio.h>
#include <iostream>
#include <vector>
#include <string>
#include <unordered_map>

using namespace std;

class StmtTable
{
public:
	bool insertStmt(int, string);
	string getType(int);
	vector<int> getStmtNumForType(string);
	bool isAssign(int);
	bool isWhile(int);
	bool isIf(int);
	bool isCall(int);
	int getSize();

private:
	unordered_map<int, string> stmtNumMap; //key is given by stmt#
	//unordered_map<string, int> stmtTypeMap; //key is given by stmtType
	vector<int> whileStmt;
	vector<int> assignStmt;
	vector<int> ifStmt;
	vector<int> callStmt;
};