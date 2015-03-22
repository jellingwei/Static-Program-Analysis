#include <iostream>
#include "ResultProjector.h"
#include "PKB.h"  //To use varTable for conversion from index to variable

namespace ResultProjector {

	PKB pkb = PKB::getInstance();  //Declare the PKB instance here to avoid repeated calls

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
					string value = convertValueToString(*itr, synonymType);
					resultList.push_back(value);
				}
			}
		} else if (resultVector.size() > 1) {
			int numOfRows = resultVector[0].getValues().size();
			vector<vector<int>> allValues;  //[i][j] where i denotes the column and j denotes the row
			
			for (unsigned int i = 0; i < resultVector.size(); i++) {
				vector<int> values = resultVector[i].getValues();
				allValues.push_back(values);
			}
			
			for (int i = 0; i < numOfRows; i++) {
				vector<int> oneRow;
				for (unsigned int j = 0; j < allValues.size(); j++) {
					oneRow.push_back(allValues[j][i]);
				}
				
				string outputStr = "<";
				
				for (unsigned int j = 0; j < oneRow.size(); j++) {
					SYNONYM_TYPE synonymType = resultVector[j].getType();
					int individual = oneRow[j];
					string value = convertValueToString(individual, synonymType);
					outputStr += value;

					if (j + 1 != allValues.size()) {
						outputStr += ", ";
					}
				}
				
				outputStr += ">";
				resultList.push_back(outputStr);
			}
		}
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
