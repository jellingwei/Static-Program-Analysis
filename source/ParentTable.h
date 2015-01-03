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
	
	vector<int> getParent(int stmtNum2);
	vector<int> getChild(int stmtNum1);

	bool isParent(int stmtNum1, int stmtNum2);

	pair<vector<int>, vector<int>> getAllParentPairs(bool transitiveClosure);

};