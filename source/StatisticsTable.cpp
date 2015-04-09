#pragma once

#include "StatisticsTable.h"

StatisticsTable::StatisticsTable(unordered_map<string, SYNONYM_TYPE> synonymNameToTypeMap)
{
	PKB pkb = PKB::getInstance();
	_stmtSize = pkb.getStmtTableSize();
	_assignSize = pkb.getStmtNumForType(ASSIGN).size();
	_whileSize = pkb.getStmtNumForType(WHILE).size();
	_ifSize = pkb.getStmtNumForType(IF).size();
	_callSize = pkb.getStmtNumForType(CALL).size();
	_varSize = pkb.getVarTableSize();
	_constantSize = pkb.getConstantTableSize();
	_procedureSize = pkb.getProcTableSize();
	_progLineSize = _stmtSize;

	_averageLinesPerProc = ceil(_stmtSize / _procedureSize);
	_averageAssignPerProc = ceil(_assignSize / _procedureSize);
	_averageContainersPerProc = ceil((_whileSize + _ifSize) / _procedureSize);
	_averageCallsPerProc = ceil(_callSize / _procedureSize);

	_COST_MODIFIES = 1;
	_COST_USES = 1;
	_COST_FOLLOWS = 1;
	_COST_PARENT = 1;
	_COST_CALLS = 1;
	_COST_NEXT = 1;
	_COST_AFFECTS = 0;
	_COST_AFFECTSS = 0;
	_COST_FOLLOWSS = 0;
	_COST_PARENTS = 0;
	_COST_CALLSS = 0;
	_COST_NEXTS = 0;
	_COST_PATTERN = 0;
	_COST_WITH = 1;

	_synonymNameToTypeMap = synonymNameToTypeMap;
	for (auto itr = synonymNameToTypeMap.begin(); itr != synonymNameToTypeMap.end(); ++itr) {
		_synonymsCount[itr->first] = getCountForType(itr->second);
	}
}

StatisticsTable::~StatisticsTable() 
{
}

double StatisticsTable::getCountForType(SYNONYM_TYPE type)
{
	switch (type) {
	case STMT:
		return _stmtSize;
	case ASSIGN:
		return _assignSize;
	case WHILE:
		return _whileSize;
	case IF:
		return _ifSize;
	case CALL:
		return _callSize;
	case VARIABLE:
		return _varSize;
	case CONSTANT:
		return _constantSize;
	case PROCEDURE:
		return _procedureSize;
	case PROG_LINE:
		return _stmtSize;
	default:
		return _stmtSize / 2;  //An arbitrary number
	}
}

double StatisticsTable::calculateCost(QNODE_TYPE rel_type, Synonym LHS, Synonym RHS, DIRECTION direction)
{
	double numberOfValues;
	string nameLHS = LHS.getName();
	string nameRHS = RHS.getName();

	if (direction == LeftToRight) {
		if (LHS.isUndefined()) {
			return UINT_MAX;
		} else if (LHS.isConstant()) {
			numberOfValues = 1;
		} else {
			numberOfValues = _synonymsCount[nameLHS];
		}
	} else {
		if (RHS.isUndefined()) {
			return UINT_MAX;
		} else if (RHS.isConstant()) {
			numberOfValues = 1;
		} else {
			numberOfValues = _synonymsCount[nameRHS];
		}
	}

	switch (rel_type) {
	case ModifiesS:
	case ModifiesP:
		return numberOfValues * _COST_MODIFIES;
	case UsesS:
	case UsesP:
		return numberOfValues * _COST_USES;
	case Parent:
		return numberOfValues * _COST_PARENT;
	case ParentT:
		if (_COST_PARENTS == 0) {
			_COST_PARENTS = getParentSCost();
		}
		return numberOfValues * _COST_PARENTS;
	case Follows:
		return numberOfValues * _COST_FOLLOWS;
	case FollowsT:
		if (_COST_FOLLOWSS == 0) {
			_COST_FOLLOWSS = getFollowsSCost();
		}
		return numberOfValues * _COST_FOLLOWSS;
	case Calls:
		return numberOfValues * _COST_CALLS;
	case CallsT:
		if (_COST_CALLSS == 0) {
			_COST_CALLSS = getCallsSCost();
		}
		return numberOfValues * _COST_CALLSS;
	case Next:
		return numberOfValues * _COST_NEXT;
	case NextT:
		if (_COST_NEXTS == 0) {
			_COST_NEXTS = getNextSCost();
		}
		return numberOfValues * _COST_NEXTS;
	case Affects:
		if (_COST_AFFECTS == 0) {
			_COST_AFFECTS = getAffectsCost();
		}
		return numberOfValues * _COST_AFFECTS;
	case AffectsT:
		if (_COST_AFFECTSS == 0) {
			_COST_AFFECTSS = getAffectsSCost();
		}
		return numberOfValues * _COST_AFFECTSS;
	case Pattern:
		return getPatternCost(LHS);
	case With:
		return numberOfValues * _COST_WITH;
	default:
		return UINT_MAX;  //It should never reach here
	}
}

