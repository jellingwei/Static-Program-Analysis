#pragma once

//testing table-driven approach
#include <iostream>
#include <algorithm>
#include <vector>
#include <string>
#include <iterator>
#include <unordered_map>

//catch std::out_of_range exception
#include <stdexcept> 

#include "QNode.h"
#include "Synonym.h"

/**
 * Checks to verify query with program design entity relationships rules. 
 */
class QueryValidator {
public:
	 QueryValidator();
	 ~ QueryValidator();
	void initTable(); //creates a program design entity relationships table
	bool validateSuchThatQueries(QNODE_TYPE type, Synonym arg1, Synonym arg2);//Validates all such that queries based on relationship table initialised. 
	bool validatePatternQueries(Synonym arg0, Synonym arg1, Synonym arg2);//Validates all pattern queries based on relationship table initialised. 
	bool validateWithQueries(Synonym arg1, Synonym arg2);

protected:
	bool validateModifiesOrUsesArgs(Synonym arg1, Synonym arg2);//Validates the inputs for modifies and uses
	bool validateParentArgs(Synonym arg1, Synonym arg2);//Validates the inputs for parent
	bool validateFollowsArgs(Synonym arg1, Synonym arg2);//Validates the inputs for follows

	std::tr1::unordered_map<QNODE_TYPE, vector<SYNONYM_TYPE>> relationshipArg1Map; //maps the relationship to its type of argument 1
	std::tr1::unordered_map<QNODE_TYPE, vector<SYNONYM_TYPE>> relationshipArg2Map; //maps the relationship to its type of argument 2
	std::tr1::unordered_map<SYNONYM_TYPE, vector<SYNONYM_TYPE>> patternsArg1Map; //maps the relationship to its type of argument 1
	std::tr1::unordered_map<SYNONYM_TYPE, vector<SYNONYM_TYPE>> patternsArg2Map; //maps the relationship to its type of argument 2
	std::tr1::unordered_map<SYNONYM_ATTRIBUTE, vector<SYNONYM_TYPE>> withAttrRefMap;
	
};