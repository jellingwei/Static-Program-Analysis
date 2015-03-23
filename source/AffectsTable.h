#pragma once

#include<stdio.h>
#include <iostream>
#include <string>
#include <vector>
#include <unordered_map>
#include "TNode.h"
#include "CNode.h"

using namespace std;
using namespace stdext;


class AffectsTable {
public:

	bool isAffects(int progLine1, int progLine2, bool transitiveClosure);
	
	vector<int> getProgLinesAffectedBy(int progLine1, bool transitiveClosure, bool terminateOnOneResult = false);
	vector<int> getProgLinesAffecting(int progLine2, bool transitiveClosure, bool terminateOnOneResult = false);

	pair<vector<int>, vector<int>> getAllAffectsPairs(bool transitiveClosure);

	
	vector<int> getLhs();
	vector<int> getRhs();

	static bool canSkipNodesBackwards(CNode* node);
	static bool canSkipNodesForwards(CNode* node);


};

