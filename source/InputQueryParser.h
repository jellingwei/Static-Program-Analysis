#include <string>
#include <vector>
#include <assert.h>
#include "QueryTree.h"

namespace QueryParser {
	bool initParser(std::string);
	std::string parseToken();
	bool parseQuery();
	QueryTree* getQueryTree();
}