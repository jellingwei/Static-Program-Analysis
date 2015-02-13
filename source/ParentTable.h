#pragma once

#include<stdio.h>
#include <iostream>
#include <string>
#include <vector>
#include "TNode.h"

using namespace std;

class ParentTable {
public:
	bool setParent(TNode* stmtNum1, TNode* stmtNum2);
	
	vector<int> getParent(int stmtNum2, bool transitiveClosure);
	vector<int> getChild(int stmtNum1, bool transitiveClosure);

	bool isParent(int stmtNum1, int stmtNum2, bool transitiveClosure);

	pair<vector<int>, vector<int>> getAllParentPairs(bool transitiveClosure);
	vector<int> getAllChildren();
	vector<int> getAllParents();

};