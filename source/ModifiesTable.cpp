#pragma once

#include<stdio.h>
#include <iostream>
#include <string>
#include <vector>
#include <unordered_map>
#include <algorithm>

#include "PKB.h"
#include "ParentSolver.h"
#include "ModifiesTable.h"

using namespace std;
using namespace stdext;

//@todo change to bool
void ModifiesTable::init(INTEGER numVariables) {
	this->numVariables = numVariables;
}

STATUS ModifiesTable::setModifies(STATEMENT stmtNum, VAR_INDEX varIndex) {
	if (this->numVariables == 0) {
		throw logic_error("init modifiesTable first");
	}
	if (stmtNum <= 0 || varIndex <= 0) {
		throw exception("ModifiesTable error: Negative statement number or varIndex");
	}

	vector<int> newVarList;
	vector<int> varIndexList;

	//check if varIndexMap key stmtNum contains the variable
	if (varIndexMap.count(stmtNum) > 0) {
		boost::dynamic_bitset<> varIndexList;

		varIndexList = varIndexMap.at(stmtNum);

		bool result = varIndexList[varIndex];
		if (result) { // varIndex can be found already
		   //@todo
		} else {
			varIndexList[varIndex] = true;
			varIndexMap.erase(stmtNum);
			varIndexMap.insert(pair<int, boost::dynamic_bitset<> > (stmtNum, varIndexList));
		}

	} else {
		boost::dynamic_bitset<> varIndexList(this->numVariables);
		auto position = varIndexList[varIndex];
		position = true;
		varIndexMap.insert(pair<int, boost::dynamic_bitset<> > (stmtNum, varIndexList));
	}

	vector<int> newStmtList;
	vector<int> stmtList;

	//check if stmtNumMap key varIndex contains the stmtNum
	if (stmtNumMap.count(varIndex) > 0) {
		stmtList = stmtNumMap.at(varIndex);

		auto result = find(begin(stmtList), end(stmtList), stmtNum);
		if (result != end(stmtList)) 	{
			
		} else {
			stmtList.push_back(stmtNum);
			stmtNumMap.erase(varIndex);
			stmtNumMap.insert(pair<int, vector<int>> (varIndex, stmtList));
		}

	} else {
		newStmtList.push_back(stmtNum);
		stmtNumMap.insert(pair<int, vector<int>> (varIndex, newStmtList));
	}
	return true;
}

BOOLEAN_ ModifiesTable::isModifies(STATEMENT stmtNum, VAR_INDEX varIndex) {
	if (stmtNum <= 0 || varIndex <= 0) {
		return false;
	}

	boost::dynamic_bitset<> varIndexList;

	if (varIndexMap.count(stmtNum) > 0) {
		varIndexList = varIndexMap.at(stmtNum);
	} else {
		return false;
	}

	bool result = varIndexMap.at(stmtNum)[varIndex];
	// TRUE if varindex is new in the table
	return result; 
}

STATEMENT_LIST ModifiesTable::getModStmtNum(VAR_INDEX varIndex) 
{
	if (varIndex <= 0) {
		return vector<int>();
	}

	if (stmtNumMap.count(varIndex) == 0) {
		vector<int> result;
		return result;
	}

	vector<int> stmtList = stmtNumMap.at(varIndex);
	return stmtList;
}


VARINDEX_LIST ModifiesTable::getModVarForStmt(STATEMENT stmtNum) {
	if (stmtNum <= 0) {
		return vector<int>();
	}

	if (varIndexMap.count(stmtNum) == 0) {
		vector<int> result;
		return result;
	}

	boost::dynamic_bitset<> varIndexList = varIndexMap.at(stmtNum);

	vector<int> result;
	for (int i = 0; i < numVariables; i++) {
		if (varIndexList[i]) {
			result.push_back(i);
		}
	}
	return result;
}

boost::dynamic_bitset<> ModifiesTable::getModVarInBitvectorForStmt(STATEMENT stmtNum) {

	if (varIndexMap.count(stmtNum) == 0) {
		return boost::dynamic_bitset<>(numVariables);
	}

	return varIndexMap.at(stmtNum);
}

pair<STATEMENT_LIST, VARINDEX_LIST> ModifiesTable::getAllModPair() {
	pair<vector<int>, vector<int>> result;
	for (auto iter = varIndexMap.begin(); iter != varIndexMap.end(); ++iter) {
		for (unsigned int i = 0; i < iter->second.size(); i++) {
			result.first.push_back(iter->first);
			result.second.push_back(iter->second[i]);
		}
		
	}

	return result;
}

