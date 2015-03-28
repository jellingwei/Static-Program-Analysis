#include <stdio.h>
#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <queue>

#include "PatternMatch.h"

using namespace std;
bool checkPatternMatchAssign(TNode* _rightChildNodeA, TNode* _rootNodeQ, bool isExact);
bool BFS(TNode* _rightChildNodeA, TNode* _rootNodeQ, bool isExact);
vector<int> SingleVariableConstant(TNode* _rightChildNodeA, TNode* _rootNodeQ, bool isExact);
bool recurseChecking(vector<TNode*> *GrandChildrenList, TNode* _rootNodeQ, bool isExact);

PatternMatch::PatternMatch()
{}

/**
 * Pattern matching for assign statements.
 * @param _rootNodeQ  the Root Node of the query tree build for the expression and a flag for exact or non-exact matching
 * @param isExact  a flag for exact or non-exact matching
 * @return a vector of statement numbers which are assign stmts, and uses the input of the query subtree root node.
 */
STATEMENT_LIST PatternMatch::patternMatchAssign(TNode* _rootNodeQ, bool isExact, string usedOperand)
{
	PKB pkb = PKB::getInstance();
	vector<int> assignTable;
	if(_rootNodeQ->getDescendent() < 2) {
		if(_rootNodeQ->getNodeType() == Variable) {
			assignTable = pkb.getUsesStmtNum(_rootNodeQ->getNodeValueIdx());
		} else if(_rootNodeQ->getNodeType() == Constant) {
			assignTable = pkb.getStmtNum(_rootNodeQ->getNodeValueIdx());
		}
	} else if(_rootNodeQ->getDescendent() >= 2) {
		if (isdigit(usedOperand.at(0))) {
			assignTable = pkb.getStmtNum(stoi(usedOperand));
		} else {
			int toCheck = pkb.getVarIndex(usedOperand);
			assignTable = pkb.getUsesStmtNum(toCheck);
		}
	}

	TNode* _currentAssign; TNode* _rightChildNodeA;
	vector<int> results;
	TNODE_TYPE assignType = Assign, plusType = Plus, constType = Constant, varType = Variable;

	for(size_t i=0; i<assignTable.size(); i++) 
	{
		_currentAssign = pkb.getNodeForStmt(assignTable.at(i));
		if(_currentAssign->getNodeType() == Assign) {
			_rightChildNodeA = _currentAssign->getChildren()->at(1);
			if((_rootNodeQ->getNodeType() == Constant) || (_rootNodeQ->getNodeType() == Variable))
			{
				vector<int> tempResults = SingleVariableConstant(_rightChildNodeA, _rootNodeQ, isExact);
				results.insert( results.end(), tempResults.begin(), tempResults.end() );
			
				if(!isExact) break;
			}
			else {
				bool skipThis = true;
				if(isExact) {
					if(_rightChildNodeA->getDescendent() != _rootNodeQ->getDescendent()) skipThis = false;
				} else {
					if(_rightChildNodeA->getDescendent() < _rootNodeQ->getDescendent()) skipThis = false;
				}
			
				if(skipThis) {
					if(checkPatternMatchAssign(_rightChildNodeA, _rootNodeQ, isExact))
					{
						results.push_back(_rightChildNodeA->getStmtNumber());
					}
					else 
					{
						if(!isExact && _rightChildNodeA->getChildren()->size() > 0) {
							if(recurseChecking(_rightChildNodeA->getChildren(), _rootNodeQ, isExact))
								results.push_back(_rightChildNodeA->getStmtNumber());
						}
					}
				} 
			}
		} else {
			continue;
		}
	}
	return results;
}

/**
 * Checks Match Pattern in Particular Assignment Statement [Recursive Step]
 * @param _rightChildNodeA  the Root node of the AST assignment sub-tree being examined, 
 * @param _rootNodeQ  the Root node of the query tree build for the expression
 * @param isExact  a flag for exact or non-exact matching
 * @return true if Assignment contains Pattern
 */
bool checkPatternMatchAssign(TNode* _rightChildNodeA, TNode* _rootNodeQ, bool isExact)
{
	TNODE_TYPE plusType = Plus, minusType = Minus, timesType = Times;
	
	if(_rootNodeQ->getNodeType() == _rightChildNodeA->getNodeType())
	{
		if((_rootNodeQ->getNodeType() == Plus) || (_rootNodeQ->getNodeType() == Minus) || (_rootNodeQ->getNodeType() == Times))
		{
			return BFS(_rightChildNodeA, _rootNodeQ, isExact);
		}
		else
		{
			return false;
		}
	}
	else
	{
		return false;
	}
}

