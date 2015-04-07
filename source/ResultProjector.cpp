#include <iostream>
#include "ResultProjector.h"
#include "PKB.h"  //To use varTable for conversion from index to variable

namespace ResultProjector {

	PKB pkb = PKB::getInstance();  //Declare the PKB instance here to avoid repeated calls

	unordered_map<string, vector<string>> convertTuplesToString(vector<Synonym> resultVector);

	/**
	* Given a list, inserts all the values into the list from the vector of Synonyms.
	* @todo Also performs the cartesian product if there is a tuple specified
	*/
	void projectResultToList(vector<Synonym> resultVector, list<string>& resultList) { 
		if (resultVector.size() == 1) {
			SYNONYM_TYPE synonymType = resultVector[0].getType();
			SYNONYM_ATTRIBUTE synonymAttribute = resultVector[0].getAttribute();
			
			if (synonymType == BOOLEAN) {
				resultList.push_back(resultVector[0].getName());
			} else {
				vector<int> resultSet = resultVector[0].getValues();
				for (vector<int>::iterator itr = resultSet.begin(); itr != resultSet.end(); ++itr) {
					string value = ResultProjector::convertValueToString(*itr, synonymType, synonymAttribute);
					resultList.push_back(value);
				}
			}
		} else if (resultVector.size() > 1) {
			unordered_map<string, vector<string>> valuesMap = convertTuplesToString(resultVector);
			int size = valuesMap.begin()->second.size();

			for (int i = 0; i < size; i++) {
				string oneRow;

				for (unsigned int j = 0; j < resultVector.size(); j++) {
					string name = resultVector[j].getName();
					vector<string> values = valuesMap[name];

					if (j != 0) {
						oneRow += " ";
					}
					oneRow += values[i];
				}
				resultList.push_back(oneRow);
			}
		}
	}

	unordered_map<string, vector<string>> convertTuplesToString(vector<Synonym> resultVector)
	{
		unsigned int mainFactor = 1;
		unsigned int singletonFactor = 1;
		unsigned int lastSingletonIndex = 0;
		unordered_map<string, int> individualFactor;
		unordered_map<string, bool> isInMainTableMap;
		unordered_map<string, vector<string>> stringValuesMap;
		set<string> insertedNames;

		//Calculate the factors to replicate the rows
		for (unsigned int i = 0; i < resultVector.size(); i++) {
			Synonym synonym = resultVector[i];
			string name = synonym.getName();

			if (insertedNames.count(name) == 0) {
				if (ValuesHandler::isExistInMainTable(name)) {
					mainFactor = synonym.getValues().size();
					insertedNames.insert(name);
					isInMainTableMap[name] = true;
				} else {
					int size = synonym.getValues().size();
					individualFactor[name] = size;
					singletonFactor *= size;
					insertedNames.insert(name);
					isInMainTableMap[name] = false;
					if (i > lastSingletonIndex) {
						lastSingletonIndex = i;
					}
				}
			}
		}

		for (unsigned int i = 0; i < resultVector.size(); i++) {
			Synonym synonym = resultVector[i];
			string name = synonym.getName();
			SYNONYM_TYPE type = synonym.getType();
			SYNONYM_ATTRIBUTE attribute = synonym.getAttribute();
			vector<int> valuesNumbers = synonym.getValues();
			vector<string> valuesStrings;

			for (unsigned int j = 0; j < valuesNumbers.size(); j++) {
				valuesStrings.push_back(ResultProjector::convertValueToString(valuesNumbers[j], type, attribute));
			}

			if (isInMainTableMap[name] == true) {
				valuesStrings = expandEachRow(valuesStrings, singletonFactor);
				stringValuesMap[name] = valuesStrings;
			} else {
				if (i == lastSingletonIndex) {
					valuesStrings = expandRange(valuesStrings, mainFactor);
					stringValuesMap[name] = valuesStrings;
				} else {
					valuesStrings = expandRange(valuesStrings, mainFactor);
					mainFactor *= individualFactor[name];
					valuesStrings = expandEachRow(valuesStrings, singletonFactor / mainFactor);
					stringValuesMap[name] = valuesStrings;
				}
			}
		}
		return stringValuesMap;
	}

	vector<string> expandEachRow(vector<string> values, int factor)
	{
		vector<string> returnStrings;

		for (unsigned int i = 0; i < values.size(); i++) {
			for (int j = 0; j < factor; j++) {
				returnStrings.push_back(values[i]);
			}
		}
		return returnStrings;
	}

	vector<string> expandRange(vector<string> values, int factor)
	{
		vector<string> returnStrings;

		for (int i = 0; i < factor; i++) {
			returnStrings.insert(returnStrings.end(), values.begin(), values.end());
		}
		return returnStrings;
	}

	/**
	* Helper function to convert index to variable strings and to convert int to strings
	* Returns the converted value as a string
	*/
	string convertValueToString(int value, SYNONYM_TYPE type, SYNONYM_ATTRIBUTE attribute) {
		switch (type) {
		case VARIABLE:
			return pkb.getVarName(value);
		case PROCEDURE:
			return pkb.getProcName(value);
		case CALL:
			if (attribute == procName) {
				return pkb.getProcNameCalledByStatement(value);
			} else {
				return to_string(static_cast<long long>(value));
			}
		default:
			return to_string(static_cast<long long>(value));
		}
	}
}
