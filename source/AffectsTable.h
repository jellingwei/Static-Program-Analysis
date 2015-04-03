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


class AffectsTable {
public:

	BOOLEAN_ isAffects(PROG_LINE_ progLine1, PROG_LINE_ progLine2, TRANS_CLOSURE transitiveClosure);
	
	PROGLINE_LIST getProgLinesAffectedBy(PROG_LINE_ progLine1, TRANS_CLOSURE transitiveClosure, bool terminateOnOneResult = false);
	PROGLINE_LIST getProgLinesAffecting(PROG_LINE_ progLine2, TRANS_CLOSURE transitiveClosure, bool terminateOnOneResult = false);

	pair<PROGLINE_LIST, PROGLINE_LIST> getAllAffectsPairs(TRANS_CLOSURE transitiveClosure);

	
	PROGLINE_LIST getLhs();
	PROGLINE_LIST getRhs();

	BOOLEAN_ isValid();
	BOOLEAN_ isLhsValid(PROG_LINE_);
	BOOLEAN_ isRhsValid(PROG_LINE_);

	static BOOLEAN_ canSkipNodesBackwards(CNode* node);
	static BOOLEAN_ canSkipNodesForwards(CNode* node);


};

