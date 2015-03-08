#pragma once

#include <stdio.h>
#include <iostream>
#include <unordered_map>
#include "CNode.h"

class NextTable 
{
public:
	NextTable();
	vector<int> getNextAfter(int progLine1, bool transitiveClosure = false);
	vector<int> getNextBefore(int progLine2, bool transitiveClosure = false);

	bool isNext(int progLine1, int progLine2, bool transitiveClosure = false);
	vector<int> getLhs();
	vector<int> getRhs();

	CNode* getCNodeForProgLine(int);

};