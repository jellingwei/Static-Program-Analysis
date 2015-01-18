#pragma once

#include<stdio.h>
#include <iostream>
#include <string>
#include <vector>
#include <unordered_map>
#include "TNode.h"

using namespace std;
using namespace stdext;


class FollowsTable {
public:
	bool setFollows(TNode* stmt1, TNode* stmtNum2);
	
	// Given stmtNum2, return stmtNum1 such that Follows(stmt1, stmt2) is satisfied
	vector<int> getStmtFollowedTo(int stmtNum2, bool transitiveClosure);

	// Given stmtNum1, return stmtNum2 such that Follows(stmt1, stmt2) is satisfied
	vector<int> getStmtFollowedFrom(int stmtNum1, bool transitiveClosure);

	bool isFollows(int stmtNum1, int stmtNum2, bool transitiveClosure);

	pair<vector<int>, vector<int>> getAllFollowsPairs(bool transitiveClosure);



};
