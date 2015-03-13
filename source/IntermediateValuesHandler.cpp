#include <vector>
#include <set>
#include <string>
using std::string;
using std::set;
using std::vector;
using std::string;
using std::swap;
using std::stoi;

#include "IntermediateValuesHandler.h"

/**
	@brief Namespace containing functions for the handling of intermediate values.

 */
namespace IntermediateValuesHandler 
{
	//Private functions
	bool filterEqualPairByString(Synonym LHS, Synonym RHS);
	bool filterEqualPairByNumber(Synonym LHS, Synonym RHS);
	string convertIndexToString(int index, SYNONYM_TYPE type);

	//Private attributes
	vector<vector<int>> allIntermediateValues;
	unordered_map<string, SYNONYM_TYPE> synonymMap;
	unordered_map<string, int> allIntermediateNamesMap;
	PKB pkb = PKB::getInstance();

	/**
	* Initialize the handler for intermediate values.
	* @param synonymsMap which maps from the synonym to the synonym type (e.g. STMT)
	*/
	void initialize(unordered_map<string, SYNONYM_TYPE> synonymsMap) 
	{
		allIntermediateValues.clear();
		allIntermediateNamesMap.clear();
		synonymMap = synonymsMap;
	}

	/**
	* Initialize the handler for intermediate values. For unit testing purposes. 
	*/
	void clear()
	{
		allIntermediateValues.clear();
		allIntermediateNamesMap.clear();
		synonymMap.clear();
	}

	/**
	* Find the column number in which this synonym is stored in the table
	* Made public for testing purposes. 
	* @param synonymName the name of the synonym
	* @return the column number if the synonym exists in the table
	*         -1 if the synonym does not exist in the table
	*/
	int findIntermediateSynonymIndex(string synonymName)
	{
		auto itr = allIntermediateNamesMap.find(synonymName);

		if (itr == allIntermediateNamesMap.end()) {
			return -1;
		} else {
			return itr->second;
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
			return allIntermediateValues.size() != 0;
		}
		
		string name = synonym.getName();
		int synonymIndex = findIntermediateSynonymIndex(name);

		if (synonymIndex == -1) {
			//If this synonym is not yet in the table,
			//get all the default values, intersect and join

			SYNONYM_TYPE type = synonym.getType();
			set<int> intermediateValues = synonym.getValuesSet();
			set<int> finalValues;

			//Get the default values
			vector<int> allValues = getDefaultValues(type);

			//Intersection
			for (unsigned int i = 0; i < allValues.size(); i++) {
				if (isValueExist(intermediateValues, allValues[i])) {
					finalValues.insert(allValues[i]);
				}
			}

			//Join
			synonym.setValues(finalValues);
			joinWithExistingValues(synonym);
		} else {
			//This synonym is already in the table
			//Just do intersection with the existing intermediate values
			intersectWithExistingValues(synonymIndex, synonym.getValues());
		}
		return allIntermediateValues.size() != 0;
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
		if (LHS.getValues().size() == 0) {
			return false;
		}
		
		if (LHS.getType() == UNDEFINED && RHS.getType() == UNDEFINED) {
			return allIntermediateValues.size() != 0;
		} else if (LHS.getType() == UNDEFINED) {
			return addAndProcessIntermediateSynonym(RHS);
		} else if (RHS.getType() == UNDEFINED) {
			return addAndProcessIntermediateSynonym(LHS);
		}

		//If it reaches here, it is two proper synonyms that require handling
		int indexLHS = findIntermediateSynonymIndex(LHS.getName());
		int indexRHS = findIntermediateSynonymIndex(RHS.getName());

		if (indexLHS == -1 && indexRHS == -1) {
			//Both LHS and RHS are not in the table
			//Cartesian product the two values with the current table
			joinWithExistingValues(LHS, RHS);
		} else if (indexLHS == indexRHS) {
			//These are the same synonyms
			//Only take values that are the same on both sides
			vector<int> valuesLHS = LHS.getValues();
			vector<int> valuesRHS = RHS.getValues();
			vector<int> acceptedValues;
			
			for (unsigned int i = 0; i < valuesLHS.size(); i++) {
				if (valuesLHS[i] == valuesRHS[i]) {
					acceptedValues.push_back(valuesLHS[i]);
				}
			}
			LHS.setValues(acceptedValues);  //Can take either LHS or RHS
			return addAndProcessIntermediateSynonym(LHS);
		} else if (indexLHS == -1) {
			//LHS is not in the table while RHS is
			//Use RHS to intersect with the table values then join with the LHS
			intersectAndJoinWithExistingValues(indexRHS, RHS, LHS);
		} else if (indexRHS == -1) {
			//RHS is not in the table while LHS is
			//Use LHS to intersect with the table values then join with the RHS
			intersectAndJoinWithExistingValues(indexLHS, LHS, RHS);
		} else {
			//Both LHS and RHS are in the table
			//Use both the values to intersect with the table
			intersectWithExistingValues(indexLHS, LHS.getValues(), indexRHS, RHS.getValues());
		}
		return allIntermediateValues.size() != 0;
	}

