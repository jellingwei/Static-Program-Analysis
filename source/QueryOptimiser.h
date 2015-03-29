#pragma once

#include "cmath"

#include "StatisticsTable.h"
#include "PKB.h"
#include "Synonym.h"
#include "QueryTree.h" 
#include "common.h"
#include "QNode.h"

namespace QueryOptimiser
{
	QueryTree* optimiseQueryTree(QueryTree* queryTree);
}