/**
 * Breadth-First Search variation for searching and comparing nodes.
 * @param _rightChildNodeA  the Root node of the AST assignment sub-tree being examined, 
 * @param _rootNodeQ  the Root node of the query tree build for the expression
 * @param isExact  a flag for exact or non-exact matching
 * @return true if a valid "path" (nodes of same value and type) in both AST and queryAST is found
 */
bool BFS(TNode* _rightChildNodeA, TNode* _rootNodeQ, bool isExact)
{
	TNode* _currentNodeQ; TNode* _currentNodeA;
	int currentChild = 0;
	
	queue<TNode*> queueQ;
	queue<TNode*> queueA;
	
	bool queueAEmpty = queueA.empty();
	bool queueQEmpty = queueQ.empty();

	queueQ.push(_rootNodeQ);
	queueA.push(_rightChildNodeA);

	int cqSize = 0, caSize = 0;

	while(!queueQ.empty() || !queueA.empty())
	{
		if(!queueQ.empty())
		{
			_currentNodeQ = queueQ.front();
			queueQ.pop();
		}

		if(!queueA.empty())
		{
			_currentNodeA = queueA.front();
			queueA.pop();
		} 
		
		cqSize = (_currentNodeQ->getChildren())->size();
		caSize = (_currentNodeA->getChildren())->size();

		while(cqSize!=0 || caSize!=0)
		{
			if((_currentNodeQ->getNodeType() == _currentNodeA->getNodeType()) && (_currentNodeQ->getNodeValueIdx() == _currentNodeA->getNodeValueIdx()))
			{
				queueQ.push(_currentNodeQ->getChildren()->at(currentChild));
				queueA.push(_currentNodeA->getChildren()->at(currentChild));
				cqSize--;
				caSize--;
				currentChild++;
			}
			else
			{
				if(!isExact) {
					return recurseChecking(_rightChildNodeA->getChildren(), _rootNodeQ, isExact);
				}
				return false; 
			}
		}
		currentChild = 0;

		//**Important**//
		if(cqSize==0 || caSize==0) {
			if(!((_currentNodeQ->getNodeType() == _currentNodeA->getNodeType()) && (_currentNodeQ->getNodeValueIdx() == _currentNodeA->getNodeValueIdx())))
			{
				return false;;
			}
		}
	}

	if(cqSize>0 && queueAEmpty)
	{
		return false;
	}
	else if(caSize>0 && queueQEmpty)
	{
		if(!isExact)
		{
			if((_currentNodeQ->getNodeType() == _currentNodeA->getNodeType()) && (_currentNodeQ->getNodeValueIdx() == _currentNodeA->getNodeValueIdx()))
			{
				return true;
			}
		}
		return false;
	}
	else if(cqSize==0 && caSize==0)
	{
		if((_currentNodeQ->getNodeType() == _currentNodeA->getNodeType()) && (_currentNodeQ->getNodeValueIdx() == _currentNodeA->getNodeValueIdx()))
		{
			return true;
		}
		return false;
	}
	else {
		return false;
	}

	return false;
}

/**
 * Auxilliary method for Non-Exact Pattern Match, recursively checks subtrees to find a matching pattern.
 * @param GrandChildrenList  a list of children nodes of current Root node
 * @param _rootNodeQ  the Root node of the query tree build for the expression
 * @param isExact  a flag for exact or non-exact matching
 * @return true if Assignment contains Pattern
 */
bool recurseChecking(vector<TNode*> *GrandChildrenList, TNode* _rootNodeQ, bool isExact) {

		TNode* _GrandChildNodeAA = NULL;
		TNode* _GrandChildNodeAB = NULL;
		bool found = false;
		if(GrandChildrenList->size() > 0 ) {
			if((GrandChildrenList->at(0)->getNodeType() == Plus) || (GrandChildrenList->at(0)->getNodeType() == Minus) || (GrandChildrenList->at(0)->getNodeType() == Times)) {
				_GrandChildNodeAA = GrandChildrenList->at(0);
				
				if(_GrandChildNodeAA->getDescendent() >= _rootNodeQ->getDescendent()) {
					found = checkPatternMatchAssign(_GrandChildNodeAA, _rootNodeQ, isExact);
				}
			}
			
			if((found==false) && ((GrandChildrenList->at(1)->getNodeType() == Plus) || (GrandChildrenList->at(1)->getNodeType() == Minus) || (GrandChildrenList->at(1)->getNodeType() == Times))) {
				_GrandChildNodeAB = GrandChildrenList->at(1);

				if(_GrandChildNodeAB->getDescendent() >= _rootNodeQ->getDescendent()) {
					found = checkPatternMatchAssign(_GrandChildNodeAB, _rootNodeQ, isExact);
				} else {
					return false;
				}
			}

			if(found)
				return true;
			else {
				//** Important **//
				if(_GrandChildNodeAA != NULL) {
					found = recurseChecking(_GrandChildNodeAA->getChildren(), _rootNodeQ, isExact);
				}
				if(found==false && _GrandChildNodeAB != NULL) {
					found = recurseChecking(_GrandChildNodeAB->getChildren(), _rootNodeQ, isExact);
				}
				return found;
			}
		}

	return false;
}

