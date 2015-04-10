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

STATEMENT_LIST StmtTable::getStmtNumForType(STATEMENT_TYPE type) 
{

	string whileType = "while";
	string assignType = "assign";
	string ifType = "if";
	string callType = "call";
	string stmtType = "stmt";
	string progLineType = "prog_line";
	

	if(whileType.compare(type) == 0) {
		sort(whileStmt.begin(), whileStmt.end());  
		return whileStmt;

	} else if(assignType.compare(type) == 0) {
		sort(assignStmt.begin(), assignStmt.end());
		return assignStmt;

	} else if (ifType.compare(type) == 0) {
		sort(ifStmt.begin(), ifStmt.end());
		return ifStmt;

	} else if (callType.compare(type) == 0) {
		sort(callStmt.begin(), callStmt.end());
		return callStmt;

	} else if (stmtType.compare(type) == 0 || progLineType.compare(type) == 0) {
		vector<int> result;
		for (unordered_map<int, string>::iterator iter = stmtNumMap.begin(); iter != stmtNumMap.end(); ++iter) {
			result.push_back((*iter).first);
		}

		// sort(result.begin(), result.end()); // TODO: is there a need to sort?
		return result;
	} else {
		throw exception("StmtTable error: Invalid statement type");
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