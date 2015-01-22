#include <stdio.h>
#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <cstring>
#include <regex>

#include "ExpressionParser.h"
#include "TNode.h"
#include "PKB.h"
#include "SourceParser.h"

using namespace std;

ExpressionParser::ExpressionParser() 
{
	varTable = NULL;
	operPrecedence = makeOperatorMap();
	this->stmtNum = 0;
}

/**
 * Constructor. For use in testing, to avoid potential problems with using a global pkb singleton that may interfere with other tests.
 */
ExpressionParser::ExpressionParser(VarTable* varTable) : varTable(varTable)  {
	operPrecedence = makeOperatorMap();
	this->stmtNum = 0;
}

ExpressionParser::~ExpressionParser() 
{
	if (varTable) 
	{
		delete varTable;
	}
}

/**
 * Before calling parse, call this function to initialise the buffer in the expression parser.
 * 
 * @param newBuffer A vector containing the tokenized right side of expression. e.g. For "a = x + y;" , a vector containing ["x", "+", "y", ";"] should be passed in.
 *                  Alternatively, ["x", "+", "y"] can be passed in as well.
 * @param skip		optional. The number of tokens to skip in the newBuffer. Use if the buffer contains other tokens in front.
 */
void ExpressionParser::updateBuffer(vector<string> newBuffer, int skip) 
{
	// the expression parser uses ';' to determine if its at the end
	if (newBuffer.back().compare(";") != 0) {
		newBuffer.push_back(";");
	}
	
	buffer = newBuffer;
	bufferIter = buffer.begin() + skip; // 
	
	token = *(bufferIter);
	++bufferIter;
}

/**
 * Updates the stmtNum of the expression. For parsing queries, there is no need to call this.
 */
void ExpressionParser::updateStmtNum(int stmtNum) 
{
	this->stmtNum = stmtNum;
}

//@Todo code duplication between here and Parser. Might not need to bother though
string matchInteger(string token) 
{
	std::regex intRegex("\\d+");
	return (std::regex_match(token,intRegex)) ? token : "";
}

int parseConstant(string value, int stmtNum) 
{
	PKB pkb = PKB::getInstance();
	int constant = atoi(value.c_str());
	
	pkb.insertConstant(constant, stmtNum);
	
	return constant;
}

int parseVariable(string value, int stmtNum, VarTable* varTable = NULL)  
{
	PKB pkb = PKB::getInstance();
	int varIndx;

	if (varTable == NULL) 
	{
		pkb.insertVar(value, stmtNum);
		varIndx = pkb.getVarIndex(value);
	} else 
	{
		varTable->insertVar(value, stmtNum);
		varIndx = varTable->getVarIndex(value);
	}

	// update pkb 
	// @TODO this should not be done here
	pkb.setUses(stmtNum, varIndx);

	while (pkb.getParent(stmtNum).size()) 
	{
		stmtNum = pkb.getParent(stmtNum).at(0);
		if (stmtNum > 0) 
		{
			pkb.setUses(stmtNum, varIndx);	
		}
	}

	return varIndx;
}

int parseConstantOrVariable(string value, int stmtNum, VarTable* varTable = NULL) 
{
	string constant = matchInteger(value);
	if (constant.empty()) 
	{ 
		return parseVariable(value, stmtNum, varTable);
	} else 
	{
		return parseConstant(value, stmtNum);
	}
}

int ExpressionParser::getOperatorPrecedence(string token) 
{
	if (operPrecedence.count(token)) 
	{
		return operPrecedence[token];
	} else {
		return 0;
	}
}

TNode* ExpressionParser::operatorAdd(TNode* left) 
{
	TNode* right = parse(operPrecedence["+"]);
	PKB pkb = PKB::getInstance();
	TNode* top = pkb.createTNode(Plus, stmtNum, -2);

	pkb.createLink(Child, top, left);
	pkb.createLink(Child, top, right);

	return top;
}


TNode* ExpressionParser::operatorMultiply(TNode* left) 
{
	TNode* right = parse(operPrecedence["*"]);
	PKB pkb = PKB::getInstance();
	TNode* top = pkb.createTNode(Plus, stmtNum, -2);

	pkb.createLink(Child, top, left);
	pkb.createLink(Child, top, right);

	return top;
}

TNode* ExpressionParser::operatorSubtract(TNode* left) 
{
	TNode* right = parse(operPrecedence["-"]);
	PKB pkb = PKB::getInstance();
	TNode* top = pkb.createTNode(Plus, stmtNum, -2);

	pkb.createLink(Child, top, left);
	pkb.createLink(Child, top, right);

	return top;
}


/**
 * Method to parse an expression. Returns the root of a tree.
 * For example, given an expression x + (y + z), the following tree is the output.
 *                           + <- root
 *                         /   \
 *                        x     +
 *                             / \
 *                            y   z
 * @param bindingLevel Leave empty for normal usage. Used internally during parsing.
 * @sa ExpressionParser::updateBuffer
 * @sa ExpressionParser::updateStmtNum
 */
TNode* ExpressionParser::parse(int bindingLevel)  //@Todo make private
{
	PKB pkb = PKB::getInstance();

	string prevToken = token;

	token = *(bufferIter ++);

	TNode* leftNode;
	if (prevToken.compare("(") == 0) 
	{
		leftNode = parse();
		// @Todo, for validation: verify ')'
	} else {
		// prevToken is either variable or constant now
		bool isVariable = matchInteger(prevToken).empty();

		if (varTable == NULL) 
		{
			leftNode = pkb.createTNode(isVariable ? Variable : Constant, stmtNum, parseConstantOrVariable(prevToken, stmtNum));
		} else 
		{
			// this is called when running in unit test
			leftNode = pkb.createTNode(isVariable ? Variable : Constant, stmtNum, parseConstantOrVariable(prevToken, stmtNum, varTable));
		}
				   
	}

	
	while (bindingLevel < getOperatorPrecedence(token) && (token.compare(";") != 0)) 
	{
		prevToken = token;

		token = *(bufferIter ++);
		if (prevToken.compare("+") == 0) 
		{
			leftNode = operatorAdd(leftNode);
		} else if (prevToken.compare("*") == 0) 
		{
			leftNode = operatorMultiply(leftNode);
		} else if (prevToken.compare("-") == 0) 
		{
			leftNode = operatorSubtract(leftNode);
		}
	}

	return leftNode;
}

/**
 * Method to parse an expression. Returns the root of a tree.
 * For example, given an expression x + y + z, the following tree is the output.
 *                           + <- root
 *                         /   \
 *                        +     z
 *                       / \
 *                      x   y
 * @param buffer A vector containing the tokenized right side of expression. e.g. For "a = x + y;" , a vector containing ["x", "+", "y", ";"] should be passed in
 * @sa ExpressionParser::updateBuffer
 * @sa ExpressionParser::updateStmtNum
 */
TNode* ExpressionParser::parseExpression(vector<string> buffer) {
	updateBuffer(buffer);
	return parse();
} 