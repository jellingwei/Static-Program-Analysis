#pragma once

#include <stdio.h>
#include <iostream>
#include <string>
#include <vector>
#include <unordered_map>
#include <algorithm>

#include "StmtTable.h"

using namespace std;

/* 
 **Currently, parser to call method to insert stmtNumber read and its type("assign", "while" or "if")
 * @exception exception. exceptions are thrown when a negative statement number or if the stmt type is invalid.
*/
bool StmtTable::insertStmt(int stmtNum, string type) 
{
	if(stmtNum <= 0) 
	{
		throw exception("StmtTable error: Negative statment number");
	} else if(!(type=="while" || type=="assign" || type=="if")) 
	{
		throw exception("StmtTable error: Invalid statement type");
	}

	int toCheck = stmtNumMap.count(stmtNum);
	
	if(toCheck == 0) 
	{ //no such entry for given stmtNum
		pair<int, string> stmtNum_Type (stmtNum, type);
		stmtNumMap.insert(stmtNum_Type);

		string whileType = "while";
		string assignType = "assign";
		
		if(whileType.compare(type) == 0) 
		{
			whileStmt.push_back(stmtNum);
		}
		else if(assignType.compare(type) == 0) 
		{
			assignStmt.push_back(stmtNum);
		}
	//	else throw exception("No such statement type");
		
		return true;
	} else {
		return false;
		//throw exception("Statement already exists");
	} 
}


/*
**For Query processor to find the typeofStmt("assign" or "while") by supplying the stmt Number
*/
string StmtTable::getType(int stmtNum) 
{
	if(stmtNum <= 0) 
	{
		throw exception("StmtTable error: Negative statment number");
	}

	if(stmtNumMap.count(stmtNum)==1) 
	{
		return stmtNumMap.at(stmtNum);
	} /* else { 
		throw exception("No such statement number");
	} */
	return "";
}

/*
**For Query processor to find all stmt of type("assign", "while" or "if")
*/
vector<int> StmtTable::getStmtNumForType(string type) 
{

	string whileType = "while";
	string assignType = "assign";
	string stmtType = "stmt";
	string progLineType = "prog_line";
	string ifType = "if";

	if(whileType.compare(type) == 0) 
	{
		sort(whileStmt.begin(), whileStmt.end());
		return whileStmt;

	} else if(assignType.compare(type) == 0) 
	{
		sort(assignStmt.begin(), assignStmt.end());
		return assignStmt;

	} else if (stmtType.compare(type) == 0 || progLineType.compare(type) == 0) 
	{
		vector<int> result;
		for (unordered_map<int, string>::iterator iter = stmtNumMap.begin(); iter != stmtNumMap.end(); ++iter) 
		{
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
	if(stmtNo <= 0) 
	{
		throw exception("StmtTable error: Negative statment number");
	}

	return stmtNumMap.at(stmtNo) == "assign";
}

bool StmtTable::isWhile(int stmtNo) 
{
	if(stmtNo <= 0) 
	{
		throw exception("StmtTable error: Negative statment number");
	}

	return stmtNumMap.at(stmtNo) == "while";
	
}

/*
**To obtain total number of statements in program
*/
int StmtTable::getSize() 
{
	return stmtNumMap.size();
}