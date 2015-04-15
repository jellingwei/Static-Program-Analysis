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
	SYNONYM_TYPE list1array[] = { ASSIGN, CALL, IF, WHILE, STMT, PROG_LINE, PROCEDURE, STRING_INT, STRING_CHAR };
	vector<SYNONYM_TYPE> list1; list1.insert(list1.begin(), list1array, list1array + 9);
	relationshipArg1Map.insert(make_pair(QNODE_TYPE(ModifiesP), list1));

	//Modifies arguement 2
	SYNONYM_TYPE list2array[] = { VARIABLE, STRING_CHAR, UNDEFINED }; 
	vector<SYNONYM_TYPE> list2; list2.insert(list2.begin(), list2array, list2array + 3);
	relationshipArg2Map.insert(make_pair(QNODE_TYPE(ModifiesP), list2));

	//Uses argument 1 & 2
	relationshipArg1Map.insert(make_pair(QNODE_TYPE(UsesP), list1));
	relationshipArg2Map.insert(make_pair(QNODE_TYPE(UsesP), list2));

	SYNONYM_TYPE list3array[] = { ASSIGN, CALL, IF, WHILE, STMT, PROG_LINE, PROCEDURE, STRING_INT };
	vector<SYNONYM_TYPE> list3; list3.insert(list3.begin(), list3array, list3array + 8);
	relationshipArg1Map.insert(make_pair(QNODE_TYPE(ModifiesS), list3));
	relationshipArg2Map.insert(make_pair(QNODE_TYPE(ModifiesS), list2));

	//Uses* argument 1 & 2
	relationshipArg1Map.insert(make_pair(QNODE_TYPE(UsesS), list3));
	relationshipArg2Map.insert(make_pair(QNODE_TYPE(UsesS), list2));



	SYNONYM_TYPE list4array[] = { IF, WHILE, STMT, PROG_LINE, STRING_INT, UNDEFINED };
	vector<SYNONYM_TYPE> list4; list4.insert(list4.begin(), list4array, list4array + 6);

	SYNONYM_TYPE list5array[] = { ASSIGN, CALL, IF, WHILE, STMT, PROG_LINE, STRING_INT, UNDEFINED }; 
	vector<SYNONYM_TYPE> list5; list5.insert(list5.begin(), list5array, list5array + 8);
	
	//constant is invalid for Parent
	//Parent argument 1
	relationshipArg1Map.insert(make_pair(QNODE_TYPE(Parent), list4));
	//Parent argument 2
	relationshipArg2Map.insert(make_pair(QNODE_TYPE(Parent), list5));
	//ParentS argument 1
	relationshipArg1Map.insert(make_pair(QNODE_TYPE(ParentT), list4));
	//ParentS argument 2
	relationshipArg2Map.insert(make_pair(QNODE_TYPE(ParentT), list5));

	//Follows argument 1
	relationshipArg1Map.insert(make_pair(QNODE_TYPE(Follows), list5));
	//Follows argument 2
	relationshipArg2Map.insert(make_pair(QNODE_TYPE(Follows), list5));
	//FollowsS argument 1
	relationshipArg1Map.insert(make_pair(QNODE_TYPE(FollowsT), list5));
	//FollowsS argument 2
	relationshipArg2Map.insert(make_pair(QNODE_TYPE(FollowsT), list5));


	SYNONYM_TYPE list6array[] = { ASSIGN, STMT, PROG_LINE, STRING_INT, UNDEFINED };
	vector<SYNONYM_TYPE> list6; list6.insert(list6.begin(), list6array, list6array + 5);

	//Affects argument 1
	relationshipArg1Map.insert(make_pair(QNODE_TYPE(Affects), list6));
	//Affects argument 2
	relationshipArg2Map.insert(make_pair(QNODE_TYPE(Affects), list6));
	//AffectsS argument 1
	relationshipArg1Map.insert(make_pair(QNODE_TYPE(AffectsT), list6));
	//AffectsS argument 2
	relationshipArg2Map.insert(make_pair(QNODE_TYPE(AffectsT), list6));


	//AffectsBip argument 1
	relationshipArg1Map.insert(make_pair(QNODE_TYPE(AffectsBip), list6));
	//AffectsBip argument 2
	relationshipArg2Map.insert(make_pair(QNODE_TYPE(AffectsBip), list6));
	//AffectsBipT argument 1
	relationshipArg1Map.insert(make_pair(QNODE_TYPE(AffectsBipT), list6));
	//AffectsBipT argument 2
	relationshipArg2Map.insert(make_pair(QNODE_TYPE(AffectsBipT), list6));


	//Next
	SYNONYM_TYPE list7array[] = { ASSIGN, CALL, IF, STMT, WHILE, PROG_LINE, STRING_INT, UNDEFINED };
	vector<SYNONYM_TYPE> list7; list7.insert(list7.begin(), list7array, list7array + 8);
	//Next argument 1
	relationshipArg1Map.insert(make_pair(QNODE_TYPE(Next), list7));
	//Next argument 2
	relationshipArg2Map.insert(make_pair(QNODE_TYPE(Next), list7));
	//NextS argument 1
	relationshipArg1Map.insert(make_pair(QNODE_TYPE(NextT), list7));
	//NextS argument 2
	relationshipArg2Map.insert(make_pair(QNODE_TYPE(NextT), list7));


	//NextBip argument 1
	relationshipArg1Map.insert(make_pair(QNODE_TYPE(NextBip), list7));
	//NextBip argument 2
	relationshipArg2Map.insert(make_pair(QNODE_TYPE(NextBip), list7));
	//NextBipT argument 1
	relationshipArg1Map.insert(make_pair(QNODE_TYPE(NextBipT), list7));
	//NextBipT argument 2
	relationshipArg2Map.insert(make_pair(QNODE_TYPE(NextBipT), list7));


	//Calls argument 1
	SYNONYM_TYPE list8array[] = { PROCEDURE, STRING_CHAR, UNDEFINED }; 
	vector<SYNONYM_TYPE> list8; list8.insert(list8.begin(), list8array, list8array + 3);
	relationshipArg1Map.insert(make_pair(QNODE_TYPE(Calls), list8));
	//Calls argument 2
	relationshipArg2Map.insert(make_pair(QNODE_TYPE(Calls), list8));
	//CallsS argument 1
	relationshipArg1Map.insert(make_pair(QNODE_TYPE(CallsT), list8));
	//CallsS argument 2
	relationshipArg2Map.insert(make_pair(QNODE_TYPE(CallsT), list8));


	/* Contains */
	SYNONYM_TYPE list9array[] = { PROCEDURE, STRING_INT, STMTLST, STMT, ASSIGN, WHILE, IF, PLUS, MINUS, TIMES, PROG_LINE};
	vector<SYNONYM_TYPE> list9; list9.insert(list9.begin(), list9array, list9array + 11);

	SYNONYM_TYPE list10array[] = { STRING_INT, STMTLST, STMT, ASSIGN, CALL, WHILE, IF, PLUS, MINUS, TIMES, VARIABLE, CONSTANT, PROG_LINE}; 
	vector<SYNONYM_TYPE> list10; list10.insert(list10.begin(), list10array, list10array + 13);

	//constant is valid for Contains
	//Contains argument 1
	relationshipArg1Map.insert(make_pair(QNODE_TYPE(Contains), list9));
	//Contains argument 2
	relationshipArg2Map.insert(make_pair(QNODE_TYPE(Contains), list10));
	//ContainsT argument 1
	relationshipArg1Map.insert(make_pair(QNODE_TYPE(ContainsT), list9));
	//ContainsT argument 2
	relationshipArg2Map.insert(make_pair(QNODE_TYPE(ContainsT), list10));

	/* Siblings */
	SYNONYM_TYPE list11array[] = {PROCEDURE, STMTLST, STMT, ASSIGN, CALL, IF, WHILE, PROG_LINE, CONSTANT, VARIABLE, PLUS, MINUS, TIMES, STRING_INT }; 
	vector<SYNONYM_TYPE> list11; list11.insert(list11.begin(), list11array, list11array + 14);

	//Siblings argument 1
	relationshipArg1Map.insert(make_pair(QNODE_TYPE(Sibling), list11));
	//Siblings argument 2
	relationshipArg2Map.insert(make_pair(QNODE_TYPE(Sibling), list11));


	/* patterns queries */
	//assign/while patterns argument 1
	SYNONYM_TYPE list12array[] = { STRING_CHAR, VARIABLE, UNDEFINED };
	vector<SYNONYM_TYPE> list12; list12.insert(list12.begin(), list12array, list12array + 3);
	patternsArg1Map.insert(make_pair(ASSIGN, list12));  
	patternsArg1Map.insert(make_pair(WHILE, list12));
	patternsArg1Map.insert(make_pair(IF, list12));

	SYNONYM_TYPE list13array[] = {STMTLST, UNDEFINED};
	vector<SYNONYM_TYPE> list13; list13.insert(list13.begin(), list13array, list13array + 2);
	patternsArg2Map.insert(make_pair(IF, list13)); 
	patternsArg2Map.insert(make_pair(WHILE, list13)); 
	patternsArg3Map.insert(make_pair(IF, list13));

	vector<SYNONYM_TYPE> list14; 
	patternsArg2Map.insert(make_pair(ASSIGN, list14)); //empty vector means no restrictions on arg2 of pattern assign.
	patternsArg3Map.insert(make_pair(ASSIGN, list14));
	patternsArg3Map.insert(make_pair(WHILE, list14));


	/* init withAttrRefMap */
	//procName
	SYNONYM_TYPE with1array[] = { PROCEDURE, CALL };
	vector<SYNONYM_TYPE> with1; with1.insert(with1.begin(), with1array, with1array + 2);
	withAttrRefMap.insert(make_pair(SYNONYM_ATTRIBUTE(procName), with1));

	//varName
	SYNONYM_TYPE with2array[] = { VARIABLE };
	vector<SYNONYM_TYPE> with2; with2.insert(with2.begin(), with2array, with2array + 1);
	withAttrRefMap.insert(make_pair(SYNONYM_ATTRIBUTE(varName), with2));

	//value
	SYNONYM_TYPE with3array[] = { CONSTANT };
	vector<SYNONYM_TYPE> with3; with3.insert(with3.begin(), with3array, with3array + 1);
	withAttrRefMap.insert(make_pair(SYNONYM_ATTRIBUTE(value), with3));

	//stmt#
	SYNONYM_TYPE with4array[] = { STMT, ASSIGN, IF, WHILE, CALL };
	vector<SYNONYM_TYPE> with4; with4.insert(with4.begin(), with4array, with4array + 5);
	withAttrRefMap.insert(make_pair(SYNONYM_ATTRIBUTE(stmtNo), with4));

}