double StatisticsTable::getAffectsCost()
{
	return _assignSize / 2;
}

double StatisticsTable::getAffectsSCost()
{
	return _stmtSize / 2;
}

double StatisticsTable::getFollowsSCost()
{
	//Average number of assign and call statements per procedure
	return _averageLinesPerProc - _averageContainersPerProc;
}

double StatisticsTable::getParentSCost()
{
	return _averageContainersPerProc;
}

double StatisticsTable::getCallsSCost()
{
	return _procedureSize - 1;
}

double StatisticsTable::getNextSCost()
{
	return _averageLinesPerProc;
}

double StatisticsTable::getPatternCost(Synonym synonym)
{
	if (synonym.getType() == ASSIGN) {
		return _assignSize / 2;
	} else if (synonym.getType() == IF) {
		return _ifSize / 2;
	} else {
		return _whileSize / 2;
	}
}

void StatisticsTable::reduceCount(QNODE_TYPE rel_type, Synonym LHS, Synonym RHS, DIRECTION direction)
{
	switch (rel_type) {
	case ModifiesP:
	case ModifiesS:
		reduceCountModifies(LHS, RHS, direction);
		break;
	case UsesP:
	case UsesS:
		reduceCountUses(LHS, RHS, direction);
		break;
	case Parent:
		reduceCountParent(LHS, RHS, direction);
		break;
	case ParentT:
		reduceCountParentS(LHS, RHS, direction);
		break;
	case Follows:
		reduceCountFollows(LHS, RHS, direction);
		break;
	case FollowsT:
		reduceCountFollowsS(LHS, RHS, direction);
		break;
	case Calls:
		reduceCountCalls(LHS, RHS, direction);
		break;
	case CallsT:
		reduceCountCallsS(LHS, RHS, direction);
		break;
	case Next:
		reduceCountNext(LHS, RHS, direction);
		break;
	case NextT:
		reduceCountNextS(LHS, RHS, direction);
		break;
	case Affects:
		reduceCountAffects(LHS, RHS, direction);
		break;
	case AffectsT:
		reduceCountAffectsS(LHS, RHS, direction);
		break;
	case Pattern:
		reduceCountPattern(LHS, RHS, direction);
		break;
	case With:
		reduceCountWith(LHS, RHS, direction);
		break;
	}
}

void StatisticsTable::reduceCountModifies(Synonym LHS, Synonym RHS, DIRECTION direction)
{
	if (LHS.isConstant() && RHS.isSynonym()) {
		_synonymsCount[RHS.getName()] = 1;  //There will only be one variable that is modified
	} else if (RHS.isConstant() && LHS.isSynonym()) {
		double original = _synonymsCount[LHS.getName()];
		double estimated = ceil(_assignSize / _varSize);
		_synonymsCount[LHS.getName()] = min(original, estimated);
	} else {
		if (LHS.isUndefined() || RHS.isUndefined()) {
			return;
		}
		//Both are synonyms
		if (direction == LeftToRight) {
			double original = _synonymsCount[RHS.getName()];
			double estimated = min(_synonymsCount[LHS.getName()], _assignSize);
			_synonymsCount[RHS.getName()] = min(original, estimated);
		} else {
			double original = _synonymsCount[LHS.getName()];
			double estimated = ceil(_assignSize / _varSize * _synonymsCount[RHS.getName()]);
			_synonymsCount[LHS.getName()] = min(original, estimated);
		}
	}
}

