#pragma once

#include <vector>
using std::vector;
#include <list>
using std::list;
#include <string>
using std::string;
using std::to_string;

#include "Synonym.h"
#include "ValuesHandler.h"

namespace ResultProjector {
	void projectResultToList(vector<Synonym> resultVector, list<string> & outputList);
	string convertValueToString(int value, SYNONYM_TYPE type, SYNONYM_ATTRIBUTE attribute);
	vector<string> expandEachRow(vector<string> values, unsigned int factor);
	vector<string> expandRange(vector<string> values, unsigned int factor);
}
