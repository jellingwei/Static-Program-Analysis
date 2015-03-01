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
	/* such that queries */

	//Modifies argument 1
	string list1array[] = { "assign", "call", "if", "while", "stmt", "prog_line", "procedure", "string-int" };
	vector<string> list1; list1.insert(list1.begin(), list1array, list1array + 8);
	relationshipArg1Map.insert(make_pair(QNODE_TYPE(Modifies), list1));

	//Modifies arguement 2
	string list2array[] = { "variable", "string-char", "_" };
	vector<string> list2; list2.insert(list2.begin(), list2array, list2array + 3);
	relationshipArg2Map.insert(make_pair(QNODE_TYPE(Modifies), list2));

	//Uses argument 1
	relationshipArg1Map.insert(make_pair(QNODE_TYPE(Uses), list1));
	//Uses argument 2
	relationshipArg2Map.insert(make_pair(QNODE_TYPE(Uses), list2));


	string list3array[] = { "if", "while", "stmt", "prog_line", "string-int", "_" };
	vector<string> list3; list3.insert(list3.begin(), list3array, list3array + 6);

	string list4array[] = { "assign", "call", "if", "while", "stmt", "prog_line", "string-int", "_" };
	vector<string> list4; list4.insert(list4.begin(), list4array, list4array + 8);
	
	//constant is invalid for Parent
	//Parent argument 1
	relationshipArg1Map.insert(make_pair(QNODE_TYPE(Parent), list3));

	//Parent argument 2
	relationshipArg2Map.insert(make_pair(QNODE_TYPE(Parent), list4));

	//ParentS argument 1
	relationshipArg1Map.insert(make_pair(QNODE_TYPE(ParentS), list3));
	//ParentS argument 2
	relationshipArg2Map.insert(make_pair(QNODE_TYPE(ParentS), list4));

	//Follows argument 1
	relationshipArg1Map.insert(make_pair(QNODE_TYPE(Follows), list4));

	//Follows argument 2
	relationshipArg2Map.insert(make_pair(QNODE_TYPE(Follows), list4));

	//FollowsS argument 1
	relationshipArg1Map.insert(make_pair(QNODE_TYPE(FollowsS), list4));

	//FollowsS argument 2
	relationshipArg2Map.insert(make_pair(QNODE_TYPE(FollowsS), list4));


	string list5array[] = { "assign", "stmt", "prog_line", "string-int", "_" };
	vector<string> list5; list5.insert(list5.begin(), list5array, list5array + 5);

	//Affects argument 1
	relationshipArg1Map.insert(make_pair(QNODE_TYPE(Affects), list5));

	//Affects argument 2
	relationshipArg2Map.insert(make_pair(QNODE_TYPE(Affects), list5));
	
	//AffectsS argument 1
	relationshipArg1Map.insert(make_pair(QNODE_TYPE(AffectsS), list5));

	//AffectsS argument 2
	relationshipArg2Map.insert(make_pair(QNODE_TYPE(AffectsS), list5));

	//Next
	string list6array[] = { "assign", "call", "if", "stmt", "while", "prog_line","string-int", "_"};
	vector<string> list6; list6.insert(list6.begin(), list6array, list6array + 8);
	//Next argument 1
	relationshipArg1Map.insert(make_pair(QNODE_TYPE(Next), list6));
	//Next argument 2
	relationshipArg2Map.insert(make_pair(QNODE_TYPE(Next), list6));
	//NextS argument 1
	relationshipArg1Map.insert(make_pair(QNODE_TYPE(NextS), list6));
	//NextS argument 2
	relationshipArg2Map.insert(make_pair(QNODE_TYPE(NextS), list6));


	//Calls argument 1
	string list7array[] = { "procedure","string-char", "_"};
	vector<string> list7; list7.insert(list7.begin(), list7array, list7array + 3);
	relationshipArg1Map.insert(make_pair(QNODE_TYPE(Calls), list7));
	//Calls argument 2
	relationshipArg2Map.insert(make_pair(QNODE_TYPE(Calls), list7));
	//CallsS argument 1
	relationshipArg1Map.insert(make_pair(QNODE_TYPE(CallsS), list7));
	//CallsS argument 2
	relationshipArg2Map.insert(make_pair(QNODE_TYPE(CallsS), list7));


	/* patterns queries */
	//assign/while patterns argument 1
	string list8array[] = {"string","variable", "_"};
	vector<string> list8; list8.insert(list8.begin(), list8array, list8array + 3);
	patternsArg1Map.insert(make_pair("assign", list8));  
	patternsArg1Map.insert(make_pair("while", list8));
	patternsArg1Map.insert(make_pair("if", list8));

	vector<string> list9; //empty vector means no restrictions on arg2 of pattern assign.
	patternsArg2Map.insert(make_pair("assign", list9)); 
	patternsArg2Map.insert(make_pair("if", list9)); 

	//while patterns argument 2
	string list10array[] = {"_"};
	vector<string> list10; list10.insert(list10.begin(), list10array, list10array + 1);
	patternsArg2Map.insert(make_pair("while", list10));




	/* init withAttrRefMap */
	//procName
	SYNONYM_TYPE with1array[] = {SYNONYM_TYPE(PROCEDURE),SYNONYM_TYPE(CALL)};
	vector<SYNONYM_TYPE> with1; with1.insert(with1.begin(), with1array, with1array + 2);
	withAttrRefMap.insert(make_pair(SYNONYM_ATTRIBUTE(procName), with1));

	//varName
	SYNONYM_TYPE with2array[] = {SYNONYM_TYPE(VARIABLE)};
	vector<SYNONYM_TYPE> with2; with2.insert(with2.begin(), with2array, with2array + 1);
	withAttrRefMap.insert(make_pair(SYNONYM_ATTRIBUTE(varName), with2));

	//value
	SYNONYM_TYPE with3array[] = {SYNONYM_TYPE(CONSTANT)};
	vector<SYNONYM_TYPE> with3; with3.insert(with3.begin(), with3array, with3array + 1);
	withAttrRefMap.insert(make_pair(SYNONYM_ATTRIBUTE(value), with3));

	//stmt#
	SYNONYM_TYPE with4array[] = {SYNONYM_TYPE(STMT), SYNONYM_TYPE(ASSIGN), SYNONYM_TYPE(IF), 
								SYNONYM_TYPE(WHILE), SYNONYM_TYPE(CALL)};
	vector<SYNONYM_TYPE> with4; with4.insert(with4.begin(), with4array, with4array + 5);
	withAttrRefMap.insert(make_pair(SYNONYM_ATTRIBUTE(stmtNo), with4));

}

