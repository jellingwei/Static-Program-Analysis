#pragma once

#include<stdio.h>
#include <iostream>
#include <string>
#include <vector>
#include <unordered_map>

#include "ProcTable.h"

using namespace std;
using namespace stdext;

PROC_INDEX ProcTable::insertProc(PROCNAME procName) 
{

	if (procName.empty()) {
		throw exception("procTable error: Empty procedure name");
	}

	if (indexMap.count(procName) > 0) {
		return indexMap.at(procName);
	} 

	procMap.push_back(procName);

	int index = procMap.size() - 1;
	pair<string,int> pairProcIndex (procName, index);
	indexMap.insert(pairProcIndex);

	return index;
	
}


INTEGER ProcTable::getProcTableSize() 
{
	return procMap.size();
}


PROCNAME ProcTable::getProcName(PROC_INDEX index) 
{
	if (index < 0) {
		return "";
	}
	return (procMap.size() > index) ? procMap.at(index) : "";
}

PROC_INDEX ProcTable::getProcIndex(PROCNAME procName) 
{
	if (procName.empty()) {
		throw exception("procTable error: Empty variable name");
	}

	return (indexMap.count(procName) > 0) ? indexMap.at(procName) : -1;
}


PROCINDEX_LIST ProcTable::getAllProcIndex() 
{
	vector<int> allKeys;

	for(int i = 0; i< procMap.size();i++) {
		allKeys.push_back(i);
	}

	return allKeys;
}
