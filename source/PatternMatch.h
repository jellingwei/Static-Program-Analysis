#pragma once

#include <stdio.h>
#include <iostream>
#include <vector>
#include <string>
#include <utility>
#include <queue>


#include "TNode.h"
#include "PKB.h"
using std::string;
using namespace std;

class PatternMatch
{
public:
	PatternMatch();
	vector<int> PatternMatchAssign(TNode *queryASTroot, string isExact);
	//bool checkPatternMatchAssign(TNode *_rightChildNodeA, TNode *_rootNodeQ, string isExact);
	vector<int> PatternMatchWhile(string LHS);
private:
	
};