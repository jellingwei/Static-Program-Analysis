#include <stdio.h>
#include <iostream>
#include <string>
#include <vector>
#include <algorithm>

#include "PatternMatch.h"

using namespace std;
bool checkPatternMatchAssign(TNode *_rightChildNodeA, TNode *_rootNodeQ, string isExact);
bool BFS(TNode *_rightChildNodeA, TNode *_rootNodeQ, string isExact);
//bool checkPatternMatchAssign(TNode *_rightChildNodeA, TNode *_rootNodeQ, string isExact);
vector<int> SingleVariableConstant(TNode *_rightChildNodeA, TNode *_rootNodeQ, string isExact);

PatternMatch::PatternMatch()
{}

/**
 * Pattern matching for assign statements.
 * @return a vector of statement numbers which are assign stmts, and uses the input of the query subtree root node.
 * @param Root node of the query tree build for the expression and a flag for exact or non-exact matching
 */
vector<int> PatternMatch::PatternMatchAssign(TNode *_rootNodeQ, string isExact)
{
	vector<int> assignTable = PKB::getInstance().getStmtNumForType("assign");
	
	TNode *_currentAssign, *_rightChildNodeA;
	vector<int> results;
	TNODE_TYPE assignType = Assign, plusType = Plus, constType = Constant, varType = Variable;

	for(size_t i=0; i<assignTable.size(); i++) 
	{
		_currentAssign = PKB::getInstance().nodeTable.at(assignTable.at(i));
		_rightChildNodeA = _currentAssign->getChildren()->at(1);
		if(checkPatternMatchAssign(_rightChildNodeA, _rootNodeQ, isExact))
		{
cout << "stmtnumber true = " << _rightChildNodeA->getStmtNumber();
			results.push_back(_rightChildNodeA->getStmtNumber());
		}

		//_rootNodeQ is an Operand 
		if((_rootNodeQ->getNodeType() == Constant) || (_rootNodeQ->getNodeType() == Variable))
		{
			vector<int> tempResults = SingleVariableConstant(_rightChildNodeA, _rootNodeQ, isExact);
			results.insert( results.end(), tempResults.begin(), tempResults.end() );
		}
	}

	return results;
}


bool checkPatternMatchAssign(TNode *_rightChildNodeA, TNode *_rootNodeQ, string isExact)
{
	TNODE_TYPE plusType = Plus, minusType = Minus, timesType = Times;
	
	if(_rootNodeQ->getNodeType() == _rightChildNodeA->getNodeType())
	{
		if((_rootNodeQ->getNodeType() == Plus) || (_rootNodeQ->getNodeType() == Minus) || (_rootNodeQ->getNodeType() == Times))
		{
cout << "enter bfs with AST stmtno " << _rightChildNodeA->getStmtNumber() << " and queryAST";
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

bool BFS(TNode *_rightChildNodeA, TNode *_rootNodeQ, string isExact)
{
	TNode *_currentNodeQ, *_currentNodeA;
	int currentChild;
	
	queue <TNode*> queueQ;
	queue <TNode*> queueA;

	queueQ.push(_rootNodeQ);
	queueA.push(_rightChildNodeA);

	TNode *_GrandChildNodeAA = NULL;
	TNode *_GrandChildNodeAB = NULL;
	vector<TNode*> *GrandChildrenList = _rightChildNodeA->getChildren();


	if((GrandChildrenList->at(0)->getNodeType() == Plus) || (GrandChildrenList->at(0)->getNodeType() == Minus) || (GrandChildrenList->at(0)->getNodeType() == Times))
	{
		_GrandChildNodeAA = GrandChildrenList->at(0);
	}
	else if((GrandChildrenList->at(1)->getNodeType() == Plus) || (GrandChildrenList->at(1)->getNodeType() == Minus) || (GrandChildrenList->at(1)->getNodeType() == Times))
	{
		_GrandChildNodeAB = GrandChildrenList->at(1);
	}

	while(!queueQ.empty() || !queueA.empty())
	{
		_currentNodeQ = queueQ.front();
		queueQ.pop();
		_currentNodeA = queueQ.front();
		queueA.pop();

		currentChild = 0;

		while(((_currentNodeQ->getChildren())->size()!=0) && (_currentNodeA->getChildren())->size()!=0)
		{
			if((_currentNodeQ->getNodeType() == _currentNodeA->getNodeType()) && (_currentNodeQ->getNodeValueIdx() == _currentNodeA->getNodeValueIdx()))
			{
				queueQ.push(_currentNodeQ->getChildren()->at(currentChild));
				queueA.push(_currentNodeQ->getChildren()->at(currentChild));
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
				}
				else
				{
					return false;
				}
			}
		}

		if(_currentNodeQ->getChildren()->size() > 0)
		{
			return false;
		}
		else if(_currentNodeA->getChildren()->size() > 0)
		{
			if(isExact != "*")
			{
				if((_currentNodeQ->getNodeType() == _currentNodeA->getNodeType()) && (_currentNodeQ->getNodeValueIdx() == _currentNodeA->getNodeValueIdx()))
				{
					return true;
				}
			}
		}
		else if(_currentNodeQ->getChildren()->size()==0 && _currentNodeA->getChildren()->size()==0)
		{
			if((_currentNodeQ->getNodeType() == _currentNodeA->getNodeType()) && (_currentNodeQ->getNodeValueIdx() == _currentNodeA->getNodeValueIdx()))
			{
				return true;
			}
		}
		else {
			return false;
		}
	}
}

vector<int> SingleVariableConstant(TNode *_rightChildNodeA, TNode *_rootNodeQ, string isExact) {
	TNODE_TYPE constType = Constant, varType = Variable;
	vector<int> tempResults, results;

	//Short-hand check by ExpressionParser if particular node in _rootNodeQ already exist in AST while building queryAST
	if(isExact == "*")
	{
		//No difference for Variable or Constant
		if(_rightChildNodeA->getNodeValueIdx() == _rootNodeQ->getNodeValueIdx())
		{
cout << "stmtnumber true for single Exact = " << _rightChildNodeA->getStmtNumber();
			results.push_back(_rightChildNodeA->getStmtNumber());
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
cout << "stmtnumber true for single Non-Exact = " << _rightChildNodeA->getStmtNumber();
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
/*vector<int> PatternMatch::PatternMatchWhile(string LHS)
{
}*/