/**
 * Validate arguments of all such that clauses.
 * @param arg1 
 * @param arg2 
 * @return TRUE if the arguments are valid. FALSE if the arguments are not valid.
 */
BOOLEAN_ QueryValidator::validateSuchThatQueries(QNODE_TYPE type, Synonym arg1, Synonym arg2)
{	
	vector<SYNONYM_TYPE> listArg1;
	vector<SYNONYM_TYPE> listArg2;

	try {
		//if enum QNODE_TYPE is not found it throws an out of range exception
		listArg1 = relationshipArg1Map.at(type);
		listArg2 = relationshipArg2Map.at(type); 
	
	}catch (const std::out_of_range& oor){

		throw exception("QueryValidator error: Out of Range");
	}

	SYNONYM_TYPE arg1Type = arg1.getType();
	SYNONYM_TYPE arg2Type = arg2.getType();
	
	auto result1 = std::find(std::begin(listArg1), std::end(listArg1), arg1Type);
	auto result2 = std::find(std::begin(listArg2), std::end(listArg2), arg2Type);

	#ifdef DEBUG
		cout<<"arg1Type : "<<Synonym::convertToString(arg1Type)<<endl;
		cout<<"arg2Type : "<<Synonym::convertToString(arg2Type)<<endl;
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

	/* Contains requires extra validation checks, checking for static semantic errors*/
	if(type==Contains || type==ContainsT){

		// for both Contains and Contains*
		if(arg1Type==ASSIGN || arg1Type==PLUS || arg1Type==MINUS || arg1Type==TIMES){
			
			if(arg2Type!=PLUS && arg2Type!=MINUS && arg2Type!=TIMES && arg2Type!=CONSTANT && arg2Type!=VARIABLE){
				return false;
			}
		
		}

		if(type==Contains){

			if(arg1Type==PROCEDURE){

				if(arg2Type!=STMTLST)
					return false;

			}else if(arg1Type==STMTLST){

				if(arg2Type!=IF && arg2Type!=WHILE && arg2Type!=ASSIGN && arg2Type!=CALL && arg2Type!=STMT && arg2Type!=PROG_LINE && arg2Type!=STRING_INT)
					return false;

			}else if(arg1Type==WHILE || arg1Type==IF){

				if(arg2Type!=STMTLST && arg2Type!=VARIABLE)
					return false;

			}else if(arg1Type==STMT || arg1Type==PROG_LINE || arg1Type==STRING_INT){

				if(arg2Type!=PLUS && arg2Type!=MINUS && arg2Type!=TIMES && arg2Type!=CONSTANT && arg2Type!=VARIABLE && arg2Type!=STMTLST)
					return false;
			}

		}

	}
	

	return true;
}

