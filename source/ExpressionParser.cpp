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
}


void ExpressionParser::updateBuffer(vector<string> newBuffer, int skip) {
	buffer = newBuffer;
	cout << "size of buffer = " << buffer.size() << endl;
	cout << "skip " << skip << endl;
	bufferIter = buffer.begin() + skip; // 

	token = *(bufferIter++);
}

void ExpressionParser::updateStmtNum(int stmtNum) {
	this->stmtNum = stmtNum;
}

//@TODO: code duplication between here and Parser. Might not need to bother though
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
	// @TODO 
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

TNode* ExpressionParser::operator_add_token(TNode* left) {
	TNode* right = parse(operPrecedence["+"]);
	PKB pkb = PKB::getInstance();
	TNode* top = pkb.createTNode(Plus, stmtNum, -2);

	pkb.createLink(Child, top, left);
	pkb.createLink(Child, top, right);

	return top;
}

TNode* ExpressionParser::parse(int bindingLevel) {
	PKB pkb = PKB::getInstance();

	string prevToken = token;

	token = *(bufferIter ++);


	bool isVariable = matchInteger(prevToken).empty();
	TNode* leftNode = pkb.createTNode(isVariable ? Variable : Constant, stmtNum, parseConstantOrVariable(prevToken, stmtNum));

	
	while (bindingLevel < getOperatorPrecedence(token) && (token.compare(";") != 0)) {
		prevToken = token;

		token = *(bufferIter ++);
		if (prevToken.compare("+") == 0) {
			leftNode = operator_add_token(leftNode);
		}
	}

	return leftNode;
}
