#pragma once

#include <stdio.h>
#include <iostream>
#include <unordered_map>

#include <boost/dynamic_bitset.hpp>

#include "CNode.h"
#include "common.h"

class NextBipTable 
{
public:
	NextBipTable();
	PROGLINE_LIST getNextBipAfter(PROG_LINE_ progLine1, TRANS_CLOSURE transitiveClosure = false);
	PROGLINE_LIST getNextBipBefore(PROG_LINE_ progLine2, TRANS_CLOSURE transitiveClosure = false);

	BOOLEAN_ isNextBip(PROG_LINE_ progLine1, PROG_LINE_ progLine2, TRANS_CLOSURE transitiveClosure = false);
	PROGLINE_LIST getLhs();
	PROGLINE_LIST getRhs();


private:


};