/**
 * Validate arguments of all pattern clauses.
 * @param arg0 
 * @param arg1 
 * @param arg2 
 * @param arg3 
 * @return TRUE if the arguments are valid. FALSE if the arguments are not valid.
 */
BOOLEAN_ QueryValidator::validatePatternQueries(Synonym arg0, Synonym arg1, Synonym arg2, Synonym arg3)
{
	vector<SYNONYM_TYPE> listArg1, listArg2, listArg3;
	
	// can only be 'assign','while' or 'if' pattern type
	SYNONYM_TYPE patternType = arg0.getType();
	
	try {
		//if string is not found it throws an out of range exception. 
		listArg1 = patternsArg1Map.at(patternType);
		listArg2 = patternsArg2Map.at(patternType); 
		listArg3 = patternsArg3Map.at(patternType); 
	
	}catch (const std::out_of_range& oor){
		throw exception("QueryValidator error: Out of Range");
	}

	SYNONYM_TYPE arg1Type = arg1.getType();
	SYNONYM_TYPE arg2Type = arg2.getType();
	SYNONYM_TYPE arg3Type = arg3.getType();


	#ifdef DEBUG
		cout<<"arg1Type : "<<arg1Type<<endl;
		cout<<"arg2Type : "<<arg2Type<<endl;
		cout<<"arg3Type : "<<arg3Type<<endl;
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

	if(!listArg3.empty()){ // if there are restrictions place on argument 2

		auto result3 = std::find(std::begin(listArg3), std::end(listArg3), arg3Type);

		if(result3 == std::end(listArg3)){ // not inside list of type of argument 2
			return false;
		}
	}


	return true;
}


