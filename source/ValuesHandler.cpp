#include "ValuesHandler.h"

struct IntegerPair
{
	int value1;
	int value2;

	bool operator==(const IntegerPair &other) const
	{ 
		return (value1 == other.value1 && value2 == other.value2);
	}
};

struct Pair_Hasher
{
public:
	std::size_t operator() ( const IntegerPair &p ) const
	{
		using std::size_t;
		return p.value1 * MAX_SYNONYMS_ASSUMED + p.value2;
	}

	bool operator() ( const IntegerPair &a, const IntegerPair &b) const
	{
		return (a.value1 * MAX_SYNONYMS_ASSUMED + a.value2) == (b.value1 * MAX_SYNONYMS_ASSUMED + b.value2);
	}
};

/**
	@brief Namespace containing functions for the handling of intermediate values.

 */
namespace ValuesHandler 
{
	//Private attributes
	PKB pkb = PKB::getInstance();  //Used to get the default values of synonyms from the PKB
	unordered_map<string, SYNONYM_TYPE> synonymMap;  //Maps from the name to the synonym type
	
	//This table should only store 2 or more synonyms
	vector<vector<int>> mainTable;  //[i][j] where i denotes the column number and j denotes the row number
	unordered_map<string, int> mainTableIndex;  //Maps from synonym name to main table column number
	
	//This table is used to store singleton synonyms with no dependent synonyms
	unordered_map<string, vector<int>> singletonTable;  //Maps the singleton synonym name to its values
	
	//Private functions
	inline bool addToSingletonTable(Synonym synonym);
	string convertIndexToString(int index, SYNONYM_TYPE type);
	bool filterEqualValueByString(string synonymName, string wantedValue);
	bool filterEqualValueByNumber(string synonymName, int wantedValue);
	bool filterMainTableByValue(string synonymName, int wantedValue);
	bool filterMainTableByString(string synonymName, string wantedValue);
	bool filterSingletonTableByValue(string synonymName, int wantedValue);
	bool filterSingletonTableByString(string synonymName, string wantedValue);

	bool filterEqualPairByNumber(Synonym LHS, Synonym RHS);
	bool filterEqualNumberInMain(Synonym LHS, Synonym RHS);
	bool filterEqualPairByString(Synonym LHS, Synonym RHS);
	bool filterEqualStringInMain(Synonym LHS, Synonym RHS);

	/**
	* Initialize the handler for intermediate values.
	* @param synonymsMap which maps from the synonym name to the synonym type (e.g. stmt)
	*/
	void initialize(unordered_map<string, SYNONYM_TYPE> synonymsMap) 
	{
		clearAll();
		synonymMap = synonymsMap;
	}

	/**
	* Initialize the handler for intermediate values.
	*/
	inline void clearAll()
	{
		synonymMap.clear();
		mainTable.clear();
		mainTableIndex.clear();
		singletonTable.clear();
	}

	/**
	* Find the column number in which this synonym is stored in the main table
	* Made public for testing purposes. 
	* @param synonymName the name of the synonym
	* @return the column number if the synonym exists in the table
	*         -1 if the synonym does not exist in the table
	*/
	int findIndexInMainTable(string synonymName)
	{
		auto itr = mainTableIndex.find(synonymName);

		if (itr == mainTableIndex.end()) {
			return -1;
		} else {
			return itr->second;
		}
	}
	
	
	bool isExistInMainTable(string synonymName)
	{
		auto itr = mainTableIndex.find(synonymName);
		
		if (itr == mainTableIndex.end()) {
			return false;
		} else {
			return true;
		}
	}
	
	bool isExistInSingletonTable(string synonymName)
	{
		auto itr = singletonTable.find(synonymName);
		
		if (itr == singletonTable.end()) {
			return false;
		} else {
			return true;
		}
	}
	

	/**
	* Get a list of all the values depending on the SYNONYM_TYPE which can be VARIABLE/CONSTANT/PROCEDURE/STATEMENT.
	* @param type the type of synonym which can be VARIABLE/CONSTANT/PROCEDURE/STATEMENT
	* @return the list of values of the SYNONYM_TYPE, type.
	*/
	vector<int> getDefaultValues(SYNONYM_TYPE type) 
	{
		if (type == VARIABLE) {
			return pkb.getAllVarIndex();
		} else if (type == CONSTANT) {
			return pkb.getAllConstant();
		} else if (type == PROCEDURE) {
			return pkb.getAllProcIndex();
		} else {
			return pkb.getStmtNumForType(type);
		}
	}
	
