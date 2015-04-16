#pragma once

#include<stdio.h>
#include <iostream>
#include <string>
#include <vector>
#include <unordered_map>

#include <boost/dynamic_bitset.hpp>

#include "common.h"

using namespace std;

/**
	@brief Refer to the PKB for the documentation.
*/
class UsesTable {
public:
	
	void init(INTEGER numVariables);
	// for statement numbers
	STATUS setUses(STATEMENT stmtNum, VAR_INDEX varIndex);
	BOOLEAN_ isUses(STATEMENT stmtNum, VAR_INDEX varIndex);
	STATEMENT_LIST getUsesStmtNum(VAR_INDEX varIndex);
	VARINDEX_LIST getUsesVarForStmt(STATEMENT stmtNum);
	pair<STATEMENT_LIST, VARINDEX_LIST> getAllUsesPair();
	boost::dynamic_bitset<> getUseVarInBitvectorForStmt(STATEMENT stmtNum);
	STATEMENT_LIST getLhs();
	STATEMENT_LIST getRhs();

	BOOLEAN_ isValid();

	// for procedures 
	STATUS setUsesProc(PROC_INDEX procIndex, VAR_INDEX varIndex);
	BOOLEAN_ isUsesProc(PROC_INDEX procIndex, VAR_INDEX varIndex);
	PROCINDEX_LIST getUsesProcIndex(VAR_INDEX varIndex);
	VARINDEX_LIST getUsesVarForProc(PROC_INDEX procIndex);
	pair<PROCINDEX_LIST, VARINDEX_LIST> getAllUsesProcPair();

private:  
	unordered_map<int, boost::dynamic_bitset<>> varIndexMap; // key is stmtNum 
	unordered_map<int, vector<int>> stmtNumMap; // key is variable index

	unordered_map<int, vector<int>> procVarIndexMap; // key is procedure index 
	unordered_map<int, vector<int>> procIndexMap; // key is variable index

	int numVariables;
};
