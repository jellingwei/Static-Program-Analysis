#pragma once

#include <stdio.h>
#include <iostream>
#include <string>
#include <vector>
#include <unordered_map>
#include <algorithm>

#include "StmtTable.h"

using namespace std;

bool StmtTable::insertStmt(int stmtNum, string type) 
{
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

bool StmtTable::isAssign(int stmtNo) 
{
	if(stmtNo <= 0) {
		throw exception("StmtTable error: Negative statment number");
	}

	return stmtNumMap.at(stmtNo) == "assign";
}

/**
* Return TRUE if stmtNo is of While Type. Otherwise, return FALSE. 
* If stmtNo is out of range, return FALSE.
*/
bool StmtTable::isWhile(int stmtNo) 
{
	if(stmtNo <= 0) {
		throw exception("StmtTable error: Negative statment number");
	}

	return stmtNumMap.at(stmtNo) == "while";
	
}

/**
* Return TRUE if stmtNo is of If Type. Otherwise, return FALSE. 
* If stmtNo is out of range, return FALSE.
*/
bool StmtTable::isIf(int stmtNo) 
{
	if(stmtNo <= 0) {
		throw exception("StmtTable error: Negative statment number");
	}

	return stmtNumMap.at(stmtNo) == "if";
	
}

/**
* Return TRUE if stmtNo is of call Type. Otherwise, return FALSE. 
* If stmtNo is out of range, return FALSE.
*/
bool StmtTable::isCall(int stmtNo) 
{
	if(stmtNo <= 0) {
		throw exception("StmtTable error: Negative statment number");
	}

	return stmtNumMap.at(stmtNo) == "call";
	
}

/**
 * Return the total number of statements in the the StmtTable.
 */
int StmtTable::getSize() 
{
	return stmtNumMap.size();
}