#pragma once

#include <cmath>
#include <stdio.h>
#include <string>
#include <vector>
#include <unordered_map>

using namespace std;

#include "common.h"
#include "PKB.h"

/*
cost: 
All * relation 
Modifies: 1
Uses: 1
Follows*: 3
Parent*: 2
Call*: 1.5
Next*: 3.5
Affects: 3
Affects*: 5
*/

class StatisticsTable 
{
public:
	StatisticsTable(unordered_map<string, SYNONYM_TYPE> synonymNameToTypeMap);
	~StatisticsTable();
	
	double getCountForType(SYNONYM_TYPE);

	double calculateCost(QNODE_TYPE rel_type, Synonym LHS, Synonym RHS, DIRECTION direction);
	double getAffectsCost();
	double getAffectsSCost();
	double getFollowsSCost();
	double getParentSCost();
	double getCallsSCost();
	double getNextSCost();
	double getPatternCost(Synonym synonym);
	
	void reduceCount(QNODE_TYPE rel_type, Synonym LHS, Synonym RHS, DIRECTION direction);
	void reduceCountModifies(Synonym LHS, Synonym RHS, DIRECTION direction);
	void reduceCountUses(Synonym LHS, Synonym RHS, DIRECTION direction);
	void reduceCountParent(Synonym LHS, Synonym RHS, DIRECTION direction);
	void reduceCountParentS(Synonym LHS, Synonym RHS, DIRECTION direction);
	void reduceCountFollows(Synonym LHS, Synonym RHS, DIRECTION direction);
	void reduceCountFollowsS(Synonym LHS, Synonym RHS, DIRECTION direction);
	void reduceCountCalls(Synonym LHS, Synonym RHS, DIRECTION direction);
	void reduceCountCallsS(Synonym LHS, Synonym RHS, DIRECTION direction);
	void reduceCountNext(Synonym LHS, Synonym RHS, DIRECTION direction);
	void reduceCountNextS(Synonym LHS, Synonym RHS, DIRECTION direction);
	void reduceCountAffects(Synonym LHS, Synonym RHS, DIRECTION direction);
	void reduceCountAffectsS(Synonym LHS, Synonym RHS, DIRECTION direction);
	
	void reduceCountPattern(Synonym LHS, Synonym RHS, DIRECTION direction);
	void reduceCountPatternAssign(Synonym LHS, Synonym RHS);
	void reduceCountPatternIf(Synonym LHS, Synonym RHS);
	void reduceCountPatternWhile(Synonym LHS, Synonym RHS);
	void reduceCountWith(Synonym LHS, Synonym RHS, DIRECTION direction);

private:

	unordered_map<string, double> _synonymsCount;  //Maps synonyms to the expected number of values
	unordered_map<string, SYNONYM_TYPE> _synonymNameToTypeMap;

	double _stmtSize;
	double _assignSize;
	double _whileSize;
	double _ifSize;
	double _callSize;
	double _varSize;
	double _constantSize;
	double _procedureSize;
	double _progLineSize;
	
	double _averageLinesPerProc;
	double _averageAssignPerProc;
	double _averageContainersPerProc;
	double _averageCallsPerProc;
	
	double _COST_MODIFIES;
	double _COST_USES;
	double _COST_FOLLOWS;
	double _COST_PARENT;
	double _COST_CALLS;
	double _COST_NEXT;
	double _COST_AFFECTS;
	double _COST_AFFECTSS;
	double _COST_FOLLOWSS;
	double _COST_PARENTS;
	double _COST_CALLSS;
	double _COST_NEXTS;
	double _COST_PATTERN;
	double _COST_WITH;
};