void StatisticsTable::reduceCountUses(Synonym LHS, Synonym RHS, DIRECTION direction)
{
	if (RHS.isUndefined()) {
		double original = _synonymsCount[LHS.getName()];
		double estimated = ceil(_stmtSize * 0.75);
		_synonymsCount[LHS.getName()] = min(original, estimated);
	} else if (LHS.isConstant() && RHS.isSynonym()) {
		double original = _synonymsCount[RHS.getName()];
		double estimated = ceil(_varSize / 3);
		_synonymsCount[RHS.getName()] = min(original, estimated);
	} else if (RHS.isConstant() && LHS.isSynonym()) {
		_synonymsCount[LHS.getName()] = ceil(_synonymsCount[LHS.getName()] / 5);  //Arbitrary
	} else {
		if (LHS.isUndefined() || RHS.isUndefined()) {
			return;
		}
		//Both are synonyms
		if (direction == LeftToRight) {
			double original = _synonymsCount[RHS.getName()];
			double estimated = ceil(_varSize / 3 * _synonymsCount[LHS.getName()]);
			_synonymsCount[RHS.getName()] = min(original, estimated);
		} else {
			double original = _synonymsCount[LHS.getName()];
			double estimated = ceil(_synonymsCount[LHS.getName()] / 5 * _synonymsCount[RHS.getName()]);
			_synonymsCount[LHS.getName()] = min(original, estimated);
		}
	}
}

void StatisticsTable::reduceCountParent(Synonym LHS, Synonym RHS, DIRECTION direction)
{
	if (LHS.isConstant() && RHS.isSynonym()) {
		_synonymsCount[RHS.getName()] = 1;
	} else if (RHS.isConstant() && LHS.isSynonym()) {
		_synonymsCount[LHS.getName()] = 1;
	} else {
		if (LHS.isUndefined() || RHS.isUndefined()) {
			return;
		}
		//Both are synonyms
		if (direction == LeftToRight) {
			double original = _synonymsCount[RHS.getName()];
			double estimated = ceil(_synonymsCount[LHS.getName()] / 2);
			_synonymsCount[RHS.getName()] = min(original, estimated);
		} else {
			double original = _synonymsCount[LHS.getName()];
			double estimated = ceil(_synonymsCount[RHS.getName()] / 2);
			_synonymsCount[LHS.getName()] = min(original, estimated);
		}
	}
}

void StatisticsTable::reduceCountParentS(Synonym LHS, Synonym RHS, DIRECTION direction)
{
	if (LHS.isConstant() && RHS.isSynonym()) {
		double original = _synonymsCount[RHS.getName()];
		double estimated = _averageContainersPerProc;
		_synonymsCount[RHS.getName()] = min(original, estimated);
	} else if (RHS.isConstant() && LHS.isSynonym()) {
		double original = _synonymsCount[LHS.getName()];
		double estimated = _averageContainersPerProc;
		_synonymsCount[LHS.getName()] = min(original, estimated);
	} else {
		if (LHS.isUndefined() || RHS.isUndefined()) {
			return;
		}
		//Both are synonyms
		if (direction == LeftToRight) {
			double original = _synonymsCount[RHS.getName()];
			double estimated = min(_averageLinesPerProc, ceil(_synonymsCount[LHS.getName()] / _averageContainersPerProc) * _averageContainersPerProc);
			_synonymsCount[RHS.getName()] = min(original, estimated);
		} else {
			double original = _synonymsCount[LHS.getName()];
			double estimated = min(_averageLinesPerProc, ceil(_synonymsCount[RHS.getName()] / _averageContainersPerProc) * _averageContainersPerProc);
			_synonymsCount[LHS.getName()] = min(original, estimated);
		}
	}
}

void StatisticsTable::reduceCountFollows(Synonym LHS, Synonym RHS, DIRECTION direction)
{
	if (LHS.isConstant() && RHS.isSynonym()) {
		_synonymsCount[RHS.getName()] = 1;
	} else if (RHS.isConstant() && LHS.isSynonym()) {
		_synonymsCount[LHS.getName()] = 1;
	} else {
		if (LHS.isUndefined() || RHS.isUndefined()) {
			return;
		}
		//Both are synonyms
		if (direction == LeftToRight) {
			double original = _synonymsCount[RHS.getName()];
			double estimated = ceil(_synonymsCount[LHS.getName()] * 0.75);
			_synonymsCount[RHS.getName()] = min(original, estimated);
		} else {
			double original = _synonymsCount[LHS.getName()];
			double estimated = ceil(_synonymsCount[RHS.getName()] * 0.75);
			_synonymsCount[LHS.getName()] = min(original, estimated);
		}
	}
}

