//Validating with PQL grammar

#pragma once

#include "QNode.h"
#include "QueryValidator.h"

QueryValidator::QueryValidator() {
}
QueryValidator::~QueryValidator() {
}

/**
* Method to validate the arguments of all such that clause
* Returns true if the arguments are valid, false otherwise
*/
bool QueryValidator::validateSuchThatQueries(QNODE_TYPE type, Synonym arg1, Synonym arg2){
	
	switch (type) {

		case Modifies:
		case Uses:
			return validateModifiesOrUsesArgs(arg1, arg2);
		case Parent:
		case ParentS:
			return validateParentArgs(arg1, arg2);
		case Follows:
			return validateFollowsArgs(arg1, arg2);
		default:
			return false;

	}

}
/**
 * Method to validate the arguments of modifies or uses clause
 * Returns true if the arguments are valid, false otherwise
 */
bool QueryValidator::validateModifiesOrUsesArgs(Synonym arg1, Synonym arg2){
	//arg1 must be a statement(but not constant) and cannot have "_"
	//arg2 must be a character, variable or "_"
	string arg1Type = arg1.getType();
	string arg2Type = arg2.getType();
	if (arg1Type != "String" && arg1Type != "stmt" && arg1Type != "assign" && 
		arg1Type != "while" && arg1Type != "prog_line")
	{
		return false;
	}
	if (arg2Type != "String" && arg2Type != "variable" && arg2Type != "_")
	{
		return false;
	}

	if (arg1Type != "String" && arg2Type != "String" && arg1.getName() == arg2.getName())
	{
		return false;  //arg1 and arg2 cannot have the same names if they are synoyms
	}

	return true;  //All the types are valid
}

/**
 * Method to validate the arguments of parent or follows clause
 * Returns true if the arguments are valid, false otherwise
 */
bool QueryValidator::validateParentArgs(Synonym arg1, Synonym arg2){
	//arg1 and arg2 must be numbers(but not assign, constants), number synonyms or "_"
	string arg1Type = arg1.getType();
	string arg2Type = arg2.getType();
	if (arg1Type != "String" && arg1Type != "stmt" && arg1Type != "_" && 
		arg1Type != "while" && arg1Type != "prog_line")
	{
		return false;
	}
	if (arg2Type != "String" && arg2Type != "stmt" && arg2Type != "assign" && arg2Type != "_" && 
		arg2Type != "while" && arg2Type != "prog_line")
	{
		return false;
	}

	return true;  //All the types are valid
}

/**
 * Method to validate the arguments of parent or follows clause
 * Returns true if the arguments are valid, false otherwise
 */
bool QueryValidator::validateFollowsArgs(Synonym arg1, Synonym arg2){
	//arg1 and arg2 must be numbers(but not constants), number synonyms or "_"
	string arg1Type = arg1.getType();
	string arg2Type = arg2.getType();
	if (arg1Type != "String" && arg1Type != "stmt" && arg1Type != "assign" && arg1Type != "_" && 
		arg1Type != "while" && arg1Type != "prog_line")
	{
		return false;
	}
	if (arg2Type != "String" && arg2Type != "stmt" && arg2Type != "assign" && arg2Type != "_" && 
		arg2Type != "while" && arg2Type != "prog_line")
	{
		return false;
	}

	return true;  //All the types are valid
}