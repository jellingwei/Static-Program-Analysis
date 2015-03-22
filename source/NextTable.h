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

	int getFirstProgLineInProc(int);
	int getLastProgLineInProc(int);
	void setFirstProgLineInProc(int, int);
	void setLastProgLineInProc(int, int);

	int getFirstProgLineInContainer(int);
	int getLastProgLineInContainer(int);
	void setFirstProgLineInContainer(int, int);
	void setLastProgLineInContainer(int, int);

private:
	vector<int> firstProgLineInProc;
	vector<int> lastProgLineInProc;

	unordered_map<int, int> firstProgLineInContainer;
	unordered_map<int, int> lastProgLineInContainer;

};