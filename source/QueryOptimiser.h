#pragma once

#include <cmath>
#include <stack>

#include "StatisticsTable.h"
#include "PKB.h"
#include "Synonym.h"
#include "QueryTree.h" 
#include "common.h"
#include "QNode.h"
#include "QueryEvaluator.h"

using std::stack;

namespace QueryOptimiser
{
	QueryTree* optimiseQueryTree(QueryTree* queryTree);
}
