#pragma once

#include <vector>
#include <string>
#include <cassert>
#include "TNode.h"
#include "VarTable.h"
#include "PKB.h"

using namespace std;

/**
 * Parser to create a tree from a math expression. 
 * Based on Pratt Parser.
 */
class ExpressionParser {
	private:		
		unordered_map<string, int> operPrecedence;
		vector<string>::iterator bufferIter;
		vector<string> buffer;
		int stmtNum;
		int procIndex;
		string token; // current token

		bool readOnly; /*! if this is set to true, the expression will not make any updates onto the pkb. This is used when parsing expressions for querying */

		VarTable* varTable;

		TNode* operatorAdd(TNode* left);
		TNode* operatorMultiply(TNode* left);
		TNode* operatorSubtract(TNode* left);
		TNode* operatorBracket(TNode* left);

		unordered_map<string, int> makeOperatorMap() {
			unordered_map<string, int> operMap;
			operMap["+"] = 10;
			operMap["*"] = 20;
			operMap["-"] = 10;
			operMap["("] = 0;
			operMap[")"] = 0;
			return operMap;
		};

		int getOperatorPrecedence(string oper);
		TNode* parseExpression(vector<string>, bool readOnly);
		TNode* parse(int bindingLevel = 0);	
		void updateBuffer(vector<string>);  
		void init();

	public:
		ExpressionParser();						// use this in files that are not "TestExpressionParser"
		ExpressionParser(VarTable* varTable);   /*! used during testing, to not involve the entire pkb singleton */
		~ExpressionParser();
		
		void updateStmtNum(int);					
		void updateProcIndex(int);		
		
		TNode* parseExpressionForAST(vector<string>);		// use this in parser for parsing SIMPLE
		TNode* parseExpressionForQuerying(vector<string>);  // use this when parsing queries
};