	/**
	* Helper method to check if a particular value is contained in the set
	* @param setToSearch a given set to search
	* @param value the value that is to be searched for
	* @return TRUE if the value exists in the set given
	*         FALSE if the value does not exist in the set given
	*/
	bool isValueExistInSet(set<int> setToSearch, int value) 
	{
		int count = setToSearch.count(value);

		if (count >= 1) {
			return true;
		}
		return false;
	}
	
	/**
	* Helper method to convert an index to it's actual name
	* e.g. converting index 1 of procedure type yields "ProcOne"
	* @param index the variable or procedure index
	* @param type the type of synonym (e.g. procedure)
	* @return The string representation of the index
	*/
	string convertIndexToString(int index, SYNONYM_TYPE type)
	{
		switch (type) {
		case VARIABLE:
			return pkb.getVarName(index);
		case PROCEDURE:
			return pkb.getProcName(index);
		case CALL:
			return pkb.getProcNameCalledByStatement(index);
		default:
			return to_string(static_cast<long long>(index));
		}
	}
	
	inline bool addToSingletonTable(Synonym synonym)
	{
		vector<int> defaultValues = getDefaultValues(synonym.getType());
		singletonTable[synonym.getName()] = defaultValues;
		return hashIntersectWithSingletonTable(synonym);
	}

	bool addToSingletonTableForTesting(Synonym synonym)
	{
		singletonTable[synonym.getName()] = synonym.getValues();
		return true;
	}

	inline void removeFromSingletonTable(string synonymName)
	{
		if (isExistInSingletonTable(synonymName)) {
			singletonTable.erase(synonymName);
		}
	}
	
	/**
	* Helper method to get the synonym with its final values
	* Gets the default values if this synonym is not in the intermediate values
	* Otherwise, performs gets the existing intermediate values
	* @param wantedName the name of the wanted synonym
	* @return Synonym object with the type, name and values
	*/
	Synonym getSynonym(string wantedName) 
	{
		SYNONYM_TYPE type = synonymMap[wantedName];

		if (isExistInMainTable(wantedName)) {
			int index = findIndexInMainTable(wantedName);
			set<int> valuesSet = getIntermediateValuesSetInMain(index);
			Synonym synonym(type, wantedName, valuesSet);
			return synonym;
		} else if (isExistInSingletonTable(wantedName)) {
			vector<int> values = singletonTable[wantedName];
			Synonym synonym(type, wantedName, values);
			return synonym;
		} else {
			vector<int> values = getDefaultValues(type);
			Synonym synonym(type, wantedName, values);
			return synonym;
		}
	}
	
	/**
	* Helper method to get all intermediate values of the column
	* Might contain duplicates
	* Made public for testing
	* @param synonymIndex the index of the column
	* @return A vector containing all the intermediate values so far
	*/
	vector<int> getIntermediateValuesInMain(int synonymIndex) 
	{
		vector<int> values;
		for (unsigned int i = 0; i < mainTable.size(); i++) {
			values.push_back(mainTable[i][synonymIndex]);
		}
		return values;
	}

	/**
	* Helper method to get all intermediate values of the column
	* There are no duplicates contain duplicates
	* Made public for testing
	* @param synonymIndex the index of the column
	* @return A set containing all the intermediate values with no duplicates
	*/
	set<int> getIntermediateValuesSetInMain(int synonymIndex) 
	{
		set<int> values;
		for (unsigned int i = 0; i < mainTable.size(); i++) {
			values.insert(mainTable[i][synonymIndex]);
		}
		return values;
	}

	/**
	* Check if the adding and processing of intermediate synonym is successful.
	* @param synonym
	* @return TRUE if the processing of intermediate values is successful.
	*		  FALSE if the processing of intermediate values causes the table to have zero rows.
	*/
	bool addAndProcessIntermediateSynonym(Synonym synonym) 
	{
		if (synonym.getValues().size() == 0) {
			return false;
		}

		if (synonym.getType() == UNDEFINED) {
			return true;  //Can return true because there are values in the synonym
		}
		
		string name = synonym.getName();
		if (isExistInMainTable(name)) {
			return hashIntersectWithMainTable(synonym);
		} else if (isExistInSingletonTable(name)) {
			return hashIntersectWithSingletonTable(synonym);
		} else {
			return addToSingletonTable(synonym);
		}
	}

