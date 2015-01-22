#pragma once

#include<stdio.h>
#include <iostream>
#include <string>
#include <vector>
#include <unordered_map>

#include "ProcTable.h"

using namespace std;
using namespace stdext;


/**
 * If procName is not in the ProcTable, inserts it into the ProcTable
 * and return its index. Otherwise, return its INDEX
 * and the table remains unchanged.
 * @exception if procName is empty.
 */
int ProcTable::insertProc(string procName) 
{

	if (procName.empty()) 
	{
		throw exception("procTable error: Empty procedure name");
	}

	if (indexMap.count(procName) > 0) 
	{
		return indexMap.at(procName);
	} 

	procMap.push_back(procName);

	int index = procMap.size() - 1;
	pair<string,int> pairProcIndex (procName, index);
	indexMap.insert(pairProcIndex);

	return index;
	
}

/**
 * Return the total number of procedures in the the ProcTable.
 */
int ProcTable::getProcTableSize() 
{
	return procMap.size();
}

/**
 * Return the name of the procedure in the ProcTable with the given index.
 * If index is out of range, return an empty string.
 */
string ProcTable::getProcName(int index) 
{
	return (procMap.size() > 0) ? procMap.at(index) : "";
}

/**
 * If procName is in the ProcTable, return its index. Otherwise, return -1 to
 * indicate there is no such procedure in the the ProcTable. If procName is empty or null, -1 is
 * returned.
 * @exception if procName is empty
 */
int ProcTable::getProcIndex(string procName) 
{
	if (procName.empty()) 
	{
		throw exception("procTable error: Empty variable name");
	}

	return (indexMap.count(procName) > 0) ? indexMap.at(procName) : -1;
}

