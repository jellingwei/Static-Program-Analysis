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
	STATEMENT_LIST patternMatchAssign(TNode *queryASTroot, bool isExact, string usedOperand);
	STATEMENT_LIST patternMatchParentStmt(EXPRESSION LHS, TNODE_TYPE type);
	~PatternMatch();

private:
};