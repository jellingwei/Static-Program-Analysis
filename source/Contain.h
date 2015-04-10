#pragma once

#include <stdio.h>
#include <iostream>
#include <vector>
#include <string>
#include <utility>
#include <tuple>
#include "common.h"
#include "TNode.h"
#include "PKB.h"
//using std::string;
using namespace std;

class Contain
{
public:
	vector<pair<int, vector<int>>> contains(TNODE_TYPE predecessorType, TNODE_TYPE descendentType, bool transitiveClosure);

	//vector<pair<int, vector<int>>> containsConstVar(TNODE_TYPE parentType, TNODE_TYPE childType, bool transitiveClosure);

	vector<int> checkForWhileThen(TNode* currentNode, TNODE_TYPE descendentType, bool transitiveClosure);
	vector<int> checkForIfThenElse(TNode* currentNode, TNODE_TYPE descendentType, bool transitiveClosure);

private:
};