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
			
			if (synonymType == BOOLEAN) {
				resultList.push_back(resultVector[0].getName());
			} else {
				vector<int> resultSet = resultVector[0].getValues();
				for (vector<int>::iterator itr = resultSet.begin(); itr != resultSet.end(); ++itr) {
					string value = ResultProjector::convertValueToString(*itr, synonymType);
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
		int mainFactor = 1;
		int singletonFactor = 1;
		unordered_map<string, int> individualFactor;
		vector<bool> isInMainTable;
		unordered_map<string, vector<string>> stringValuesMap;

		for (unsigned int i = 0; i < resultVector.size(); i++) {
			Synonym synonym = resultVector[i];
			string name = synonym.getName();

			if (ValuesHandler::isExistInMainTable(name)) {
				mainFactor = synonym.getValues().size();
				isInMainTable.push_back(true);
			} else {
				int size = synonym.getValues().size();
				individualFactor[name] = size;
				singletonFactor *= size;
				isInMainTable.push_back(false);
			}
		}

		for (unsigned int i = 0; i < resultVector.size(); i++) {
			Synonym synonym = resultVector[i];
			string name = synonym.getName();
			SYNONYM_TYPE type = synonym.getType();
			vector<int> valuesNumbers = synonym.getValues();
			vector<string> valuesStrings;

			for (unsigned int j = 0; j < valuesNumbers.size(); j++) {
				valuesStrings.push_back(ResultProjector::convertValueToString(valuesNumbers[j], type));
			}

			if (isInMainTable[i]) {
				valuesStrings = expandEachRow(valuesStrings, singletonFactor);
				stringValuesMap[name] = valuesStrings;
			} else {
				valuesStrings = expandRange(valuesStrings, mainFactor);
				valuesStrings = expandEachRow(valuesStrings, singletonFactor / individualFactor[name]);
				stringValuesMap[name] = valuesStrings;
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
	string convertValueToString(int value, SYNONYM_TYPE type) {
		switch (type) {
		case VARIABLE:
			return pkb.getVarName(value);
		case PROCEDURE:
			return pkb.getProcName(value);
		default:
			return to_string(static_cast<long long>(value));
		}
	}
}
