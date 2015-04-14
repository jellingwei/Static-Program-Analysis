#pragma once

#include <stdio.h>
#include <iostream>
#include <string>
#include <vector>
#include <unordered_map>
#include <algorithm>

#include "StmtTable.h"
#include "TNode.h"
#include "PKB.h"

using namespace std;

STATUS StmtTable::insertStmt(STATEMENT stmtNum, STATEMENT_TYPE type, TNode* node, PROC_INDEX procIndex) {
	if(stmtNum <= 0) {
		throw exception("StmtTable error: Negative statment number");
	} else if(!(type=="while" || type=="assign" || type=="if" || type=="call")) {
		throw exception("StmtTable error: Invalid statement type");
	}

	int toCheck = stmtNumMap.count(stmtNum);
	
	if(toCheck == 0) {	//no such entry for given stmtNum
		pair<int, string> stmtNum_Type (stmtNum, type);
		stmtNumMap.insert(stmtNum_Type);

		string whileType = "while";
		string assignType = "assign";
		string callType = "call";
		string ifType = "if";
		
		if(whileType.compare(type) == 0) {
			whileStmt.push_back(stmtNum);
		}
		else if(assignType.compare(type) == 0) {
			assignStmt.push_back(stmtNum);
		}
		else if (callType.compare(type) == 0) {
			callStmt.push_back(stmtNum);
		} 
		else if (ifType.compare(type) == 0) {
			ifStmt.push_back(stmtNum);
		}
		
		pair<int, TNode*> stmtNumToNodePair(stmtNum, node);
		nodeTable.insert(stmtNumToNodePair);

		return true;
	} else {
		return false;
	} 
}

STATEMENT_TYPE StmtTable::getType(STATEMENT stmtNum) 
{
	if(stmtNum <= 0) {
		throw exception("StmtTable error: Negative statment number");
	}

	if(stmtNumMap.count(stmtNum)==1) {
		return stmtNumMap.at(stmtNum);
	} 
	return "";
}


BOOLEAN_ isAssignmentContainOperator(TNode* assignNode, TNODE_TYPE oper) {
	assert(oper == Plus || oper == Minus || oper == Times);

	assert(assignNode->getChildren()->size() == 2);
	TNode* rhsExpression = assignNode->getChildren()->at(1);

	stack<TNode*> nodesForTraversal;
	nodesForTraversal.push(rhsExpression);

	//dfs through children to find the operator
	while (!nodesForTraversal.empty()) {
		TNode* curNode = nodesForTraversal.top(); nodesForTraversal.pop();

		if (curNode->getNodeType() == oper) {
			return true;
		} 

		TNODE_LIST nextNodesToTraverse = curNode->getChildren();
		for (auto iter = nextNodesToTraverse->begin(); iter != nextNodesToTraverse->end(); ++iter ) {
			nodesForTraversal.push(*iter);
		}

	}
	return false;
}

/**
 * @param oper  operator to match
 * @param initialList  the initial list of statements to filter, default use should be to pass in all assignment statements
 */
STATEMENT_LIST getAssignmentsWithOperator(string oper, STATEMENT_LIST initialList) {
	PKB pkb = PKB::getInstance();

	TNODE_TYPE typeOfOper;
	if (oper == "plus") {
		typeOfOper = Plus;
	} else if (oper == "minus") {
		typeOfOper = Minus;
	} else if (oper == "times") {
		typeOfOper = Times;
	}

	STATEMENT_LIST finalResults;

	for (auto iter = initialList.begin(); iter != initialList.end(); ++iter) {
		TNode* nodeForStmt = pkb.getNodeForStmt(*iter);
		if (isAssignmentContainOperator(nodeForStmt, typeOfOper)) {
			finalResults.push_back(*iter);
		}
	}

	return finalResults;
}


// pass in all stmt nodes of procedures, whiles and ifs
// nodeList should contain only stmt list nodes
STATEMENT_LIST firstLineContainedInNodes(vector<TNode*> nodeList) {
	STATEMENT_LIST results;
	for (auto nodeIter = nodeList.begin(); nodeIter != nodeList.end(); ++nodeIter) {
		assert((*nodeIter)->getNodeType() == StmtLst);

		TNode* firstChild = (*nodeIter)->getChildren()->at(0);

		results.push_back(firstChild->getStmtNumber());
	}

	return results;
}

