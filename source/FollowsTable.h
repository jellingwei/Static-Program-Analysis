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


class FollowsTable {
private:
	vector<int> lhs;
	vector<int> rhs;

public:
	STATUS setFollows(TNode* stmt1, TNode* stmtNum2);
	
	// Given stmtNum2, return stmtNum1 such that Follows(stmt1, stmt2) is satisfied
	STATEMENT_LIST getStmtFollowedTo(STATEMENT stmtNum2, TRANS_CLOSURE transitiveClosure);

	// Given stmtNum1, return stmtNum2 such that Follows(stmt1, stmt2) is satisfied
	STATEMENT_LIST getStmtFollowedFrom(STATEMENT stmtNum1, TRANS_CLOSURE transitiveClosure);

	BOOLEAN_ isFollows(STATEMENT stmtNum1, STATEMENT stmtNum2, TRANS_CLOSURE transitiveClosure);

	pair<vector<int>, vector<int>> getAllFollowsPairs(TRANS_CLOSURE transitiveClosure);
	STATEMENT_LIST getLhs();
	STATEMENT_LIST getRhs();

};
