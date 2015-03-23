#pragma once

#include <vector>
#include <set>
#include <unordered_map>
using std::unordered_map;
using std::unordered_multimap;
using std::set;
using std::vector;
using std::pair;
using std::string;
using std::swap;

#include "Synonym.h"
#include "PKB.h"

#define MAX_SYNONYMS_ASSUMED 100

namespace ValuesHandler 
{
	//Public functions to be used
	void initialize(unordered_map<string, SYNONYM_TYPE> synonymMap);
	bool addAndProcessIntermediateSynonym(Synonym synonym);
	bool addAndProcessIntermediateSynonyms(Synonym LHS, Synonym RHS);
	
	Synonym getSynonym(string wantedName);
	vector<Synonym> getSynonymTuples(vector<string> wantedNames);
	bool filterEqualValue(Synonym synonym, string wantedValue);
	bool filterEqualPair(Synonym LHS, Synonym RHS);
	bool isValueExistInSet(set<int> setToSearch, int value);
	vector<int> getDefaultValues(SYNONYM_TYPE type);
	set<int> getIntermediateValuesSet(string wantedName);
	vector<int> getIntermediateValues(string wantedName);

	//Functions made public for testing purposes
	void clearAll();
	int findIndexInMainTable(string synonymName);
	bool isExistInMainTable(string synonymName);
	bool isExistInSingletonTable(string synonymName);
	bool addToSingletonTableForTesting(Synonym synonym);
	inline void removeFromSingletonTable(string synonymName);
	set<int> getIntermediateValuesSetInMain(int synonymIndex);
	vector<int> getIntermediateValuesInMain(int synonymIndex);

	bool hashIntersectWithMainTable(Synonym synonym);
	bool hashIntersectWithMainTable(Synonym LHS, Synonym RHS);
	bool hashJoinWithMainTable(Synonym mainSynonym, Synonym pairedSynonym);
	bool joinWithMainTable(Synonym LHS, Synonym RHS);

	pair<vector<int>, vector<int>> getPairBySingletonIntersect(Synonym singleton, Synonym singletonPair);
	pair<vector<int>, vector<int>> getPairBySingletonStringIntersect(Synonym LHS, Synonym RHS);
	bool hashIntersectWithSingletonTable(Synonym synonym);

	bool processPairWithMainTable(Synonym LHS, Synonym RHS);
	bool processPairWithSingletonTable(Synonym LHS, Synonym RHS);
	bool mergeSingletonToMain(Synonym mainSynonym, Synonym singleton);
}
