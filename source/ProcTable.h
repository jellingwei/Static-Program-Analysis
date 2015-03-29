#pragma once

#include<stdio.h>
#include <iostream>
#include <string>
#include <vector>
#include <unordered_map>

#include "common.h"
#include "common_list.h"

using namespace std;

class ProcTable {
public:
	PROC_INDEX insertProc(PROCNAME procName);
	INTEGER getProcTableSize();
	PROCNAME getProcName(PROC_INDEX index);
	PROC_INDEX getProcIndex(PROCNAME procName);
	PROCINDEX_LIST getAllProcIndex();

private: 
	vector<string> procMap; 
	unordered_map<string, int> indexMap; // key is variable name
};