void StatisticsTable::reduceCountFollowsS(Synonym LHS, Synonym RHS, DIRECTION direction)
{
	if (LHS.isConstant() && RHS.isSynonym()) {
		double original = _synonymsCount[RHS.getName()];
		double estimated = _averageLinesPerProc - _averageContainersPerProc;
		_synonymsCount[RHS.getName()] = min(original, estimated);
	} else if (RHS.isConstant() && LHS.isSynonym()) {
		double original = _synonymsCount[LHS.getName()];
		double estimated = _averageLinesPerProc - _averageContainersPerProc;
		_synonymsCount[LHS.getName()] = min(original, estimated);
	} else {
		if (LHS.isUndefined() || RHS.isUndefined()) {
			return;
		}
		//Both are synonyms
		if (direction == LeftToRight) {
			double original = _synonymsCount[RHS.getName()];
			double estimated = min(_stmtSize, ceil(_synonymsCount[LHS.getName()] / _averageLinesPerProc) * (_averageLinesPerProc - _averageContainersPerProc));
			_synonymsCount[RHS.getName()] = min(original, estimated);
		} else {
			double original = _synonymsCount[LHS.getName()];
			double estimated = min(_stmtSize, ceil(_synonymsCount[RHS.getName()] / _averageLinesPerProc) * (_averageLinesPerProc - _averageContainersPerProc));
			_synonymsCount[LHS.getName()] = min(original, estimated);
		}
	}
}

void StatisticsTable::reduceCountCalls(Synonym LHS, Synonym RHS, DIRECTION direction)
{
	if (LHS.isConstant() && RHS.isSynonym()) {
		_synonymsCount[RHS.getName()] = _averageCallsPerProc;
	} else if (RHS.isConstant() && LHS.isSynonym()) {
		_synonymsCount[LHS.getName()] = _averageCallsPerProc;
	} else {
		if (LHS.isUndefined() || RHS.isUndefined()) {
			return;
		}
		//Both are synonyms
		if (direction == LeftToRight) {
			double original = _synonymsCount[RHS.getName()];
			double estimated = ceil(_synonymsCount[LHS.getName()] * 0.75);
			_synonymsCount[RHS.getName()] = min(original, estimated);
		} else {
			double original = _synonymsCount[LHS.getName()];
			double estimated = ceil(_synonymsCount[RHS.getName()] * 0.75);
			_synonymsCount[LHS.getName()] = min(original, estimated);
		}
	}
}

void StatisticsTable::reduceCountCallsS(Synonym LHS, Synonym RHS, DIRECTION direction)
{
	if (LHS.isConstant() && RHS.isSynonym()) {
		double original = _synonymsCount[RHS.getName()];
		double estimated = max(_averageCallsPerProc, _callSize / 2);
		_synonymsCount[RHS.getName()] = min(original, estimated);
	} else if (RHS.isConstant() && LHS.isSynonym()) {
		double original = _synonymsCount[LHS.getName()];
		double estimated = max(_averageCallsPerProc, _callSize / 2);
		_synonymsCount[LHS.getName()] = min(original, estimated);
	} else {
		if (LHS.isUndefined() || RHS.isUndefined()) {
			return;
		}
		//Both are synonyms
		if (direction == LeftToRight) {
			double original = _synonymsCount[RHS.getName()];
			double estimated = min(_callSize, ceil(_synonymsCount[LHS.getName()] / _averageCallsPerProc) * _averageCallsPerProc);
			_synonymsCount[RHS.getName()] = min(original, estimated);
		} else {
			double original = _synonymsCount[LHS.getName()];
			double estimated = min(_callSize, ceil(_synonymsCount[RHS.getName()] / _averageCallsPerProc) * _averageCallsPerProc);
			_synonymsCount[LHS.getName()] = min(original, estimated);
		}
	}
}

void StatisticsTable::reduceCountNext(Synonym LHS, Synonym RHS, DIRECTION direction)
{
	if (LHS.isConstant() && RHS.isSynonym()) {
		_synonymsCount[RHS.getName()] = 1;
	} else if (RHS.isConstant() && LHS.isSynonym()) {
		_synonymsCount[LHS.getName()] = 1;
	} else {
		if (LHS.isUndefined() || RHS.isUndefined()) {
			return;
		}
		//Both are synonyms
		if (direction == LeftToRight) {
			double original = _synonymsCount[RHS.getName()];
			double estimated = ceil(_synonymsCount[LHS.getName()] * 0.8);
			_synonymsCount[RHS.getName()] = min(original, estimated);
		} else {
			double original = _synonymsCount[LHS.getName()];
			double estimated = ceil(_synonymsCount[RHS.getName()] * 0.8);
			_synonymsCount[LHS.getName()] = min(original, estimated);
		}
	}
}

