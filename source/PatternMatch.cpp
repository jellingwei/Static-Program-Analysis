#include <stdio.h>
#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <queue>

#include "PatternMatch.h"

using namespace std;
bool checkPatternMatchAssign(TNode* _rightChildNodeA, TNode* _rootNodeQ, string isExact);
bool BFS(TNode* _rightChildNodeA, TNode* _rootNodeQ, string isExact);
vector<int> SingleVariableConstant(TNode* _rightChildNodeA, TNode* _rootNodeQ, string isExact);

PatternMatch::PatternMatch()
{}

/**
 * Pattern matching for assign statements.
 * @return a vector of statement numbers which are assign stmts, and uses the input of the query subtree root node.
 * @param Root node of the query tree build for the expression and a flag for exact or non-exact matching
 */
vector<int> PatternMatch::PatternMatchAssign(TNode* _rootNodeQ, string isExact)
{
	vector<int> assignTable = PKB::getInstance().getStmtNumForType("assign");
	
	TNode* _currentAssign; TNode* _rightChildNodeA;
	vector<int> results;
	TNODE_TYPE assignType = Assign, plusType = Plus, constType = Constant, varType = Variable;

	for(size_t i=0; i<assignTable.size(); i++) 
	{
		_currentAssign = PKB::getInstance().nodeTable.at(assignTable.at(i));
		_rightChildNodeA = _currentAssign->getChildren()->at(1);
		if(checkPatternMatchAssign(_rightChildNodeA, _rootNodeQ, isExact))
		{
			results.push_back(_rightChildNodeA->getStmtNumber());
		}

		if((_rootNodeQ->getNodeType() == Constant) || (_rootNodeQ->getNodeType() == Variable))
		{
			vector<int> tempResults = SingleVariableConstant(_rightChildNodeA, _rootNodeQ, isExact);
			results.insert( results.end(), tempResults.begin(), tempResults.end() );
		}
	}

	return results;
}

/**
 * Checks Match Pattern in Particular Assignment Statement [Recursive Step]
 * @return true if Assignment contains Pattern
 * @param Root Node of Assignment sub-tree, Root node of the query tree build for the expression and a flag for exact or non-exact matching
 */
bool checkPatternMatchAssign(TNode* _rightChildNodeA, TNode* _rootNodeQ, string isExact)
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
 * @return true if a valid "path" (nodes of same value and type) in both AST and queryAST is found
 * @param Root Node of Assignment sub-tree, Root node of the query tree build for the expression and a flag for exact or non-exact matching
 */
