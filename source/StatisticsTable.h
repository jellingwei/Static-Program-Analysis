#pragma once

#include <stdio.h>
#include <string>
#include <vector>
#include <unordered_map>

using namespace std;

#include "common.h"
#include "PKB.h"

class StatisticsTable 
{
public:
	StatisticsTable();
	~StatisticsTable();
	
	double getCountForType(SYNONYM_TYPE);

	void initialize();

	double getAffectsCost();
	double getAffectsSCost();
	double getFollowsSCost();
	double getParentSCost();
	double getCallsSCost();
	double getNextSCost();
	double getPatternCost();
	
	double getReductionFactor(QNODE_TYPE rel_type, SYNONYM_TYPE typeLHS, SYNONYM_TYPE typeRHS, DIRECTION direction);
	double getModifiesReductionFactor(SYNONYM_TYPE typeLHS, SYNONYM_TYPE typeRHS, DIRECTION direction);
	double getUsesReductionFactor(SYNONYM_TYPE typeLHS, SYNONYM_TYPE typeRHS, DIRECTION direction);
	double getParentReductionFactor(SYNONYM_TYPE typeLHS, SYNONYM_TYPE typeRHS, DIRECTION direction);
	double getParentSReductionFactor(SYNONYM_TYPE typeLHS, SYNONYM_TYPE typeRHS, DIRECTION direction);
	double getFollowsReductionFactor(SYNONYM_TYPE typeLHS, SYNONYM_TYPE typeRHS, DIRECTION direction);
	double getFollowsSReductionFactor(SYNONYM_TYPE typeLHS, SYNONYM_TYPE typeRHS, DIRECTION direction);
	double getCallsReductionFactor(SYNONYM_TYPE typeLHS, SYNONYM_TYPE typeRHS, DIRECTION direction);
	double getCallsSReductionFactor(SYNONYM_TYPE typeLHS, SYNONYM_TYPE typeRHS, DIRECTION direction);
	double getNextReductionFactor(SYNONYM_TYPE typeLHS, SYNONYM_TYPE typeRHS, DIRECTION direction);
	double getNextSReductionFactor(SYNONYM_TYPE typeLHS, SYNONYM_TYPE typeRHS, DIRECTION direction);
	double getAffectsReductionFactor(SYNONYM_TYPE typeLHS, SYNONYM_TYPE typeRHS, DIRECTION direction);
	double getAffectsSReductionFactor(SYNONYM_TYPE typeLHS, SYNONYM_TYPE typeRHS, DIRECTION direction);
	double getPatternReductionFactor(SYNONYM_TYPE typeLHS, SYNONYM_TYPE typeRHS, DIRECTION direction);
	double getWithReductionFactor(SYNONYM_TYPE typeLHS, SYNONYM_TYPE typeRHS, DIRECTION direction);

private:

	double stmtSize;
	double assignSize;
	double whileSize;
	double ifSize;
	double callSize;
	double varSize;
	double constantSize;
	double procedureSize;
	double progLineSize;
	
	double averageLinesPerProc;
	double averageContainersPerProc;
};