	/**
	* Check if the adding and processing of intermediate synonyms is successful.
	* @param LHS
	* @param RHS
	* @return TRUE if the processing of intermediate values is successful.
	*		  FALSE if the processing of intermediate values causes the table to have zero rows.
	*/
	bool addAndProcessIntermediateSynonyms(Synonym LHS, Synonym RHS) 
	{
		//@TODO: Assert that both sizes are the same
		if (LHS.getValues().size() == 0) {
			return false;
		} 

		string nameLHS = LHS.getName();
		string nameRHS = RHS.getName();
		
		if (nameLHS == nameRHS) {
			vector<int> valuesLHS = LHS.getValues();
			vector<int> valuesRHS = RHS.getValues();
			vector<int> acceptedValues;
			
			//Can take any side for the looping
			for (unsigned int i = 0; i < valuesLHS.size(); i++) {
				if (valuesLHS[i] == valuesRHS[i]) {
					acceptedValues.push_back(valuesLHS[i]);
				}
			}
			LHS.setValues(acceptedValues);  //Can take any side
			return addAndProcessIntermediateSynonym(LHS);
		} else if (LHS.getType() == UNDEFINED && RHS.getType() == UNDEFINED) {
			return true;  //Can return true because there are values in the synonyms
		} else if (LHS.getType() == UNDEFINED) {
			return addAndProcessIntermediateSynonym(RHS);
		} else if (RHS.getType() == UNDEFINED) {
			return addAndProcessIntermediateSynonym(LHS);
		}

		//If it reaches here, it is two proper synonyms that require handling
		if (isExistInMainTable(nameLHS) || isExistInMainTable(nameRHS)) {
			return processPairWithMainTable(LHS, RHS);
		} else if (isExistInSingletonTable(nameLHS) || isExistInSingletonTable(nameRHS)){
			return processPairWithSingletonTable(LHS, RHS);
		} else {
			//The two synonyms does not exist in the main table or the singleton table
			return joinWithMainTable(LHS, RHS);
		}
	}
	
	bool hashIntersectWithMainTable(Synonym synonym)
	{
		int index = findIndexInMainTable(synonym.getName());
		vector<int> newValues = synonym.getValues();
		unordered_map<int, int> hashTable;
		vector<vector<int>> acceptedValues;
		
		//Hash the rows in the new values into the hash table
		for (unsigned int i = 0; i < newValues.size(); i++) {
			hashTable[newValues[i]] = 1;
		}
		
		//Using the main table as the probe, check whether the value exists in the hash table
		//Keep this row if it exists
		for (unsigned int i = 0; i < mainTable.size(); i++) {
			if (hashTable.count(mainTable[i][index]) != 0) {
				acceptedValues.push_back(mainTable[i]);
			}
		}
		swap(mainTable, acceptedValues);
		return mainTable.size() != 0;
	}
	
	bool hashIntersectWithMainTable(Synonym LHS, Synonym RHS)
	{
		int indexLHS = findIndexInMainTable(LHS.getName());
		int indexRHS = findIndexInMainTable(RHS.getName());
		vector<int> newValuesLHS = LHS.getValues();
		vector<int> newValuesRHS = RHS.getValues();
		unordered_map<IntegerPair , int, Pair_Hasher> hashTable;
		vector<vector<int>> acceptedValues;
		
		//Hash the rows in the new values into the hash table
		for (unsigned int i = 0; i < newValuesLHS.size(); i++) {
			IntegerPair valuesPair;
			valuesPair.value1 = newValuesLHS[i];
			valuesPair.value2 = newValuesRHS[i];
			hashTable[valuesPair] = 1;
		}
		
		//Using the main table as the probe, check whether the values exists in the hash table
		//Keep this row if it exists
		for (unsigned int i = 0; i < mainTable.size(); i++) {
			IntegerPair valuesPair;
			valuesPair.value1 = mainTable[i][indexLHS];
			valuesPair.value2 = mainTable[i][indexRHS];
			if (hashTable.count(valuesPair) != 0) {
				acceptedValues.push_back(mainTable[i]);
			}
		}		
		swap(mainTable, acceptedValues);
		return mainTable.size() != 0;
	}
	
