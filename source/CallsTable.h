#pragma once

#include<stdio.h>
#include <iostream>
#include <string>
#include <vector>
#include <unordered_map>
#include "TNode.h"
#include "common.h"

using namespace std;
using namespace stdext;


class CallsTable {
public:
	STATUS setCalls(PROC_INDEX procIndex1, PROC_INDEX procIndex2);

	BOOLEAN_ isCalls(PROC_INDEX procIndex1, PROC_INDEX procIndex2, TRANS_CLOSURE transitiveClosure);
	
	// Given procIndex2, return procIndex1 such that Calls(procIndex1, procIndex2) is satisfied
	PROCINDEX_LIST getProcsCalling(PROC_INDEX procIndex2, TRANS_CLOSURE transitiveClosure, bool terminateOnFinding = false, int lineToFind = -1);

	// Given procIndex1, return procIndex2 such that Calls(procIndex1, procIndex2) is satisfied
	PROCINDEX_LIST getProcsCalledBy(PROC_INDEX procIndex1, TRANS_CLOSURE transitiveClosure, bool terminateOnFinding = false, int lineToFind= -1);

	pair<PROCINDEX_LIST, PROCINDEX_LIST> getAllCallsPairs(TRANS_CLOSURE transitiveClosure);

	PROCNAME getProcNameCalledByStatement(STATEMENT stmtNum);

	PROCINDEX_LIST getLhs();
	PROCINDEX_LIST getRhs();

	BOOLEAN_ isValid();

private: 

	unordered_map<int, vector<int>> procIndex1Map; // key is procIndex2
	unordered_map<int, vector<int>> procIndex2Map; // key is procIndex1 

};
