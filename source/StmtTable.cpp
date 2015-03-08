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

bool StmtTable::insertStmt(int stmtNum, string type, TNode* node, int procIndex) {
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

string StmtTable::getType(int stmtNum) 
{
	if(stmtNum <= 0) {
		throw exception("StmtTable error: Negative statment number");
	}

	if(stmtNumMap.count(stmtNum)==1) {
		return stmtNumMap.at(stmtNum);
	} 
	return "";
}

vector<int> StmtTable::getStmtNumForType(string type) 
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

int StmtTable::getProcIndexForStmt(int stmtNo) {
	throw exception("not implemented yet");
}

bool StmtTable::isAssign(int stmtNo) 
{
	if(stmtNo <= 0) {
		throw exception("StmtTable error: Negative statment number");
	}

	return stmtNumMap.at(stmtNo) == "assign";
}

bool StmtTable::isWhile(int stmtNo) 
{
	if(stmtNo <= 0) {
		throw exception("StmtTable error: Negative statment number");
	}

	return stmtNumMap.at(stmtNo) == "while";
	
}

bool StmtTable::isIf(int stmtNo) 
{
	if(stmtNo <= 0) {
		throw exception("StmtTable error: Negative statment number");
	}

	return stmtNumMap.at(stmtNo) == "if";
	
}

bool StmtTable::isCall(int stmtNo) 
{
	if(stmtNo <= 0) {
		throw exception("StmtTable error: Negative statment number");
	}

	return stmtNumMap.at(stmtNo) == "call";
	
}

int StmtTable::getSize() 
{
	return stmtNumMap.size();
}

TNode* StmtTable::getNodeForStmt(int stmtNum) {
	if (nodeTable.count(stmtNum) > 0) {
		return nodeTable.at(stmtNum);
	} else {
		return NULL;
	}
}