	bool hashJoinWithMainTable(Synonym mainSynonym, Synonym pairedSynonym)
	{
		int mainIndex = findIndexInMainTable(mainSynonym.getName());
		vector<int> newValues = mainSynonym.getValues();
		vector<int> newValuesPair = pairedSynonym.getValues();
		unordered_multimap<int, int> hashTable;
		vector<vector<int>> acceptedValues;
		
		for (unsigned int i = 0; i < newValues.size(); i++) {
			hashTable.emplace(make_pair(newValues[i], newValuesPair[i]));
		}
		
		for (unsigned int i = 0; i < mainTable.size(); i++) {
			vector<int> oneRow = mainTable[i];
			int value = oneRow[mainIndex];
			auto range = hashTable.equal_range(value);
			
			for (auto itr = range.first; itr != range.second; ++itr) {
				int joinValue = itr->second;
				vector<int> newRow(oneRow);
				newRow.push_back(joinValue);
				acceptedValues.push_back(newRow);
			}
		}
		swap(mainTable, acceptedValues);
		if (mainTable.size() == 0) {
			return false;
		} else {
			mainTableIndex[pairedSynonym.getName()] = mainTable[0].size() - 1;
			return true;
		}
	}
	
	bool joinWithMainTable(Synonym LHS, Synonym RHS)
	{
		vector<int> valuesLHS = LHS.getValues();
		vector<int> valuesRHS = RHS.getValues();
		vector<vector<int>> acceptedValues;
		int mainTableSize = mainTable.size();

		if (mainTableSize == 0) {
			for (unsigned int i = 0; i < valuesLHS.size(); i++) {
				vector<int> values;
				values.push_back(valuesLHS[i]);
				values.push_back(valuesRHS[i]);
				acceptedValues.push_back(values);
			}
		} else {
			for (int i = 0; i < mainTableSize; i++) {
				for (unsigned int j = 0; j < valuesLHS.size(); j++) {
					vector<int> values = mainTable[i];
					values.push_back(valuesLHS[j]);
					values.push_back(valuesRHS[j]);
					acceptedValues.push_back(values);
				}
			}
		}
		swap(mainTable, acceptedValues);
		if (mainTable.size() == 0) {
			return false;
		} else {
			//Since there will always be values inside, can directly access row 0
			mainTableIndex[LHS.getName()] = mainTable[0].size() - 2;
			mainTableIndex[RHS.getName()] = mainTable[0].size() - 1;
			return true;
		}
	}

	pair<vector<int>, vector<int>> getPairBySingletonIntersect(Synonym singleton, Synonym singletonPair)
	{
		vector<int> existingValues = singletonTable[singleton.getName()];
		vector<int> newValues = singleton.getValues();
		vector<int> newValuesPair = singletonPair.getValues();
		unordered_map<int, int> hashTable;
		vector<int> acceptedValues;
		vector<int> acceptedValuesPair;

		for (unsigned int i = 0; i < existingValues.size(); i++) {
			hashTable[existingValues[i]] = 1;
		}
		
		for (unsigned int i = 0; i < newValues.size(); i++) {
			int singletonValue = newValues[i];
			if (hashTable.count(singletonValue) != 0) {
				acceptedValues.push_back(singletonValue);
				acceptedValuesPair.push_back(newValuesPair[i]);
			}
		}
		return make_pair(acceptedValues, acceptedValuesPair);
	}

	pair<vector<int>, vector<int>> getPairBySingletonStringIntersect(Synonym LHS, Synonym RHS)
	{
		//TODO: Assert that both are either varName or procName and exists in the singleton table
		SYNONYM_TYPE typeLHS = LHS.getType();
		SYNONYM_TYPE typeRHS = RHS.getType();
		vector<int> existingLHS = singletonTable[LHS.getName()];
		vector<int> existingRHS = singletonTable[RHS.getName()];
		unordered_map<string, int> hashTable;
		unordered_map<string, int> hashLHS;
		vector<int> acceptedLHS;
		vector<int> acceptedRHS;

		for (unsigned int i = 0; i < existingLHS.size(); i++) {
			string existingString = convertIndexToString(existingLHS[i], typeLHS);
			hashTable[existingString] = 1;
			hashLHS[existingString] = existingLHS[i];
		}

		for (unsigned int i = 0; i < existingRHS.size(); i++) {
			string existingString = convertIndexToString(existingRHS[i], typeRHS);
			if (hashTable.count(existingString) != 0) {
				acceptedLHS.push_back(hashLHS[existingString]);
				acceptedRHS.push_back(existingRHS[i]);
			}
		}
		return make_pair(acceptedLHS, acceptedRHS);
	}
	
