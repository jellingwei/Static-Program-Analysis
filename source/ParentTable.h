#pragma once

#include<stdio.h>
#include <iostream>
#include <string>
#include <vector>
#include "TNode.h"
#include "common.h"

using namespace std;

class ParentTable {
public:
	STATUS setParent(TNode* stmtNum1, TNode* stmtNum2);
	
	STATEMENT_LIST getParent(STATEMENT stmtNum2, TRANS_CLOSURE transitiveClosure);
	STATEMENT_LIST getChild(STATEMENT stmtNum1, TRANS_CLOSURE transitiveClosure);

	BOOLEAN_ isParent(STATEMENT stmtNum1, STATEMENT stmtNum2, TRANS_CLOSURE transitiveClosure);

	pair<STATEMENT_LIST, STATEMENT_LIST> getAllParentPairs(TRANS_CLOSURE transitiveClosure);
	STATEMENT_LIST getAllChildren();
	STATEMENT_LIST getAllParents();

};