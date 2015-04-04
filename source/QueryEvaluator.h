#pragma once

#include <vector>
#include <set>
using std::set;
using std::vector;   //To store the results of the query
using std::pair;

#include "QueryTree.h"  //For the definition of the query tree nodes
#include "Synonym.h"

namespace QueryEvaluator 
{
	vector<Synonym> processQueryTree(QueryTree* queryTree);

	//Expose this function for use in the query optimiser
	inline void resetValues(unordered_map<string, SYNONYM_TYPE> name_to_type_map);
	inline BOOLEAN_ processClausesNode(QNode* clausesNode);
}