	bool hashIntersectWithSingletonTable(Synonym synonym)
	{
		string name = synonym.getName();
		vector<int> existingValues = singletonTable[name];
		vector<int> newValues = synonym.getValues();
		unordered_map<int, int> hashTable;
		vector<int> acceptedValues;
		
		//Hash the rows in the new values into the hash table
		for (unsigned int i = 0; i < newValues.size(); i++) {
			hashTable[newValues[i]] = 1;
		}
		
		for (unsigned int i = 0; i < existingValues.size(); i++) {
			if (hashTable.count(existingValues[i]) != 0) {
				acceptedValues.push_back(existingValues[i]);
			}
		}
		singletonTable[name] = acceptedValues;
		return acceptedValues.size() != 0;
	}
	
	bool processPairWithMainTable(Synonym LHS, Synonym RHS)
	{
		//Either LHS or RHS is in the main table or both
		string nameLHS = LHS.getName();
		string nameRHS = RHS.getName();
		bool isLHSInMainTable = isExistInMainTable(nameLHS);
		bool isRHSInMainTable = isExistInMainTable(nameRHS);
		
		if (isLHSInMainTable && isRHSInMainTable) {
			return hashIntersectWithMainTable(LHS, RHS);  //Both synonyms exists in the main table
		} else if (isLHSInMainTable) {
			//Only the LHS synonym exists in the main table
			if (isExistInSingletonTable(nameRHS)) {
				//RHS exists in the singleton table
				return mergeSingletonToMain(LHS, RHS);  //Merge RHS into the main table using LHS
			} else {
				return hashJoinWithMainTable(LHS, RHS);  //RHS does not exist
			}
		} else {
			//If it comes here, only the RHS synonym exists in the main table
			if (isExistInSingletonTable(nameLHS)) {
				//LHS exists in the singleton table
				return mergeSingletonToMain(RHS, LHS);  //Merge LHS into the main table using RHS
			} else {
				return hashJoinWithMainTable(RHS, LHS);  //LHS does not exist
			}
		}
	}
	
	bool processPairWithSingletonTable(Synonym LHS, Synonym RHS)
	{
		//Either LHS or RHS is in the singleton table or both
		string nameLHS = LHS.getName();
		string nameRHS = RHS.getName();
		bool isLHSInSingletonTable = isExistInSingletonTable(nameLHS);
		bool isRHSInSingletonTable = isExistInSingletonTable(nameRHS);
		
		if (isLHSInSingletonTable && isRHSInSingletonTable) {
			//Both synonyms exists in the singleton table
			pair<vector<int>, vector<int>> synonymPair = getPairBySingletonIntersect(LHS, RHS);
			LHS.setValues(synonymPair.first);
			RHS.setValues(synonymPair.second);
			
			synonymPair = getPairBySingletonIntersect(RHS, LHS);
			RHS.setValues(synonymPair.first);
			LHS.setValues(synonymPair.second);
			
			removeFromSingletonTable(LHS.getName());
			removeFromSingletonTable(RHS.getName());
			return joinWithMainTable(LHS, RHS);
		} else if (isLHSInSingletonTable) {
			//Only the LHS synonym exists in the singleton table
			pair<vector<int>, vector<int>> synonymPair = getPairBySingletonIntersect(LHS, RHS);
			removeFromSingletonTable(LHS.getName());
			LHS.setValues(synonymPair.first);
			RHS.setValues(synonymPair.second);
			return joinWithMainTable(LHS, RHS);
		} else {
			//If it comes here, only the RHS synonym exists in the singleton table
			pair<vector<int>, vector<int>> synonymPair = getPairBySingletonIntersect(RHS, LHS);
			removeFromSingletonTable(RHS.getName());
			RHS.setValues(synonymPair.first);
			LHS.setValues(synonymPair.second);
			return joinWithMainTable(LHS, RHS);
		}
	}
	
	bool mergeSingletonToMain(Synonym mainSynonym, Synonym singleton)
	{
		pair<vector<int>, vector<int>> synonymPair = getPairBySingletonIntersect(singleton, mainSynonym);
		singleton.setValues(synonymPair.first);
		mainSynonym.setValues(synonymPair.second);
		removeFromSingletonTable(singleton.getName());
		return hashJoinWithMainTable(mainSynonym, singleton);
	}
	
