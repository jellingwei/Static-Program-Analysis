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

	int getWeightsForRelation(string relation, string LHS, string RHS);

private:
	//TODO storage method

};