vector<TNode*> getStmtListNodes() {
	PKB pkb = PKB::getInstance();

	// get procs first
	TNODE_LIST procs = pkb.getRoot()->getChildren();
	vector<TNode*> procStmtList;
	for (auto iter = procs->begin(); iter != procs->end(); ++iter) {
		procStmtList.push_back((*iter)->getChildren()->at(0));
	}

	//get containers
	STATEMENT_LIST whiles = pkb.getStmtNumForType(WHILE);
	vector<TNode*> whileStmtList ;
	for (auto iter = whiles.begin(); iter != whiles.end(); ++iter) {
		TNode* node = pkb.getNodeForStmt(*iter);
		whileStmtList.push_back(node->getChildren()->at(1));
	}
	STATEMENT_LIST ifs = pkb.getStmtNumForType(IF);
	vector<TNode*> ifStmtList;
	for (auto iter = ifs.begin(); iter != ifs.end(); ++iter) {
		TNode* node = pkb.getNodeForStmt(*iter);
		ifStmtList.push_back(node->getChildren()->at(1)); // then
		ifStmtList.push_back(node->getChildren()->at(2)); // else
	}

	vector<TNode*> allStmtLists;
	allStmtLists.insert(allStmtLists.end(), procStmtList.begin(), procStmtList.end());
	allStmtLists.insert(allStmtLists.end(), whileStmtList.begin(), whileStmtList.end());
	allStmtLists.insert(allStmtLists.end(), ifStmtList.begin(), ifStmtList.end());

	return allStmtLists;
}


STATEMENT_LIST StmtTable::getStmtNumForType(STATEMENT_TYPE type) {

	if(string("while").compare(type) == 0) {
		return whileStmt;
	} else if(string("assign").compare(type) == 0) {
		return assignStmt;
	} else if (string("if").compare(type) == 0) {
		return ifStmt;
	} else if (string("call").compare(type) == 0) {
		return callStmt;
	} else if (string("stmt").compare(type) == 0 || string("prog_line").compare(type) == 0) {
		vector<int> result;
		for (unordered_map<int, string>::iterator iter = stmtNumMap.begin(); iter != stmtNumMap.end(); ++iter) {
			result.push_back((*iter).first);
		}

		return result;
	} 
	else if (string("plus") == type || string("minus") == type || string("times") == type ) {
		return getAssignmentsWithOperator(type, assignStmt);
	} else if (string("stmtLst") == type) {
		vector<TNode*> stmtListNodes = getStmtListNodes();
		return firstLineContainedInNodes(stmtListNodes);
	} else {

	}

}



BOOLEAN_ StmtTable::isAssign(STATEMENT stmtNum) 
{
	if(stmtNum <= 0) {
		throw exception("StmtTable error: Negative statment number");
	}

	return stmtNumMap.at(stmtNum) == "assign";
}

BOOLEAN_ StmtTable::isWhile(STATEMENT stmtNum) 
{
	if(stmtNum <= 0) {
		throw exception("StmtTable error: Negative statment number");
	}

	return stmtNumMap.at(stmtNum) == "while";
	
}

BOOLEAN_ StmtTable::isIf(STATEMENT stmtNum) 
{
	if(stmtNum <= 0) {
		throw exception("StmtTable error: Negative statment number");
	}

	return stmtNumMap.at(stmtNum) == "if";
	
}

BOOLEAN_ StmtTable::isCall(STATEMENT stmtNum) 
{
	if(stmtNum <= 0) {
		throw exception("StmtTable error: Negative statment number");
	}

	return stmtNumMap.at(stmtNum) == "call";
	
}

INTEGER StmtTable::getSize() 
{
	return stmtNumMap.size();
}

TNode* StmtTable::getNodeForStmt(STATEMENT stmtNum) {
	if (nodeTable.count(stmtNum) > 0) {
		return nodeTable.at(stmtNum);
	} else {
		return NULL;
	}
}