void StatisticsTable::reduceCountNextS(Synonym LHS, Synonym RHS, DIRECTION direction)
{
	if (LHS.isConstant() && RHS.isSynonym()) {
		double original = _synonymsCount[RHS.getName()];
		double estimated = _averageLinesPerProc;
		_synonymsCount[RHS.getName()] = min(original, estimated);
	} else if (RHS.isConstant() && LHS.isSynonym()) {
		double original = _synonymsCount[LHS.getName()];
		double estimated = _averageLinesPerProc;
		_synonymsCount[LHS.getName()] = min(original, estimated);
	} else {
		if (LHS.isUndefined() || RHS.isUndefined()) {
			return;
		}
		//Both are synonyms
		if (direction == LeftToRight) {
			double original = _synonymsCount[RHS.getName()];
			double estimated = min(_stmtSize, ceil(_synonymsCount[LHS.getName()] / _averageLinesPerProc) * _averageLinesPerProc);
			_synonymsCount[RHS.getName()] = min(original, estimated);
		} else {
			double original = _synonymsCount[LHS.getName()];
			double estimated = min(_stmtSize, ceil(_synonymsCount[RHS.getName()] / _averageLinesPerProc) * _averageLinesPerProc);
			_synonymsCount[LHS.getName()] = min(original, estimated);
		}
	}
}

void StatisticsTable::reduceCountAffects(Synonym LHS, Synonym RHS, DIRECTION direction)
{
	if (LHS.isConstant() && RHS.isSynonym()) {
		_synonymsCount[RHS.getName()] = 1;
	} else if (RHS.isConstant() && LHS.isSynonym()) {
		_synonymsCount[LHS.getName()] = 1;
	} else {
		if (LHS.isUndefined() || RHS.isUndefined()) {
			return;
		}
		//Both are synonyms
		//Assume that each statement affects another
		if (direction == LeftToRight) {
			double original = _synonymsCount[RHS.getName()];
			double estimated = ceil(_synonymsCount[LHS.getName()] * 0.6);
			_synonymsCount[RHS.getName()] = min(original, estimated);
		} else {
			double original = _synonymsCount[LHS.getName()];
			double estimated = ceil(_synonymsCount[RHS.getName()] * 0.6);
			_synonymsCount[LHS.getName()] = min(original, estimated);
		}
	}
}

void StatisticsTable::reduceCountAffectsS(Synonym LHS, Synonym RHS, DIRECTION direction)
{
	if (LHS.isConstant() && RHS.isSynonym()) {
		double original = _synonymsCount[RHS.getName()];
		double estimated = _averageAssignPerProc / 2;
		_synonymsCount[RHS.getName()] = min(original, estimated);
	} else if (RHS.isConstant() && LHS.isSynonym()) {
		double original = _synonymsCount[LHS.getName()];
		double estimated = _averageAssignPerProc / 2;
		_synonymsCount[LHS.getName()] = min(original, estimated);
	} else {
		if (LHS.isUndefined() || RHS.isUndefined()) {
			return;
		}
		//Both are synonyms
		if (direction == LeftToRight) {
			double original = _synonymsCount[RHS.getName()];
			double estimated = min(_assignSize / 2, ceil(_synonymsCount[LHS.getName()] / _averageAssignPerProc) * _averageAssignPerProc / 2);
			_synonymsCount[RHS.getName()] = min(original, estimated);
		} else {
			double original = _synonymsCount[LHS.getName()];
			double estimated = min(_assignSize / 2, ceil(_synonymsCount[RHS.getName()] / _averageAssignPerProc) * _averageAssignPerProc / 2);
			_synonymsCount[LHS.getName()] = min(original, estimated);
		}
	}
}

void StatisticsTable::reduceCountPattern(Synonym LHS, Synonym RHS, DIRECTION direction)
{
	//LHS cannot be constant in pattern
	SYNONYM_TYPE typeLHS = LHS.getType();

	if (typeLHS == ASSIGN) {
		reduceCountPatternAssign(LHS, RHS);
	} else if (typeLHS == IF) {
		reduceCountPatternIf(LHS, RHS);
	} else if (typeLHS == WHILE) {
		reduceCountPatternWhile(LHS, RHS);
	}
}

