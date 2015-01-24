#include <vector>
#include <set>
#include <string>
using std::string;
using std::set;
using std::vector;
using std::string;
using std::swap;

#include "IntermediateValuesHandler.h"

namespace IntermediateValuesHandler 
{

	//Private attributes
	vector<vector<int>> allIntermediateValues;
	vector<string> allIntermediateNames;
	unordered_map<string, string> synonymMap;
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
		for (unsigned int i = 0; i < allIntermediateNames.size(); i++) 
		{
			if (synonymName == allIntermediateNames[i]) 
			{
				return i;
			}
		}
		return -1;
	}

	void addAndProcessIntermediateSynonym(Synonym synonym) 
	{
		if (synonym.getType() == "_") 
		{
			return;
		}

		string name = synonym.getName();
		int synonymIndex = findIntermediateSynonymIndex(name);

		if (synonymIndex == -1) 
		{
			//If this synonym is not yet in the table,
			//get all the default values, intersect and join

			string type = synonym.getType();
			vector<int> allValues;
			set<int> intermediateValues = synonym.getValuesSet();
			set<int> finalValues;

			//Get the default values
			if (type == "variable") 
			{
				allValues = pkb.getAllVarIndex();
			} 
			else if (type == "constant") 
			{
				allValues = pkb.getAllConstant();
			} 
			else 
			{
				allValues = pkb.getStmtNumForType(type);
			}

			//Intersection
			for (unsigned int i = 0; i < allValues.size(); i++) 
			{
				if (isValueExist(intermediateValues, allValues[i])) 
				{
					finalValues.insert(allValues[i]);
				}
			}

			//Join
			Synonym newSynonym(type, name, finalValues);
			joinWithExistingValues(newSynonym);
		} 
		else 
		{
			//This synonym is already in the table
			//Just do intersection with the existing intermediate values
			intersectWithExistingValues(synonymIndex, synonym.getValues());
		}
	}

	void addAndProcessIntermediateSynonyms(Synonym LHS, Synonym RHS) 
	{
		if (LHS.getType() == "_" && RHS.getType() == "_") 
		{
			return;
		} 
		else if (LHS.getType() == "_") 
		{
			addAndProcessIntermediateSynonym(RHS);
			return;
		} 
		else if (RHS.getType() == "_") 
		{
			addAndProcessIntermediateSynonym(LHS);
			return;
		}

		//If it reaches here, it is two proper synonyms that require handling
		int indexLHS = findIntermediateSynonymIndex(LHS.getName());
		int indexRHS = findIntermediateSynonymIndex(RHS.getName());

		if (indexLHS == -1 && indexRHS == -1) 
		{
			//Both LHS and RHS are not in the table
			//Cartesian product the two values with the current table
			joinWithExistingValues(LHS, RHS);
		} 
		else if (indexLHS == -1) 
		{
			//LHS is not in the table while RHS is
			//Use RHS to intersect with the table values then join with the LHS
			intersectAndJoinWithExistingValues(indexRHS, RHS, LHS);
		} 
		else if (indexRHS == -1) 
		{
			//RHS is not in the table while LHS is
			//Use LHS to intersect with the table values then join with the RHS
			intersectAndJoinWithExistingValues(indexLHS, LHS, RHS);
		} 
		else 
		{
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

		if (allIntermediateValues.size() == 0) 
		{
			for (vector<int>::iterator itr = synonymValues.begin(); itr != synonymValues.end(); ++itr) 
			{
				vector<int> newRow;
				newRow.push_back(*itr);
				acceptedValues.push_back(newRow);
			}
		} 
		else 
		{
			for (vector<int>::iterator itr = synonymValues.begin(); itr != synonymValues.end(); ++itr) 
			{
				for (unsigned int i = 0; i < allIntermediateValues.size(); i++) 
				{
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
		if (allIntermediateValues.size() == 0) 
		{
			for (unsigned int i = 0; i < valuesLHS.size(); i++) 
			{
				vector<int> newRow;
				newRow.push_back(valuesLHS[i]);
				newRow.push_back(valuesRHS[i]);
				acceptedValues.push_back(newRow);
			}
		} 
		else
		{
			//If there are values in the table, do a cartesian product
			for (unsigned int i = 0; i < valuesLHS.size(); i++) 
			{
				for (unsigned int j = 0; j < allIntermediateValues.size(); j++) 
				{
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

		for (unsigned int i = 0; i < probeValues.size(); i++) 
		{
			probeValuesSet.insert(probeValues[i]);
		}

		for (unsigned int i = 0; i < allIntermediateValues.size(); i++) 
		{
			if (isValueExist(probeValuesSet, (allIntermediateValues[i])[synonymIndex])) 
			{
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

		for (unsigned int i = 0; i < allIntermediateValues.size(); i++) 
		{
			for (unsigned int j = 0; j < LHSValues.size(); j++)
			{
				if (allIntermediateValues[i][indexLHS] == LHSValues[j] && allIntermediateValues[i][indexRHS] == RHSValues[j])
				{
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

		for (unsigned int i = 0; i < allIntermediateValues.size(); i++) 
		{
			for (unsigned int j = 0; j < probeValues.size(); j++) 
			{
				if (probeValues[j] == allIntermediateValues[i][existingIndex]) 
				{
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

		if (synonymIndex == -1) 
		{
			vector<int> synonymValues;

			if (type == "variable") 
			{
				synonymValues = pkb.getAllVarIndex();
			}
			else if (type == "constant") 
			{
				synonymValues = pkb.getAllConstant();
			} 
			else 
			{
				synonymValues = pkb.getStmtNumForType(type);
			}

			Synonym synonym(type, name, synonymValues);
			return synonym;
		} 
		else 
		{
			set<int> intermediateValues = getIntermediateValuesSet(synonymIndex);
			Synonym synonym(type, name, intermediateValues);
			return synonym;
		}
	}

	vector<int> getIntermediateValues(int synonymIndex) 
	{
		vector<int> intermediateValues;
		for (unsigned int i = 0; i < allIntermediateValues.size(); i++) 
		{
			intermediateValues.push_back(allIntermediateValues[i][synonymIndex]);
		}
		return intermediateValues;
	}

	set<int> getIntermediateValuesSet(int synonymIndex) 
	{
		set<int> intermediateValues;
		for (unsigned int i = 0; i < allIntermediateValues.size(); i++) 
		{
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

		if (count >= 1) 
		{
			return true;
		}
		return false;
	}
}
