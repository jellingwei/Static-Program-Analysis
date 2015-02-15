#pragma once

#include <stdio.h>
#include <iostream>
#include <vector>
#include <string>
#include <utility>


#include "TNode.h"
#include "PKB.h"
using std::string;
using namespace std;

class PatternMatch
{
public:
	PatternMatch();
	vector<int> PatternMatchAssign(TNode *queryASTroot, string isExact);
	vector<int> patternMatchParentStmt(string LHS, TNODE_TYPE type);
	~PatternMatch();

private:
	
};