void StatisticsTable::reduceCountPatternAssign(Synonym LHS, Synonym RHS)
{
	if (RHS.isSynonym()) {
		//The clause is pattern assign(variable, expression)
		double originalLHS = _synonymsCount[LHS.getName()];
		double estimatedLHS = ceil(_assignSize / 3);  //Arbitrary
		_synonymsCount[LHS.getName()] = min(originalLHS, estimatedLHS);

		double originalRHS = _synonymsCount[RHS.getName()];
		double estimatedRHS = ceil(_assignSize / 3);  //Arbitrary
		_synonymsCount[RHS.getName()] = min(originalRHS, estimatedRHS);
	} else if (RHS.isUndefined()) {
		//The clause is pattern assign(_, expression)
		double originalLHS = _synonymsCount[LHS.getName()];
		double estimatedLHS = ceil(_assignSize / 3);  //Arbitrary
		_synonymsCount[LHS.getName()] = min(originalLHS, estimatedLHS);
	} else if (RHS.isConstant()) {
		//The clause is pattern assign("variable", expression)
		double originalLHS = _synonymsCount[LHS.getName()];
		double estimatedLHS = ceil(_assignSize / 5);  //Arbitrary
		_synonymsCount[LHS.getName()] = min(originalLHS, estimatedLHS);
	}
}

void StatisticsTable::reduceCountPatternIf(Synonym LHS, Synonym RHS)
{
	if (RHS.isSynonym()) {
		//The clause is pattern if(variable, _, _)
		double originalLHS = _synonymsCount[LHS.getName()];
		double estimatedLHS = ceil(_ifSize / 3);  //Arbitrary
		_synonymsCount[LHS.getName()] = min(originalLHS, estimatedLHS);

		double originalRHS = _synonymsCount[RHS.getName()];
		double estimatedRHS = ceil(_ifSize / 3);  //Arbitrary
		_synonymsCount[RHS.getName()] = min(originalRHS, estimatedRHS);
	} else if (RHS.isUndefined()) {
		//The clause is pattern if(_, _, _)
		double originalLHS = _synonymsCount[LHS.getName()];
		double estimatedLHS = ceil(_ifSize / 3);  //Arbitrary
		_synonymsCount[LHS.getName()] = min(originalLHS, estimatedLHS);
	} else if (RHS.isConstant()) {
		//The clause is pattern if("variable", _, _)
		double originalLHS = _synonymsCount[LHS.getName()];
		double estimatedLHS = ceil(_ifSize / 5);  //Arbitrary
		_synonymsCount[LHS.getName()] = min(originalLHS, estimatedLHS);
	}
}

void StatisticsTable::reduceCountPatternWhile(Synonym LHS, Synonym RHS)
{
	if (RHS.isSynonym()) {
		//The clause is pattern while(variable, _)
		double originalLHS = _synonymsCount[LHS.getName()];
		double estimatedLHS = ceil(_whileSize / 3);  //Arbitrary
		_synonymsCount[LHS.getName()] = min(originalLHS, estimatedLHS);

		double originalRHS = _synonymsCount[RHS.getName()];
		double estimatedRHS = ceil(_whileSize / 3);  //Arbitrary
		_synonymsCount[RHS.getName()] = min(originalRHS, estimatedRHS);
	} else if (RHS.isUndefined()) {
		//The clause is pattern while(_, _)
		double originalLHS = _synonymsCount[LHS.getName()];
		double estimatedLHS = ceil(_whileSize / 3);  //Arbitrary
		_synonymsCount[LHS.getName()] = min(originalLHS, estimatedLHS);
	} else if (RHS.isConstant()) {
		//The clause is pattern while("variable", _)
		double originalLHS = _synonymsCount[LHS.getName()];
		double estimatedLHS = ceil(_whileSize / 5);  //Arbitrary
		_synonymsCount[LHS.getName()] = min(originalLHS, estimatedLHS);
	}
}

void StatisticsTable::reduceCountWith(Synonym LHS, Synonym RHS, DIRECTION direction)
{
	if (LHS.isConstant()) {
		_synonymsCount[RHS.getName()] = 1;
	} else if (RHS.isConstant()) {
		_synonymsCount[LHS.getName()] = 1;
	} else {
		//Both are synonyms
		double originalLHS = _synonymsCount[LHS.getName()];
		double originalRHS = _synonymsCount[RHS.getName()];
		double estimated = min(originalLHS, originalLHS);
		_synonymsCount[LHS.getName()] = min(originalLHS, estimated);
		_synonymsCount[RHS.getName()] = min(originalRHS, estimated);
	}
}