	/**
	* Helper method to iterate through the main table and filter rows containing only that value
	* For "with" clauses that are comparing synonyms with constants
	* @param synonym the synonym that is to be filtered
	* @param wantedValue the value that is to be filtered
	* @return TRUE if there are rows left in the table after filtering
	*         FALSE if there are no rows left in the table after filtering
	*/
	bool filterEqualValue(Synonym synonym, string wantedValue)
	{
		if (synonym.getAttribute() == procName || synonym.getAttribute() == varName) {
			return filterEqualValueByString(synonym.getName(), wantedValue);
		} else {
			return filterEqualValueByNumber(synonym.getName(), stoi(wantedValue));
		}
	}

	bool filterEqualValueByString(string synonymName, string wantedValue)
	{
		SYNONYM_TYPE type = synonymMap[synonymName];

		if (isExistInMainTable(synonymName)) {
			return filterMainTableByString(synonymName, wantedValue);
		} else if (isExistInSingletonTable(synonymName)) {
			return filterSingletonTableByString(synonymName, wantedValue);
		} else {
			//This synonym does not exist in both tables
			vector<int> values = getDefaultValues(type);
			for (unsigned int i = 0; i < values.size(); i++) {
				int valueIndex = values[i];
				string valueString = convertIndexToString(valueIndex, type);
				if (valueString == wantedValue) {
					vector<int> finalValue;
					finalValue.push_back(valueIndex);
					singletonTable[synonymName] = finalValue;  //Insert into singleton table
					return true;
				}
			}
			return false;
		}
	}

	bool filterEqualValueByNumber(string synonymName, int wantedValue)
	{
		if (isExistInMainTable(synonymName)) {
			return filterMainTableByValue(synonymName, wantedValue);
		} else if (isExistInSingletonTable(synonymName)) {
			return filterSingletonTableByValue(synonymName, wantedValue);
		} else {
			//This synonym does not exist in both tables
			SYNONYM_TYPE type = synonymMap[synonymName];
			vector<int> values = getDefaultValues(type);
			for (unsigned int i = 0; i < values.size(); i++) {
				if (values[i] == wantedValue) {
					vector<int> finalValue;
					finalValue.push_back(wantedValue);
					singletonTable[synonymName] = finalValue;  //Insert into singleton table
					return true;
				}
			}
			return false;
		}
	}

	bool filterMainTableByValue(string synonymName, int wantedValue)
	{
		int index = findIndexInMainTable(synonymName);
		vector<vector<int>> acceptedValues;
	
		for (unsigned int i = 0; i < mainTable.size(); i++) {
			vector<int> oneRow = mainTable[i];
			if (oneRow[index] == wantedValue) {
				acceptedValues.push_back(oneRow);
			}
		}
		swap(mainTable, acceptedValues);
		return mainTable.size() != 0;
	}

	bool filterMainTableByString(string synonymName, string wantedValue)
	{
		int index = findIndexInMainTable(synonymName);
		SYNONYM_TYPE type = synonymMap[synonymName];
		vector<vector<int>> acceptedValues;
		
		for (unsigned int i = 0; i < mainTable.size(); i++) {
			vector<int> oneRow = mainTable[i];
			string valueString = convertIndexToString(oneRow[index], type);
			if (valueString == wantedValue) {
				acceptedValues.push_back(oneRow);
			}
		}
		swap(mainTable, acceptedValues);
		return mainTable.size() != 0;
	}

	bool filterSingletonTableByValue(string synonymName, int wantedValue)
	{
		vector<int> singletonValues = singletonTable[synonymName];
		vector<int> acceptedValue;
		
		for (unsigned int i = 0; i < singletonValues.size(); i++) {
			if (singletonValues[i] == wantedValue) {
				acceptedValue.push_back(wantedValue);
				singletonTable[synonymName] = acceptedValue;
				return true;
			}
		}
		return false;
	}

	bool filterSingletonTableByString(string synonymName, string wantedValue)
	{
		vector<int> singletonValues = singletonTable[synonymName];
		vector<int> acceptedValue;
		SYNONYM_TYPE type = synonymMap[synonymName];
		
		for (unsigned int i = 0; i < singletonValues.size(); i++) {
			int valueIndex = singletonValues[i];
			string valueString = convertIndexToString(valueIndex, type);
			if (valueString == wantedValue) {
				acceptedValue.push_back(valueIndex);
				singletonTable[synonymName] = acceptedValue;
				return true;
			}
		}
		return false;
	}