	/**
	* Helper method to do a join or cartesian product of the intermediate synonym values with other values
	* Made public for testing purposes
	* @param synonym
	*/
	void joinWithExistingValues(Synonym synonym) 
	{
		vector<int> synonymValues = synonym.getValues();
		vector<vector<int>> acceptedValues;

		if (allIntermediateValues.size() == 0) {
			allIntermediateNamesMap[synonym.getName()] = 0;
			for (vector<int>::iterator itr = synonymValues.begin(); itr != synonymValues.end(); ++itr) {
				vector<int> newRow;
				newRow.push_back(*itr);
				acceptedValues.push_back(newRow);
			}
		} else {
			allIntermediateNamesMap[synonym.getName()] = allIntermediateValues[0].size();
			for (vector<int>::iterator itr = synonymValues.begin(); itr != synonymValues.end(); ++itr) {
				for (unsigned int i = 0; i < allIntermediateValues.size(); i++) {
					vector<int> newRow(allIntermediateValues[i]);
					newRow.push_back(*itr);
					acceptedValues.push_back(newRow);
				}
			}
		}

		swap(allIntermediateValues, acceptedValues);
	}

	/**
	* Helper method to do a join or cartesian product of the intermediate synonym values with other values
	* Overloaded method which takes in two arguments instead of one
	* Made public for testing purposes
	* @param LHS
	* @param RHS
	*/
	void joinWithExistingValues(Synonym LHS, Synonym RHS) 
	{
		vector<int> valuesLHS = LHS.getValues();
		vector<int> valuesRHS = RHS.getValues();
		vector<vector<int>> acceptedValues;

		//Insert the values if there is nothing in the intermediate values table
		if (allIntermediateValues.size() == 0) {
			allIntermediateNamesMap[LHS.getName()] = 0;
			allIntermediateNamesMap[RHS.getName()] = 1;
			for (unsigned int i = 0; i < valuesLHS.size(); i++) {
				vector<int> newRow;
				newRow.push_back(valuesLHS[i]);
				newRow.push_back(valuesRHS[i]);
				acceptedValues.push_back(newRow);
			}
		} else {
			allIntermediateNamesMap[LHS.getName()] = allIntermediateValues[0].size();
			allIntermediateNamesMap[RHS.getName()] = allIntermediateValues[0].size() + 1;
			//If there are values in the table, do a cartesian product
			for (unsigned int i = 0; i < valuesLHS.size(); i++) {
				for (unsigned int j = 0; j < allIntermediateValues.size(); j++) {
					vector<int> newRow(allIntermediateValues[j]);
					newRow.push_back(valuesLHS[i]);
					newRow.push_back(valuesRHS[i]);
					acceptedValues.push_back(newRow);
				}
			}
		}
		swap(allIntermediateValues, acceptedValues);
	}

	/**
	* Helper method to do a set intersection of the intermediate values in the table
	* Made public for testing purposes
	* @param synonymIndex the column number of the synonym
	* @param probeValues the new values that are to be inserted
	*/
	void intersectWithExistingValues(int synonymIndex, vector<int> probeValues) 
	{
		vector<vector<int>> acceptedValues;
		set<int> probeValuesSet;

		for (unsigned int i = 0; i < probeValues.size(); i++) {
			probeValuesSet.insert(probeValues[i]);
		}

		for (unsigned int i = 0; i < allIntermediateValues.size(); i++) {
			if (isValueExist(probeValuesSet, (allIntermediateValues[i])[synonymIndex])) {
				acceptedValues.push_back(allIntermediateValues[i]);
			}
		}
		swap(allIntermediateValues, acceptedValues);
	}

