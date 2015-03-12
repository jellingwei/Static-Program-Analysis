#pragma once

#include<stdio.h>
#include <iostream>
#include <string>
#include <vector>
#include <unordered_map>
#include <algorithm>
#include <queue>
#include <unordered_set>

#include "boost/dynamic_bitset.hpp"

#include "AffectsTable.h"
#include "PKB.h"


bool AffectsTable::isAffects(int progLine1, int progLine2, bool transitiveClosure) {
	return false;
}


CNode* getMandatoryNextNode(CNode* node) {
	CNODE_TYPE type = node->getNodeType();
	switch (type) {
	case (Assign_C): {


					 }
	case (Call_C) : {


					}
	case (While_C) : {

					 }
	case (If_C) : {


				  }

	}

	return NULL;

}



vector<int> AffectsTable::getProgLinesAffectedBy(int progLine1, bool transitiveClosure) {
	return vector<int>();
}


vector<int> AffectsTable::getProgLinesAffecting(int progLine2, bool transitiveClosure) {
	return vector<int>();
}



pair<vector<int>, vector<int>> AffectsTable::getAllAffectsPairs(bool transitiveClosure) {
	pair<vector<int>, vector<int>> results;

	return results;
}
