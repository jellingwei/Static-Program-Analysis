#pragma once

#include<stdio.h>
#include <iostream>
#include <string>
#include <vector>
#include <unordered_map>
#include "TNode.h"
#include "CNode.h"
#include "common.h"

using namespace std;
using namespace stdext;

/**
	@brief Refer to the PKB for the documentation.
*/

class AffectsBipSolver {
public:

	BOOLEAN_ isAffectsBip(PROG_LINE_ progLine1, PROG_LINE_ progLine2, TRANS_CLOSURE transitiveClosure);
	
	PROGLINE_LIST getProgLinesAffectsBipAfter(PROG_LINE_ progLine1, TRANS_CLOSURE transitiveClosure, bool terminateOnOneResult = false);
	PROGLINE_LIST getProgLinesAffectsBipBefore(PROG_LINE_ progLine2, TRANS_CLOSURE transitiveClosure, bool terminateOnOneResult = false);

	pair<PROGLINE_LIST, PROGLINE_LIST> getAllAffectsBipPairs(TRANS_CLOSURE transitiveClosure);

	
	PROGLINE_LIST getLhs();
	PROGLINE_LIST getRhs();

};

