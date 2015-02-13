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

void ExpressionParser::init() {
	operPrecedence = makeOperatorMap();
	this->stmtNum = 1;
	this->procIndex = 0;
	this->readOnly = false;
}

ExpressionParser::ExpressionParser() 
{
	varTable = NULL;
	init();
}


/**
 * Constructor. Only for use in unit testing, to avoid potential problems with using a global pkb singleton that may interfere with other unit tests.
 */
ExpressionParser::ExpressionParser(VarTable* varTable) : varTable(varTable)  {
	init();  
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
 */
void ExpressionParser::updateBuffer(vector<string> newBuffer) 
{
	// the expression parser uses ';' to determine if its at the end
	if (newBuffer.back().compare(";") != 0) {
		newBuffer.push_back(";");
	}
	
	buffer = newBuffer;
	bufferIter = buffer.begin(); // 
	
	token = *(bufferIter);
	++bufferIter;
}

void ExpressionParser::updateProcIndex(int procIndex) {
	this->procIndex = procIndex;
}

/**
 * Updates the stmtNum of the expression. For parsing queries, there is no need to call this.
 */
void ExpressionParser::updateStmtNum(int stmtNum) 
{
	this->stmtNum = stmtNum;
}

int matchConstant(string value, int stmtNum, VarTable* varTable = NULL, bool readOnly = false) 
{
	PKB pkb = PKB::getInstance();
	int constant = atoi(value.c_str());
	
	bool isUnderTest = varTable != NULL;
	if (!readOnly && !isUnderTest) {
		pkb.insertConstant(constant, stmtNum);
	}
	
	return constant;
}

int matchVariable(string value, int stmtNum, int procIndex, VarTable* varTable = NULL, bool readOnly = false)  
{
	PKB pkb = PKB::getInstance();
	int varIndx;

	// prevent writing to pkb when parsing expression on the query side
	if (readOnly) {
		varIndx = varTable == NULL? pkb.getVarIndex(value) : varTable->getVarIndex(value);
		if (varIndx == -1) {
			throw runtime_error("ExpressionParser: variable '" + value + "' is not found in varTable!");
		}
		return varIndx;
	}

	// write to pkb if not under test, otherwise write to vartable
	bool isUnderTest = varTable != NULL;
	if (!isUnderTest) {
		pkb.insertVar(value, stmtNum);
		varIndx = pkb.getVarIndex(value);

		// update pkb 
		// @todo this really should not be done here
		//pkb.setUses(stmtNum, varIndx);
		pkb.setUsesProc(procIndex, varIndx);
	
		// propagate Uses to parent nodes
		/*while (pkb.getParent(stmtNum).size())  {
			stmtNum = pkb.getParent(stmtNum).at(0);
			if (stmtNum > 0) {
				pkb.setUses(stmtNum, varIndx);	
			}
		}*/
	} else {
		varTable->insertVar(value, stmtNum);
		varIndx = varTable->getVarIndex(value);
	}
	
	
	return varIndx;
}

int parseConstantOrVariable(string value, int stmtNum, int procIndex, VarTable* varTable = NULL, bool readOnly = false) 
{
	string constant = Parser::matchInteger(value);
	if (constant.empty()) 
	{ 
		return matchVariable(value, stmtNum, procIndex, varTable, readOnly);
	} else 
	{
		return matchConstant(value, stmtNum, varTable, readOnly);
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
	TNode* top = pkb.createTNode(Times, stmtNum, -2);

	pkb.createLink(Child, top, left);
	pkb.createLink(Child, top, right);

	return top;
}

TNode* ExpressionParser::operatorSubtract(TNode* left) 
{
	TNode* right = parse(operPrecedence["-"]);
	PKB pkb = PKB::getInstance();
	TNode* top = pkb.createTNode(Minus, stmtNum, -2);

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
TNode* ExpressionParser::parse(int bindingLevel) 
{
	PKB pkb = PKB::getInstance();

	string prevToken = token;

	token = *(bufferIter ++);

	TNode* leftNode;
	if (prevToken.compare("(") == 0) 
	{
		leftNode = parse();
		
		if (token != ")") {
			throw logic_error("expressionParser: ) expected but not found");
		}

		prevToken = token;
		token = *(bufferIter ++);
	} else {
		// prevToken is either variable or constant now
		bool isVariable = Parser::matchInteger(prevToken).empty();

		//@todo branch
		leftNode = pkb.createTNode(isVariable ? Variable : Constant, stmtNum, parseConstantOrVariable(prevToken, stmtNum, procIndex, varTable, readOnly));		   
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
 * Method to parse an expression. Returns the root of a tree. This function writes data to the pkb (UsesTable and VarTable).
 * 
 * For example, given an expression x + y + z, the following tree is the output.
 *                           + <- root
 *                         /   \
 *                        +     z
 *                       / \
 *                      x   y
 * @param buffer A vector containing the tokenized right side of expression. e.g. For "a = x + y;" , a vector containing ["x", "+", "y", ";"] should be passed in
 * @sa ExpressionParser::updateBuffer
 * @sa ExpressionParser::updateStmtNum
 * @sa ExpressionParser::parseExpressionForQuerying
 */
TNode* ExpressionParser::parseExpressionForAST(vector<string> buffer) {
	return parseExpression(buffer, false);
} 

/**
 * Method to parse an expression. Returns the root of a tree. This method differs from parseExpressionForAST as it does not 
 * write data to the PKB.
 * For example, given an expression x + y + z, the following tree is the output.
 *                           + <- root
 *                         /   \
 *                        +     z
 *                       / \
 *                      x   y
 * @param buffer A vector containing the tokenized right side of expression. e.g. For "a = x + y;" , a vector containing ["x", "+", "y"] should be passed in
 * @sa ExpressionParser::updateBuffer
 * @sa ExpressionParser::updateStmtNum
 * @sa ExpressionParser::parseExpressionForAST
 */
TNode* ExpressionParser::parseExpressionForQuerying(vector<string> buffer) {
	return parseExpression(buffer, true);
} 

TNode* ExpressionParser::parseExpression(vector<string> buffer, bool readOnly) {
	updateBuffer(buffer);
	this->readOnly = readOnly;
	return parse();
} 
