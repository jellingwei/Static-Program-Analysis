#pragma once

#include <stdio.h>
#include <iostream>
#include <vector>
#include <string>
#include <utility>
#include <tuple>
#include "common.h"
#include "common_list.h"
#include "TNode.h"
#include "PKB.h"
//using std::string;
using namespace std;

/**
	@brief Refer to the PKB for the documentation.

*/
class Contain
{
public:
	PAIR_LIST contains(SYNONYM_TYPE predecessorType, SYNONYM_TYPE descendentType, TRANS_CLOSURE transitiveClosure);

	VALUE_LIST contains(STATEMENT stmtNO, SYNONYM_TYPE descendentType, TRANS_CLOSURE transitiveClosure);
	VALUE_LIST contains(SYNONYM_TYPE predecessorType, STATEMENT stmtNo, TRANS_CLOSURE transitiveClosure);

	VALUE_LIST checkForWhileThen(TNode* currentNode, SYNONYM_TYPE descendentType, TRANS_CLOSURE transitiveClosure);
	VALUE_LIST checkForIfThenElse(TNode* currentNode, SYNONYM_TYPE descendentType, TRANS_CLOSURE transitiveClosure);
	PAIR checkForIf(TNode* thenNode, SYNONYM_TYPE thenS, TNode* elseNode, SYNONYM_TYPE elseS);

private:
};