/**
 *	Ensures that both left and right hand side types are consistent. 
 *  @param arg1
 *  @param arg2
 *  @return TRUE if both left and right hand side types are consistent.
 *			FALSE if one side is INTEGER and the other side is character string. 
 */
BOOLEAN_ QueryValidator::validateWithQueries(Synonym arg1, Synonym arg2)
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
	if (arg1Type == SYNONYM_TYPE(STRING_INT)){

		LHS_integer = true;         //INTEGER

	}else if(arg1Type == SYNONYM_TYPE(STRING_CHAR)){

		LHS_integer = false;        //"IDENT"

	}else{

		//case 1: synonym.attrName
		if(withAttrRefMap.count(arg1Attr)>0){

			//case 1: synonym.attrName
			//checks if there are restrictions place on argument 1
			listArg1 = withAttrRefMap.at(arg1Attr);
		

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
				#ifdef DEBUG
					cout<<"arg1Attr, ref is a synonym, and synonym can only be prog_line"<<endl;
				#endif
				return false; //error. ref is a synonym, and synonym can only be prog_line
			
			}
		}
	}




	/* RHS */
	if (arg2Type == SYNONYM_TYPE(STRING_INT)){

		RHS_integer = true;         //INTEGER

	}else if (arg2Type == SYNONYM_TYPE(STRING_CHAR)){

		RHS_integer = false;        //"IDENT"

	}else{


		//case 1: synonym.attrName
		if(withAttrRefMap.count(arg2Attr)>0){

			//case 1: synonym.attrName
			//checks if there are restrictions place on argument 2
			listArg2 = withAttrRefMap.at(arg2Attr);
		
			
			auto result2 = std::find(std::begin(listArg2), std::end(listArg2), arg2Type);
		
			if(result2 == std::end(listArg2)){ // not inside list of type of argument 2
				return false;
			}


			//label if RHS is int type
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
				#ifdef DEBUG
					cout<<"arg2Attr, ref is a synonym, and synonym can only be prog_line"<<endl;
				#endif
				return false; //error. ref is a synonym, and synonym can only be prog_line
			
			}
		}
	}


	if(!LHS_integer && RHS_integer){

		#ifdef DEBUG
			cout<<"In QueryValidator for with clause, !LHS_int && RHS_int"<<endl;
		#endif

		return false;
	}else if(LHS_integer && !RHS_integer){

		#ifdef DEBUG
			cout<<"In QueryValidator for with clause, LHS_int && !RHS_int"<<endl;
		#endif

		return false;
	}


	return true;
}