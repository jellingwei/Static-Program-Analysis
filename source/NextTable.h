#pragma once

#include <stdio.h>
#include <iostream>
#include <unordered_map>

#include <boost/dynamic_bitset.hpp>

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
	void setFirstProgLineInElse(int, int);
	void setLastProgLineInContainer(int, int);

	bool setProgLineInWhile(int); 

private:
	vector<int> firstProgLineInProc;
	vector<int> lastProgLineInProc;

	boost::dynamic_bitset<> isProgLineInWhile;

	unordered_map<int, int> firstProgLineInElse;
	unordered_map<int, int> lastProgLineInContainer;

};