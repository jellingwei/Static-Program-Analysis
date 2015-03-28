#pragma once

#include <stdio.h>
#include <string>
#include <vector>
#include <unordered_map>

using namespace std;

#include "PKB.h"

class StatisticsTable 
{
public:
	StatisticsTable();

	int getAffectsCost();
	int getAffectsSCost();
	int getFollowsSCost();
	int getParentSCost();
	int getCallsSCost();
	int getNextSCost();
	int getPatternCost();

private:
	
	int affectsWeight;
	int affectsSWeight;
	int followsSWeight;
	int parentSWeight;
	int callsSWeight;
	int nextSWeight;
	int patternWeight;

};