/**
 * Pattern matching when query is a single value.
 * @param _rightChildNodeA  the Root node of the AST assignment sub-tree being examined, 
 * @param _rootNodeQ  the Root node of the query tree build for the expression
 * @param isExact  a flag for exact or non-exact matching
 * @return a vector of statement numbers which are assign stmts, and uses the input of the query subtree root node.
 */
vector<int> SingleVariableConstant(TNode *_rightChildNodeA, TNode *_rootNodeQ, bool isExact) {
	TNODE_TYPE constType = Constant, varType = Variable;
	vector<int> tempResults, results;
	PKB pkb = PKB::getInstance();

	//Short-hand check by ExpressionParser if particular node in _rootNodeQ already exist in AST while building queryAST
	if(isExact)
	{
		if(_rightChildNodeA->getNodeType()==Constant && _rootNodeQ->getNodeType()==Constant)
		{
			if(_rightChildNodeA->getNodeValueIdx() == _rootNodeQ->getNodeValueIdx())
			{
				results.push_back(_rightChildNodeA->getStmtNumber());
			}
		}
		else if(_rightChildNodeA->getNodeType()==Variable && _rootNodeQ->getNodeType()==Variable)
		{
			if(_rightChildNodeA->getNodeValueIdx() == _rootNodeQ->getNodeValueIdx())
			{
				results.push_back(_rightChildNodeA->getStmtNumber());
			}
		}
	}
	else
	{
		try{
			if(_rootNodeQ->getNodeType() == Variable)
			{
				
				string NodeValue = pkb.getVarName(_rootNodeQ->getNodeValueIdx());
				tempResults = pkb.getUsesStmtNum(PKB::getInstance().getVarIndex(NodeValue));
			}
			else
			{
				tempResults = pkb.getStmtNum(_rootNodeQ->getNodeValueIdx());
			}
		} catch (const invalid_argument& e) {
			return results;
		} catch (const exception& e) {
			return results;
		}
		
		for(size_t i=0; i<tempResults.size(); i++) {
			if(pkb.isAssign(tempResults.at(i))) 
			{
				results.push_back(tempResults.at(i));
			}
		}
	}
	return results;
}

/**
 * Pattern matching for while or if statements.
 * @param LHS  the name of the variable that acts as the control variable for the while statements we are interested in.
 * @param type the type of statement to match, either While or If
 * @return a vector of statement numbers which are while or if loops, depending on type, and uses the input LHS as its control variable.
 */
STATEMENT_LIST PatternMatch::patternMatchParentStmt(EXPRESSION LHS, TNODE_TYPE type) {
	// strip leading and trailing space
	LHS.erase(0, LHS.find_first_not_of(" "));
	LHS.erase(LHS.find_last_not_of(" ") + 1);
	
	PKB pkb = PKB::getInstance();

	if (LHS.empty()) {
		vector<int> emptyVector;
		return emptyVector;
	}

	int varIndex = pkb.getVarIndex(LHS);
	if (varIndex <= 0) {
		vector<int> emptyVector;
		return emptyVector;
	}
	
	// Get statements using the variable
	vector<int> candidateList = pkb.getUsesStmtNum(varIndex);

	// Filter non-matching types from these statements
	vector<int> result;
	for (auto stmt = candidateList.begin(); stmt != candidateList.end(); ++stmt) {
		bool isCorrectType = type == While ? 
							 pkb.isWhile(*stmt) : 
							 pkb.isIf(*stmt); ;
		if (!isCorrectType) {
			continue;
		}

		TNode* stmtNode = pkb.getNodeForStmt(*stmt);

		assert(stmtNode->getChildren()->size() >= 2); // < 2 children is an invalid state for a while node
		if (stmtNode->getChildren()->size() < 1) {
			continue;
		}

		int controlVariable = stmtNode->getChildren()->at(0)->getNodeValueIdx();
		if (controlVariable == varIndex) {
			result.push_back(*stmt);
		}
	}

	return result;
}

PatternMatch::~PatternMatch() 
{}