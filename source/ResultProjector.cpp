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
		for (unsigned int i = 0; i < resultVector.size(); i++) {
			SYNONYM_TYPE synonymType = resultVector[i].getType();
			
			if (synonymType == BOOLEAN) {
				resultList.push_back(resultVector[i].getName());
				return;
			}
			
			vector<int> resultSet = resultVector[i].getValues();
			for (vector<int>::iterator itr = resultSet.begin(); itr != resultSet.end(); ++itr) {
				string value = convertValueToString(*itr, synonymType);
				resultList.push_back(value);
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
