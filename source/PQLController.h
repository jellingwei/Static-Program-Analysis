#pragma once

#include <list>

#include "QueryParser.h"
#include "QueryOptimiser.h"
#include "QueryEvaluator.h"
#include "ResultProjector.h"
#include "QueryTree.h"

class PQLController
{
public:
	static void evaluate(std::string query, std::list<std::string> &results);
};