bool BFS(TNode* _rightChildNodeA, TNode* _rootNodeQ, string isExact)
{
	TNode* _currentNodeQ; TNode* _currentNodeA;
	int currentChild;
	
	queue<TNode*> queueQ;
	queue<TNode*> queueA;
	
	bool queueAEmpty = queueA.empty();
	bool queueQEmpty = queueQ.empty();

	queueQ.push(_rootNodeQ);
	queueA.push(_rightChildNodeA);

	TNode* _GrandChildNodeAA = NULL;
	TNode* _GrandChildNodeAB = NULL;
	vector<TNode*> *GrandChildrenList = _rightChildNodeA->getChildren();


	if((GrandChildrenList->at(0)->getNodeType() == Plus) || (GrandChildrenList->at(0)->getNodeType() == Minus) || (GrandChildrenList->at(0)->getNodeType() == Times))
	{
		_GrandChildNodeAA = GrandChildrenList->at(0);
	}
	else if((GrandChildrenList->at(1)->getNodeType() == Plus) || (GrandChildrenList->at(1)->getNodeType() == Minus) || (GrandChildrenList->at(1)->getNodeType() == Times))
	{
		_GrandChildNodeAB = GrandChildrenList->at(1);
	}

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
		currentChild = 0;
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
			}
			else
			{
				if(isExact != "*")
				{
					if(_GrandChildNodeAA != NULL)
					{
						return checkPatternMatchAssign(_GrandChildNodeAA, _rootNodeQ, isExact);
					}
					if(_GrandChildNodeAB != NULL)
					{
						return checkPatternMatchAssign(_GrandChildNodeAB, _rootNodeQ, isExact);
					}
					return false;
				}
				else
				{
					return false;
				}
			}
		}
	}
		if(cqSize>0 && queueAEmpty)
		{
			return false;
		}
		else if(caSize>0 && queueQEmpty)
		{
			if(isExact != "*")
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
 * Pattern matching when query is a single value.
 * @return a vector of statement numbers which are assign stmts, and uses the input of the query subtree root node.
 * @param Root Node of Assignment sub-tree, Root node of the query tree build for the expression and a flag for exact or non-exact matching
 */
vector<int> SingleVariableConstant(TNode *_rightChildNodeA, TNode *_rootNodeQ, string isExact) {
	TNODE_TYPE constType = Constant, varType = Variable;
	vector<int> tempResults, results;

	//Short-hand check by ExpressionParser if particular node in _rootNodeQ already exist in AST while building queryAST
	if(isExact == "*")
	{
		//No difference for Variable or Constant
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
		if(_rootNodeQ->getNodeType() == Variable)
		{
			UsesTable* usesTable = PKB::getInstance().usesTable;
			string NodeValue = PKB::getInstance().varTable->getVarName(_rootNodeQ->getNodeValueIdx());
			tempResults = usesTable->getUsesStmtNum(PKB::getInstance().getVarIndex(NodeValue));
		}
		else
		{
			tempResults = PKB::getInstance().getStmtNum(PKB::getInstance().getConstantIndex(_rootNodeQ->getNodeValueIdx()));
		}
		
		for(size_t i=0; i<tempResults.size(); i++) {
			if(PKB::getInstance().isAssign(tempResults.at(i))) 
			{
				results.push_back(tempResults.at(i));
			}
		}
	}
	return results;
}



/**
 * Pattern matching for while statements.
 * @return a vector of statement numbers which are while loops, and uses the input LHS as its control variable.
 * @param LHS  the name of the variable that acts as the control variable for the while statements we are interested in.
 */
vector<int> PatternMatch::PatternMatchWhile(string LHS) {
	// strip leading and trailing space
	LHS.erase(0, LHS.find_first_not_of(" "));
	LHS.erase(LHS.find_last_not_of(" ") + 1);
	
	if (LHS.empty()) {
		vector<int> emptyVector;
		return emptyVector;
	}

	int varIndex = PKB::getInstance().getVarIndex(LHS);
	if (varIndex <= 0) {
		vector<int> emptyVector;
		return emptyVector;
	}
	
	// Get statements using the variable
	vector<int> candidateList = PKB::getInstance().getUsesStmtNum(varIndex);

	// Filter non-while statements from these statements
	vector<int> result;
	for (auto stmt = candidateList.begin(); stmt != candidateList.end(); ++stmt) {
		bool isWhile = PKB::getInstance().isWhile(*stmt); 
		if (isWhile) {
			TNode* stmtNode = PKB::getInstance().nodeTable.at(*stmt);

			assert(stmtNode->getChildren()->size() >= 2); // < 2 children is an invalid state for a while node
			if (stmtNode->getChildren()->size() < 1) {
				continue;
			}
			int controlVariable = stmtNode->getChildren()->at(0)->getNodeValueIdx();

			if (controlVariable == varIndex) {
				result.push_back(*stmt);
			}
		}
	}

	return result;
}

/**
 * Pattern matching for if statements.
 * @return a vector of statement numbers which are if statements, and uses the input LHS as its control variable.
 * @param LHS the name of the variable that acts as the control variable for the if statements we are interested in.
 */
//vector<int> PatternMatch::PatternMatchIf(string LHS) {
//}