#pragma once

#include<stdio.h>
#include <iostream>
#include <string>
#include <vector>
#include <unordered_map>

#include "ProcTable.h"

using namespace std;
using namespace stdext;

int ProcTable::insertProc(string procName) 
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


int ProcTable::getProcTableSize() 
{
	return procMap.size();
}


string ProcTable::getProcName(int index) 
{
	if (index < 0) {
		return "";
	}
	return (procMap.size() > index) ? procMap.at(index) : "";
}

int ProcTable::getProcIndex(string procName) 
{
	if (procName.empty()) {
		throw exception("procTable error: Empty variable name");
	}

	return (indexMap.count(procName) > 0) ? indexMap.at(procName) : -1;
}


vector<int> ProcTable::getAllProcIndex() 
{
	vector<int> allKeys;

	for(int i = 0; i< procMap.size();i++) {
		allKeys.push_back(i);
	}

	return allKeys;
}
