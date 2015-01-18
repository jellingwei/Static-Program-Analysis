/* Program: QueryParser.cpp
   Author: Si Ling, referencing SPA parser by HongJin
   Description: QueryParser checks the query input to see if it matches with the PQL grammar. 
   				It reads in the queries and builds a query tree. 
*/
#pragma once

//To print out debug error msges
//#ifndef DEBUG
//#define DEBUG 
//#endif

#include<stdio.h>
#include <unordered_map>
#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <sstream>
#include <regex>
   
#include "QueryParser.h"
#include "InputQueryParser.h"
#include "Synonym.h"
#include "QueryTree.h"

using std::string;
using std::vector;

namespace QueryParser{

	ifstream inputFile;
	vector<string> buffer;
	vector<string>::iterator bufferIter;
	string currentParsedLine;

	string designEntities[] = {"stmt","assign","while","variable","constant","prog_line"};
	string relRef[] = {"Modifies", "Uses", "Parent", "Parent*", "Follows", "Follows*"};
	QueryTree* myQueryTree;
	unordered_map<string, string> synonymsMap; //key: synonyms
	unordered_map<string, QNODE_TYPE> nodetypeMap; //key: synonyms

	/**
	 * Initialise the parser with a filename
	 */
	bool initParser(string query){

		buffer.clear();
		if (query.size() == 0) {

			#ifdef DEBUG
				cout<< "ERROR - In initParser: empty query string"<<endl;
			#endif

			return false;
		}

		// remove blocks of multiple whitespace
		string multipleSpaces = "[\\s]";
		std::tr1::regex separatorRegex(multipleSpaces);
		std::tr1::sregex_token_iterator reg_end;

		std::tr1::sregex_token_iterator rs(query.begin(), query.end(), separatorRegex, -1);

		// tokenise words and operators
		string operators = "([\\w\\d\\*]+|[_+;,(\\)\"])";
		std::regex operRegex(operators);

		for (; rs != reg_end; ++rs) {
			std::smatch match;
			string res(rs->str());
			
			while (std::regex_search(res, match, operRegex)) {
				if (match.empty()) {
					break;
				}

				buffer.push_back(match[0]);
				res = match.suffix().str();
			} 
		}
	
		#ifdef DEBUG
			cout<< "Initialise Query Parser is done"<<endl;
		#endif

		bufferIter = buffer.begin();

		return buffer.size() > 0;
	}

	/**
	 * Returns the next token
	 */
	string parseToken(){

		if (buffer.size() && bufferIter != buffer.end()) {
			string currToken= (*(bufferIter ++));

			//cout<<"*******print parse token *****"<<endl;
			//cout<<currToken<<endl;
			//cout<<"******************************"<<endl;
			return currToken;
		} 
		#ifdef DEBUG
			cout<<"QueryParser: token is null"<<endl;
		#endif

		return "";
	}


	void testingQueryParser(){
			string nxtToken = parseToken();
			while (nxtToken.compare("")!= 0)
				nxtToken = parseToken();
			cout<<endl;
	}
	/**************************************************************/
	/**   Supporting Functions - To read the buffer of queries   **/
	/**************************************************************/
	string peekInToTheNextToken(){
		return (*(bufferIter));
	}

	/**
	 * Returns the token these number of steps backwards from the currToken.
	 * If steps = 0 , it returns the currToken
	 * If steps = 1, it returns the previous token
	 */
	string peekBackwards(int steps){
		return (*(bufferIter-1-steps));
	}

	/**************************************************************/
	/**     Supporting functions - To build query tree           **/
	/**************************************************************/
		
