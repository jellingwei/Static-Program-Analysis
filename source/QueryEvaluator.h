#pragma once

#include <vector>
#include <set>
using std::set;
using std::vector;   //To store the results of the query
using std::pair;

#include "QueryTree.h"  //For the definition of the query tree nodes
#include "Synonym.h"

namespace QueryEvaluator {
	vector<Synonym> processQueryTree(QueryTree* queryTree);

	//Public helper functions
	set<int> intersectWithCurrentValues(vector<int> newValues, set<int> oldValues);
	bool isValueExist(set<int> setToSearch, int value);  //Returns true if the value exists in the set
}
