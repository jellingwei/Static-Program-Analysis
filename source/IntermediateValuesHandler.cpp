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

namespace IntermediateValuesHandler 
{

	//Private attributes
	vector<vector<int>> allIntermediateValues;
	vector<string> allIntermediateNames;
	unordered_map<string, string> synonymMap;
	unordered_map<string, int> allIntermediateNamesMap;  //@todo change to map
	PKB pkb = PKB::getInstance();
	
	void initialize(unordered_map<string, string> synonymsMap) 
	{
		allIntermediateValues.clear();
		allIntermediateNames.clear();
		synonymMap = synonymsMap;
	}

	void clear()
	{
		allIntermediateValues.clear();
		allIntermediateNames.clear();
		synonymMap.clear();
	}

	int findIntermediateSynonymIndex(string synonymName)
	{
		for (unsigned int i = 0; i < allIntermediateNames.size(); i++) {
			if (synonymName == allIntermediateNames[i]) {
				return i;
			}
		}
		return -1;
	}

	vector<int> getDefaultValues(SYNONYM_TYPE type) 
	{
		if (type == VARIABLE) {
			return pkb.getAllVarIndex();
		} else if (type == CONSTANT) {
			return pkb.getAllConstant();
		} else if (type == PROCEDURE) {
			return pkb.getAllProcIndex();
		} else {
			return pkb.getStmtNumForType(Synonym::convertToString(type));
		}
	}

	void addAndProcessIntermediateSynonym(Synonym synonym) 
	{
		if (synonym.getType() == UNDEFINED) {
			return;
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
	}

	void addAndProcessIntermediateSynonyms(Synonym LHS, Synonym RHS) 
	{
		if (LHS.getType() == UNDEFINED && RHS.getType() == UNDEFINED) {
			return;
		} else if (LHS.getType() == UNDEFINED) {
			addAndProcessIntermediateSynonym(RHS);
			return;
		} else if (RHS.getType() == UNDEFINED) {
			addAndProcessIntermediateSynonym(LHS);
			return;
		}

		//If it reaches here, it is two proper synonyms that require handling
		int indexLHS = findIntermediateSynonymIndex(LHS.getName());
		int indexRHS = findIntermediateSynonymIndex(RHS.getName());

		if (indexLHS == -1 && indexRHS == -1) {
			//Both LHS and RHS are not in the table
			//Cartesian product the two values with the current table
			joinWithExistingValues(LHS, RHS);
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
	}


	/**
	* Helper method to do a cartesian product of the intermediate synonym values with other values
	*/
	void joinWithExistingValues(Synonym synonym) 
	{
		vector<int> synonymValues = synonym.getValues();
		vector<vector<int>> acceptedValues;

		allIntermediateNames.push_back(synonym.getName());

		if (allIntermediateValues.size() == 0) {
			for (vector<int>::iterator itr = synonymValues.begin(); itr != synonymValues.end(); ++itr) {
				vector<int> newRow;
				newRow.push_back(*itr);
				acceptedValues.push_back(newRow);
			}
		} else {
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
	* Helper method to do a cartesian product of the intermediate synonym values with other values
	* Overloaded method which takes in two arguments instead of one
	*/
	void joinWithExistingValues(Synonym LHS, Synonym RHS) 
	{
		vector<int> valuesLHS = LHS.getValues();
		vector<int> valuesRHS = RHS.getValues();
		vector<vector<int>> acceptedValues;

		allIntermediateNames.push_back(LHS.getName());
		allIntermediateNames.push_back(RHS.getName());

		//Insert the values if there is nothing in the intermediate values table
		if (allIntermediateValues.size() == 0) {
			for (unsigned int i = 0; i < valuesLHS.size(); i++) {
				vector<int> newRow;
				newRow.push_back(valuesLHS[i]);
				newRow.push_back(valuesRHS[i]);
				acceptedValues.push_back(newRow);
			}
		} else {
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

	void intersectAndJoinWithExistingValues(int existingIndex, Synonym probe, Synonym newSynonym)
	{
		vector<vector<int>> acceptedValues;
		vector<int> probeValues = probe.getValues();
		vector<int> newValues = newSynonym.getValues();

		allIntermediateNames.push_back(newSynonym.getName());

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
	
	//@todo Now can process varName. Need procName also.
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
				if (type == PROCEDURE && synonym.getAttribute() == procName) {
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
			int value;

			if (synonym.getType() == VARIABLE) {
				value = pkb.getVarIndex(wantedValue);
			} else if (synonym.getType() == PROCEDURE && synonym.getAttribute() == procName) {
				value = pkb.getProcIndex(wantedValue);
			} else {
				value = stoi(wantedValue);
			}
		
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
	
	//Assume that varIndex and int cannot be compared
	//Assume that process with clauses last
	//@todo special handling for comparison between varName and procName
	bool filterEqualPair(Synonym LHS, Synonym RHS)
	{
		int indexLHS = findIntermediateSynonymIndex(LHS.getName());
		int indexRHS = findIntermediateSynonymIndex(RHS.getName());
		
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
			LHS.setValues(valuesLHS);
			intersectAndJoinWithExistingValues(indexRHS, RHS, LHS);
			return (allIntermediateValues.size() != 0);
		} else if (indexRHS == -1) {
			vector<int> valuesRHS = getDefaultValues(RHS.getType());
			LHS.setValues(valuesRHS);
			intersectAndJoinWithExistingValues(indexLHS, LHS, RHS);
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
	* Helper method to get the synonym with its final values
	* Gets the default values if this synonym is not in the intermediate values
	* Otherwise, performs set intersection with the intermediate values
	* Returns a new synonym with the final values
	*/
	Synonym getSynonymWithName(string wantedName) 
	{
		unordered_map<string, string>::iterator itr = synonymMap.find(wantedName);

		string name = itr->first;
		string type = itr->second;		
		int synonymIndex = findIntermediateSynonymIndex(name);

		if (synonymIndex == -1) {
			vector<int> synonymValues = getDefaultValues(Synonym::convertToEnum(type));
			Synonym synonym(Synonym::convertToEnum(type), name, synonymValues);
			return synonym;
		} else {
			Synonym synonym(Synonym::convertToEnum(type), name, getIntermediateValuesSet(synonymIndex));
			return synonym;
		}
	}

	vector<int> getIntermediateValues(int synonymIndex) 
	{
		vector<int> intermediateValues;
		for (unsigned int i = 0; i < allIntermediateValues.size(); i++) {
			intermediateValues.push_back(allIntermediateValues[i][synonymIndex]);
		}
		return intermediateValues;
	}

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