	bool initQueryTreeAndSymbolsTable(){
		pair<string,QNODE_TYPE> pairNodeType;
		
		myQueryTree = new QueryTree();
		
		synonymsMap.clear();
		nodetypeMap.clear();

		//populate nodetypeMap
		pair<string,QNODE_TYPE> pairNodeType1 ("Modifies", QNODE_TYPE(Modifies));
		nodetypeMap.insert(pairNodeType1);
		pair<string,QNODE_TYPE> pairNodeType2 ("Uses",QNODE_TYPE(Uses));
		nodetypeMap.insert(pairNodeType2);
		pair<string,QNODE_TYPE> pairNodeType3 ("Parent",QNODE_TYPE(Parent));
		nodetypeMap.insert(pairNodeType3);
		pair<string,QNODE_TYPE> pairNodeType4 ("Parent*",QNODE_TYPE(ParentS));
		nodetypeMap.insert(pairNodeType4);
		pair<string,QNODE_TYPE> pairNodeType5 ("Follows",QNODE_TYPE(Follows));
		nodetypeMap.insert(pairNodeType5);
		pair<string,QNODE_TYPE> pairNodeType6 ("Follows*",QNODE_TYPE(FollowsS));
		nodetypeMap.insert(pairNodeType6);

		return true;
	}

	/**
	 * prints out the query tree onto console (for testing and debugging)
	 **/
	void queryTreeTesting(){
		myQueryTree->printTree();

	}

	/**************************************************************/
	/**                     Matching                             **/
	/**************************************************************/

	/**
	 * Matches if the given token follows the naming convention for NAME, as per the given grammar
	 */
	bool matchName(string token){
		std::regex nameRegex("[A-Za-z][\\w]*");

		return std::regex_match(token, nameRegex) ? true : false;
	}

	/**
	 * Matches if the given token follows the naming convention for INTEGER, as per the given grammar
	 */
	bool matchInteger(string token){
		std::regex intRegex("\\d+");
		return (std::regex_match(token,intRegex)) ? true : false;
	}

	bool matchFactor(string token){

		if(!(matchInteger(token) || matchName(token))){
			#ifdef DEBUG
				cout<< "QueryParser error : at matchFactor."<<endl;
			#endif
		}
		return (matchInteger(token) || matchName(token));
	}
	/**
	 *  Checks if it's a factor 
	 **/
	bool isFactor(string token){
		return (matchInteger(token) || matchName(token));
	}
	/**
	 *Matches if the given token follows the naming convention of Synonym and IDENT
	 */
	bool matchSynonymAndIdent(string token, bool comma){
		std::regex synonymRegex("");

		if (comma){
			std::regex synonymRegex("[\"+][A-Za-z][A-Za-z0-9#]*[\"+]");
			return (std::regex_match(token,synonymRegex)) ? true : false;
		}
		else {
			std::regex synonymRegex("[A-Za-z][A-Za-z0-9#]*");
			return (std::regex_match(token,synonymRegex)) ? true : false;
		}

		//won't reach here
		return false;
	}

	/**
	 *Matches if the given token follows the naming convention of entity reference
	 */
	bool matchUnderscore(string token){
		return (token.compare("_") == 0);
	}

	/**
	 *Matches if the given token follows the naming convention of stmt reference
	 */
	bool matchStmtRef(string token){
		if(matchSynonymAndIdent(token,false)){
			return true;
		}
		else if(matchInteger(token)){
			return true;
		}
		else if(matchUnderscore(token)){
			return true;
		}

		#ifdef DEBUG
			cout<<"QueryParser error : at matchStmtRef"<<endl;
		#endif
		return false;
	}

	/**
	 *Matches if the given token follows the naming convention of entity reference
	 */
	bool matchEntRef(string token){
		if(matchSynonymAndIdent(token, false))
			return true;
		else if(matchUnderscore(token))
			return true;
		else if(matchSynonymAndIdent(token, true))
			return true;
	
		#ifdef DEBUG
			cout<<"QueryParser error : at matchEntRef"<<endl;
		#endif

		return false;
	}

	bool matchDesignEntity(string token){
		for(int i=0; i<(sizeof(designEntities)/sizeof(*designEntities)); i++){
			if(designEntities[i].compare(token) == 0)
				return true;
		}

		#ifdef DEBUG
			cout<< "QueryParser error : at matchDesignEntity."<<endl;
		#endif
		
		return false;
	}
	/**************************************************************/
	/**                      Parsing                             **/
	/**************************************************************/

