#pragma once

#include <string>
#include <vector>
#include <assert.h>
#include "QueryTree.h"

using namespace std;

namespace QueryParser {
	BOOLEAN_ initParser(std::string);
	std::string parseToken();
	BOOLEAN_ parseQuery();
	QueryTree* getQueryTree();
}