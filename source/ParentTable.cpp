#pragma once

#include<stdio.h>
#include <iostream>
#include <string>
#include <stack>
#include <vector>
#include <unordered_map>
#include <assert.h>

#include "PKB.h"
#include "AST.h"
#include "ParentTable.h"

using namespace std;
using namespace stdext;



bool ParentTable::setParent(TNode* stmt1, TNode* stmt2) 
{
	if (stmt1 == NULL || stmt2 == NULL) 
	{
		throw exception("ParentTable invalid parameters provided");
	}

	AST* ast = PKB::getInstance().ast;
	ast->createLink(Child, stmt1, stmt2);
	
	return true;
}


vector<int> ParentTable::getParent(int stmtNum2, bool transitiveClosure) 
{
	vector<int> result;

	if (PKB::getInstance().nodeTable.count(stmtNum2) == 0) 
	{
		// no such statement
		return vector<int>();
	}
	TNode* node2 = PKB::getInstance().nodeTable.at(stmtNum2);

	if (!node2) 
	{ 
		// defensive coding, in case nodeTable is implemented poorly
		return vector<int>();
	}

	if (node2->getParent()->getParent() == NULL) 
	{  //@Todo prevents retrieving root of ast tree, but should rewrite when AST can support multiple procedure
		return vector<int>();
	}

	if (!transitiveClosure) 
	{
		int possibleStmt1 = node2->getParent()->getStmtNumber();

		if (possibleStmt1 > 0) 
		{
			result.push_back(possibleStmt1);
		}
	} 
	else 
	{
		while (node2->getStmtNumber() != 0 && node2->getParent()->getParent() != NULL) //stmt# 0 used as dummy value, might need to reconsider this
		{ 
			int possibleStmt1 = node2->getParent()->getStmtNumber();

			if (possibleStmt1 > 0) 
			{
				result.push_back(possibleStmt1);
			}
			
			node2 = node2->getParent()->getParent();
		}
	}
	
	return result;
}

/**
 * Returns the transitive Closure of getChild. 
 */
vector<int> getChildDfs(TNode* node1, TNode* node2) 
{
	vector<int> result;
	using std::stack;

	// initialise stack
	stack<TNode*> nodeStack;
	nodeStack.push(node1);
	if (node2 != NULL) 
	{
		nodeStack.push(node2);
	}

	while (!nodeStack.empty()) 
	{
		node1 = nodeStack.top(); nodeStack.pop();

		if (!node1->hasChild()) 
		{
			continue;
		}

		// iterate through children of the node, and add stmtList nodes to stack if is If or is While
		vector<TNode*>* children = node1->getChildren();
		for (size_t i = 0 ; i < children->size(); i++ ) 
		{
			TNode* child = children->at(i);
			int possibleStmt2 = child->getStmtNumber();

			if (child->getNodeType() == If) 
			{
				TNode* ifStmtLst1 = child->getChildren()->at(1);
				assert(ifStmtLst1->getNodeType() == StmtLst);
				nodeStack.push(ifStmtLst1);

				if (child->getChildren()->size() > 2) 
				{
					TNode* ifStmtLst2 = child->getChildren()->at(2);
					assert(ifStmtLst2->getNodeType() == StmtLst);
					nodeStack.push(ifStmtLst2);
				}

			} 
			else 
			if (child->getNodeType() == While) 
			{
				TNode* whileStmtLst = child->getChildren()->at(1);
				assert(whileStmtLst->getNodeType() == StmtLst);
				nodeStack.push(whileStmtLst);
			}

			result.push_back(possibleStmt2);
		}
	}
		
	return result;
}


vector<int> ParentTable::getChild(int stmtNum1, bool transitiveClosure) 
{
	vector<int> result;

	if (PKB::getInstance().nodeTable.count(stmtNum1) == 0) 
	{
		return vector<int>();
	}


	PKB pkb = PKB::getInstance();
	TNODE_TYPE stmtType = pkb.nodeTable.at(stmtNum1)->getNodeType();
	TNode* node1 = NULL;
	TNode* node2 = NULL;

	if (stmtType == If) 
	{
		node1 = pkb.nodeTable.at(stmtNum1)->getChildren()->at(1); 
		assert(node1->getNodeType() == StmtLst);

		if (pkb.nodeTable.at(stmtNum1)->getChildren()->size() > 2) 
		{
			node2 = pkb.nodeTable.at(stmtNum1)->getChildren()->at(2); 
			assert(node2->getNodeType() == StmtLst);
		}
		
	} else if (stmtType == While) 
	{
		node1 = pkb.nodeTable.at(stmtNum1)->getChildren()->at(1); //@Todo refactor

		assert(node1->getNodeType() == StmtLst);
	} else 
	{
		return vector<int>();
		//throw logic_error("ParentTable: invalid stmt type for finding children");
	}


	if (!transitiveClosure) 
	{
		if (node1->hasChild()) 
		{
			vector<TNode*>* children = node1->getChildren();
			for (size_t i = 0 ; i < children->size(); i++ ) 
			{
				int possibleStmt2 = children->at(i)->getStmtNumber();
				result.push_back(possibleStmt2);
			}
		}

		if (node2 != NULL && node2->hasChild()) 
		{
			vector<TNode*>* children = node2->getChildren();
			for (size_t i = 0 ; i < children->size(); i++ ) 
			{
				int possibleStmt2 = children->at(i)->getStmtNumber();
				result.push_back(possibleStmt2);
			}
		}
		return result;

	} else 
	{
		return getChildDfs(node1, node2);
	}
	
	
}


