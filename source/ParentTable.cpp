#pragma once

#include<stdio.h>
#include <iostream>
#include <string>
#include <vector>
#include <unordered_map>
#include <assert.h>

#include "PKB.h"
#include "AST.h"
#include "ParentTable.h"

using namespace std;
using namespace stdext;


bool ParentTable::setParent(TNode* stmt1, TNode* stmt2) {
	if (stmt1 == NULL || stmt2 == NULL) {
		throw exception("ParentTable invalid parameters provided");
	}

	AST* ast = PKB::getInstance().ast;
	ast->createLink(Child, stmt1, stmt2);

	pair<int, TNode*> stmtNumToNodePair1(stmt1->getStmtNumber(), stmt1);
	PKB::getInstance().nodeTable.insert(stmtNumToNodePair1);

	pair<int, TNode*> stmtNumToNodePair2(stmt2->getStmtNumber(), stmt2);
	PKB::getInstance().nodeTable.insert(stmtNumToNodePair2);
	
	return true;
}


vector<int> ParentTable::getParent(int stmtNum2) {
	vector<int> result;

	if (PKB::getInstance().nodeTable.count(stmtNum2) == 0) {
		return vector<int>();
	}
	TNode* node2 = PKB::getInstance().nodeTable.at(stmtNum2);

	if (!node2) {
		//throw exception("ParentTable exception: invalid stmtNum provided");
		return vector<int>();
	}

	if (node2->getParent()->getParent() != NULL) {
		int possibleStmt1 = node2->getParent()->getStmtNumber();

		if (possibleStmt1 > 0) {
			result.push_back(possibleStmt1);
		}
	}
	
	return result;
}


vector<int> ParentTable::getChild(int stmtNum1) {
	vector<int> result;

	if (PKB::getInstance().nodeTable.count(stmtNum1) == 0) {
		return vector<int>();
	}

	if (PKB::getInstance().nodeTable.at(stmtNum1)->getNodeType() != While) {
		return result;
	}

	
	TNode* node1 = PKB::getInstance().nodeTable.at(stmtNum1)->getChildren()->at(1);
	assert(node1->getNodeType() == StmtLst);

	if (!node1) {
	//	throw exception("ParentTable exception: invalid stmtNum provided");
		return vector<int>();
	}
	
	if (node1->hasChild()) {
		vector<TNode*>* children = node1->getChildren();
		for (size_t i = 0 ; i < children->size(); i++ ) {
			int possibleStmt2 = children->at(i)->getStmtNumber();
			result.push_back(possibleStmt2);
		}
	}
	
	return result;
}


bool ParentTable::isParent(int stmtNum1, int stmtNum2) {
	if (PKB::getInstance().nodeTable.count(stmtNum2) == 0) {
		return false;
	}
	TNode* node2 = PKB::getInstance().nodeTable.at(stmtNum2);

	if (node2 && node2->getParent()) {
		int possibleStmt1 = node2->getParent()->getStmtNumber();

		if (possibleStmt1 == stmtNum1 && stmtNum1 != 0) {
			return true;
		}
	}

	return false;
}


void generateTransitiveParentPairs(vector<int> parentList, TNode* curNode, vector<int>* result1, vector<int>* result2) {
	
	vector<TNode*>* curNodeChildren = curNode->getChildren();
	for (size_t i = 0; i < curNodeChildren->size(); ++i) {

		for (vector<int>::iterator iter = parentList.begin(); iter != parentList.end(); ++iter) {
			result1->push_back(*iter);
			result2->push_back(curNodeChildren->at(i)->getStmtNumber());
		}

		if (curNodeChildren->at(i)->getNodeType() == While && curNodeChildren->at(i)->getChildren()->at(1)->hasChild()) {
			parentList.push_back(curNodeChildren->at(i)->getStmtNumber());

			generateTransitiveParentPairs(parentList, curNodeChildren->at(i)->getChildren()->at(1), result1, result2);

			parentList.erase(parentList.end() - 1);
		}
	}

		
}


/**
 * Returns all pairs of statements, <s1, s2>, where Parent(s1, s2) is satisfied
 */
pair<vector<int>, vector<int>> ParentTable::getAllParentPairs(bool transitiveClosure) {
	pair<vector<int>, vector<int>> result;

	if (!transitiveClosure) {
		for (auto iter = PKB::getInstance().nodeTable.begin(); iter != PKB::getInstance().nodeTable.end(); ++iter) {
			TNode* node = iter->second;
			if (!node->hasChild() || node->getStmtNumber() <= 0) {
				continue;
			}
			if (node->getNodeType() != While) {
				continue;
			}

			vector<TNode*>* childrenList = node->getChildren()->at(1)->getChildren();
			
			for (size_t i = 0; i < childrenList->size(); ++i) {
				if (childrenList->at(i)->getNodeType() != Assign && childrenList->at(i)->getNodeType() != While) {
					continue;
				}

				result.first.push_back(node->getStmtNumber());
				result.second.push_back(childrenList->at(i)->getStmtNumber());
			}
		}
	} else {
		vector<TNode*>* rootChildren = PKB::getInstance().ast->getRoot()->getChildren();

		vector<int> emptyList;
		generateTransitiveParentPairs(emptyList, PKB::getInstance().ast->getRoot(), &(result.first), &(result.second));
	
	}

	return result;
}