	/**
	* Helper method to iterate through the main table and filter rows having equal values across both columns
	* For "with" clauses that are comparing synonyms with synonyms
	* @param LHS
	* @param RHS
	* @return TRUE if there are rows left after filtering
	*         FALSE if there are no rows left after filtering
	*/
	bool filterEqualPair(Synonym LHS, Synonym RHS)
	{
		SYNONYM_TYPE arg1Type = LHS.getType();
		SYNONYM_TYPE arg2Type = RHS.getType();

		if ((LHS.getAttribute() == procName || LHS.getAttribute() == varName) 
			&& (RHS.getAttribute() == varName || RHS.getAttribute() == procName)) {
				return filterEqualPairByString(LHS, RHS);
		} else if ((LHS.getAttribute() == stmtNo || LHS.getAttribute() == value) 
			&& (RHS.getAttribute() == stmtNo || RHS.getAttribute() == value)) {
				return filterEqualPairByNumber(LHS, RHS);
		} else {
			return false;  //Cannot compare between numbers and strings
		}
	}

	/**
	* Helper method to iterate through the main table and filter rows having equal numbers across both columns
	* For "with" clauses that are comparing number attributes such as stmtNo
	* @param LHS
	* @param RHS
	* @return TRUE if there are rows left after filtering
	*         FALSE if there are no rows left after filtering
	*/
	bool filterEqualPairByNumber(Synonym LHS, Synonym RHS)
	{
		string nameLHS = LHS.getName();
		string nameRHS = RHS.getName();
		bool isLHSInMainTable = isExistInMainTable(nameLHS);
		bool isRHSInMainTable = isExistInMainTable(nameRHS);
		bool isLHSInSingletonTable = isExistInSingletonTable(nameLHS);
		bool isRHSInSingletonTable = isExistInSingletonTable(nameRHS);

		if (isLHSInMainTable && isRHSInMainTable) {
			return filterEqualNumberInMain(LHS, RHS);
		} else if (isLHSInMainTable) {
			//Only the LHS synonym exists in the main table
			RHS = getSynonym(nameRHS);
			removeFromSingletonTable(nameRHS);

			if (hashJoinWithMainTable(LHS, RHS)) {
				return filterEqualNumberInMain(LHS, RHS);
			} else {
				return false;
			}
		} else if (isRHSInMainTable) {
			//Only the RHS synonym exists in the main table
			LHS = getSynonym(nameLHS);
			removeFromSingletonTable(nameLHS);

			if (hashJoinWithMainTable(RHS, LHS)) {
				return filterEqualNumberInMain(RHS, LHS);
			} else {
				return false;
			}
		} else {
			//Both synonyms do not exist in the main table
			if (isLHSInSingletonTable && isRHSInSingletonTable) {
				//If both synonyms exist in the singleton table
				LHS = getSynonym(nameLHS);  //Assign LHS to the current values
				RHS.setValues(LHS.getValues());  //Assign RHS to the same values as LHS

				//Get the reduced pair by intersecting with RHS
				pair<vector<int>, vector<int>> pair = getPairBySingletonIntersect(RHS, LHS);
				RHS.setValues(pair.first);
				LHS.setValues(pair.second);
				removeFromSingletonTable(nameLHS);
				removeFromSingletonTable(nameRHS);
				return joinWithMainTable(RHS, LHS);
			} else if (isLHSInSingletonTable) {
				//Only LHS exists in the singleton table
				RHS = getSynonym(nameRHS);  //Assign RHS to its default values
				LHS.setValues(RHS.getValues());  //Assign LHS to the RHS values

				//Get the reduced pair by intersecting with LHS
				pair<vector<int>, vector<int>> pair = getPairBySingletonIntersect(LHS, RHS);
				LHS.setValues(pair.first);
				RHS.setValues(pair.second);
				removeFromSingletonTable(nameLHS);
				return joinWithMainTable(LHS, RHS);
			} else if (isRHSInSingletonTable) {
				//Only RHS exists in the singleton table
				LHS = getSynonym(nameLHS);  //Assign LHS to its default values
				RHS.setValues(LHS.getValues());  //Assign RHS to the LHS values

				//Get the reduced pair by intersecting with LHS
				pair<vector<int>, vector<int>> pair = getPairBySingletonIntersect(RHS, LHS);
				RHS.setValues(pair.first);
				LHS.setValues(pair.second);
				removeFromSingletonTable(nameRHS);
				return joinWithMainTable(RHS, LHS);
			} else {
				//These two synonyms do not exist at all
				LHS = getSynonym(nameLHS);  //Assign LHS to its default values
				addToSingletonTable(LHS);

				RHS = getSynonym(nameRHS);  //Assigh RHS to its default values
				LHS.setValues(RHS.getValues());  //Assign LHS to the values of RHS

				pair<vector<int>, vector<int>> pair = getPairBySingletonIntersect(LHS, RHS);
				LHS.setValues(pair.first);
				RHS.setValues(pair.second);
				removeFromSingletonTable(nameLHS);
				return joinWithMainTable(LHS, RHS);
			}
		}
	}

