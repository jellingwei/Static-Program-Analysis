#pragma once

#include "StatisticsTable.h"

StatisticsTable::StatisticsTable()
{
	initWeights();
	tabulateWeights();
}
StatisticsTable::~StatisticsTable() {
}

void StatisticsTable::initWeights()
{	
	affectsWeight = affectsSWeight = followsSWeight = 1;
	parentSWeight = callsSWeight = nextSWeight = 1;
	patternWeight = 1;
}
void StatisticsTable::tabulateWeights()
{
	tabulateAffects();
	tabulateFollows();
	tabulateParent();
	tabulateCalls();
	tabulateNext();
	tabulatePattern();

	//tabulateModifies();
	//tabulateUses();
}
void StatisticsTable::tabulateAffects()
{

}
void StatisticsTable::tabulateFollows()
{
	PKB pkb = PKB::getInstance();

	// Follows
	int totalNumberOfStmts = pkb.getStmtTableSize();
	int numberOfIf = pkb.getStmtNumForType("if").size();
	int numberOfWhile = pkb.getStmtNumForType("while").size();
	int numberOfProcedure = pkb.getProcTableSize();

	/*TODO: The worst case would depend on the length of the longest stmtList. */

}

void StatisticsTable::tabulateParent()
{
	PKB pkb = PKB::getInstance();
	vector<int> parent = pkb.getParentLhs();
	vector<int> child = pkb.getParentRhs();

	//Parent*
	//Parent*(s,s)
	int parentSS_S = parent.size() + child.size();

	//Parent*(s,"#")
	int childWithMostAncestors = 0;
	for(vector<int>::iterator it = child.begin(); it != child.end(); ++it) {
		if(pkb.getParentS(*it).size() > childWithMostAncestors) 
			childWithMostAncestors = *it;
	}
	int parentSS_ANY = childWithMostAncestors;

	//Parent*("#",s)
	int parentWithMostDescendants = 0;
	for(vector<int>::iterator it = parent.begin(); it != parent.end(); ++it) {
		if(pkb.getChildS(*it).size() > parentWithMostDescendants) 
			parentWithMostDescendants = *it;
	}
	int parentSANY_S = parentWithMostDescendants;

	/* Take the average of the 3 possible worst case scenerio */
	int parentSCost = (1/3) * (parentSS_S + parentSS_ANY + parentSANY_S);
}

void StatisticsTable::tabulateCalls()
{
	PKB pkb = PKB::getInstance();
	int numberOfProcedure = pkb.getProcTableSize();

	//TODO - compute callsSWeight
	/* To compute Calls*("#",p2) all possible p2 is computed from a given procedure
	   It's done using double-ended queue and DFS to get all p2, and each procedure 
	   would only be visited once. Therefore getting all p1, or all p2 is O(V), 
	   where V is the total number of procedures.

	   This is also similar for get all pairs <p1,p2> Calls*(p1,p2). */


	/* Take the average of the 3 possible worst case scenerio */
	int callSCost = (1/3) * (numberOfProcedure + numberOfProcedure + numberOfProcedure);
}

void StatisticsTable::tabulateNext()
{
	PKB pkb = PKB::getInstance();
	int numberOfProcedure = pkb.getProcTableSize();

	CFG* cfg = pkb.cfgTable.at(0);
	int cfgSize = cfg->getInsideSize(cfg->getProcRoot());

	//TODO - compute nextSWeight: 
	/* The worst case is when Next*(1, k) where k the the last prog_line in the procedure. */
	int nextSCost = cfgSize;
}

void StatisticsTable::tabulatePattern()
{
	//TODO - compute patternWeight: find the largest possible number of RHS operators/variables in an assignment stmt. 
	/* Patterns match assign - builts a vector<string> and calls ExpressionParser to build the sub-expression tree. */

}

/** Getters **/
int StatisticsTable::getAffectsCost()
{
	return affectsWeight;
}
int StatisticsTable::getAffectsSCost()
{
	return affectsSWeight;
}
int StatisticsTable::getFollowsSCost()
{
	return followsSWeight;
}
int StatisticsTable::getParentSCost()
{
	return parentSWeight;
}
int StatisticsTable::getCallsSCost()
{
	return callsSWeight;
}
int StatisticsTable::getNextSCost()
{
	return nextSWeight;
}
int StatisticsTable::getPatternCost()
{
	return patternWeight;
}