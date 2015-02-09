#pragma once

//To print out debug error msges
//#ifndef DEBUG
//#define DEBUG 
//#endif

#include<stdio.h>
#include <tuple>
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
using std::make_tuple;

/**
	@brief Namespace containing functions for parsing PQL queries.
	QueryParser checks the query input to see if it matches with 
	PQL grammar and builds a query tree. 

 */
namespace QueryParser
{
	enum REF_TYPE
	{
		entRef, stmtRef, lineRef, varRef, ref
	};

	ifstream inputFile;
	vector<string> buffer;
	vector<string>::iterator bufferIter;
	string currentParsedLine;

	string attrName[] = {"procName", "varName", "value", "stmt#"};
	string designEntities[] = {"procedure", "stmtLst", "stmt","assign", "call", "while", "if", "variable","constant","prog_line", "plus", "minus", "times"};
	string relRef[] = {"Modifies", "Uses", "Parent", "Parent*", "Follows", "Follows*", "Calls", "Calls*"};
	QueryTree* myQueryTree;
	unordered_map<string, string> synonymsMap; //key: synonyms
	unordered_map<string, QNODE_TYPE> nodetypeMap; //key: synonyms
	unordered_map<string, tuple<REF_TYPE, REF_TYPE>> relRefMap; //key: relRef
	unordered_map<string, SYNONYM_ATTRIBUTE> attrNameMap; //key: synonyms
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
		string operators = "([\\w\\d\\*\\#]+|[.=_+;,(\\)\"])";
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
		while (nxtToken.compare("")!= 0){
			cout<<"nextToken : "<<nxtToken<<endl;
			nxtToken = parseToken();
		}
		cout<<endl;
	}
	/**************************************************************/
	/**   Supporting Functions - To read the buffer of queries   **/
	/**************************************************************/
	string peekInToTheNextToken()
	{
		if(buffer.size()==0 || bufferIter == buffer.end()) // if there is no next token
			return "";

		return (*(bufferIter));
	}

	/**
	 * @return a token these number of steps backwards from the currToken.
	 * If steps = 0 , it returns the currToken
	 * If steps = 1, it returns the previous token
	 */
	string peekBackwards(int steps)
	{
		if((steps==-1) && (buffer.size()==0 || bufferIter == buffer.end())) // if there is no next token
			return "";

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
		myQueryTree = new QueryTree();
		
		synonymsMap.clear();
		nodetypeMap.clear();
		relRefMap.clear();
		attrNameMap.clear();

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
		pair<string,QNODE_TYPE> pairNodeType7 ("Calls",QNODE_TYPE(Calls));
		nodetypeMap.insert(pairNodeType7);
		pair<string,QNODE_TYPE> pairNodeType8 ("Calls*",QNODE_TYPE(CallsS));
		nodetypeMap.insert(pairNodeType8);

		//populate relRefMap
		
		auto node1 = make_tuple(REF_TYPE(stmtRef), REF_TYPE(entRef));
		auto node2 = make_tuple(REF_TYPE(entRef), REF_TYPE(entRef));
		auto node3 = make_tuple(REF_TYPE(stmtRef), REF_TYPE(stmtRef));
		auto node4 = make_tuple(REF_TYPE(ref), REF_TYPE(ref));

		pair<string,tuple<REF_TYPE,REF_TYPE>> pairRelNode1 ("Modifies", node1);
		relRefMap.insert(pairRelNode1);
		pair<string,tuple<REF_TYPE,REF_TYPE>> pairRelNode2 ("Uses", node1);
		relRefMap.insert(pairRelNode2);
		pair<string,tuple<REF_TYPE,REF_TYPE>> pairRelNode3 ("Calls", node2);
		relRefMap.insert(pairRelNode3);
		pair<string,tuple<REF_TYPE,REF_TYPE>> pairRelNode4 ("Calls*", node2);
		relRefMap.insert(pairRelNode4);
		pair<string,tuple<REF_TYPE,REF_TYPE>> pairRelNode5 ("Parent", node3);
		relRefMap.insert(pairRelNode5);
		pair<string,tuple<REF_TYPE,REF_TYPE>> pairRelNode6 ("Parent*", node3);
		relRefMap.insert(pairRelNode6);
		pair<string,tuple<REF_TYPE,REF_TYPE>> pairRelNode7 ("Follows", node3);
		relRefMap.insert(pairRelNode7);
		pair<string,tuple<REF_TYPE,REF_TYPE>> pairRelNode8 ("Follows*", node3);
		relRefMap.insert(pairRelNode8);
		pair<string,tuple<REF_TYPE,REF_TYPE>> pairRelNode9 ("attrCompare", node4);
		relRefMap.insert(pairRelNode9);

		pair<string,SYNONYM_ATTRIBUTE> pairAttrNameType1 ("procName", SYNONYM_ATTRIBUTE(procName));
		attrNameMap.insert(pairAttrNameType1);
		pair<string,SYNONYM_ATTRIBUTE> pairAttrNameType2 ("varName", SYNONYM_ATTRIBUTE(varName));
		attrNameMap.insert(pairAttrNameType2);
		pair<string,SYNONYM_ATTRIBUTE> pairAttrNameType3 ("value", SYNONYM_ATTRIBUTE(value));
		attrNameMap.insert(pairAttrNameType3);
		pair<string,SYNONYM_ATTRIBUTE> pairAttrNameType4 ("stmt#", SYNONYM_ATTRIBUTE(stmtNo));
		attrNameMap.insert(pairAttrNameType4);

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
	
	bool matchAttrName(string token)
	{
		for(int i=0; i<(sizeof(attrName)/sizeof(*attrName)); i++){
			if(attrName[i].compare(token) == 0)
				return true;
		}


		#ifdef DEBUG
			cout<< "QueryParser error : at matchAttrName."<<endl;
		#endif
		return false;
	}

	/**
	 * @siling
	 * Matches if has "synonym.attrName" syntax
	 **/
	bool matchAttrRef(string token)
	{

		if(!matchSynonymAndIdent(peekBackwards(2), false)){
			cout<<"error synonymn "<<endl;
			return false;
		}
		if (peekBackwards(1).compare(".") != 0){
			cout<<"error in . "<<endl;
			return false;
		}
		if(!matchAttrName(peekBackwards(0))){
			cout<<"error attrName "<<endl;
			return false;
		}

		return true;
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
	 * Matches if the given token follows the naming convention of a reference.
	 */
	 bool matchRef(string token)
	 {
	 	if(matchSynonymAndIdent(token, false))  
	 		return true;
	 	else if(matchSynonymAndIdent(token, true))
	 		return true;
	 	else if(matchInteger(token))
			return true;
		else if(matchAttrRef(token))
			return true;

		#ifdef DEBUG
			cout<<"QueryParser error : at matchRef"<<endl;
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
	 * @return an empty string if parsing fails
	 */
	string parseStmtRef()
	{
		string nextToken = parseToken();
		return matchStmtRef(nextToken) ? nextToken: "";
	}

	/**
	 * Parses the next token and check if is a entity reference.
	 * @return an empty string if parsing fails
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
	 * Parses the next token and check if is a reference. 
	 * Supporting method for parsing with clause. 
	 */
	 string parseRef()
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
			if(peekInToTheNextToken().compare(".")==0){
				nextToken += parseToken();
				nextToken += parseToken();
			}
			returnToken = nextToken;
		}

	 	return matchRef(nextToken) ? returnToken: "";

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
	bool buildQueryTree(string relRef, Synonym s1, Synonym s2)
	{				
		bool res;
		QNODE_TYPE nodeType;

		//get the nodeType
		if (nodetypeMap.count(relRef) > 0){
			nodeType = nodetypeMap.at(relRef);
		}else{
			return false;
		}

		//validate the query
		res = myQueryV->validateSuchThatQueries(nodeType, s1, s2);
		if(!res){return false;}

		//build the tree
		QNode* childNode = myQueryTree->createQNode(nodeType, Synonym(), s1, s2);
		res = myQueryTree->linkNode(myQueryTree->getSuchThatNode(), childNode);

		return res;
	}

	Synonym createSynonym(string relRef,string value, int arg)
	{
		string DE_type;
		REF_TYPE node;
		SYNONYM_ATTRIBUTE attribute;
		std::regex apostrophe("[\"]");

		auto nodeTuple = relRefMap.at(relRef);
		if(arg==1)
			node = std::get<0>(nodeTuple);
		else if(arg==2)
			node = std::get<1>(nodeTuple);
		else
			return Synonym(); //arg can only be 1 or 2 


		if(node==REF_TYPE(stmtRef)){

			if (synonymsMap.count(value) > 0){
				DE_type = synonymsMap.at(value); 
			}else if(value.compare("_")==0){
				DE_type = value;
			}else if(matchInteger(value)){
				DE_type = "string";
			}else{
				return Synonym();  //error type mismatch
			}

			//build Synonym and return 
			return Synonym(Synonym::convertToEnum(DE_type),value);

		}else if(node==REF_TYPE(entRef)){

			if (std::regex_match(peekBackwards(0),apostrophe)){   //if it has apostrophe(ie it's """IDENT""")
				DE_type = "string";
			}else if(value.compare("_")==0){
				DE_type =value;
			}else if(synonymsMap.count(value) > 0){
				DE_type = synonymsMap.at(value);
			}
			else{
				return Synonym();  //error type mismatch
			}

			//build Synonym and return
			return Synonym(Synonym::convertToEnum(DE_type),value);

		}else if(node==REF_TYPE(ref)){
			if (std::regex_match(peekBackwards(0),apostrophe)){   //if it has apostrophe(ie it's """IDENT""")
				DE_type = "string";
			}else if(synonymsMap.count(value) > 0){
				DE_type = synonymsMap.at(value);
			}else if(matchInteger(value)){
				DE_type = "string";
			}else{
				// it must be an attrRef
				if(synonymsMap.count(peekBackwards(2)) > 0)
					DE_type = synonymsMap.at(peekBackwards(2));
				else{
					#ifdef DEBUG
						cout<<"####error no such synonym ##### "	<<endl;
						cout<<"synonym can't be found : " <<peekBackwards(2)<<endl;
					#endif
					return Synonym();  //error no such synonym
				}
				value = peekBackwards(2);

				if(attrNameMap.count(peekBackwards(0)) > 0)
					attribute = attrNameMap.at(peekBackwards(0));
				else{
					#ifdef DEBUG
						cout<<"####error no such attrName ##### "	<<endl;
						cout<<"attrName can't be found : " <<peekBackwards(0)<<endl;
					#endif
					return Synonym();  //error no such attrName
				}

				return Synonym(Synonym::convertToEnum(DE_type),value, attribute);
			}


			//build Synonym and return
			return Synonym(Synonym::convertToEnum(DE_type),value,SYNONYM_ATTRIBUTE());
		}

		return Synonym(); //wont reach here

	}

	/**
	 *	Helper function for parse such that clause
	 *  @param[in] input the relRef (ie "Follows*") and the argument it's parsing(either 1 or 2 only) 
	 *  Argument 1 can be an entRef, stmtRef or lineRef.
	 *  Argument 2 can be an entRef, stmtRef, lineRef or varRef.
	 *  If @return is an empty string, means parsing failed.
	 *  @To-do return synonym  
	 */
	string parseArg(string relRef, int arg)
	{
		REF_TYPE node;
		string entRef_value="";

		auto nodeTuple = relRefMap.at(relRef);
		if(arg==1)
			node = std::get<0>(nodeTuple);
		else if(arg==2)
			node = std::get<1>(nodeTuple);
		else
			return ""; //arg can only be 1 or 2 
		
		if(node==REF_TYPE(stmtRef)){

			entRef_value = parseStmtRef();
			
		}else if(node==REF_TYPE(entRef)){

			entRef_value = parseEntRef();
		
		}//else do nothing and an empty string is passed.


		return entRef_value;
	}



	/**
	 * Creates QNode for such that clause and validates the query.
	 * @return FALSE if there is an error parsing query.
	 */
	bool parseSuchThatClause()
	{
		string DE_type, nextToken, entRef_value1,entRef_value2;

		bool res = parse("such");
		if (!res){return false;}
		res = parse("that");
		if (!res){return false;}

		nextToken = parseToken();
		for(int i=0; i<(sizeof(relRef)/sizeof(*relRef)); i++){

			if(nextToken.compare(relRef[i]) == 0){
				res = parse("(");
				if (!res){return false;} 

				entRef_value1 = parseArg(relRef[i], 1);
				if(entRef_value1.compare("")==0){
					#ifdef DEBUG
						cout<<"QueryParser parseSuchThatClause error: invalid arg1 used"<<endl;
					#endif
					return false;
				} //empty string if parsing fails.

				//create synonym s1
				Synonym s1 = createSynonym(relRef[i], entRef_value1, 1);

				res = parse(",");
				if (!res){return false;} 

				entRef_value2 = parseArg(relRef[i], 2);
				if(entRef_value2.compare("")==0){

					#ifdef DEBUG
						cout<<"QueryParser parseSuchThatClause error: invalid arg2 used"<<endl;
					#endif
					return false;
				} //empty string if parsing fails.

				//create synonym s2
				Synonym s2 = createSynonym(relRef[i], entRef_value2, 2);
				
				res = parse(")");
				if (!res){ return false;} 



				/***Parsing is done. Building Query Tree ***/


				res = buildQueryTree(relRef[i], s1, s2);
				if(!res){
					#ifdef DEBUG
						cout<<"QueryParser parseSuchThatClause error: in buildQueryTree"<<endl;
					#endif
				}

				return res;
			}
		}

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
		Synonym pattern_arg0(Synonym::convertToEnum(DE_type), peekBackwards(0)); 
		

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
			DE_type = "string";
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
		Synonym pattern_var(Synonym::convertToEnum(DE_type), pattern_variable);
		Synonym pattern_pattern(SYNONYM_TYPE(STRING), pattern_patterns);

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

	bool parseAttrCompare()
	{
		string ref1 = parseRef();
		if(ref1.compare("")==0){
			
			#ifdef DEBUG
				cout<<"QueryParser parseAttrCompare error: parseRef argument 1 "<<endl;
			#endif
			return false;
		}
		Synonym s1 = createSynonym("attrCompare", ref1, 1);

		bool res = parse("=");
		if(!res) {return false;}

		string ref2 = parseRef();
		if(ref2.compare("")==0){

			#ifdef DEBUG
				cout<<"QueryParser parseAttrCompare error: parseRef argument 2 "<<endl;
			#endif
			return false;}
		Synonym s2 = createSynonym("attrCompare", ref2, 2);

		//build query tree
		QNode* withQueryNode = myQueryTree->createQNode(WITH,Synonym(),s1,s2);
		myQueryTree->linkNode(myQueryTree->getWithNode(), withQueryNode);

		return  true;
	}
	bool parseWithClause()
	{
		bool res = parse("with");
		if(!res) {return false;}

		res = parseAttrCompare();
		return res;
	}

	bool parseOptionalClauses()
	{

		bool res;
		
		if(peekInToTheNextToken().compare("such")==0){
			res = parseSuchThatClause();
			if (!res){return false;}
		}else if(peekInToTheNextToken().compare("pattern")==0){
			res = parsePatternClause();
		}else if(peekInToTheNextToken().compare("with")==0){
			res = parseWithClause();
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
		QNODE_TYPE nodeType;
		QNode* childNode;

		bool res = parse("Select");
		if (!res){
			#ifdef DEBUG
				cout<<"QueryParser parseSelect error: missing 'Select' keyword"<<endl;
			#endif
			return false;
		}

		if(peekInToTheNextToken().compare("BOOLEAN")==0){
		
			parseToken();
			Synonym s1(SYNONYM_TYPE(BOOLEAN),"");
			childNode = myQueryTree->createQNode(Selection, Synonym(), s1, Synonym());
		
		}else{
			res = parseSynonymns();
			if (!res){return false;}


			/*** Building Query Tree ***/
			nodeType = RESULT;

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

			Synonym s1(Synonym::convertToEnum(DE_type),peekBackwards(0));
			childNode = myQueryTree->createQNode(Selection, Synonym(), s1, Synonym());

		}
		res = myQueryTree->linkNode(myQueryTree->getResultNode(), childNode);
		return res;

	}

	/**
	 * @return FALSE if there's errors in query declarations and select synonym.
	 */
	bool parseQuerySelectClause()
	{
		bool res; 
		res = parseDeclarations();
		if (!res){
			#ifdef DEBUG
				cout<<"QueryParser declarations failed."<<endl;
			#endif
			return false;}

		res = parseSelect();
		if (!res){
			#ifdef DEBUG
				cout<<"QueryParser select failed."<<endl;
			#endif
			return false;}

		while(buffer.size() && bufferIter != buffer.end()){
			res = parseOptionalClauses();
			if (!res){
			#ifdef DEBUG
				cout<<"QueryParser optional clauses failed."<<endl;
			#endif
			return false;}
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