	/**
	* Helper method to do a set intersection of the intermediate values in the table
	* Overloaded method which takes in two pairs of arguments instead of one pair
	* Made public for testing purposes
	* @param indexLHS the column number of the LHS synonym
	* @param LHSValues the new values of the LHS synonym
	* @param indexRHS the column number of the RHS synonym
	* @param RHSValues the values of the RHS synonym
	*/
	void intersectWithExistingValues(int indexLHS, vector<int> LHSValues, int indexRHS, vector<int> RHSValues) 
	{
		vector<vector<int>> acceptedValues;

		for (unsigned int i = 0; i < allIntermediateValues.size(); i++) {
			for (unsigned int j = 0; j < LHSValues.size(); j++){
				if (allIntermediateValues[i][indexLHS] == LHSValues[j] && allIntermediateValues[i][indexRHS] == RHSValues[j]) {
					acceptedValues.push_back(allIntermediateValues[i]);
				}
			}
		}
		swap(allIntermediateValues, acceptedValues);
	}

	/**
	* Helper method to do a set intersection and joining of paired synonyms
	* One synonym exists in the main table while the other does not
	* Made public for testing purposes
	* @param existingIndex the column number of the existing synonym
	* @param probe the synonym that is being used as the join value
	* @param newSynonym the new synonym that is to be joined
	*/
	void intersectAndJoinWithExistingValues(int existingIndex, Synonym probe, Synonym newSynonym)
	{
		vector<vector<int>> acceptedValues;
		vector<int> probeValues = probe.getValues();
		vector<int> newValues = newSynonym.getValues();

		allIntermediateNamesMap[newSynonym.getName()] = allIntermediateValues[0].size();

		for (unsigned int i = 0; i < allIntermediateValues.size(); i++) {
			for (unsigned int j = 0; j < probeValues.size(); j++) {
				if (probeValues[j] == allIntermediateValues[i][existingIndex]) {
					vector<int> newRow(allIntermediateValues[i]);
					newRow.push_back(newValues[j]);
					acceptedValues.push_back(newRow);
				} /*else if (probeValues[j] > allIntermediateValues[i][existingIndex]) {
				  break;  //Stop looping the inner probe values if it exceeds the outer value
				  }*/
			}
		}

		swap(allIntermediateValues, acceptedValues);
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
		int synonymIndex = findIntermediateSynonymIndex(synonym.getName());
		vector<vector<int>> acceptedValues;
		//This synonym is not in the intermediate values table
		//Get all the values from the synonyms map and do the comparison
		if (synonymIndex == -1) {
			SYNONYM_TYPE type = synonym.getType();
			vector<int> allValues = getDefaultValues(type);

			for (unsigned int i = 0; i < allValues.size(); i++) {
				if (type == PROCEDURE) {
					string name = pkb.getProcName(allValues[i]);
					if (name == wantedValue) {
						vector<int> temp;
						temp.push_back(allValues[i]);
						synonym.setValues(temp);
						joinWithExistingValues(synonym);
						return (allIntermediateValues.size() != 0);
					}
				} else if (type == VARIABLE) {
					string var = pkb.getVarName(allValues[i]);
					if (var == wantedValue) {
						vector<int> temp;
						temp.push_back(allValues[i]);
						synonym.setValues(temp);
						joinWithExistingValues(synonym);
						return (allIntermediateValues.size() != 0);
					}
				} else if (type == CALL && synonym.getAttribute() == procName) {
					string name = pkb.getProcNameCalledByStatement(allValues[i]);
					if (name == wantedValue) {
						vector<int> temp;
						temp.push_back(allValues[i]);
						synonym.setValues(temp);
						joinWithExistingValues(synonym);
						return (allIntermediateValues.size() != 0);
					}
				} else {
					if (allValues[i] == stoi(wantedValue)) {
						vector<int> temp;
						temp.push_back(allValues[i]);
						synonym.setValues(temp);
						joinWithExistingValues(synonym);
						return (allIntermediateValues.size() != 0);
					}
				}
			}
		} else {
			//This synonym is in the intermediate values table
			int value;

			if (synonym.getType() == VARIABLE) {
				value = pkb.getVarIndex(wantedValue);
			} else if (synonym.getType() == PROCEDURE && synonym.getAttribute() == procName) {
				value = pkb.getProcIndex(wantedValue);
			} else {
				value = stoi(wantedValue);
			}

			//Do the comparison
			for (unsigned int i = 0; i < allIntermediateValues.size(); i++) {
				if (allIntermediateValues[i][synonymIndex] == value) {
					acceptedValues.push_back(allIntermediateValues[i]);
				}
			}

			swap(allIntermediateValues, acceptedValues);
			return (allIntermediateValues.size() != 0);
		}
		return (allIntermediateValues.size() != 0);
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
		int indexLHS = findIntermediateSynonymIndex(LHS.getName());
		int indexRHS = findIntermediateSynonymIndex(RHS.getName());

		//Assumes that comparisons are between numbers
		if (indexLHS == -1 && indexRHS == -1) {
			vector<int> valuesLHS = getDefaultValues(LHS.getType());
			vector<int> valuesRHS = getDefaultValues(RHS.getType());

			vector<int> finalValuesLHS;
			vector<int> finalValuesRHS;
			for (unsigned int i = 0; i < valuesLHS.size(); i++) {
				for (unsigned int j = 0; j < valuesRHS.size(); j++) {
					if (valuesLHS[i] == valuesRHS[j]) {
						finalValuesLHS.push_back(valuesLHS[i]);
						finalValuesRHS.push_back(valuesRHS[j]);
					}
				}
			}
			LHS.setValues(finalValuesLHS);
			RHS.setValues(finalValuesRHS);
			joinWithExistingValues(LHS, RHS);
			return (allIntermediateValues.size() != 0);
		} else if (indexLHS == -1) {
			vector<int> valuesLHS = getDefaultValues(LHS.getType());
			vector<vector<int>> acceptedValues;

			for (unsigned int i = 0; i < allIntermediateValues.size(); i++) {
				for (unsigned int j = 0; j < valuesLHS.size(); j++) {
					if (allIntermediateValues[i][indexRHS] == valuesLHS[j]) {
						vector<int> row = allIntermediateValues[i];
						row.push_back(valuesLHS[j]);
						acceptedValues.push_back(row);
					}
				}
			}

			allIntermediateNamesMap[LHS.getName()] = allIntermediateValues[0].size();
			swap(acceptedValues, allIntermediateValues);
			return (allIntermediateValues.size() != 0);
		} else if (indexRHS == -1) {
			vector<int> valuesRHS = getDefaultValues(RHS.getType());
			
			vector<vector<int>> acceptedValues;

			for (unsigned int i = 0; i < allIntermediateValues.size(); i++) {
				for (unsigned int j = 0; j < valuesRHS.size(); j++) {
					if (allIntermediateValues[i][indexLHS] == valuesRHS[j]) {
						vector<int> row = allIntermediateValues[i];
						row.push_back(valuesRHS[j]);
						acceptedValues.push_back(row);
					}
				}
			}

			allIntermediateNamesMap[RHS.getName()] = allIntermediateValues[0].size();
			swap(acceptedValues, allIntermediateValues);
			return (allIntermediateValues.size() != 0);
		} else {
			vector<vector<int>> acceptedValues;

			for (unsigned int i = 0; i < allIntermediateValues.size(); i++) {
				if (allIntermediateValues[i][indexLHS] == allIntermediateValues[i][indexRHS]) {
					acceptedValues.push_back(allIntermediateValues[i]);
				}
			}

			swap(allIntermediateValues, acceptedValues);
			return (allIntermediateValues.size() != 0);
		}
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
		SYNONYM_TYPE arg1Type = LHS.getType();
		SYNONYM_TYPE arg2Type = RHS.getType();
		int indexLHS = findIntermediateSynonymIndex(LHS.getName());
		int indexRHS = findIntermediateSynonymIndex(RHS.getName());

		if (indexLHS == -1 && indexRHS == -1) {
			vector<int> valuesLHS = getDefaultValues(LHS.getType());
			vector<int> valuesRHS = getDefaultValues(RHS.getType());

			vector<int> finalValuesLHS;
			vector<int> finalValuesRHS;
			for (unsigned int i = 0; i < valuesLHS.size(); i++) {
				for (unsigned int j = 0; j < valuesRHS.size(); j++) {
					string stringLHS = convertIndexToString(valuesLHS[i], arg1Type);
					string stringRHS = convertIndexToString(valuesRHS[j], arg2Type);
					if (stringLHS == stringRHS) {
						finalValuesLHS.push_back(valuesLHS[i]);
						finalValuesRHS.push_back(valuesRHS[j]);
					}
				}
			}
			LHS.setValues(finalValuesLHS);
			RHS.setValues(finalValuesRHS);
			joinWithExistingValues(LHS, RHS);
			return (allIntermediateValues.size() != 0);
		} else if (indexLHS == -1) {
			vector<int> valuesLHS = getDefaultValues(LHS.getType());
			set<int> valuesRHS = getIntermediateValuesSet(indexRHS);
			vector<int> finalLHS;
			vector<int> finalRHS;
			for (set<int>::iterator itr = valuesRHS.begin(); itr != valuesRHS.end(); ++itr) {
				for (unsigned int i = 0; i < valuesLHS.size(); i++) {
					string stringLHS = convertIndexToString(valuesLHS[i], arg1Type);
					string stringRHS = convertIndexToString(*itr, arg2Type);
					if (stringLHS == stringRHS) {
						finalLHS.push_back(valuesLHS[i]);
						finalRHS.push_back(*itr);
					}
				}
			}
			LHS.setValues(finalLHS);
			RHS.setValues(finalRHS);
			intersectAndJoinWithExistingValues(indexRHS, RHS, LHS);
			return (allIntermediateValues.size() != 0);
		} else if (indexRHS == -1) {
			vector<int> valuesRHS = getDefaultValues(RHS.getType());
			set<int> valuesLHS = getIntermediateValuesSet(indexLHS);
			vector<int> finalLHS;
			vector<int> finalRHS;
			for (set<int>::iterator itr = valuesLHS.begin(); itr != valuesLHS.end(); ++itr) {
				for (unsigned int i = 0; i < valuesRHS.size(); i++) {
					string stringLHS = convertIndexToString(*itr, arg1Type);
					string stringRHS = convertIndexToString(valuesRHS[i], arg2Type);
					if (stringLHS == stringRHS) {
						finalRHS.push_back(valuesRHS[i]);
						finalLHS.push_back(*itr);
					}
				}
			}
			LHS.setValues(finalLHS);
			RHS.setValues(finalRHS);
			intersectAndJoinWithExistingValues(indexLHS, LHS, RHS);
			return (allIntermediateValues.size() != 0);
		} else {
			vector<vector<int>> acceptedValues;

			for (unsigned int i = 0; i < allIntermediateValues.size(); i++) {
				string LHS = convertIndexToString(allIntermediateValues[i][indexLHS], arg1Type);
				string RHS = convertIndexToString(allIntermediateValues[i][indexRHS], arg2Type);
				if (LHS == RHS) {
					acceptedValues.push_back(allIntermediateValues[i]);
				}
			}

			swap(allIntermediateValues, acceptedValues);
			return (allIntermediateValues.size() != 0);
		}
	}

