#pragma once

#include "StatisticsTable.h"

StatisticsTable::StatisticsTable()
{
	initialize();
}

StatisticsTable::~StatisticsTable() 
{
}

void StatisticsTable::initialize()
{
	PKB pkb = PKB::getInstance();
	stmtSize = pkb.getStmtTableSize();
	assignSize = pkb.getStmtNumForType(ASSIGN).size();
	whileSize = pkb.getStmtNumForType(WHILE).size();
	ifSize = pkb.getStmtNumForType(IF).size();
	callSize = pkb.getStmtNumForType(CALL).size();
	varSize = pkb.getVarTableSize();
	constantSize = pkb.getConstantTableSize();
	procedureSize = pkb.getProcTableSize();
	progLineSize = stmtSize;

	averageLinesPerProc = stmtSize / procedureSize;
	averageContainersPerProc = (whileSize + ifSize) / procedureSize;
}

double StatisticsTable::getCountForType(SYNONYM_TYPE type)
{
	switch (type) {
	case STMT:
		return stmtSize;
	case ASSIGN:
		return assignSize;
	case WHILE:
		return whileSize;
	case IF:
		return ifSize;
	case CALL:
		return callSize;
	case VARIABLE:
		return varSize;
	case CONSTANT:
		return constantSize;
	case PROCEDURE:
		return procedureSize;
	case PROG_LINE:
		return stmtSize;
	default:
		return 50;  //An arbitrary number
	}
}

double StatisticsTable::getAffectsCost()
{
	return assignSize / 2;
}

double StatisticsTable::getAffectsSCost()
{
	return assignSize;
}

double StatisticsTable::getFollowsSCost()
{
	//Average number of assign and call statements per procedure
	return averageLinesPerProc - averageContainersPerProc;
}

double StatisticsTable::getParentSCost()
{
	return averageContainersPerProc;
}

double StatisticsTable::getCallsSCost()
{
	return procedureSize - 1;
}

double StatisticsTable::getNextSCost()
{
	return averageLinesPerProc;
}

double StatisticsTable::getPatternCost()
{
	return assignSize / 3;
}

double StatisticsTable::getReductionFactor(QNODE_TYPE rel_type, SYNONYM_TYPE typeLHS, SYNONYM_TYPE typeRHS, DIRECTION direction)
{
	switch (rel_type) {
	case ModifiesP:
	case ModifiesS:
		getModifiesReductionFactor(typeLHS, typeRHS, direction);
	case UsesP:
	case UsesS:
		getUsesReductionFactor(typeLHS, typeRHS, direction);
	case Parent:
		getParentReductionFactor(typeLHS, typeRHS, direction);
	case ParentT:
		getParentSReductionFactor(typeLHS, typeRHS, direction);
	case Follows:
		getFollowsReductionFactor(typeLHS, typeRHS, direction);
	case FollowsT:
		getFollowsSReductionFactor(typeLHS, typeRHS, direction);
	case Calls:
		getCallsReductionFactor(typeLHS, typeRHS, direction);
	case CallsT:
		getCallsSReductionFactor(typeLHS, typeRHS, direction);
	case Next:
		getNextReductionFactor(typeLHS, typeRHS, direction);
	case NextT:
		getNextSReductionFactor(typeLHS, typeRHS, direction);
	case Affects:
		getAffectsReductionFactor(typeLHS, typeRHS, direction);
	case AffectsT:
		getAffectsSReductionFactor(typeLHS, typeRHS, direction);
	case Pattern:
		getPatternReductionFactor(typeLHS, typeRHS, direction);
	case With:
		getWithReductionFactor(typeLHS, typeRHS, direction);
	default:
		return -1;  //It should never reach here
	}
}

double StatisticsTable::getModifiesReductionFactor(SYNONYM_TYPE typeLHS, SYNONYM_TYPE typeRHS, DIRECTION direction)
{
	switch (direction) {
	case LeftToRight:
		return 1 / varSize;
	default:
		return getCountForType(typeLHS) / stmtSize;
	}
}

double StatisticsTable::getUsesReductionFactor(SYNONYM_TYPE typeLHS, SYNONYM_TYPE typeRHS, DIRECTION direction)
{
	switch (direction) {
	case LeftToRight:
		return 1 / varSize;
	default:
		return getCountForType(typeLHS) / stmtSize;
	}
}

double StatisticsTable::getParentReductionFactor(SYNONYM_TYPE typeLHS, SYNONYM_TYPE typeRHS, DIRECTION direction)
{
	switch (direction) {
	case LeftToRight:
		return 1 / getCountForType(typeRHS);
	default:
		return 1 / getCountForType(typeLHS);
	}
}

double StatisticsTable::getParentSReductionFactor(SYNONYM_TYPE typeLHS, SYNONYM_TYPE typeRHS, DIRECTION direction)
{
	return 1 / procedureSize;
}

double StatisticsTable::getFollowsReductionFactor(SYNONYM_TYPE typeLHS, SYNONYM_TYPE typeRHS, DIRECTION direction)
{
	switch (direction) {
	case LeftToRight:
		return 1 / getCountForType(typeRHS);
	default:
		return 1 / getCountForType(typeLHS);
	}
}

double StatisticsTable::getFollowsSReductionFactor(SYNONYM_TYPE typeLHS, SYNONYM_TYPE typeRHS, DIRECTION direction)
{
	return 1 / procedureSize;
}

double StatisticsTable::getCallsReductionFactor(SYNONYM_TYPE typeLHS, SYNONYM_TYPE typeRHS, DIRECTION direction)
{
	return 1 / procedureSize;
}

double StatisticsTable::getCallsSReductionFactor(SYNONYM_TYPE typeLHS, SYNONYM_TYPE typeRHS, DIRECTION direction)
{
	return 1 / 2;  //Arbitrary
}

double StatisticsTable::getNextReductionFactor(SYNONYM_TYPE typeLHS, SYNONYM_TYPE typeRHS, DIRECTION direction)
{
	return 1 / stmtSize;
}

double StatisticsTable::getNextSReductionFactor(SYNONYM_TYPE typeLHS, SYNONYM_TYPE typeRHS, DIRECTION direction)
{
	return 1 / procedureSize;
}

double StatisticsTable::getAffectsReductionFactor(SYNONYM_TYPE typeLHS, SYNONYM_TYPE typeRHS, DIRECTION direction)
{
	return 1 / assignSize;
}

double StatisticsTable::getAffectsSReductionFactor(SYNONYM_TYPE typeLHS, SYNONYM_TYPE typeRHS, DIRECTION direction)
{
	return assignSize / averageLinesPerProc;
}

double StatisticsTable::getPatternReductionFactor(SYNONYM_TYPE typeLHS, SYNONYM_TYPE typeRHS, DIRECTION direction)
{
	return 1 / 10;  //Arbitrary
}

double StatisticsTable::getWithReductionFactor(SYNONYM_TYPE typeLHS, SYNONYM_TYPE typeRHS, DIRECTION direction)
{
	double sizeLHS = getCountForType(typeLHS);
	double sizeRHS = getCountForType(typeRHS);
	double minSize = std::min(sizeLHS, sizeRHS);
	return 1 / minSize;
}
