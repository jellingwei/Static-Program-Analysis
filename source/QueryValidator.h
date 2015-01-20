#pragma once

#include "Synonym.h"

class QueryValidator {
public:
	 QueryValidator();
	 ~ QueryValidator();
	bool validateSuchThatQueries(QNODE_TYPE type, Synonym arg1, Synonym arg2);
protected:
	bool validateModifiesOrUsesArgs(Synonym arg1, Synonym arg2);//Validates the inputs for modifies and uses
	bool validateParentArgs(Synonym arg1, Synonym arg2);//Validates the inputs for parent
	bool validateFollowsArgs(Synonym arg1, Synonym arg2);//Validates the inputs for follows

};