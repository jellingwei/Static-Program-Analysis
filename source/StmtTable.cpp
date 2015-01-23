#pragma once

#include <stdio.h>
#include <iostream>
#include <string>
#include <vector>
#include <unordered_map>
#include <algorithm>

#include "StmtTable.h"

using namespace std;

/**
* Return TRUE if the StmtTable is updated accordingly. Otherwise, return FALSE. 
* If stmtNum and type are already present in the StmtTable and are previously set, the StmtTable will not be updated.
* @exception if stmtNum is negative or 0, or type is not while/assign/if.
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
	{	//no such entry for given stmtNum
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
		
		return true;
	} else {
		return false;
	} 
}


/**
 * Return the statement type in the StmtTable with the given statement number.
 * If stmtNum is out of range, return an empty string.
 * @exception if stmtNum is negative or 0.
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
	} 
	return "";
}

/**
 * Return all the statement number of the statement type in the the StmtTable. 
 * If there is no answer or if type is an invalid STATEMENT_TYPE, return an empty list.
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

/**
* Return TRUE if stmtNo is of Assignment Type. Otherwise, return FALSE. 
* If stmtNo is out of range, return FALSE.
*/
bool StmtTable::isAssign(int stmtNo) 
{
	if(stmtNo <= 0) 
	{
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
	if(stmtNo <= 0) 
	{
		throw exception("StmtTable error: Negative statment number");
	}

	return stmtNumMap.at(stmtNo) == "while";
	
}

/**
 * Return the total number of statements in the the StmtTable.
 */
int StmtTable::getSize() 
{
	return stmtNumMap.size();
}