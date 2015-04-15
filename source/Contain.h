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
	vector<pair<int, vector<int>>> contains(SYNONYM_TYPE predecessorType, SYNONYM_TYPE descendentType, bool transitiveClosure);

	//bool contains(int stmtNo, int stmtNo2, bool transitiveClosure);	
	vector<int> contains(int stmtNO, SYNONYM_TYPE descendentType, bool transitiveClosure);
	vector<int> contains(SYNONYM_TYPE predecessorType, int stmtNo, bool transitiveClosure);
	//vector<pair<int, vector<int>>> containsConstVar(TNODE_TYPE parentType, TNODE_TYPE childType, bool transitiveClosure);

	vector<int> checkForWhileThen(TNode* currentNode, SYNONYM_TYPE descendentType, bool transitiveClosure);
	vector<int> checkForIfThenElse(TNode* currentNode, SYNONYM_TYPE descendentType, bool transitiveClosure);
	pair<int, int> checkForIf(TNode* thenNode, SYNONYM_TYPE thenS, TNode* elseNode, SYNONYM_TYPE elseS);

private:
};