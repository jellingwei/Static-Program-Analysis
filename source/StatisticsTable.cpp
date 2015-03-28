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

	// Follows(s,s)
	int followsS_S = (totalNumberOfStmts - 1 - numberOfIf - numberOfWhile - numberOfProcedure) * 2;

	// Follows(s,ANY) Follows(ANY,s)
	int followsANY = 1;

	// Follows*

	// TODO Follows*(s,s) s^2
	int followsSS_S = 0;

	//Follows*(a,s) Follows*(s,a)



	// TODO Follows*("#",s) Follows*(s,"#")
	int n = 0; // n = #
	int followsSANY = (totalNumberOfStmts - n)/totalNumberOfStmts * followsSS_S;
}

void StatisticsTable::tabulateParent()
{
	PKB pkb = PKB::getInstance();
	vector<int> parent = pkb.getParentLhs();
	vector<int> child = pkb.getParentRhs();

	// Parent
	// Parent(s,s)
	int parentS_S = parent.size() + child.size();

	//Parent(s,"#")
	int parentS_ANY = 1;

	// Parent("#", s)
	
	int parentWithMostChild = 0;
	for(vector<int>::iterator it = parent.begin(); it != parent.end(); ++it) {
		if(*it > parentWithMostChild) 
			parentWithMostChild = *it;
	}
	int parentANY_S = parentWithMostChild;


	//Parent*
	//Parent*(s,s)
	int parentSS_S = parentS_S;

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
}

void StatisticsTable::tabulateCalls()
{
	PKB pkb = PKB::getInstance();
	int numberOfProcedure = pkb.getProcTableSize();

	//TODO - compute callsSWeight

}

void StatisticsTable::tabulateNext()
{
	PKB pkb = PKB::getInstance();
	int numberOfProcedure = pkb.getProcTableSize();

	//TODO - compute nextSWeight: find the procedure with the greatest number of program lines. 
}

void StatisticsTable::tabulatePattern()
{
	//TODO - compute patternWeight: find the largest possible number of RHS operators/variables in an assignment stmt. 
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