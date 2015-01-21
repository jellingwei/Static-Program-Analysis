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
* Returns the index of the variable.
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
* Returns the size of the procTable.
*/
int ProcTable::getProcTableSize() 
{
	return procMap.size();
}

/**
* Returns the name of a variable in varTable using the index as the key.
*/
string ProcTable::getProcName(int index) 
{
	return (procMap.size() > 0) ? procMap.at(index) : "";
}

/**
* Returns the index of a variable in varTable using the variable name as the key.
*/
int ProcTable::getProcIndex(string procName) 
{
	if (procName.empty()) 
	{
		throw exception("procTable error: Empty variable name");
	}

	return (indexMap.count(procName) > 0) ? indexMap.at(procName) : -1;
}