/**
 * Method to validate arguments of all such that clauses.
 * Returns true if the arguments are valid, false otherwise.
 */
bool QueryValidator::validateSuchThatQueries(QNODE_TYPE type, Synonym arg1, Synonym arg2)
{	
	vector<string> listArg1;
	vector<string> listArg2;

	try {
		//if enum QNODE_TYPE is not found it throws an out of range exception
		listArg1 = relationshipArg1Map.at(type);
		listArg2 = relationshipArg2Map.at(type); 
	
	}catch (const std::out_of_range& oor){

		throw exception("QueryValidator error: Out of Range");
	}

	string arg1Type = Synonym::convertToString(arg1.getType());
	string arg2Type = Synonym::convertToString(arg2.getType());
	
	if (arg1Type == "string"){

		char arg1Value = arg1.getName()[0];  //Get the value of arg1
		
		if (isdigit(arg1Value)){
			arg1Type = "string-int";
		}else if (isalpha(arg1Value)){
			arg1Type = "string-char";
		}else{
			arg1Type = "string-mix";	
		}
	}

	if (arg2Type == "string"){

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
		cout<<"arg1Type : "<<arg1Type<<endl;
		cout<<"arg2Type : "<<arg2Type<<endl;
	#endif


	if(result1 == std::end(listArg1)){ // not inside list of type of argument 1
		#ifdef DEBUG
			throw exception("QueryValidator error: not inside list of type of argument 1.");
		#endif
		
		return false;
	}
	if(result2 == std::end(listArg2)){ // not inside list of type of argument 2
		#ifdef DEBUG
			throw exception("QueryValidator error: not inside list of type of argument 2.");
		#endif
		return false;
	}

	if((type != QNODE_TYPE(Next)) && (type != QNODE_TYPE(NextS))){
		//Since the two are constant strings, they must be digits by the checks above
		if ((arg1Type == "string-int" && arg2Type == "string-int") &&
			(stoi(arg1.getName()) >= stoi(arg2.getName())) ){

			#ifdef DEBUG
				throw exception("QueryValidator error: arg1 must be smaller than arg2 or else it is false.");
			#endif
			return false;  //arg1 must be smaller than arg2 or else it is false
		
		}	
	
		if(arg1Type!="string-int" && arg2Type!="string-int" &&
			arg1Type!="string-char" && arg2Type!="string-char" && 
			arg1Type!="_" && arg2Type!="_" &&
			arg1.getName() == arg2.getName()){

			#ifdef DEBUG
				throw exception("QueryValidator error:arg1 and arg2 cannot have the same names if they are synonyms.");
			#endif
			return false; //arg1 and arg2 cannot have the same names if they are synonyms
		
		}
	}

	return true;

}

/**
 * Method to validate arguments of all pattern clauses.
 * Returns true if the arguments are valid, false otherwise.
 */
bool QueryValidator::validatePatternQueries(Synonym arg0, Synonym arg1, Synonym arg2)
{
	vector<string> listArg1;
	vector<string> listArg2;
	
	// can only be 'assign' or 'while' pattern type
	string patternType = Synonym::convertToString(arg0.getType());
	
	try {
		//if string is not found it throws an out of range exception. 
		listArg1 = patternsArg1Map.at(patternType);
		listArg2 = patternsArg2Map.at(patternType); 
	
	}catch (const std::out_of_range& oor){
		throw exception("QueryValidator error: Out of Range");
	}



	string arg1Type = Synonym::convertToString(arg1.getType());
	string arg2Type = Synonym::convertToString(arg2.getType());

	#ifdef DEBUG
		cout<<"arg1Type : "<<arg1Type<<endl;
		cout<<"arg2Type : "<<arg2Type<<endl;
	#endif



	if(!listArg1.empty()){ // if there are restrictions place on argument 1
		
		auto result1 = std::find(std::begin(listArg1), std::end(listArg1), arg1Type);
	
		if(result1 == std::end(listArg1)){ // not inside list of type of argument 1
			return false;
		}
	}

	if(!listArg2.empty()){ // if there are restrictions place on argument 2

		auto result2 = std::find(std::begin(listArg2), std::end(listArg2), arg2Type);

		if(result2 == std::end(listArg2)){ // not inside list of type of argument 2
			return false;
		}
	}


	return true;
}


/**
 *	Ensures that both left and right hand side types are consistent. 
 *  @return false if one side is INTEGER and the other side is character string. 
 */
bool QueryValidator::validateWithQueries(Synonym arg1, Synonym arg2)
{
	
	bool LHS_integer; // true if it is integer type. false if it is char type. 
	bool RHS_integer;
	vector<SYNONYM_TYPE> listArg1;
	vector<SYNONYM_TYPE> listArg2;


	SYNONYM_TYPE arg1Type = arg1.getType();
	SYNONYM_ATTRIBUTE arg1Attr = arg1.getAttribute();
	SYNONYM_TYPE arg2Type = arg2.getType();
	SYNONYM_ATTRIBUTE arg2Attr = arg2.getAttribute();


	/* LHS */
	if (arg1Type == SYNONYM_TYPE(STRING)){

		char arg1Value = arg1.getName()[0];  //Get the value of arg1
		
		if (isdigit(arg1Value)){

			LHS_integer = true;         //INTEGER

		}else if (isalpha(arg1Value)){

			LHS_integer = false;        //"IDENT"
		
		}else{
			LHS_integer = false;	    //"IDENT"
		}

	}else{

		listArg1 = withAttrRefMap.at(arg1Attr);

		//case 1: synonym.attrName
		if(!listArg1.empty()){ // if there are restrictions place on argument 1
			
			auto result1 = std::find(std::begin(listArg1), std::end(listArg1), arg1Type);
		
			if(result1 == std::end(listArg1)){ // not inside list of type of argument 1
				return false;
			}


			//label if LHS is int type
			if(arg1Attr == SYNONYM_ATTRIBUTE(procName) || arg1Attr == SYNONYM_ATTRIBUTE(varName)){

				LHS_integer = false;
			}else{
				LHS_integer = true; //stmt#, value
			}	


		}else{
		//case 2: synonym

			if(arg1Type == SYNONYM_TYPE(PROG_LINE)){

				LHS_integer = true;
			
			}else{

				cout<<"arg1Attr, ref is a synonym, and synonym can only be prog_line"<<endl;
				return false; //error. ref is a synonym, and synonym can only be prog_line
			
			}
		}
	}




	/* RHS */
	if (arg2Type == SYNONYM_TYPE(STRING)){

		char arg2Value = arg2.getName()[0];  //Get the value of arg2
		
		if (isdigit(arg2Value)){

			RHS_integer = true;         //INTEGER

		}else if (isalpha(arg2Value)){

			RHS_integer = false;        //"IDENT"
		
		}else{
			RHS_integer = false;	    //"IDENT"
		}
	}else{

		listArg2 = withAttrRefMap.at(arg2Attr);

		//synonym.attrName
		if(!listArg2.empty()){ // if there are restrictions place on argument 2
			
			auto result2 = std::find(std::begin(listArg2), std::end(listArg2), arg2Type);
		
			if(result2 == std::end(listArg2)){ // not inside list of type of argument 2
				return false;
			}


			//label if LHS is int type
			if(arg2Attr == SYNONYM_ATTRIBUTE(procName) || arg2Attr == SYNONYM_ATTRIBUTE(varName)){

				RHS_integer = false;
			}else{
				RHS_integer = true; //stmt#, value
			}	


		}else{
		//synonym

			if(arg2Type == SYNONYM_TYPE(PROG_LINE)){

				RHS_integer = true;
			
			}else{
				
				cout<<"arg2Attr, ref is a synonym, and synonym can only be prog_line"<<endl;
				return false; //error. ref is a synonym, and synonym can only be prog_line
			
			}
		}
	}


	if(!LHS_integer && RHS_integer)
		return false;
	else if(LHS_integer && !RHS_integer)
		return false;


	return true;
}