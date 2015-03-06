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

namespace IntermediateValuesHandler 
{
	//Public functions to be used
	void initialize(unordered_map<string, SYNONYM_TYPE> synonymMap);
	bool addAndProcessIntermediateSynonym(Synonym synonym);
	bool addAndProcessIntermediateSynonyms(Synonym LHS, Synonym RHS);
	Synonym getSynonymWithName(string wantedName);
	bool filterEqualValue(Synonym synonym, string wantedValue);
	bool filterEqualPair(Synonym LHS, Synonym RHS);
	inline bool isValueExist(set<int> setToSearch, int value);
	vector<int> getDefaultValues(SYNONYM_TYPE type);

	//Functions made public for testing purposes
	void clear();
	void joinWithExistingValues(Synonym synonym);
	void joinWithExistingValues(Synonym LHS, Synonym RHS);
	void intersectWithExistingValues(int synonymIndex, vector<int> probeValues);
	void intersectWithExistingValues(int indexLHS, vector<int> LHSValues, int indexRHS, vector<int> RHSValues);
	void intersectAndJoinWithExistingValues(int existingIndex, Synonym probe, Synonym newValues);
	int findIntermediateSynonymIndex(string synonymName);
	set<int> getIntermediateValuesSet(int synonymIndex);
	vector<int> getIntermediateValues(int synonymIndex);
}
