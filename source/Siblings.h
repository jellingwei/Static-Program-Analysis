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

using namespace std;

/**
	@brief Refer to the PKB for the documentation.

*/
class Siblings
{
public:
	PAIR_LIST siblings(SYNONYM_TYPE first_siblingType, SYNONYM_TYPE second_siblingType);

	VALUE_LIST siblings(STATEMENT stmtNo, SYNONYM_TYPE second_siblingType);
	VALUE_LIST siblings(SYNONYM_TYPE first_siblingType, STATEMENT stmtNo);
private:
};