bool ParentTable::isParent(int stmtNum1, int stmtNum2, bool transitiveClosure) 
{
	if (PKB::getInstance().nodeTable.count(stmtNum2) == 0) 
	{
		return false;
	}
	if (PKB::getInstance().nodeTable.count(stmtNum1) == 0) 
	{
		return false;
	}

	TNode* node2 = PKB::getInstance().nodeTable.at(stmtNum2);

	if (!transitiveClosure) 
	{
		if (node2 && node2->getParent()) 
		{
			int possibleStmt1 = node2->getParent()->getStmtNumber();

			if (possibleStmt1 == stmtNum1 && stmtNum1 != 0) 
			{
				return true;
			}
		}
		return false;
	}
	else 
	{
		TNode* node1 = PKB::getInstance().nodeTable.at(stmtNum1);
		if (!node1 || !node2) 
		{
			return false;
		}

		// @Todo change after ast changes, right now root of ast is a node that has gone out of scope
		//		 hence the need to check for stmt 0
		while (node2 != NULL && node2->getStmtNumber() != 0) 
		{ 
			if (node2->getStmtNumber() == node1->getStmtNumber()) 
			{
				return true;
			}

			node2 = node2->getParent();
		}
		return false;
	}

}


void generateTransitiveParentPairs(vector<int> parentList, TNode* curNode, vector<int>* result1, vector<int>* result2) 
{
	vector<TNode*>* curNodeChildren = curNode->getChildren();

	for (size_t i = 0; i < curNodeChildren->size(); ++i)
	{
		TNode* childNode = curNodeChildren->at(i);
		
		// for every node x in the parentList, add (x, childNode) to results
		for (vector<int>::iterator iter = parentList.begin(); iter != parentList.end(); ++iter) 
		{
			result1->push_back(*iter);
			result2->push_back(childNode->getStmtNumber());
		}

		// @Todo don't use recursion next time
		if (childNode->getNodeType() == While && childNode->getChildren()->at(1)->hasChild()) 
		{
			parentList.push_back(childNode->getStmtNumber());
			generateTransitiveParentPairs(parentList, childNode->getChildren()->at(1), result1, result2);
			parentList.erase(parentList.end() - 1);
		} 
		else if (childNode->getNodeType() == If) 
		{
			parentList.push_back(childNode->getStmtNumber());
			generateTransitiveParentPairs(parentList, childNode->getChildren()->at(1), result1, result2);
			parentList.erase(parentList.end() - 1);
			
			if (childNode->getChildren()->size() > 2 && childNode->getChildren()->at(2)->hasChild()) 
			{
				parentList.push_back(childNode->getStmtNumber());
				generateTransitiveParentPairs(parentList, childNode->getChildren()->at(2), result1, result2);
				parentList.erase(parentList.end() - 1);
			}
		}
	}

		
}


/**
 * Returns all pairs of statements, <s1, s2>, where Parent(s1, s2) is satisfied
 */
pair<vector<int>, vector<int>> ParentTable::getAllParentPairs(bool transitiveClosure) 
{
	pair<vector<int>, vector<int> > result;

	if (!transitiveClosure) 
	{
		//@Todo make smarter
		for (auto iter = PKB::getInstance().nodeTable.begin(); iter != PKB::getInstance().nodeTable.end(); ++iter) 
		{
			TNode* node = iter->second;
			if (!node->hasChild() || node->getStmtNumber() <= 0) 
			{
				continue;
			}
			if (node->getNodeType() != While && node->getNodeType() != If) 
			{
				continue;
			}

			// get children of the current node
			vector<TNode*>* childrenList = node->getChildren()->at(1)->getChildren();
			vector<TNode*>* elseChildrenList = NULL;
			if (node->getNodeType() == If && node->getChildren()->size() > 2)
			{
				elseChildrenList = node->getChildren()->at(2)->getChildren();
			}

			// add pairs of (node, childNode) into results
			for (size_t i = 0; i < childrenList->size(); ++i) 
			{
				TNode* childNode = childrenList->at(i);
				if (childNode->getNodeType() != Assign && childNode->getNodeType() != While && childNode->getNodeType() != If) 
				{
					continue;
				}

				result.first.push_back(node->getStmtNumber());
				result.second.push_back(childNode->getStmtNumber());
			}
			for (size_t i = 0; elseChildrenList != NULL && i < elseChildrenList->size(); ++i) 
			{
				TNode* childNode = elseChildrenList->at(i);
				if (childNode->getNodeType() != Assign && childNode->getNodeType() != While && childNode->getNodeType() != If) 
				{
					continue;
				}

				result.first.push_back(node->getStmtNumber());
				result.second.push_back(childNode->getStmtNumber());
			}
		}
	} else 
	{
		vector<TNode*>* procNodes = PKB::getInstance().ast->getRoot()->getChildren();

		for (auto iter = procNodes->begin(); iter != procNodes->end(); ++iter) 
		{
			TNode* stmtListNode = (*iter)->getChildren()->at(0);
			vector<int> emptyList;
			generateTransitiveParentPairs(emptyList, stmtListNode, &(result.first), &(result.second));//@Todo don't use recursion
		}
	
	}

	return result;
}