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
bool recurseChecking(vector<TNode*> *GrandChildrenList, TNode* _rootNodeQ, string isExact);

PatternMatch::PatternMatch()
{}

/**
 * Pattern matching for assign statements.
 * @return a vector of statement numbers which are assign stmts, and uses the input of the query subtree root node.
 * @param Root node of the query tree build for the expression and a flag for exact or non-exact matching
 */
vector<int> PatternMatch::PatternMatchAssign(TNode* _rootNodeQ, string isExact)
{
	PKB pkb = PKB::getInstance();
	vector<int> assignTable = pkb.getStmtNumForType("assign");
	
	TNode* _currentAssign; TNode* _rightChildNodeA;
	vector<int> results;
	TNODE_TYPE assignType = Assign, plusType = Plus, constType = Constant, varType = Variable;

	for(size_t i=0; i<assignTable.size(); i++) 
	{
		_currentAssign = pkb.getNodeForStmt(assignTable.at(i));
		_rightChildNodeA = _currentAssign->getChildren()->at(1);

		if((_rootNodeQ->getNodeType() == Constant) || (_rootNodeQ->getNodeType() == Variable))
		{
			vector<int> tempResults = SingleVariableConstant(_rightChildNodeA, _rootNodeQ, isExact);
			results.insert( results.end(), tempResults.begin(), tempResults.end() );
			
			if(isExact.compare("*") != 0) break;
		}
		else {
			if(checkPatternMatchAssign(_rightChildNodeA, _rootNodeQ, isExact))
			{
				results.push_back(_rightChildNodeA->getStmtNumber());
			}
			else 
			{
				if(isExact.compare("*") != 0 && _rightChildNodeA->getChildren()->size() > 0) {
					if(recurseChecking(_rightChildNodeA->getChildren(), _rootNodeQ, isExact))
						results.push_back(_rightChildNodeA->getStmtNumber());
				}
			}
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
				if(isExact.compare("*") != 0) {
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
		if(isExact.compare("*") != 0)
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
 * @return true if Assignment contains Pattern
 * @param Children nodes of current Root node in a Vector<>, Root node of the query tree build for the expression and a flag for exact or non-exact matching
 */
bool recurseChecking(vector<TNode*> *GrandChildrenList, TNode* _rootNodeQ, string isExact) {

		TNode* _GrandChildNodeAA = NULL;
		TNode* _GrandChildNodeAB = NULL;
		bool found = false;
		if(GrandChildrenList->size() > 0 ) {
			if((GrandChildrenList->at(0)->getNodeType() == Plus) || (GrandChildrenList->at(0)->getNodeType() == Minus) || (GrandChildrenList->at(0)->getNodeType() == Times)) {
				_GrandChildNodeAA = GrandChildrenList->at(0);
				found = checkPatternMatchAssign(_GrandChildNodeAA, _rootNodeQ, isExact);
			}
			
			if((found==false) && ((GrandChildrenList->at(1)->getNodeType() == Plus) || (GrandChildrenList->at(1)->getNodeType() == Minus) || (GrandChildrenList->at(1)->getNodeType() == Times))) {
				_GrandChildNodeAB = GrandChildrenList->at(1);
				found = checkPatternMatchAssign(_GrandChildNodeAB, _rootNodeQ, isExact);
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
 * @return a vector of statement numbers which are assign stmts, and uses the input of the query subtree root node.
 * @param Root Node of Assignment sub-tree, Root node of the query tree build for the expression and a flag for exact or non-exact matching
 */
vector<int> SingleVariableConstant(TNode *_rightChildNodeA, TNode *_rootNodeQ, string isExact) {
	TNODE_TYPE constType = Constant, varType = Variable;
	vector<int> tempResults, results;
	PKB pkb = PKB::getInstance();

	//Short-hand check by ExpressionParser if particular node in _rootNodeQ already exist in AST while building queryAST
	if(isExact.compare("*") == 0)
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
				UsesTable* usesTable = pkb.usesTable;
				string NodeValue = pkb.varTable->getVarName(_rootNodeQ->getNodeValueIdx());
				tempResults = usesTable->getUsesStmtNum(PKB::getInstance().getVarIndex(NodeValue));
			}
			else
			{
				tempResults = pkb.getStmtNum(pkb.getConstantIndex(_rootNodeQ->getNodeValueIdx()));
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
 * @return a vector of statement numbers which are while or if loops, depending on type, and uses the input LHS as its control variable.
 * @param LHS  the name of the variable that acts as the control variable for the while statements we are interested in.
 * @param type the type of statement to match, either While or If
 */
vector<int> PatternMatch::patternMatchParentStmt(string LHS, TNODE_TYPE type) {
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