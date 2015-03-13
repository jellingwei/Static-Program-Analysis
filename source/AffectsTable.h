#pragma once

#include<stdio.h>
#include <iostream>
#include <string>
#include <vector>
#include <unordered_map>
#include "TNode.h"

using namespace std;
using namespace stdext;


class AffectsTable {
public:

	bool isAffects(int progLine1, int progLine2, bool transitiveClosure);
	
	vector<int> getProgLinesAffectedBy(int progLine1, bool transitiveClosure);
	vector<int> getProgLinesAffecting(int progLine2, bool transitiveClosure);

	pair<vector<int>, vector<int>> getAllAffectsPairs(bool transitiveClosure);

	
	vector<int> getLhs();
	vector<int> getRhs();

	void setLhs(vector<int>);
	void setRhs(vector<int>);

private: 
	//@todo check with Jin if this is permitted
	vector<int> lhs;
	vector<int> rhs;

};

