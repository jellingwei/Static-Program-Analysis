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
	vector<int> PatternMatchWhile(string LHS);
	vector<int> PatternMatchIf(string LHS);
private:
	
};