#pragma once

#include <stdio.h>
#include <iostream>
#include <vector>
#include <string>
#include <utility>
#include <tuple>
#include "common.h"
#include "TNode.h"
#include "PKB.h"

using namespace std;

class Siblings
{
public:
	vector<pair<int, vector<int>>> siblings(SYNONYM_TYPE first_siblingType, SYNONYM_TYPE second_siblingType);

	//bool siblings(int stmtNo, int stmtNo2);
	vector<int> siblings(int stmtNo, SYNONYM_TYPE second_siblingType);
	vector<int> siblings(SYNONYM_TYPE first_siblingType, int stmtNo);
private:
};