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
#include "common.h"
#include "common_list.h"

namespace ValuesHandler 
{
	//Public functions to be used
	void initialize(unordered_map<string, SYNONYM_TYPE> synonymMap);
	BOOLEAN_ addAndProcessIntermediateSynonym(Synonym synonym);
	BOOLEAN_ addAndProcessIntermediateSynonyms(Synonym LHS, Synonym RHS);
	
	Synonym getSynonym(string wantedName);
	Synonym getSynonym(Synonym wantedSynonym);
	vector<Synonym> getSynonymTuples(vector<Synonym> wantedNames);
	BOOLEAN_ filterEqualValue(Synonym synonym, string wantedValue);
	BOOLEAN_ filterEqualPair(Synonym LHS, Synonym RHS);
	BOOLEAN_ isValueExistInSet(VALUE_SET setToSearch, VALUE value);
	VALUE_LIST getDefaultValues(SYNONYM_TYPE type);

	//Functions made public for testing purposes
	void clearAll();
	int findIndexInMainTable(SYNONYM_NAME synonymName);
	BOOLEAN_ isExistInMainTable(SYNONYM_NAME synonymName);
	BOOLEAN_ isExistInSingletonTable(SYNONYM_NAME synonymName);
	BOOLEAN_ addToSingletonTableForTesting(Synonym synonym);
	inline void removeFromSingletonTable(SYNONYM_NAME synonymName);
	VALUE_SET getIntermediateValuesSetInMain(SYNONYM_INDEX synonymIndex);
	VALUE_LIST getIntermediateValuesInMain(SYNONYM_INDEX synonymIndex);
	pair<VALUE_LIST, VALUE_LIST> getIntermediateValuesPair(SYNONYM_NAME synonym1, SYNONYM_NAME synonym2);

	BOOLEAN_ hashIntersectWithMainTable(Synonym synonym);
	BOOLEAN_ hashIntersectWithMainTable(Synonym LHS, Synonym RHS);
	BOOLEAN_ hashJoinWithMainTable(Synonym mainSynonym, Synonym pairedSynonym);
	BOOLEAN_ joinWithMainTable(Synonym LHS, Synonym RHS);

	pair<VALUE_LIST, VALUE_LIST> getPairBySingletonIntersect(Synonym singleton, Synonym singletonPair);
	pair<VALUE_LIST, VALUE_LIST> getPairBySingletonStringIntersect(Synonym LHS, Synonym RHS);
	BOOLEAN_ hashIntersectWithSingletonTable(Synonym synonym);

	BOOLEAN_ processPairWithMainTable(Synonym LHS, Synonym RHS);
	BOOLEAN_ processPairWithSingletonTable(Synonym LHS, Synonym RHS);
	BOOLEAN_ mergeSingletonToMain(Synonym mainSynonym, Synonym singleton);
}
