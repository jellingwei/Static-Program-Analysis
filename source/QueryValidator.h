#pragma once

#include "Synonym.h"

class QueryValidator {
public:
	 QueryValidator();
	 ~ QueryValidator();
	//void initTable();
	bool validateSuchThatQueries(QNODE_TYPE type, Synonym arg1, Synonym arg2);
	bool validatePatternQueries(Synonym arg0, Synonym arg1, Synonym arg2);

protected:
	bool validateModifiesOrUsesArgs(Synonym arg1, Synonym arg2);//Validates the inputs for modifies and uses
	bool validateParentArgs(Synonym arg1, Synonym arg2);//Validates the inputs for parent
	bool validateFollowsArgs(Synonym arg1, Synonym arg2);//Validates the inputs for follows

	bool validateAssignPattern(Synonym arg0, Synonym arg1, Synonym arg2);//Validates the inputs for assign pattern queries
	bool validateWhilePattern(Synonym arg0, Synonym arg1, Synonym arg2);//Validates the inputs for while pattern queries
	//unordered_map<QNODE_TYPE, vector<string>> relationshipArg1Map; //maps the relationship to its type of argument 1
	//unordered_map<QNODE_TYPE, vector<string>> relationshipArg2Map; //maps the relationship to its type of argument 2
};