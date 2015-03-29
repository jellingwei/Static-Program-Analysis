#pragma once

#include<stdio.h>
#include <iostream>
#include <string>
#include <vector>
#include <unordered_map>

#include <boost/dynamic_bitset.hpp>

#include "common.h"

using namespace std;

class ModifiesTable {
public:
	void init(INTEGER numVariables);

	//for statement numbers
	STATUS setModifies(STATEMENT stmtNum, VAR_INDEX varIndex);
	BOOLEAN_ isModifies(STATEMENT stmtNum, VAR_INDEX varIndex);
	STATEMENT_LIST getModStmtNum(VAR_INDEX varIndex);
	VARINDEX_LIST getModVarForStmt(STATEMENT stmtNum);
	boost::dynamic_bitset<> getModVarInBitvectorForStmt(STATEMENT stmtNum);
	pair<STATEMENT_LIST, VARINDEX_LIST> getAllModPair();
	STATEMENT_LIST getLhs();
	STATEMENT_LIST getRhs();

	//for procedures
	STATUS setModifiesProc(PROC_INDEX procIndex, VAR_INDEX varIndex);
	BOOLEAN_ isModifiesProc(PROC_INDEX procIndex, VAR_INDEX varIndex);
	PROCINDEX_LIST getModProcIndex(VAR_INDEX varIndex);
	VARINDEX_LIST getModVarForProc(PROC_INDEX procIndex);
	pair<PROCINDEX_LIST, VARINDEX_LIST> getAllModProcPair();

private: 
	unordered_map<int, boost::dynamic_bitset<>> varIndexMap; // key is stmtNum 
	unordered_map<int, vector<int>> stmtNumMap; // key is variable index

	unordered_map<int, vector<int>> procVarIndexMap; // key is procedure index 
	unordered_map<int, vector<int>> procIndexMap; // key is variable index

	int numVariables;
};
