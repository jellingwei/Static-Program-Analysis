#pragma once

#include<stdio.h>
#include <iostream>
#include <string>
#include <vector>
#include <unordered_map>
#include "TNode.h"

using namespace std;
using namespace stdext;


class CallsTable {
public:
	bool setCalls(int procIndex1, int procIndex2);

	bool isCalls(int procIndex1, int procIndex2, bool transitiveClosure);
	
	// Given procIndex2, return procIndex1 such that Calls(procIndex1, procIndex2) is satisfied
	vector<int> getProcsCalling(int procIndex2, bool transitiveClosure);

	// Given procIndex1, return procIndex2 such that Calls(procIndex1, procIndex2) is satisfied
	vector<int> getProcsCalledBy(int procIndex1, bool transitiveClosure);

	pair<vector<int>, vector<int>> getAllCallsPairs(bool transitiveClosure);

private: 

	unordered_map<int, vector<int>> procIndex1Map; // key is procIndex2
	unordered_map<int, vector<int>> procIndex2Map; // key is procIndex1 

};
