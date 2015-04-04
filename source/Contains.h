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

class Contains
{
public:
	vector<pair<TNode*, vector<TNode*>>> contains(TNODE_TYPE predecessorType, TNODE_TYPE descendentType, bool transitiveClosure);
	
	/*vector<TNode*> checkForProcCase(TNode* currentNode, TNODE_TYPE descendentType, bool transitiveClosure);
	vector<TNode*> checkForWhileCase(TNode* currentNode, TNODE_TYPE descendentType, bool transitiveClosure);
	vector<TNode*> checkForIfCase(TNode* currentNode, TNODE_TYPE descendentType, bool transitiveClosure);
	vector<TNode*> checkForStmtLstCase(TNode* currentNode, TNODE_TYPE descendentType, bool transitiveClosure);
	
	vector<TNode*> checkForAssignCase(TNode* currentNode, TNODE_TYPE descendentType, bool transitiveClosure);
	
	vector<TNode*> checkForOtherCase(TNode* currentNode, TNODE_TYPE descendentType, bool transitiveClosure);*/

private:
};