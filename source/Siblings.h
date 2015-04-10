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
	vector<pair<int, vector<int>>> siblings(TNODE_TYPE first_siblingType, TNODE_TYPE second_siblingType);

private:
};