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
	Query Parser parses the query according to PQL Grammar rules, 
	builds a synonym map and a Query Tree. It validates the 
	structural syntax of PQL query provided, if the query provided 
	is structurally correct, it calls upon Query Validator for 
	validation of the logical aspect of the query. 
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

	string attrName[] = { "procName", "varName", "value", "stmt#" };
	string designEntities[] = { "procedure", "stmtLst", "stmt", "assign", "call", "while", "if", "variable", "constant", "prog_line", "plus", "minus", "times"};
	string relRef[] = { "Modifies", "Uses", "Calls", "Calls*", "Parent", "Parent*", "Follows", "Follows*", "Next", "Next*", "Affects", "Affects*"};
	QueryTree* myQueryTree;
	unordered_map<string, SYNONYM_TYPE> synonymsMap; //key: synonyms
	unordered_map<string, QNODE_TYPE> nodetypeMap; //key: synonyms
	unordered_map<QNODE_TYPE, tuple<REF_TYPE, REF_TYPE>> relRefMap; //key: relRef
	unordered_map<string, SYNONYM_ATTRIBUTE> attrNameMap; //key: synonyms
	QueryValidator* myQueryV;


	/*@siling temporary declare here.
	  Private functions to parse patterns */
	bool parseExpressionSpec();
	bool parseExpr();
	bool parseTerm();
	bool parseFactor();


	/**
	 * Initialises and prepares the parser for parsing with a query.
	 * @return TRUE if the query parser have been initialized. Otherwise, return FALSE.
	 * If a query string given is empty, or the buffer\�s size is 0 after tokenizing return FALSE. 
	 */
	bool initParser(string query)
	{
		buffer.clear();
		if (query.size() == 0){
			#ifdef DEBUG
				throw exception("QueryParser error: initParser(), empty query string");
			#endif

			return false;
		}

		// remove blocks of multiple whitespace
		string multipleSpaces = "[\\s]";
		std::tr1::regex separatorRegex(multipleSpaces);
		std::tr1::sregex_token_iterator reg_end;

		std::tr1::sregex_token_iterator rs(query.begin(), query.end(), separatorRegex, -1);

		// tokenise words and operators
		string operators = "([\\w\\d\\*\\#\\-]+|[.=_+;,(\\)\"])";
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
	 * @return the next token in the buffer.
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

	/**
	 * Prints out the remaining tokens in the buffer.
	 */
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
	
	/**
	 * @return the next token in the buffer, but does not 
	 * take that token out of buffer.
	 */
	string peekInToTheNextToken()
	{
		if(buffer.size()==0 || bufferIter == buffer.end()) // if there is no next token
			return "";

		return (*(bufferIter));
	}

	/**
	 * @To-do merge with peekInToTheNextToken
	 * @return the next next token in the buffer, but does not
	 * take that token out of buffer.
	 */
	string peekInToTheNextNextToken()
	{
		return (*(bufferIter+1));
	}

	/**
	 * @return a token these number of steps backwards from the currToken.
	 * If steps = 0 , it returns the currToken.
	 * If steps = 1, it returns the previous token.
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
	 * Initialise query tree, synonymsMap, nodeTypeMap, 
	 * relRefMap and attrNameMap.
	 */	
	void initQueryTreeAndSymbolsTable()
	{
		myQueryTree = new QueryTree();
		
		synonymsMap.clear();
		nodetypeMap.clear();
		relRefMap.clear();
		attrNameMap.clear();

		//populate nodetypeMap
		//take out Modifies and Uses because they are special cases.
		pair<string,QNODE_TYPE> pairNodeType1 ("Calls",QNODE_TYPE(Calls));
		nodetypeMap.insert(pairNodeType1);
		pair<string,QNODE_TYPE> pairNodeType2 ("Calls*",QNODE_TYPE(CallsT));
		nodetypeMap.insert(pairNodeType2);
		pair<string,QNODE_TYPE> pairNodeType3 ("Parent",QNODE_TYPE(Parent));
		nodetypeMap.insert(pairNodeType3);
		pair<string,QNODE_TYPE> pairNodeType4 ("Parent*",QNODE_TYPE(ParentT));
		nodetypeMap.insert(pairNodeType4);
		pair<string,QNODE_TYPE> pairNodeType5 ("Follows",QNODE_TYPE(Follows));
		nodetypeMap.insert(pairNodeType5);
		pair<string,QNODE_TYPE> pairNodeType6 ("Follows*",QNODE_TYPE(FollowsT));
		nodetypeMap.insert(pairNodeType6);
		pair<string,QNODE_TYPE> pairNodeType7 ("Next",QNODE_TYPE(Next));
		nodetypeMap.insert(pairNodeType7);
		pair<string,QNODE_TYPE> pairNodeType8 ("Next*",QNODE_TYPE(NextT));
		nodetypeMap.insert(pairNodeType8);
		pair<string,QNODE_TYPE> pairNodeType9 ("Affects",QNODE_TYPE(Affects));
		nodetypeMap.insert(pairNodeType9);
		pair<string,QNODE_TYPE> pairNodeType10 ("Affects*",QNODE_TYPE(AffectsT));
		nodetypeMap.insert(pairNodeType10);


		//populate relRefMap
		auto node1 = make_tuple(REF_TYPE(entRef), REF_TYPE(entRef)); 
		auto node2 = make_tuple(REF_TYPE(lineRef), REF_TYPE(lineRef)); 
		auto node3 = make_tuple(REF_TYPE(stmtRef), REF_TYPE(stmtRef));
		auto node4 = make_tuple(REF_TYPE(entRef), REF_TYPE(varRef));  
		auto node5 = make_tuple(REF_TYPE(stmtRef), REF_TYPE(varRef)); 
		auto node6 = make_tuple(REF_TYPE(ref), REF_TYPE(ref));

		pair<QNODE_TYPE,tuple<REF_TYPE,REF_TYPE>> pairRelNode1 (ModifiesP, node4);
		relRefMap.insert(pairRelNode1);
		pair<QNODE_TYPE,tuple<REF_TYPE,REF_TYPE>> pairRelNode2 (ModifiesS, node5);
		relRefMap.insert(pairRelNode2); 
		pair<QNODE_TYPE,tuple<REF_TYPE,REF_TYPE>> pairRelNode3 (UsesP, node4);
		relRefMap.insert(pairRelNode3);
		pair<QNODE_TYPE,tuple<REF_TYPE,REF_TYPE>> pairRelNode4 (UsesS, node5);
		relRefMap.insert(pairRelNode4);
		pair<QNODE_TYPE,tuple<REF_TYPE,REF_TYPE>> pairRelNode5 (Calls, node1);
		relRefMap.insert(pairRelNode5);
		pair<QNODE_TYPE,tuple<REF_TYPE,REF_TYPE>> pairRelNode6 (CallsT, node1);
		relRefMap.insert(pairRelNode6);
		pair<QNODE_TYPE,tuple<REF_TYPE,REF_TYPE>> pairRelNode7 (Parent, node3);
		relRefMap.insert(pairRelNode7);
		pair<QNODE_TYPE,tuple<REF_TYPE,REF_TYPE>> pairRelNode8 (ParentT, node3);
		relRefMap.insert(pairRelNode8);
		pair<QNODE_TYPE,tuple<REF_TYPE,REF_TYPE>> pairRelNode9 (Follows, node3);
		relRefMap.insert(pairRelNode9);
		pair<QNODE_TYPE,tuple<REF_TYPE,REF_TYPE>> pairRelNode10 (FollowsT, node3);
		relRefMap.insert(pairRelNode10);
		pair<QNODE_TYPE,tuple<REF_TYPE,REF_TYPE>> pairRelNode11 (Next, node2);
		relRefMap.insert(pairRelNode11);
		pair<QNODE_TYPE,tuple<REF_TYPE,REF_TYPE>> pairRelNode12 (NextT, node2);
		relRefMap.insert(pairRelNode12);
		pair<QNODE_TYPE,tuple<REF_TYPE,REF_TYPE>> pairRelNode13 (Affects, node3);
		relRefMap.insert(pairRelNode13);
		pair<QNODE_TYPE,tuple<REF_TYPE,REF_TYPE>> pairRelNode14 (AffectsT, node3);
		relRefMap.insert(pairRelNode14);
		pair<QNODE_TYPE,tuple<REF_TYPE,REF_TYPE>> pairRelNode15 (AttrCompare, node6);
		relRefMap.insert(pairRelNode15);

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
	 * Calls Query Tree to print the query tree built by Query Parser
	 * onto console (for testing and debugging).
	 **/
	void queryTreeTesting()
	{
		myQueryTree->printTree();

	}

	/**************************************************************/
	/**                     Matching                             **/
	/**************************************************************/

	/**
	 * Matches if the given token follows the naming convention for NAME, 
	 * as per the given grammar.
	 * @return TRUE if it matches, FALSE otherwise.
	 */
	bool matchName(string token)
	{
		std::regex nameRegex("[A-Za-z][\\w]*");

		return std::regex_match(token, nameRegex) ? true : false;
	}

	/**
	 * Matches if the given token follows the naming convention for INTEGER, 
	 * as per the given grammar.
	 * @return TRUE if it matches, FALSE otherwise.
	 */
	bool matchInteger(string token)
	{
		std::regex intRegex("\\d+");
		return (std::regex_match(token,intRegex)) ? true : false;
	}

	/**
	 * Matches if the given token follows the naming convention of a factor, 
	 * as per the given grammar.
	 * @return TRUE if it matches, FALSE otherwise.
	 */
	bool matchFactor(string token)
	{
		if(!(matchInteger(token) || matchName(token))){

			#ifdef DEBUG
				throw exception("QueryParser error: at matchFactor.");
			#endif
		}
		return (matchInteger(token) || matchName(token));
	}
	/**
	 * Checks if it's a factor 
	 * @return TRUE if it's a factor, FALSE otherwise.
	 **/
	bool isFactor(string token)
	{
		return (matchInteger(token) || matchName(token));
	}
	/**
	 * Matches if the given token follows the naming convention of Synonym and IDENT
	 * as per the given grammar.
	 * @return TRUE if it matches, FALSE otherwise.
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
	 * Matches if the given token is an underscore.
	 * @return TRUE if it matches, FALSE otherwise.
	 */
	bool matchUnderscore(string token)
	{
		return (token.compare("_") == 0);
	}
	
	/**
	 * Matches if the given token is an attrName as per 
	 * the given grammar.
	 * @return TRUE if it matches, FALSE otherwise.
	 */
	bool matchAttrName(string token)
	{
		for(int i=0; i<(sizeof(attrName)/sizeof(*attrName)); i++){
			if(attrName[i].compare(token) == 0)
				return true;
		}


		#ifdef DEBUG
			throw exception("QueryParser error: at matchAttrName.");
		#endif
		return false;
	}

	/**
	 * Matches if the given token is an attrRef as per the 
	 * given grammar. It checks if it has "synonym.attrName" syntax.
	 * @return TRUE if it matches, FALSE otherwise.
	 */
	bool matchAttrRef(string token)
	{

		if(!matchSynonymAndIdent(peekBackwards(2), false)){
			#ifdef DEBUG
				throw exception("QueryParser error: matchAttrRef(), error in synonymn.");
			#endif
			return false;
		}
		if (peekBackwards(1).compare(".") != 0){
			#ifdef DEBUG
				throw exception("QueryParser error: matchAttrRef(), error in '.'");
			#endif
			return false;
		}
		if(!matchAttrName(peekBackwards(0))){
			#ifdef DEBUG
				throw exception("QueryParser error: matchAttrRef(), error in attrName ");
			#endif
			return false;
		}

		return true;
	}


	/**
	 * Matches if the given token follows the naming convention
	 * of stmt reference or line reference. 
	 * @return TRUE if it matches, FALSE otherwise.
	 */
	bool matchStmtOrLineRef(string token)
	{
		if(matchSynonymAndIdent(token,false))
			return true;
		else if(matchInteger(token))
			return true;
		else if(matchUnderscore(token))
			return true;

		#ifdef DEBUG
			throw exception("QueryParser error: at matchStmtOrLineRef");
		#endif
		return false;
	}

	/**
	 * Matches if the given token follows the naming convention of 
	 * an entity reference.
	 * @return TRUE if it matches, FALSE otherwise.
	 */
	bool matchEntRef(string token)
	{
		if(matchSynonymAndIdent(token, false))
			return true;
		else if(matchUnderscore(token))
			return true;
		else if(matchSynonymAndIdent(token, true))
			return true;
		else if(matchInteger(token))
			return true;  
	
		#ifdef DEBUG
			throw exception("QueryParser error: at matchEntRef");
		#endif

		return false;
	}

	/**
	 * Matches if the given token follows the naming convention of 
	 * an variable reference.
	 * @return TRUE if it matches, FALSE otherwise.
	 */
	bool matchVarRef(string token)
	{
		if(matchSynonymAndIdent(token, false))
			return true;
		else if(matchUnderscore(token))
			return true;
		else if(matchSynonymAndIdent(token, true))
			return true;
	
		#ifdef DEBUG
			throw exception("QueryParser error: at matchEntRef");
		#endif

		return false;
	}

	/**
	 * Matches if the given token follows the naming convention of 
	 * a reference.
	 * @return TRUE if it matches, FALSE otherwise.
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
			throw exception("QueryParser error: at matchRef");
		#endif

		return false;
	 }

	/**
	 * Matches if the given token follows the naming convention of 
	 * a design entity.
	 * @return TRUE if it matches, FALSE otherwise.
	 */
	bool matchDesignEntity(string token)
	{
		for(int i=0; i<(sizeof(designEntities)/sizeof(*designEntities)); i++){
			if(designEntities[i].compare(token) == 0)
				return true;
		}

		#ifdef DEBUG
			throw exception("QueryParser error: at matchDesignEntity.");
		#endif
		
		return false;
	}

	/**************************************************************/
	/**                      Parsing                             **/
	/**************************************************************/

	/**
	 * Parses the next token and check if it is equal to the given target.
	 * @return TRUE if it equals, FALSE otherwise.
	 */
	bool parse(string target)
	{
		string nextToken = parseToken();
		return nextToken.compare(target) == 0;
	}

	/**
	 * Parses the next token and check if it is equal to an apostrophe.
	 * @return TRUE if it equals, FALSE otherwise.
	 */
	bool parseApostrophe()
	{
		string nextToken = parseToken();
		std::regex synonymRegex("[\"]");
		return std::regex_match(nextToken,synonymRegex) ? true : false;
	}

	/**
	 * Parses the next token and check if it is a design entity.
	 * @return TRUE if it's a design entity, FALSE otherwise.
	 */
	bool parseDesignEntity()
	{
		string nextToken = parseToken();
		return matchDesignEntity(nextToken);
	}

	/**
	 * Parses the next token and check if it is a synonym.
	 * @return TRUE if it's a synonym, FALSE otherwise.
	 */
	bool parseSynonymns()
	{
		string nextToken = parseToken();
		return matchSynonymAndIdent(nextToken, false);
	}

	/**
	 * It's able to parse both stmtRef and lineRef.
	 * Parses the next token and check if it is a stmt reference
	 * or line reference.
	 * @return an empty string if parsing fails.
	 */
	string parseStmtOrLineRef()
	{
		string nextToken = parseToken();
		return matchStmtOrLineRef(nextToken) ? nextToken: "";
	}

	/**
	 * Parses tokens and check if it is an entity reference.
	 * @return an empty string if parsing fails.
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
	 * Parses tokens and check if it is a variable reference.
	 * @return an empty string if parsing fails.
	 */
	string parseVarRef()
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

		return matchVarRef(nextToken) ? returnToken: "";
	}

	/**
	 * Parses tokens and check if it is a reference. 
	 * Supporting method for parsing with clause. 
	 * @return an empty string if parsing fails.
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
	 * Parses tokens and checks if it is a factor.
	 * @return FALSE if parsing fails.
	 */
	bool parseFactor()
	{
		bool res;
		string nextToken = parseToken();

		if(nextToken.compare("(")==0){
			res = parseExpr();
			if(!res) {return false;}

			res = parse(")");
			if(!res) {return false;}
		}else{

			res = matchFactor(nextToken);
			if(!res) {return false;}
		}

		return true;
	}

	/**
	 * Parses tokens and check if it is a term.
	 * @return FALSE if parsing fails.
	 */
	bool parseTerm()
	{

		bool res = parseFactor();
		if(!res) {return false;}

		while(peekInToTheNextToken().compare("*")==0){
			parse("*");

			res = parseFactor();
			if(!res) {return false;}
		}

		return true;
	}


	/**
	 * Parses tokens and check if it is an expression.
	 * @return FALSE if parsing fails.
	 */
	bool parseExpr()
	{
		string peekToken;
		bool res;
		int isMinus, isPlus; 

		res = parseTerm();
		if(!res) {return false;}

		peekToken = peekInToTheNextToken();
		isMinus = peekToken.compare("-");
		isPlus = peekToken.compare("+");

		while(isMinus==0 || isPlus==0){
			
			//parse - or +
			if(isMinus==0){
				parse("-");
			}else if(isPlus==0){
				parse("+");
			}

			res = parseTerm();
			if(!res) {return false;}

			peekToken = peekInToTheNextToken();
			isMinus = peekToken.compare("-");
			isPlus = peekToken.compare("+");
		}

		return true;
	}


	/**
	 * Parses tokens and check if it is an expression-spec (for patterns).
	 * @return FALSE if parsing fails.
	 */
	bool parseExpressionSpec()
	{
		bool res;
		bool underscorePresent = false;

		if(peekInToTheNextToken().compare("_")==0){

			underscorePresent = true;
			res = parse("_");
		
		}

		res = parseApostrophe();
		if(!res) {return false;}

		res = parseExpr();
		if(!res) {return false;}

		res = parseApostrophe();
		if(!res) {return false;}

		

		if(underscorePresent){

			res = parse("_");
			if(!res) {return false;}
		}

		return true;
	}


	/**
	 *	Helper function for parse such that clause
	 *  After parsing the relRef, build a tree.
	 *  @To-do Temporarily it's for such-that. Extend it for patterns, select, with
	 *  @return TRUE if a QNode is created and linked to Query Tree built, 
	 *  and FALSE if query validation failed in Query Validator. 
	 */
	bool buildQueryTree(QNODE_TYPE relRef, Synonym s1, Synonym s2)
	{				
		bool res;

		//validate the query
		res = myQueryV->validateSuchThatQueries(relRef, s1, s2);
		if(!res){
			#ifdef DEBUG
				throw exception("QueryParser error: validateSuchThatQueries error");
			#endif
			return false;
		}

		//build the tree
		QNode* childNode = myQueryTree->createQNode(relRef, Synonym(), s1, s2);
		res = myQueryTree->linkNode(myQueryTree->getClausesNode(), childNode);

		return res;
	}

	/**
	 * Supporting function to create synonyms.
	 * @return Synonym() if it is unable to create a Synonym.
	 */
	Synonym createSynonym(QNODE_TYPE relRef,string value, int arg)
	{
		SYNONYM_TYPE DE_type;
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


		if(node==REF_TYPE(stmtRef) || node==REF_TYPE(lineRef)){

			if (synonymsMap.count(value) > 0){
				DE_type = synonymsMap.at(value); 
			}else if(value.compare("_")==0){
				DE_type = UNDEFINED;   //Used to denote "_"
			}else if(matchInteger(value)){
				DE_type = STRING_INT;
			}else{
				return Synonym();  //error type mismatch
			}

			//build Synonym and return 
			return Synonym(DE_type,value);

		}else if(node==REF_TYPE(entRef)){

			if (std::regex_match(peekBackwards(0),apostrophe)){   //if it has apostrophe(ie it's """IDENT""")
				DE_type = STRING_CHAR;
			}else if(value.compare("_")==0){
				DE_type = UNDEFINED;
			}else if(synonymsMap.count(value) > 0){
				DE_type = synonymsMap.at(value);
			}else if(matchInteger(value)){
				DE_type = STRING_INT;
			}else{
				return Synonym();  //error type mismatch
			}

			//build Synonym and return
			return Synonym(DE_type,value);

		}else if(node==REF_TYPE(varRef)){

			if (std::regex_match(peekBackwards(0),apostrophe)){   //if it has apostrophe(ie it's """IDENT""")
				DE_type = STRING_CHAR;
			}else if(value.compare("_")==0){
				DE_type = UNDEFINED;
			}else if(synonymsMap.count(value) > 0){
				DE_type = synonymsMap.at(value);
			}else{
				return Synonym();  //error type mismatch
			}

			//build Synonym and return
			return Synonym(DE_type,value);

		}else if(node==REF_TYPE(ref)){

			if (std::regex_match(peekBackwards(0),apostrophe)){   //if it has apostrophe(ie it's """IDENT""")
				DE_type = STRING_CHAR;
			}else if(synonymsMap.count(value) > 0){
				DE_type = synonymsMap.at(value);
			}else if(matchInteger(value)){
				DE_type = STRING_INT;
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

				
				return Synonym(DE_type,value, attribute);
			}


			//build Synonym and return
			return Synonym(DE_type,value,SYNONYM_ATTRIBUTE(empty));
		}

		return Synonym(); //wont reach here

	}

	/**
	 *	Helper function to parse such that clause.
	 *  @param[in] input the relRef (ie "Follows*") and the argument it's parsing(either 1 or 2 only) 
	 *  Argument 1 can be an entRef, stmtRef or lineRef.
	 *  Argument 2 can be an entRef, stmtRef, lineRef or varRef.
	 *  @return an empty string if parsing fails.
	 *  @To-do return synonym  
	 */
	string parseArg(QNODE_TYPE relRef, int arg)
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
		
		if(node==REF_TYPE(stmtRef) || node==REF_TYPE(lineRef)){

			entRef_value = parseStmtOrLineRef();
			
		}else if(node==REF_TYPE(entRef)){

			entRef_value = parseEntRef();
		
		}else if(node==REF_TYPE(varRef)){

			entRef_value = parseVarRef();  

		}//else do nothing and an empty string is passed.


		return entRef_value;
	}

	/**
	 * Converts valid relRef types into QNODE_TYPE.
	 * @return QNODE_TYPE if it's a valid string relRef, 
	 * otherwise return a null.
	 */
	QNODE_TYPE convertIntoEnum(string relRef){

		QNODE_TYPE nodeType;
		std::regex apostrophe("[\"]");

		//get the nodeType
		if (nodetypeMap.count(relRef) > 0){   //find in the relRef map

			nodeType = nodetypeMap.at(relRef);

		}else if(relRef.compare("Modifies")==0){

			if (std::regex_match(peekInToTheNextNextToken(),apostrophe)){

				nodeType = ModifiesP;

			}else{

				nodeType = ModifiesS;

			}

		}else if(relRef.compare("Uses")==0){

			if (std::regex_match(peekInToTheNextNextToken(),apostrophe)){

				nodeType = UsesP;

			}else{

				nodeType = UsesS;

			}

		}else{

			#ifdef DEBUG
				throw exception("QueryParser error: convertIntoEnum(), invalid relRef used.");
			#endif

		} 


		return nodeType;
	}

	/**
	 * Creates QNode Such that and validates the such that clause.
	 * @return FALSE if there are errors in the such that portion of query.
	 */
	bool parseSuchThatClause()
	{
		bool res;
		QNODE_TYPE relRef_enum;
		string DE_type, nextToken, entRef_value1,entRef_value2;

		nextToken = parseToken();
		for(int i=0; i<(sizeof(relRef)/sizeof(*relRef)); i++){

			if(nextToken.compare(relRef[i]) == 0){


				relRef_enum = convertIntoEnum(relRef[i]);  


				res = parse("(");
				if (!res){return false;} 

				entRef_value1 = parseArg(relRef_enum, 1);
				if(entRef_value1.compare("")==0){
					#ifdef DEBUG
						throw exception("QueryParser error: parseSuchThatClause(), invalid arg1 used.");
					#endif
					return false;
				} //empty string if parsing fails.

				//create synonym s1
				Synonym s1 = createSynonym(relRef_enum, entRef_value1, 1);
				if(s1.isEmpty()){ //unable to create synonym
					return false;}

				res = parse(",");
				if (!res){return false;} 

				entRef_value2 = parseArg(relRef_enum, 2);
				if(entRef_value2.compare("")==0){

					#ifdef DEBUG
						throw exception("QueryParser error: parseSuchThatClause(), invalid arg2 used.");
					#endif
					return false;
				} //empty string if parsing fails.

				//create synonym s2
				Synonym s2 = createSynonym(relRef_enum, entRef_value2, 2);
				if(s2.isEmpty()){ //unable to create synonym
					return false;}
				
				res = parse(")");
				if (!res){ return false;} 



				/***Parsing is done. Building Query Tree ***/
				res = buildQueryTree(relRef_enum, s1, s2);
				if(!res){
					#ifdef DEBUG
						throw exception("QueryParser error: parseSuchThatClause(), in buildQueryTree.");
					#endif
				}

				return res;
			}
		}

		return false;
	}

	/**
	 * Creates QNode Pattern and validates the pattern clause.
	 * @return FALSE if there are errors in the pattern portion of query.
	 */
	bool parsePatternClause()
	{
		bool res;
		SYNONYM_TYPE DE_type, DE_type2;
		bool whilePatternExp = false, ifPatternExp = false;

		//parse syn-assign
		if(synonymsMap.count(parseToken()) > 0){
			DE_type = synonymsMap.at(peekBackwards(0));
		}else{
			#ifdef DEBUG
				throw exception("QueryParser error: parsePatternClause(), invalid synonym used.");
			#endif
			return false;
		}

		if(DE_type==WHILE){
			whilePatternExp = true;
		}else if(DE_type==IF){
			ifPatternExp = true;
		}else if(DE_type!=ASSIGN){
			//patterns can only be either while, if or assign.
			#ifdef DEBUG
				throw exception("QueryParser error: parsePatternClause(), synonym not 'assign' or 'while' or 'if' type. ");
			#endif

			return false;
		}

		Synonym pattern_arg0(DE_type, peekBackwards(0)); 
		

		res = parse("(");
		if (!res){
			#ifdef DEBUG
				throw exception("QueryParser error: parsePatternClause(), missing '('.");
			#endif

			return false;
		} 

		string pattern_variable = parseVarRef();
		if (pattern_variable.compare("")==0){
			#ifdef DEBUG
				throw exception("QueryParser error: parsePatternClause(), invalid entRef arg1.");
			#endif

			return false;
		} 


		//Build Query Tree
		std::regex apostrophe("[\"]");
		if (std::regex_match(peekBackwards(0),apostrophe)){
			DE_type = STRING_CHAR;
		}else if(pattern_variable.compare("_")==0){
			DE_type = UNDEFINED;
		}else if(synonymsMap.count(pattern_variable) > 0){
			DE_type = synonymsMap.at(pattern_variable);
		}else{
			#ifdef DEBUG
				throw exception("QueryParser error: parsePatternClause(), building query tree error.");
			#endif
			return false;
		}

		res = parse(",");
		if (!res){
			#ifdef DEBUG
				throw exception("QueryParser error: parsePatternClause(), comma missing");
			#endif

			return false;
		} 

		if(whilePatternExp){

			res = parse("_");
			if (!res) {return false;}
		
		}else if(ifPatternExp){

			res = parse("_");
			if (!res) {return false;}
			res = parse(",");
			if (!res) {return false;}
			res = parse("_");
			if (!res) {return false;}
		
		}else{

			//an assign pattern 
			if((peekInToTheNextToken().compare("_")==0) && (peekInToTheNextNextToken().compare(")")==0)){

				res = parse("_"); //dont need to check res, checked in the if stmts. 

			}else{
				res = parseExpressionSpec();
				if (!res) {return false;} 
			}
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
				throw exception("QueryParser error: parsePatternClause(), missing ')' ");
			#endif
			return false;
		} 

		if(pattern_patterns.compare("_")==0)
			DE_type2 = UNDEFINED; //Used to denote "_"
		else
			DE_type2 = STRING_PATTERNS;


		//Build Query Tree
		Synonym pattern_var(DE_type, pattern_variable);
		Synonym pattern_pattern(DE_type2, pattern_patterns);
		
		res = myQueryV->validatePatternQueries(pattern_arg0, pattern_var, pattern_pattern);
		if(!res){
			#ifdef DEBUG
				throw exception("QueryParser error: validatePatternQueries(), returns error ");
			#endif
			return false;
		}

		QNode* patternQueryNode = myQueryTree->createQNode(Pattern,pattern_arg0, pattern_var, pattern_pattern);
		myQueryTree->linkNode(myQueryTree->getClausesNode(), patternQueryNode);


		return true;
	}

	/**
	 * Creates QNode With and validates the with clause.
	 * @return FALSE if there are errors in the with portion of query.
	 */
	bool parseAttrCompare()
	{
		string ref1 = parseRef();
		if(ref1.compare("")==0){
			
			#ifdef DEBUG
				throw exception("QueryParser error: parseAttrCompare(), parseRef argument 1");
			#endif
			return false;
		}
		Synonym s1 = createSynonym(AttrCompare, ref1, 1);
		if(s1.isEmpty()){ //unable to create synonym
			return false;}

		bool res = parse("=");
		if(!res) {return false;}

		string ref2 = parseRef();
		if(ref2.compare("")==0){

			#ifdef DEBUG
				throw exception("QueryParser error: parseAttrCompare(), parseRef argument 2");
			#endif
			return false;}
		Synonym s2 = createSynonym(AttrCompare, ref2, 2);
		if(s2.isEmpty()){ //unable to create synonym
			return false;}

		//Validate attrCompare: ref = ref, to check that both ref is valid
		res = myQueryV->validateWithQueries(s1,s2);
		if(!res){
			#ifdef DEBUG
				throw exception("QueryParser error: validateWithQueries(), returns error ");
			#endif
			return false;
		}

		//build query tree
		QNode* withQueryNode = myQueryTree->createQNode(With,Synonym(),s1,s2);
		myQueryTree->linkNode(myQueryTree->getClausesNode(), withQueryNode);

		return  true;
	}

	/**
	 * Calls parseAttrCompare().
	 * @return FALSE if there are errors in the with portion of query.
	 */
	bool parseWithClause()
	{
		bool res = parseAttrCompare();
		return res;
	}

	/**
	 * @return FALSE if there are errors in the optional clauses.
	 */
	bool parseOptionalClauses()
	{

		bool res;
		
		if(peekInToTheNextToken().compare("such")==0){

			res = parse("such");
			if (!res){return false;}
			res = parse("that");
			if (!res){return false;}

			res = parseSuchThatClause();
			if (!res){
				return false;}

			while(peekInToTheNextToken().compare("and")==0){
				
				parse("and");

				res = parseSuchThatClause();
				if (!res){return false;}
			}



		}else if(peekInToTheNextToken().compare("pattern")==0){
			
			res = parse("pattern");
			if (!res){return false;}

			res = parsePatternClause();
			if (!res){return false;}

			while(peekInToTheNextToken().compare("and")==0){
			
				parse("and");

				res = parsePatternClause();
				if (!res){return false;}
			}


		}else if(peekInToTheNextToken().compare("with")==0){
			
			res = parse("with");
			if(!res) {return false;}

			res = parseWithClause();
			if (!res){return false;}

			while(peekInToTheNextToken().compare("and")==0){
			
				parse("and");

				res = parseWithClause();
				if (!res){return false;}
			}

		}else{
			
			#ifdef DEBUG
				throw exception("QueryParser error: parseOptionalClauses() error.");
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
		for (unordered_map<string, SYNONYM_TYPE>::iterator it = synonymsMap.begin(); it != synonymsMap.end(); ++it){
			string type = it->first;
			string name = Synonym::convertToString(it->second);

			cout <<"name : "<<name <<" type : " <<type<<endl;
		}
	}

	/**
	 * Supporting function to parse declarations.
	 * @return FALSE if there are errors in the declarations.
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
				pair<string,SYNONYM_TYPE> pairOfSynonyms (peekBackwards((2*x)-1),Synonym::convertToEnum(peekBackwards((2*i)+2)));
				if (synonymsMap.count(peekBackwards((2*x)-1)) >0){
					#ifdef DEBUG
						throw exception("QueryParser error: synonymn declaration error, the synonymn has been declared previously.");
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
	 * @return FALSE if there are errors in the select synonym.
	 */
	bool parseSelect()
	{
		SYNONYM_TYPE DE_type;
		QNODE_TYPE nodeType;
		QNode* childNode;

		bool res = parse("Select");
		if (!res){
			#ifdef DEBUG
				throw exception("QueryParser error: parseSelect(), missing 'Select' keyword");
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
						throw exception("QueryParser error: synonymsMap error, unable to find the synonym");
					#endif
					return false;
				}

			}else{
				#ifdef DEBUG
					throw exception("QueryParser error: synonymsMap error, synonymsMap is empty");
				#endif
				return false;
			}

			Synonym s1(DE_type,peekBackwards(0));
			childNode = myQueryTree->createQNode(Selection, Synonym(), s1, Synonym());

		}
		res = myQueryTree->linkNode(myQueryTree->getResultNode(), childNode);
		return res;

	}

	/**
	 * @return FALSE if there are errors in query declarations and select synonym.
	 */
	bool parseQuerySelectClause()
	{
		bool res; 
		res = parseDeclarations();
		if (!res){
			#ifdef DEBUG
				throw exception("QueryParser error: declarations failed.");
			#endif
			return false;}

		res = parseSelect();
		if (!res){
			#ifdef DEBUG
				throw exception("QueryParser error: select failed.");
			#endif
			return false;}

		while(buffer.size() && bufferIter != buffer.end()){
			res = parseOptionalClauses();
			if (!res){
			#ifdef DEBUG
				throw exception("QueryParser error: optional clauses failed.");
			#endif
			return false;}
		}
		
		return res;
	}

	/**
	 * It takes in a query, validates the query and builds a query tree. 
	 * @returns TRUE always. If the input query is invalid, an empty 
	 * (default) query tree and synonyms map is passed.  
	 */
	bool parseQuery()
	{
		//testingQueryParser();

		myQueryV = new QueryValidator();
		myQueryV->initTable(); //propagate relationships data onto table

		initQueryTreeAndSymbolsTable();

		bool res = parseQuerySelectClause();
		//If there's an error in parsing the queries, return an empty query tree.  
		if(!res){
			#ifdef DEBUG
				throw exception("QueryParser error: Error in parsing query. Empty query tree and synonymsMap is passed.");
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