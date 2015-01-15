#include <stdio.h>
#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <cstring>
#include <regex>

using namespace std;

#include "ExpressionParser.h"
#include "TNode.h"
#include "PKB.h"
#include "SourceParser.h"

ExpressionParser::ExpressionParser() {
	operPrecedence = makeOperatorMap();
	this->stmtNum = 0;
}

/**
 * Before calling parse, call this function to initialise the buffer in the expression parser
 * @param newBuffer A vector containing the tokenized right side of expression. e.g. For "a = x + y;" , a vector containing ["x", "+", "y", ";"] should be passed in
 * @param skip		optional. The number of tokens to skip in the newBuffer. Use if the buffer contains garbage in front.
 */
void ExpressionParser::updateBuffer(vector<string> newBuffer, int skip) {
	buffer = newBuffer;
	bufferIter = buffer.begin() + skip; // 

	token = *(bufferIter++);
}

/**
 * Updates the stmtNum of the expression. If the expression parser is used to parse the query, there is no need to call this.
 */
void ExpressionParser::updateStmtNum(int stmtNum) {
	this->stmtNum = stmtNum;
}

//@Todo code duplication between here and Parser. Might not need to bother though
string matchInteger(string token) {
	std::regex intRegex("\\d+");
	return (std::regex_match(token,intRegex)) ? token : "";
}

int parseConstant(string value, int stmtNum) {
	PKB pkb = PKB::getInstance();
	int constant = atoi(value.c_str());
	
	pkb.insertConstant(constant, stmtNum);
	
	return constant;
}

int parseVariable(string value, int stmtNum) {
	PKB pkb = PKB::getInstance();
	
	pkb.insertVar(value, stmtNum);
	int varIndx = pkb.getVarIndex(value);

	// update pkb 
	// @TODO this should not be done here
	pkb.setUses(stmtNum, varIndx);

	while (pkb.getParent(stmtNum).size()) {
		stmtNum = pkb.getParent(stmtNum).at(0);
		if (stmtNum > 0) {
			pkb.setUses(stmtNum, varIndx);	
		}
	}

	return varIndx;
}

int parseConstantOrVariable(string value, int stmtNum) {
	string constant = matchInteger(value);
	if (constant.empty()) { 
		return parseVariable(value, stmtNum);
	} else {
		return parseConstant(value, stmtNum);
	}
}

int ExpressionParser::getOperatorPrecedence(string token) {
	if (operPrecedence.count(token)) {
		return operPrecedence[token];
	} else {
		return 0;
	}
}

TNode* ExpressionParser::operatorAdd(TNode* left) {
	TNode* right = parse(operPrecedence["+"]);
	PKB pkb = PKB::getInstance();
	TNode* top = pkb.createTNode(Plus, stmtNum, -2);

	pkb.createLink(Child, top, left);
	pkb.createLink(Child, top, right);

	return top;
}


TNode* ExpressionParser::operatorMultiply(TNode* left) {
	TNode* right = parse(operPrecedence["*"]);
	PKB pkb = PKB::getInstance();
	TNode* top = pkb.createTNode(Plus, stmtNum, -2);

	pkb.createLink(Child, top, left);
	pkb.createLink(Child, top, right);

	return top;
}

TNode* ExpressionParser::operatorSubtract(TNode* left) {
	TNode* right = parse(operPrecedence["-"]);
	PKB pkb = PKB::getInstance();
	TNode* top = pkb.createTNode(Plus, stmtNum, -2);

	pkb.createLink(Child, top, left);
	pkb.createLink(Child, top, right);

	return top;
}


/**
 * Method to parse an expression. Returns a tree.
 * @param bindingLevel leave empty for normal usage. Used internally during parsing.
 * @sa ::updateBuffer
 * @sa ::updateStmtNum
 */
TNode* ExpressionParser::parse(int bindingLevel) {
	PKB pkb = PKB::getInstance();

	string prevToken = token;

	token = *(bufferIter ++);

	TNode* leftNode;
	if (prevToken.compare("(") == 0) {
		leftNode = parse();
		// @Todo, for validation: verify ')'
	} else {
		// prevToken is either variable or constant now
		bool isVariable = matchInteger(prevToken).empty();
		leftNode = pkb.createTNode(isVariable ? Variable : Constant, stmtNum, parseConstantOrVariable(prevToken, stmtNum));
	}

	
	while (bindingLevel < getOperatorPrecedence(token) && (token.compare(";") != 0)) {
		prevToken = token;

		token = *(bufferIter ++);
		if (prevToken.compare("+") == 0) {
			leftNode = operatorAdd(leftNode);
		} else if (prevToken.compare("*") == 0) {
			leftNode = operatorMultiply(leftNode);
		} else if (prevToken.compare("-") == 0) {
			leftNode = operatorSubtract(leftNode);
		}
	}

	return leftNode;
}