	/**
	 * Parses the next token and check if it is equal to the given target
	 */
	bool parse(string target){
		string nextToken = parseToken();
		return nextToken.compare(target) == 0;
	}
	bool parseApostrophe(){
		string nextToken = parseToken();
		std::regex synonymRegex("[\"]");
		return std::regex_match(nextToken,synonymRegex) ? true : false;
	}
	bool parseDesignEntity(){
		string nextToken = parseToken();
		return matchDesignEntity(nextToken);
	}
	bool parseSynonymns(){
		string nextToken = parseToken();
		return matchSynonymAndIdent(nextToken, false);
	}
	bool parseStmtRef(){
		string nextToken = parseToken();
		return matchStmtRef(nextToken);
	}
	string parseEntRef(){
		string nextToken = "";
		string returnToken = "";

		if(parseApostrophe()){
			nextToken = peekBackwards(0);
			nextToken += parseToken();
			returnToken = peekBackwards(0);
			nextToken += parseToken();
		}else{
			nextToken = peekBackwards(0);
			returnToken = nextToken;
		}

		return matchEntRef(nextToken) ? returnToken: "";
	}
	bool parseFactor(){
		string nextToken = parseToken();
		return matchFactor(nextToken);
	}
	bool parseExpressionSpec(bool whilePatternExp){

		bool underscorePresent = false;

		bool res = parse("_");
		
		if (res){
			underscorePresent = true;
		}else{
			if(peekBackwards(0).compare("\"")!=0){

				#ifdef DEBUG
					cout<<"QueryParser in parsePatternClause: expression starting have to be _ or \", or error in expression"<<endl;
				#endif

				return false;
			}
			if(whilePatternExp){
				
				#ifdef DEBUG
					cout<<"QueryParser in parsePatternClause: parseExpressionSpec while missing _"<<endl;
				#endif

				return false;
			}
		}

		if (peekInToTheNextToken().compare(")") == 0){

			//for while patterns
			if(underscorePresent == true && whilePatternExp){
				return true;}

			// for assign patterns with second parameter as _
			if(underscorePresent)
				return true;

			//if the second parameter is " only
			#ifdef DEBUG
				cout<<"QueryParser in parsePatternClause: parseExpressionSpec second parameter can't be \" only"<<endl;
			#endif
				
			return false;
		}else{

			if(whilePatternExp){
				#ifdef DEBUG
					cout<<"QueryParser in parsePatternClause: parseExpressionSpec while pattern arg2 invalid"<<endl;
				#endif

				return false;
			}

			if(underscorePresent){
				res = parseApostrophe();
				if(!res){
					#ifdef DEBUG
						cout<<"QueryParser in parsePatternClause: parseExpressionSpec \" is missing "<<endl;
					#endif
					return false;
				}
			}


			//reads in the expression
			if(isFactor(peekInToTheNextToken())){
				do{

					res = parseFactor();
					if(!res){
						#ifdef DEBUG
							cout<<"QueryParser in parsePatternClause: parseExpressionSpec factor missing"<<endl;
						#endif
						return false;
					}

					res = parse("+");
					if(!res && peekBackwards(0).compare("\"")!=0){
						#ifdef DEBUG
							cout<<"QueryParser in parsePatternClause: parseExpressionSpec invalid expression"<<endl;
						#endif
						return false;
					}

				}while( res );
			}else{
				res = parseApostrophe();
				if(!res){
					#ifdef DEBUG
						cout<<"QueryParser in parsePatternClause: parseExpressionSpec \" is missing "<<endl;
					#endif
					return false;
				}
			}

			//check the _ in pattern arg2
			if(underscorePresent && peekInToTheNextToken().compare("_") != 0){

				#ifdef DEBUG
					cout<<"QueryParser in parsePatternClause: parseExpressionSpec _ missing"<<endl;
				#endif

				return false;
			}else if(peekInToTheNextToken().compare("_") == 0 && !underscorePresent){

				#ifdef DEBUG
					cout<<"QueryParser in parsePatternClause: parseExpressionSpec _ missing earlier"<<endl;
				#endif

				return false;
			}else if(peekInToTheNextToken().compare("_") == 0){
				parse("_");
			}		

		}

		return true;
	}
	bool parseSuchThatClause(){

		std::regex apostrophe("[\"]");
		string DE_type, nextToken;
		QNODE_TYPE nodeType;

		bool res = parse("such");
		if (!res){
			#ifdef DEBUG
				cout<<"QueryParser in parseSuchThatClause : 'such' keyword missing"<<endl;
			#endif
			return false;}
		res = parse("that");
		if (!res){
			#ifdef DEBUG
				cout<<"QueryParser in parseSuchThatClause : 'that' keyword missing"<<endl;
			#endif
			return false;}

		nextToken = parseToken();
		for(int i=0; i<(sizeof(relRef)/sizeof(*relRef)); i++){

			if(nextToken.compare(relRef[i]) == 0){

				res = parse("(");
				if (!res){
					#ifdef DEBUG
						cout<<"QueryParser in parseSuchThatClause : '(' missing"<<endl;
					#endif
					return false;} 

				res = parseStmtRef();
				if (!res){return false;} 

				res = parse(",");
				if (!res){
					#ifdef DEBUG
						cout<<"QueryParser in parseSuchThatClause : ',' missing"<<endl;
					#endif
					return false;} 

				string entRef_value="";
				if((relRef[i].compare("Modifies")==0) || (relRef[i].compare("Uses")==0)){
					entRef_value = parseEntRef();
					if (entRef_value.compare("")==0){return false;} 
				}
				else{
					res = parseStmtRef();
					if (!res){
						#ifdef DEBUG
							cout<<"QueryParser in parseSuchThatClause : arg2 error"<<endl;
						#endif
						return false;} 
				}
				

				res = parse(")");
				if (!res){
					#ifdef DEBUG
						cout<<"QueryParser in parseSuchThatClause : ')' missing"<<endl;
					#endif

					return false;} 


				/*** Building Query Tree ***/
				if (nodetypeMap.count(relRef[i]) > 0) {
					nodeType = nodetypeMap.at(relRef[i]);
				}else{
					#ifdef DEBUG
						cout<<"QueryParser parseSuchThatClause() error: unable to find the DE"<<endl;
					#endif
					return false;
				}


				/* Synonym s2 */
				string name2;
				if((relRef[i].compare("Modifies")==0) || (relRef[i].compare("Uses")==0)){

					name2 = entRef_value;

					if (std::regex_match(peekBackwards(1),apostrophe)){
						DE_type = "String";
					}else if(name2.compare("_")==0){
						DE_type =name2;
					}else if(synonymsMap.count(name2) > 0){
						DE_type = synonymsMap.at(name2);
					}else{
						#ifdef DEBUG
							cout<<"QueryParser in parseSuchThatClause:building query tree error"<<endl;
						#endif
						return false;
					}

				}else{

					name2 = peekBackwards(1);
					if (synonymsMap.count(name2) > 0) {
						DE_type = synonymsMap.at(name2); 
					}else if(name2.compare("_")==0){
						DE_type = name2;
					}else if(matchInteger(name2)){
						DE_type = "String";
					}else{
						#ifdef DEBUG
							cout<<"QueryParser in parseSuchThatClause:building query tree error"<<endl;
						#endif
						return false;
					}
				}
				//create synonym s2
				Synonym s2(DE_type,name2);

				/* Synonym s1 */
				string name1="";
				if(!std::regex_match(peekBackwards(1),apostrophe)){
					name1 = peekBackwards(3);
				}else{
					name1 = peekBackwards(5);
				}

				if (synonymsMap.count(name1) > 0) {
					DE_type = synonymsMap.at(name1); 
				}else if(name1.compare("_")==0){
					DE_type = name1;
				}else if(matchInteger(name1)){
					DE_type = "String";
				}else{
					#ifdef DEBUG
						cout<<"QueryParser in parseSuchThatClause:building query tree error"<<endl;
					#endif
					return false;
				}
				
				//create synonym s1
				Synonym s1(DE_type,name1);

				QNode* childNode = myQueryTree->createQNode(nodeType, Synonym(), s1, s2);
				res = myQueryTree->linkNode(myQueryTree->getSuchThatNode(), childNode);

				return res;
			}
		}

		#ifdef DEBUG
			cout<<"QueryParser in parseSuchThatClause : there is no matching relRef[]"<<endl;
		#endif

		return false;
	}
	bool parsePatternClause(){
		bool whilePatternExp = false;
		string DE_type;

		bool res = parse("pattern");
		if (!res){
			#ifdef DEBUG
				cout<<"QueryParser parsePatternClause error: missing 'pattern' keyword"<<endl;
			#endif

			return false;}

		//parse syn-assign
		if(synonymsMap.count(parseToken()) > 0){
			DE_type = synonymsMap.at(peekBackwards(0));
		}else{
			#ifdef DEBUG
				cout<<"QueryParser parsePatternClause error: invalid synonym used"<<endl;
			#endif
			return false;
		}

		if(DE_type.compare("assign")!= 0 && DE_type.compare("while") != 0){
			#ifdef DEBUG
				cout<<"QueryParser parsePatternClause error: synonym not 'assign' or 'while' type"<<endl;
			#endif

			return false;
		}
		if(DE_type.compare("while") == 0){
			whilePatternExp = true;
		}
		Synonym pattern_arg0(DE_type, peekBackwards(0)); 
		

		res = parse("(");
		if (!res){

			#ifdef DEBUG
				cout<<"QueryParser parsePatternClause error: missing '(' "<<endl;
			#endif

			return false;} 

		string pattern_variable = parseEntRef();
		if (pattern_variable.compare("")==0){
			#ifdef DEBUG
				cout<<"QueryParser parsePatternClause error: invalid entRef arg1"<<endl;
			#endif

			return false;} 


		//Build Query Tree
		std::regex apostrophe("[\"]");
		if (std::regex_match(peekBackwards(0),apostrophe)){
			DE_type = "String";
		}else if(pattern_variable.compare("_")==0){
			DE_type = pattern_variable;
		}else if(synonymsMap.count(pattern_variable) > 0){
			DE_type = synonymsMap.at(pattern_variable);
		}else{
			#ifdef DEBUG
				cout<<"QueryParser in parsePatternClause: building query tree error"<<endl;
			#endif
			return false;
		}

		res = parse(",");
		if (!res){

			#ifdef DEBUG
				cout<<"QueryParser in parsePatternClause: comma missing"<<endl;
			#endif

			return false;} 

		res = parseExpressionSpec(whilePatternExp);
		if (!res){
			#ifdef DEBUG
				cout<<"QueryParser parsePatternClause error: invalid expression arg2"<<endl;
			#endif

			return false;} 



		//Build Query Tree
		int i = 1;
		string pattern_patterns = "";
		while(peekBackwards(i).compare(",")!=0){
			i++;
		}
		for (i-=1; i>=0; i--){
			pattern_patterns+=peekBackwards(i);
		}

		res = parse(")");
		if (!res){
			#ifdef DEBUG
				cout<<"QueryParser parsePatternClause error: missing ')' "<<endl;
			#endif
			return false;} 

		//Build Query Tree
		Synonym pattern_var(DE_type, pattern_variable);
		Synonym pattern_pattern("String", pattern_patterns);
		QNode* patternQueryNode = myQueryTree->createQNode(Pattern,pattern_arg0, pattern_var, pattern_pattern);
		myQueryTree->linkNode(myQueryTree->getPatternNode(), patternQueryNode);


		return true;
	}
	bool parseOptionalClauses(){

		bool res;
		
		if(peekInToTheNextToken().compare("such")==0){

			res = parseSuchThatClause();
			if (!res){return false;}
		}else if(peekInToTheNextToken().compare("pattern")==0){
			res = parsePatternClause();
		}else{
			#ifdef DEBUG
				cout<<"QueryParser parseOptionalClauses error "<<endl;
			#endif
			return false;
		}

		return res;
	}
	/*
	 * For Testing Synonyms Map
	 */
	void printSynonymsMap(){

		for (unordered_map<string, string>::iterator it = synonymsMap.begin(); it != synonymsMap.end(); ++it)
		{
			string type = it->first;
			string name = it->second;

			cout <<"name : "<<name <<"type : " <<type<<endl;
		}
	}
	bool parseDeclarations(){

		bool res;
		int i=0; //number of times in the while loop
		string nextToken, peekToken;

		do{
			res = parseDesignEntity();
			if (!res){return false;}

			res = parseSynonymns();
			if (!res){return false;}

			nextToken = parseToken();
			while(nextToken.compare(";")!= 0){
				
				res = (nextToken.compare(",")==0);
				if (!res){return false;}

				res = parseSynonymns();
				if (!res){return false;}

				i++;
				nextToken = parseToken();
			}

			/*Symbols Table for query tree
			 *peekBackwards((2*i)+2) is to get the design entity 
			 *peekBackwards((2*x)-1) is to get the synonyms
			 *Add into the symbols table the DE and the synonymn of the node*/
			for(int x=1; x<= i+1;x++){

				pair<string,string> pairOfSynonyms (peekBackwards((2*x)-1),peekBackwards((2*i)+2));
				if (synonymsMap.count(peekBackwards((2*x)-1)) >0){
					#ifdef DEBUG
						cout<<"synonymn declaration error, the synonymn has been declared previously."<<endl;
					#endif
					return false;
				}else{
					synonymsMap.insert(pairOfSynonyms);
				}
			}

			peekToken = peekInToTheNextToken();
			i=0;  //reset i
		}while (peekToken.compare("Select")!= 0);

		myQueryTree->setSynonymsMap(synonymsMap);

		return res;
	}
	bool parseSelect(){

		string DE_type;

		bool res = parse("Select");
		if (!res){
			#ifdef DEBUG
				cout<<"QueryParser parseSelect error: missing 'Select' keyword"<<endl;
			#endif
			return false;}

		res = parseSynonymns();
		if (!res){return false;}


		/*** Building Query Tree ***/
		QNODE_TYPE nodeType = RESULT;

		if (!synonymsMap.empty()){

			if (synonymsMap.count(peekBackwards(0)) > 0) {
				DE_type = synonymsMap.at(peekBackwards(0)); 
			}else{
				#ifdef DEBUG
					cout<<"QueryParser synonymsMap error: unable to find the synonym"<<endl;
				#endif
				return false;
			}
		}else{
			#ifdef DEBUG
				cout<<"QueryParser synonymsMap error: synonymsMap is empty"<<endl;
			#endif
			return false;
		}

		Synonym s1(DE_type,peekBackwards(0));
		QNode* childNode = myQueryTree->createQNode(Selection, Synonym(), s1, Synonym());
		res = myQueryTree->linkNode(myQueryTree->getResultNode(), childNode);
		if(!res){return false;}

		return res;
	}
	bool parseQuerySelectClause(){

		bool res; 
		res = parseDeclarations();
		if (!res){return false;}

		res = parseSelect();
		if (!res){return false;}

		while(buffer.size() && bufferIter != buffer.end()){

			res = parseOptionalClauses();
			if (!res){return false;}
		}
		return res;
	}
	bool parseQuery(){

		//testingQueryParser();

		bool res = initQueryTreeAndSymbolsTable();
		if (!res){return false;}

		//does syntax chacks
		res = parseQuerySelectClause();


		//if there's an error in parsing the queries, return an empty query tree
		if(!res){
			#ifdef DEBUG
				cout<<"Error in parsing query. Empty query tree and synonymsMap is passed."<<endl;
			#endif
			initQueryTreeAndSymbolsTable();
		}


		//queryTreeTesting();
		//cout<<endl;
		//cout<<endl;

		return true;
	}

	QueryTree* getQueryTree(){
		return myQueryTree;
	}

}