// for procedures

STATUS ModifiesTable::setModifiesProc(PROC_INDEX procIndex, VAR_INDEX varIndex) 
{
	if (procIndex < 0 ) {
		throw logic_error("ModifiesTable error: Negative procedure index");
	}
	if (varIndex <= 0) {
		throw logic_error("ModifiesTable error: Negative varIndex");
	}

	vector<int> newVarList;
	vector<int> varIndexList;

	//check if varIndexMap key stmtNum contains the variable
	if (procVarIndexMap.count(procIndex) > 0) {
		varIndexList = procVarIndexMap.at(procIndex);

		auto result = find(begin(varIndexList), end(varIndexList), varIndex);
		if (result != end(varIndexList)) { // varIndex can be found already
			
		} else {
			varIndexList.push_back(varIndex);
			procVarIndexMap.erase(procIndex);
			procVarIndexMap.insert(pair<int, vector<int> > (procIndex, varIndexList));
		}

	} else {
		newVarList.push_back(varIndex);
		procVarIndexMap.insert(pair<int, vector<int>> (procIndex, newVarList));
	}

	vector<int> newProcIndexList;
	vector<int> procIndexList;

	//check if procIndexMap key varIndex contains the procedure index
	if (procIndexMap.count(varIndex) > 0) {
		procIndexList = procIndexMap.at(varIndex);

		auto result = find(begin(procIndexList), end(procIndexList), procIndex);
		if (result != end(procIndexList)) 	{
			
		} else {
			procIndexList.push_back(procIndex);
			procIndexMap.erase(varIndex);
			procIndexMap.insert(pair<int, vector<int>> (varIndex, procIndexList));
		}

	} else {
		newProcIndexList.push_back(procIndex);
		procIndexMap.insert(pair<int, vector<int>> (varIndex, newProcIndexList));
	}
	return true;
}

BOOLEAN_ ModifiesTable::isModifiesProc(PROC_INDEX procIndex, VAR_INDEX varIndex) {
	if (procIndex < 0 || varIndex <= 0) {
		return false;
	}

	vector<int> varIndexList;

	if (procVarIndexMap.count(procIndex) > 0) {
		varIndexList = procVarIndexMap.at(procIndex);
	} else {
		return false;
	}

	auto result = find(begin(varIndexList), end(varIndexList), varIndex);
	// TRUE if varindex is new in the table
	return result != end(varIndexList);
}

PROCINDEX_LIST ModifiesTable::getModProcIndex(VAR_INDEX varIndex) 
{
	if (varIndex <= 0) {
		return vector<int>();
	}

	if (procIndexMap.count(varIndex) == 0) {
		vector<int> result;
		return result;
	}

	vector<int> procIndexList = procIndexMap.at(varIndex);
	return procIndexList;
}

VARINDEX_LIST ModifiesTable::getModVarForProc(PROC_INDEX procIndex) 
{
	
	if (procIndex < 0) {
		return vector<int>();
	}

	if (procVarIndexMap.count(procIndex) == 0) {
		vector<int> result;
		return result;
	}

	vector<int> varIndexList = procVarIndexMap.at(procIndex);
	return varIndexList;
}

pair<PROCINDEX_LIST, VARINDEX_LIST> ModifiesTable::getAllModProcPair() 
{
	pair<vector<int>, vector<int>> result;
	for (auto iter = procVarIndexMap.begin(); iter != procVarIndexMap.end(); ++iter) {
		for (vector<int>::iterator varListIter = iter->second.begin(); varListIter != iter->second.end(); ++varListIter) {
			result.first.push_back(iter->first);
			result.second.push_back(*varListIter);
		}
		
	}

	return result;
}


STATEMENT_LIST ModifiesTable::getLhs() {
	// returns statement numbers  // is this just all statements?
	vector<int> result;
	for (auto iter = varIndexMap.begin(); iter != varIndexMap.end(); ++iter) {
		result.push_back(iter->first);
	}
	return result;
}

STATEMENT_LIST ModifiesTable::getRhs() {
	// returns variables
	vector<int> result;
	for (auto iter = stmtNumMap.begin(); iter != stmtNumMap.end(); ++iter) {
		result.push_back(iter->first);
	}
	return result;
}