	bool filterEqualNumberInMain(Synonym LHS, Synonym RHS)
	{
		//TODO: Assert that they both are in main
		int indexLHS = findIndexInMainTable(LHS.getName());
		int indexRHS = findIndexInMainTable(RHS.getName());
		vector<vector<int>> acceptedValues;

		for (unsigned int i = 0; i < mainTable.size(); i++) {
			vector<int> oneRow = mainTable[i];
			if (oneRow[indexLHS] == oneRow[indexRHS]) {
				acceptedValues.push_back(oneRow);
			}
		}
		swap(acceptedValues, mainTable);
		return mainTable.size() != 0;
	}

	/**
	* Helper method to iterate through the main table and filter rows having equal numbers across both columns
	* For "with" clauses that are comparing string attributes such as procName
	* @param LHS
	* @param RHS
	* @return TRUE if there are rows left after filtering
	*         FALSE if there are no rows left after filtering
	*/
	bool filterEqualPairByString(Synonym LHS, Synonym RHS)
	{
		string nameLHS = LHS.getName();
		string nameRHS = RHS.getName();
		bool isLHSInMainTable = isExistInMainTable(nameLHS);
		bool isRHSInMainTable = isExistInMainTable(nameRHS);
		bool isLHSInSingletonTable = isExistInSingletonTable(nameLHS);
		bool isRHSInSingletonTable = isExistInSingletonTable(nameRHS);

		if (isLHSInMainTable && isRHSInMainTable) {
			return filterEqualStringInMain(LHS, RHS);
		} else if (isLHSInMainTable) {
			//Only the LHS synonym exists in the main table
			RHS = getSynonym(nameRHS);
			removeFromSingletonTable(nameRHS);

			if (hashJoinWithMainTable(LHS, RHS)) {
				return filterEqualStringInMain(LHS, RHS);
			} else {
				return false;
			}
		} else if (isRHSInMainTable) {
			//If it comes here, only the RHS synonym exists in the main table
			LHS = getSynonym(nameLHS);
			removeFromSingletonTable(nameLHS);

			if (hashJoinWithMainTable(RHS, LHS)) {
				return filterEqualStringInMain(RHS, LHS);
			} else {
				return false;
			}
		} else {
			//Both synonyms do not exist in the main table
			if (!isLHSInSingletonTable && !isRHSInSingletonTable) {
				//If both synonyms do not exist at all
				LHS = getSynonym(nameLHS);  //Assign LHS to its default values
				addToSingletonTable(LHS);
				RHS = getSynonym(nameRHS);  //Assigh RHS to its default values
				addToSingletonTable(RHS);
			} else if (isLHSInSingletonTable) {
				//Only LHS exists in the singleton table
				RHS = getSynonym(nameRHS);  //Assign RHS to its default values
				addToSingletonTable(RHS);
			} else if (isRHSInSingletonTable) {
				//Only RHS exists in the singleton table
				LHS = getSynonym(nameLHS);  //Assign LHS to its default values
				addToSingletonTable(LHS);
			}

			pair<vector<int>, vector<int>> pair = getPairBySingletonStringIntersect(LHS, RHS);
			LHS.setValues(pair.first);
			RHS.setValues(pair.second);
			removeFromSingletonTable(nameLHS);
			return joinWithMainTable(LHS, RHS);
		}
	}

	bool filterEqualStringInMain(Synonym LHS, Synonym RHS)
	{
		//TODO: Assert that they both are in main
		int indexLHS = findIndexInMainTable(LHS.getName());
		int indexRHS = findIndexInMainTable(RHS.getName());
		vector<vector<int>> acceptedValues;

		for (unsigned int i = 0; i < mainTable.size(); i++) {
			vector<int> oneRow = mainTable[i];
			string valueLHS = convertIndexToString(oneRow[indexLHS], LHS.getType());
			string valueRHS = convertIndexToString(oneRow[indexRHS], RHS.getType());
			if (valueLHS == valueRHS) {
				acceptedValues.push_back(oneRow);
			}
		}
		swap(acceptedValues, mainTable);
		return mainTable.size() != 0;
	}
}