	/**
	* Helper method to get the synonym with its final values
	* Gets the default values if this synonym is not in the intermediate values
	* Otherwise, performs gets the existing intermediate values
	* @param wantedName the name of the wanted synonym
	* @return Synonym object with the type, name and values
	*/
	Synonym getSynonymWithName(string wantedName) 
	{
		unordered_map<string, SYNONYM_TYPE>::iterator itr = synonymMap.find(wantedName);

		string name = itr->first;
		SYNONYM_TYPE type = itr->second;		
		int synonymIndex = findIntermediateSynonymIndex(name);

		if (synonymIndex == -1) {
			vector<int> synonymValues = getDefaultValues(type);
			Synonym synonym(type, name, synonymValues);
			return synonym;
		} else {
			Synonym synonym(type, name, getIntermediateValuesSet(synonymIndex));
			return synonym;
		}
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

	/**
	* Helper method to get all intermediate values of the column
	* Might contain duplicates
	* Made public for testing
	* @param synonymIndex the index of the column
	* @return A vector containing all the intermediate values so far
	*/
	vector<int> getIntermediateValues(int synonymIndex) 
	{
		vector<int> intermediateValues;
		for (unsigned int i = 0; i < allIntermediateValues.size(); i++) {
			intermediateValues.push_back(allIntermediateValues[i][synonymIndex]);
		}
		return intermediateValues;
	}

	/**
	* Helper method to get all intermediate values of the column
	* There are no duplicates contain duplicates
	* Made public for testing
	* @param synonymIndex the index of the column
	* @return A set containing all the intermediate values with no duplicates
	*/
	set<int> getIntermediateValuesSet(int synonymIndex) 
	{
		set<int> intermediateValues;
		for (unsigned int i = 0; i < allIntermediateValues.size(); i++) {
			intermediateValues.insert(allIntermediateValues[i][synonymIndex]);
		}
		return intermediateValues;
	}

	/**
	* Helper method to check if a particular value is contained in the set
	* @param setToSearch a given set to search
	* @param value the value that is to be searched for
	* @return TRUE if the value exists in the set given
	*         FALSE if the value does not exist in the set given
	*/
	inline bool isValueExist(set<int> setToSearch, int value) 
	{
		int count = setToSearch.count(value);

		if (count >= 1) {
			return true;
		}
		return false;
	}
}
