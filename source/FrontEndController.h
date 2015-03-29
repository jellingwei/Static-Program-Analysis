#pragma once

#include <list>

#include "QueryParser.h"
#include "QueryEvaluator.h"
#include "ResultProjector.h"
#include "QueryTree.h"

class FrontEndController
{
public:
	static void constructPkb(string filename);
};