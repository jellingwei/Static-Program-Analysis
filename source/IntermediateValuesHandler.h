#pragma once

#include <vector>
#include <set>
#include <unordered_map>
using std::unordered_map;
using std::set;
using std::vector;
using std::string;
using std::swap;

#include "Synonym.h"
#include "PKB.h"

namespace IntermediateValuesHandler {
	
	//Public functions
	void initialize(unordered_map<string, string> synonymMap);
	void addAndProcessIntermediateSynonym(Synonym synonym);
	void addAndProcessIntermediateSynonyms(Synonym LHS, Synonym RHS);
	void joinWithExistingValues(Synonym synonym);
	void joinWithExistingValues(Synonym LHS, Synonym RHS);
	void intersectWithExistingValues(int synonymIndex, vector<int> probeValues);
	void intersectWithExistingValues(int indexLHS, vector<int> LHSValues, int indexRHS, vector<int> RHSValues);
	void intersectAndJoinWithExistingValues(int existingIndex, Synonym probe, Synonym newValues);
	set<int> getIntermediateValues(int synonymIndex);
	Synonym getSynonymWithName(string wantedName);
	inline bool isValueExist(set<int> setToSearch, int value);
}
