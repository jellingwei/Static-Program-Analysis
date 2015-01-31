//Validating with PQL grammar

#pragma once
#include <cctype>  //For isdigit and isalpha

#include "QueryValidator.h"

using namespace std;

//To print out debug error msges
//#ifndef DEBUG
//#define DEBUG 
//#endif

QueryValidator::QueryValidator() {
}
QueryValidator::~QueryValidator() {
}


/**
 * Initialise program design entity relationships table. A table 
 * approach is used for flexibility in changing its relationship. 
 * Currently, Visual studio C++ dont support  initializer lists. 
 * A string array is used to create a vector <string>. 
 */
void QueryValidator::initTable()
{
	//Modifies argument 1
	string list1array[] = { "stmt", "assign", "while", "prog_line", "string-int" };
	vector<string> list1; list1.insert(list1.begin(), list1array, list1array + 5);
	relationshipArg1Map.insert(make_pair(QNODE_TYPE(Modifies), list1));

	//Modifies arguement 2
	string list2array[] = { "variable", "string-char", "_" };
	vector<string> list2; list2.insert(list2.begin(), list2array, list2array + 3);
	relationshipArg2Map.insert(make_pair(QNODE_TYPE(Modifies), list2));

	//Uses argument 1
	relationshipArg1Map.insert(make_pair(QNODE_TYPE(Uses), list1));
	//Uses argument 2
	relationshipArg2Map.insert(make_pair(QNODE_TYPE(Uses), list2));

	//Parent argument 1
	string list3array[] = { "stmt", "while", "prog_line", "string-int", "_" };
	vector<string> list3; list3.insert(list3.begin(), list3array, list3array + 5);
	relationshipArg1Map.insert(make_pair(QNODE_TYPE(Parent), list3));

	//Parent argument 2
	string list4array[] = { "stmt", "assign", "while", "prog_line", "string-int", "_" };
	vector<string> list4; list4.insert(list4.begin(), list4array, list4array + 6);
	relationshipArg2Map.insert(make_pair(QNODE_TYPE(Parent), list4));

	//ParentS argument 1
	relationshipArg1Map.insert(make_pair(QNODE_TYPE(ParentS), list3));
	//ParentS argument 2
	relationshipArg2Map.insert(make_pair(QNODE_TYPE(ParentS), list4));

	//Follows argument 1
	string list5array[] = { "stmt", "assign", "while", "prog_line", "string-int", "_" };
	vector<string> list5; list5.insert(list5.begin(), list5array, list5array + 6);
	relationshipArg1Map.insert(make_pair(QNODE_TYPE(Follows), list5));

	//Follows argument 2
	relationshipArg2Map.insert(make_pair(QNODE_TYPE(Follows), list5));

	//FollowsS argument 1
	relationshipArg1Map.insert(make_pair(QNODE_TYPE(FollowsS), list5));

	//FollowsS argument 2
	relationshipArg2Map.insert(make_pair(QNODE_TYPE(FollowsS), list5));

	//Calls argument 1
	string list6array[] = { "procedure","string-char", "_"};
	vector<string> list6; list6.insert(list6.begin(), list6array, list6array + 3);
	relationshipArg1Map.insert(make_pair(QNODE_TYPE(Calls), list6));
	//Calls argument 2
	relationshipArg2Map.insert(make_pair(QNODE_TYPE(Calls), list6));
	//CallsS argument 1
	relationshipArg1Map.insert(make_pair(QNODE_TYPE(CallsS), list6));
	//CallsS argument 2
	relationshipArg2Map.insert(make_pair(QNODE_TYPE(CallsS), list6));

}

/**
 * Method to validate arguments of all such that clauses.
 * Returns true if the arguments are valid, false otherwise.
 */
bool QueryValidator::validateSuchThatQueries(QNODE_TYPE type, Synonym arg1, Synonym arg2)
{	
	vector<string> listArg1;
	vector<string> listArg2;

	cout<< "HEREEEE"<<endl;

	try {
		//if enum QNODE_TYPE is not found it throws an out of range exception
		listArg1 = relationshipArg1Map.at(type);
		listArg2 = relationshipArg2Map.at(type); 
	
	}catch (const std::out_of_range& oor){
		#ifdef DEBUG
			cout<< "QueryValidator error: out of range"<<endl;
		#endif

		throw exception("QueryValidator error: Out of Range");
	}

	string arg1Type = arg1.getType();
	string arg2Type = arg2.getType();
	
	if (arg1Type == "String"){

		char arg1Value = arg1.getName()[0];  //Get the value of arg1
		
		if (isdigit(arg1Value)){
			arg1Type = "string-int";
		}else if (isalpha(arg1Value)){
			arg1Type = "string-char";
		}else{
			arg1Type = "string-mix";	
		}
	}

	if (arg2Type == "String"){

		char arg2Value = arg2.getName()[0];  //Get the value of arg2
		
		if (isdigit(arg2Value)){
			arg2Type = "string-int";
		}else if (isalpha(arg2Value)){
			arg2Type = "string-char";
		}else{
			arg2Type = "string-mix";	
		}
	}

	auto result1 = std::find(std::begin(listArg1), std::end(listArg1), arg1Type);
	auto result2 = std::find(std::begin(listArg2), std::end(listArg2), arg2Type);

	#ifdef DEBUG
		cout<< "arg1Type... "<<arg1Type<<endl;
		cout<< "arg2Type... "<<arg2Type<<endl;
	#endif

	if(result1 == std::end(listArg1)){ // not inside list of type of argument 1
		return false;
	}
	if(result2 == std::end(listArg2)){ // not inside list of type of argument 2
		return false;
	}

	//Since the two are constant strings, they must be digits by the checks above
	if ((arg1Type == "string-int" && arg2Type == "string-int") &&
		(stoi(arg1.getName()) >= stoi(arg2.getName())) ){

		return false;  //arg1 must be smaller than arg2 or else it is false
	}	
	if(arg1Type!="string-int" && arg2Type!="string-int" &&
		arg1Type!="string-char" && arg2Type!="string-char" && 
		arg1Type!="_" && arg2Type!="_" &&
		arg1.getName() == arg2.getName()){

		return false; //arg1 and arg2 cannot have the same names if they are synoyms
	}
	


	return true;

}

/**
 * Method to validate arguments of all pattern clauses.
 * Returns true if the arguments are valid, false otherwise.
 */
bool QueryValidator::validatePatternQueries(Synonym arg0, Synonym arg1, Synonym arg2)
{
	string patternType = arg0.getType();

	if(patternType == "assign"){
		return validateAssignPattern(arg0, arg1, arg2);
	}else if(patternType == "while"){
		return validateWhilePattern(arg0, arg1, arg2);
	}

	return true;
}

/**
 * Supporting function for validating pattern queries. 
 * It handles assign pattern type. 
 */
bool QueryValidator::validateAssignPattern(Synonym arg0, Synonym arg1, Synonym arg2)
{
	string arg1Type = arg1.getType();
	if (arg1Type != "String" && arg1Type != "variable" && arg1Type != "_"){
			return false;
	}
	return true;
}

/**
 * Supporting function for validating pattern queries. 
 * It handles while pattern type. 
 */
bool QueryValidator::validateWhilePattern(Synonym arg0, Synonym arg1, Synonym arg2)
{
	string arg1Type = arg1.getType();
	if (arg1Type != "String" && arg1Type != "variable" && arg1Type != "_"){
		return false;  //arg1 can only be a constant string or a variable synonym or "_"
	}

	if (arg2.getName() != "_"){
		return false;  //arg2 must be "_"
	}

	return true;
}


