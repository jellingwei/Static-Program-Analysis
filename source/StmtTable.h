#pragma once

#include <stdio.h>
#include <iostream>
#include <vector>
#include <string>
#include <unordered_map>

#include "TNode.h"
using namespace std;


class StmtTable
{
public:
	STATUS insertStmt(STATEMENT stmtNum, STATEMENT_TYPE type, TNode* node, PROC_INDEX procIndex);
	STATEMENT_TYPE getType(STATEMENT stmtNum);
	STATEMENT_LIST getStmtNumForType(STATEMENT_TYPE type);
	BOOLEAN_ isAssign(STATEMENT stmtNum);
	BOOLEAN_ isWhile(STATEMENT stmtNum);
	BOOLEAN_ isIf(STATEMENT stmtNum);
	BOOLEAN_ isCall(STATEMENT stmtNum);
	INTEGER getSize();

	TNode* getNodeForStmt(STATEMENT stmtNum);

private:
	unordered_map<int, string> stmtNumMap; //key is given by stmt#
	//unordered_map<string, int> stmtTypeMap; //key is given by stmtType
	vector<int> whileStmt;
	vector<int> assignStmt;
	vector<int> ifStmt;
	vector<int> callStmt;

	unordered_map<int, TNode*> nodeTable;
};