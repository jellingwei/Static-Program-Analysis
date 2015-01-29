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
#include "QueryValidator.h"

using std::string;
using std::vector;

/**
	@brief Namespace containing functions for parsing PQL queries.
	QueryParser checks the query input to see if it matches with 
	PQL grammar and builds a query tree. 

 */
namespace QueryParser
{
	ifstream inputFile;
	vector<string> buffer;
	vector<string>::iterator bufferIter;
	string currentParsedLine;

	string designEntities[] = {"stmt","assign","while","variable","constant","prog_line"};
	string relRef[] = {"Modifies", "Uses", "Parent", "Parent*", "Follows", "Follows*"};
	QueryTree* myQueryTree;
	unordered_map<string, string> synonymsMap; //key: synonyms
	unordered_map<string, QNODE_TYPE> nodetypeMap; //key: synonyms
	QueryValidator* myQueryV;
	
	/**
	 * Initialises and prepares the parser for parsing with a query.
	 * @return TRUE if the query parser have been initialized. Otherwise, return FALSE.
	 * If a query string given is empty, or the buffer’s size is 0 after tokenizing return FALSE. 
	 */
	bool initParser(string query)
	{
		buffer.clear();
		if (query.size() == 0){
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

		for (; rs != reg_end; ++rs){

			std::smatch match;
			string res(rs->str());
			
			while (std::regex_search(res, match, operRegex)){

				if (match.empty()){
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
	 * @return the next token
	 */
	string parseToken()
	{
		if (buffer.size() && bufferIter != buffer.end()){

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

	void testingQueryParser()
	{
		string nxtToken = parseToken();
		while (nxtToken.compare("")!= 0)
			nxtToken = parseToken();
		cout<<endl;
	}
	/**************************************************************/
	/**   Supporting Functions - To read the buffer of queries   **/
	/**************************************************************/
	string peekInToTheNextToken()
	{
		return (*(bufferIter));
	}

	/**
	 * @return a token these number of steps backwards from the currToken.
	 * If steps = 0 , it returns the currToken
	 * If steps = 1, it returns the previous token
	 */
	string peekBackwards(int steps)
	{
		return (*(bufferIter-1-steps));
	}

	/**************************************************************/
	/**     Supporting functions - To build query tree           **/
	/**************************************************************/
	
	/**
	 * Initialise query tree, synonymsMap and nodeTypeMap.
	 */	
	void initQueryTreeAndSymbolsTable()
	{
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

	}

	/**
	 * Prints out the built by the query parser onto console for testing and debugging.
	 **/
	void queryTreeTesting()
	{
		myQueryTree->printTree();

	}

	/**************************************************************/
	/**                     Matching                             **/
	/**************************************************************/

	/**
	 * Matches if the given token follows the naming convention for NAME, as per the given grammar.
	 */
	bool matchName(string token)
	{
		std::regex nameRegex("[A-Za-z][\\w]*");

		return std::regex_match(token, nameRegex) ? true : false;
	}

	/**
	 * Matches if the given token follows the naming convention for INTEGER, as per the given grammar.
	 */
	bool matchInteger(string token)
	{
		std::regex intRegex("\\d+");
		return (std::regex_match(token,intRegex)) ? true : false;
	}

	/**
	 * Matches if the given token follows the naming convention of a factor.
	 */
	bool matchFactor(string token)
	{
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
	bool isFactor(string token)
	{
		return (matchInteger(token) || matchName(token));
	}
	/**
	 *Matches if the given token follows the naming convention of Synonym and IDENT.
	 */
	bool matchSynonymAndIdent(string token, bool comma)
	{
		std::regex synonymRegex("");

		if (comma){
			std::regex synonymRegex("[\"+][A-Za-z][A-Za-z0-9#]*[\"+]");
			return (std::regex_match(token,synonymRegex)) ? true : false;
		}else{
			std::regex synonymRegex("[A-Za-z][A-Za-z0-9#]*");
			return (std::regex_match(token,synonymRegex)) ? true : false;
		}

		//won't reach here
		return false;
	}

	/**
	 * Matches if the given token follows the naming convention of entity reference.
	 */
	bool matchUnderscore(string token)
	{
		return (token.compare("_") == 0);
	}

	/**
	 * Matches if the given token follows the naming convention of stmt reference.
	 */
	bool matchStmtRef(string token)
	{
		if(matchSynonymAndIdent(token,false))
			return true;
		else if(matchInteger(token))
			return true;
		else if(matchUnderscore(token))
			return true;

		#ifdef DEBUG
			cout<<"QueryParser error : at matchStmtRef"<<endl;
		#endif
		return false;
	}

	/**
	 * Matches if the given token follows the naming convention of entity reference.
	 */
	bool matchEntRef(string token)
	{
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

	/**
	 * Matches if the given token follows the naming convention of design entity reference.
	 */
	bool matchDesignEntity(string token)
	{
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
	 * Parses the next token and check if it is equal to the given target.
	 */
	bool parse(string target)
	{
		string nextToken = parseToken();
		return nextToken.compare(target) == 0;
	}

	/**
	 * Parses the next token and check if it is equal to the apostrophe.
	 */
	bool parseApostrophe()
	{
		string nextToken = parseToken();
		std::regex synonymRegex("[\"]");
		return std::regex_match(nextToken,synonymRegex) ? true : false;
	}

	/**
	 * Parses the next token and check if is a design entity.
	 */
	bool parseDesignEntity()
	{
		string nextToken = parseToken();
		return matchDesignEntity(nextToken);
	}

	/**
	 * Parses the next token and check if is a synonym.
	 */
	bool parseSynonymns()
	{
		string nextToken = parseToken();
		return matchSynonymAndIdent(nextToken, false);
	}

	/**
	 * Parses the next token and check if is a stmt reference.
	 */
	bool parseStmtRef()
	{
		string nextToken = parseToken();
		return matchStmtRef(nextToken);
	}

	/**
	 * Parses the next token and check if is a entity reference.
	 */
	string parseEntRef()
	{
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

	/**
	 * Parses the next token and check if is a factor
	 */
	bool parseFactor()
	{
		string nextToken = parseToken();
		return matchFactor(nextToken);
	}

	/**
	 * Parses the next token and check if is an expression for patterns.
	 * @todo merge this with ExpressionParser and build an expressions 
	 * query tree.
	 */
	bool parseExpressionSpec(bool whilePatternExp)
	{

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
				return true;
			}

			// for assign patterns with second parameter as _
			if(underscorePresent){
				return true;
			}
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

	/**
	 *	Helper function for parse such that clause
	 *  After parsing the relRef, build a tree.
	 *  @To-do Temporarily it's for such-that. Extend it for patterns, select, with
	 */
	bool buildQueryTree(QNODE_TYPE nodeType, Synonym s1, Synonym s2)
	{				
		bool res = myQueryV->validateSuchThatQueries(nodeType, s1, s2);
		if(!res){return false;}

		QNode* childNode = myQueryTree->createQNode(nodeType, Synonym(), s1, s2);
		res = myQueryTree->linkNode(myQueryTree->getSuchThatNode(), childNode);

		return res;
	}

	/**
	 *	Helper function for parse such that clause
	 *  argument 1 can be an entRef, stmtRef, or a lineRef
	 *  @To-do return synonym 
	 */
	bool parseArg1(string relRef)
	{

		bool res = parseStmtRef();

		return res;
	}

	/**
	 *	Helper function for parse such that clause
	 *  argument 2 can be an entRef, stmtRef, lineRef or varRef
	 *  If @return is an empty string, means parsing failed.
	 *  @To-do return synonym  
	 */
	string parseArg2(string relRef)
	{
		bool res;
		string entRef_value="";

		if((relRef.compare("Modifies")==0) || (relRef.compare("Uses")==0)){

			entRef_value = parseEntRef(); 
		
		}else{
			res = parseStmtRef();
			if(res) {
				// if parsing didn't fail, return "PASS"
				entRef_value = "PASS";
			}
		
		}

		return entRef_value;
	}



	/**
	 * Creates QNode for such that clause and validates the query.
	 * @return FALSE if there is an error parsing query.
	 */
	bool parseSuchThatClause()
	{
		std::regex apostrophe("[\"]");
		string DE_type, nextToken, entRef_value;
		QNODE_TYPE nodeType;

		bool res = parse("such");
		if (!res){return false;}
		res = parse("that");
		if (!res){return false;}

		nextToken = parseToken();
		for(int i=0; i<(sizeof(relRef)/sizeof(*relRef)); i++){

			if(nextToken.compare(relRef[i]) == 0){
				res = parse("(");
				if (!res){return false;} 

				res = parseArg1(relRef[i]);
				if (!res){return false;} 

				res = parse(",");
				if (!res){return false;} 

				entRef_value = parseArg2(relRef[i]);
				if(entRef_value.compare("")==0){return false;} //empty string if parsing fails.
				
				res = parse(")");
				if (!res){ return false;} 




				/***Parsing is done. Building Query Tree ***/

				if (nodetypeMap.count(relRef[i]) > 0){
					nodeType = nodetypeMap.at(relRef[i]);
				}else{
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
					if (synonymsMap.count(name2) > 0){
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


				if (synonymsMap.count(name1) > 0){
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


				res = buildQueryTree(nodeType, s1, s2);

				return res;
			}
		}

		#ifdef DEBUG
			cout<<"QueryParser in parseSuchThatClause : there is no matching relRef[]"<<endl;
		#endif

		return false;
	}

	/**
	 * Creates QNode Pattern and validates the pattern clause.
	 * @return FALSE if there's errors in the pattern portion of query.
	 */
	bool parsePatternClause()
	{
		bool whilePatternExp = false;
		string DE_type;

		bool res = parse("pattern");
		if (!res){
			#ifdef DEBUG
				cout<<"QueryParser parsePatternClause error: missing 'pattern' keyword"<<endl;
			#endif

			return false;
		}

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

			return false;
		} 

		string pattern_variable = parseEntRef();
		if (pattern_variable.compare("")==0){
			#ifdef DEBUG
				cout<<"QueryParser parsePatternClause error: invalid entRef arg1"<<endl;
			#endif

			return false;
		} 


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

			return false;
		} 

		res = parseExpressionSpec(whilePatternExp);
		if (!res){
			#ifdef DEBUG
				cout<<"QueryParser parsePatternClause error: invalid expression arg2"<<endl;
			#endif

			return false;
		} 



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
			return false;
		} 

		//Build Query Tree
		Synonym pattern_var(DE_type, pattern_variable);
		Synonym pattern_pattern("String", pattern_patterns);

		res = myQueryV->validatePatternQueries(pattern_arg0, pattern_var, pattern_pattern);
		if(!res){
			#ifdef DEBUG
				cout<<"QueryParser in validatePatternQueries: returns error"<<endl;
			#endif
			return false;
		}


		QNode* patternQueryNode = myQueryTree->createQNode(Pattern,pattern_arg0, pattern_var, pattern_pattern);
		myQueryTree->linkNode(myQueryTree->getPatternNode(), patternQueryNode);


		return true;
	}

	bool parseOptionalClauses()
	{

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
	 * Method for printing synonyms map.
	 */
	void printSynonymsMap()
	{
		for (unordered_map<string, string>::iterator it = synonymsMap.begin(); it != synonymsMap.end(); ++it){
			string type = it->first;
			string name = it->second;

			cout <<"name : "<<name <<"type : " <<type<<endl;
		}
	}

	/**
	 * Supporting function to parse declarations.
	 */
	bool parseDeclarations()
	{
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

	/**
	 * Creates QNode Selection and validates the select synonym.
	 * @return FALSE if there's errors in the select synonym.
	 */
	bool parseSelect()
	{
		string DE_type;

		bool res = parse("Select");
		if (!res){
			#ifdef DEBUG
				cout<<"QueryParser parseSelect error: missing 'Select' keyword"<<endl;
			#endif
			return false;
		}

		res = parseSynonymns();
		if (!res){return false;}


		/*** Building Query Tree ***/
		QNODE_TYPE nodeType = RESULT;

		if (!synonymsMap.empty()){
			if (synonymsMap.count(peekBackwards(0)) > 0){
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

	/**
	 * @return FALSE if there's errors in query declarations and select synonym.
	 */
	bool parseQuerySelectClause()
	{
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

	/**
	 * It takes in a query, validates the query and builds a query tree.  
	 */
	bool parseQuery()
	{
		//testingQueryParser();

		myQueryV = new QueryValidator();
		myQueryV->initTable(); //propagate relationships data onto table

		initQueryTreeAndSymbolsTable();


		bool res = parseQuerySelectClause();
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

	/**
	 * @return a query tree built by query parser. 
	 * If input query contain syntax errors, an empty query tree is returned. 
	 */
	QueryTree* getQueryTree()
	{
		return myQueryTree;
	}
}