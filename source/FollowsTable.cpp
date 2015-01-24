#pragma once

#include<stdio.h>
#include <iostream>
#include <string>
#include <vector>
#include <unordered_map>

#include "FollowsTable.h"
#include "AST.h"
#include "PKB.h"


using namespace std;
using namespace stdext;

/**
* Return TRUE if the FollowsTable is updated accordingly. Otherwise, return FALSE. 
* If stmt1 and stmt2 are already present in the FollowsTable and are previously set, the FollowsTable will not be updated.
* @exception if stmt1 or stmt2 is NULL.
*/
bool FollowsTable::setFollows(TNode* stmt1, TNode* stmt2) 
{
	if (stmt1 == NULL || stmt2 == NULL) {
		throw exception("FollowsTable invalid parameters provided");
	}

	AST* ast = PKB::getInstance().ast;
	ast->createLink(Right_Sibling, stmt1, stmt2);

	return true;
}

/**
 * Return a list of possible statement numbers, stmt1, such that stmtNum2 follows stmt1. 
 * If there is no answer, or if stmtNum2 is negative or 0, return an empty list. 
 */
vector<int> FollowsTable::getStmtFollowedTo(int stmtNum2, bool transitiveClosure) 
{
	vector<int> result;

	if (PKB::getInstance().nodeTable.count(stmtNum2) == 0) {
		return vector<int>();
	}
	TNode* node2 = PKB::getInstance().nodeTable.at(stmtNum2);

	if (!node2) {
		return vector<int>();
	}

	while (node2->getLeftSibling() != NULL) {
		int possibleStmt1 = node2->getLeftSibling()->getStmtNumber();
		result.push_back(possibleStmt1);

		if (transitiveClosure) 	{
			node2 = node2->getLeftSibling();
		} else {
			break;
		}
	}
	
	return result;
}

/**
 * Return a list of possible statement numbers, stmt2, such that stmt2 follows stmtNum1. 
 * If there is no answer, or if stmtNum1 is negative or 0, return an empty list.
 */
vector<int> FollowsTable::getStmtFollowedFrom(int stmtNum1, bool transitiveClosure) 
{
	vector<int> result;
	if (PKB::getInstance().nodeTable.count(stmtNum1) == 0) {
		return vector<int>();
	}
	TNode* node1 = PKB::getInstance().nodeTable.at(stmtNum1);

	if (!node1) {
		return vector<int>();
	}
	
	while (node1->hasRightSibling()) {
		int possibleStmt2 = node1->getRightSibling()->getStmtNumber();
		result.push_back(possibleStmt2);

		if (transitiveClosure) {
			node1 = node1->getRightSibling();
		} else {
			break;
		}
	}
	
	return result;
}

/**
 * Return TRUE if the Follows relationship holds between the statement numbers stmtNum1 and stmtNum2. 
 * Otherwise, return FALSE.
 * If stmtNum1 or stmtNum2 is negative or 0, return FALSE.
*/
bool FollowsTable::isFollows(int stmtNum1, int stmtNum2, bool transitiveClosure) 
{
	if (PKB::getInstance().nodeTable.count(stmtNum1) == 0) {
		return false;
	}

	TNode* node1 = PKB::getInstance().nodeTable.at(stmtNum1);

	if (!transitiveClosure) {
		if (!node1 || !node1->hasRightSibling()) {
			return false;
		}

		int possibleStmt2 = node1->getRightSibling()->getStmtNumber();

		return possibleStmt2 == stmtNum2;

	} else {
		 
		TNode* node2 = PKB::getInstance().nodeTable.at(stmtNum2);
		// First, check that they are in the first stmt list
		if (node1->getParent()->getStmtNumber() != node2->getParent()->getStmtNumber()) {
			// If the stmts are not in the same stmtlist, just return false
			return false;
		}

		while (node1->hasRightSibling()) {
			node1 = node1->getRightSibling();

			if (node1->getStmtNumber() == node2->getStmtNumber()) {
				return true;
			}
		}
		return false;
		
	}
}


/**
 * Return all pairs of statements which satisfy the condition Follows(stmt1, stmt2) in each stmtlist.
 */
bool generateAllPairs(vector<TNode*>* inputNodes, bool transitiveClosure, vector<int>* result1, vector<int>* result2) 
{
	vector<TNode*> nextLayer;

	if (transitiveClosure) {
		for (size_t i = 0 ; i < inputNodes->size(); i ++) {
			for (size_t j = i + 1; j < inputNodes->size(); j ++) {
				result1->push_back(inputNodes->at(i)->getStmtNumber());
				result2->push_back(inputNodes->at(j)->getStmtNumber());
			}

			if (inputNodes->at(i)->getNodeType() == While) {
					generateAllPairs(inputNodes->at(i)->getChildren()->at(1)->getChildren(), transitiveClosure, result1, result2);
			}
		}
	} else {
		if (inputNodes->size() == 0) {
			return true;
		}

		for (size_t i = 0; i < inputNodes->size(); i ++) {
			if (i != inputNodes->size() - 1) {
				result1->push_back(inputNodes->at(i)->getStmtNumber());

				int j = i + 1;
				result2->push_back(inputNodes->at(j)->getStmtNumber());
			}

			if (inputNodes->at(i)->getNodeType() == While) {
				generateAllPairs(inputNodes->at(i)->getChildren()->at(1)->getChildren(), transitiveClosure, result1, result2);
			}
		}
	}

	return true;
}

/**
 * Return all (stmt1, stmt2) such that Follows(stmt1, stmt2) holds. If transitiveClosure is true,
 * return all (stmt1, stmt2) such that Follows*(stmt1, stmt2) holds.
 */
pair<vector<int>, vector<int>> FollowsTable::getAllFollowsPairs(bool transitiveClosure) 
{
	pair<vector<int>, vector<int>> results;
	generateAllPairs(PKB::getInstance().ast->getRoot()->getChildren(), transitiveClosure, &(results.first), &(results.second));

	return results;
}

