#pragma once

#include <vector>
#include <string>
#include <cassert>
#include "TNode.h"
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
		string token; // current token

		TNode* operator_add_token(TNode* left);

		unordered_map<string, int> makeOperatorMap() {
			unordered_map<string, int> operMap;
			operMap["+"] = 10;
			operMap["*"] = 20;
			return operMap;
		};

		int getOperatorPrecedence(string oper);

	public:
		ExpressionParser();
		void updateBuffer(vector<string>, int skip = 2);
		void updateStmtNum(int);
		TNode* parse(int bindingLevel = 0);
};
