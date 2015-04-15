#pragma once

#include <stdio.h>
#include <iostream>
#include <unordered_map>

#include <boost/dynamic_bitset.hpp>

#include "CNode.h"
#include "common.h"

class NextTable 
{
public:
	NextTable();
	PROGLINE_LIST getNextAfter(PROG_LINE_ progLine1, TRANS_CLOSURE transitiveClosure = false);
	PROGLINE_LIST getNextBefore(PROG_LINE_ progLine2, TRANS_CLOSURE transitiveClosure = false);

	BOOLEAN_ isNext(PROG_LINE_ progLine1, PROG_LINE_ progLine2, TRANS_CLOSURE transitiveClosure = false);
	PROGLINE_LIST getLhs();
	PROGLINE_LIST getRhs();
	BOOLEAN_ isValid();

	BOOLEAN_ isLhsValid(PROG_LINE_);
	BOOLEAN_ isRhsValid(PROG_LINE_);

	BOOLEAN_ setLhs(PROG_LINE_);
	BOOLEAN_ setRhs(PROG_LINE_);


	PROG_LINE_ getFirstProgLineInProc(PROC_INDEX procIndex);
	PROG_LINE_ getLastProgLineInProc(PROC_INDEX procIndex);
	void setFirstProgLineInProc(PROC_INDEX procIndex, PROG_LINE_ firstProgline);
	void setLastProgLineInProc(PROC_INDEX procIndex, PROG_LINE_ lastProgline);

	PROG_LINE_ getFirstProgLineInContainer(CONTAINER_STATEMENT container);
	PROG_LINE_ getLastProgLineInContainer(CONTAINER_STATEMENT container);
	void setFirstProgLineInElse(CONTAINER_STATEMENT container, PROG_LINE_ progline);
	void setLastProgLineInContainer(CONTAINER_STATEMENT container, PROG_LINE_ progline);

	BOOLEAN_ setProgLineInWhile(PROG_LINE_ progline); 

private:
	vector<int> lhs;
	vector<int> rhs;

	vector<int> firstProgLineInProc;
	vector<int> lastProgLineInProc;

	boost::dynamic_bitset<> isProgLineInWhile;

	unordered_map<int, int> firstProgLineInElse;
	unordered_map<int, int> lastProgLineInContainer;

};