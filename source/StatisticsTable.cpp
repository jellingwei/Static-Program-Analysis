#pragma once

#include "StatisticsTable.h"

//TODO think of storing methods
void tabulateWeights();
void tabulateFollows();
void tabulateParent();
void tabulateModifies();
void tabulateUses();

StatisticsTable::StatisticsTable()
{
	tabulateWeights();
}

// TODO return weights to query evaluator
int StatisticsTable::getWeightsForRelation(string relation, string LHS, string RHS)
{
	return 0;
}

void tabulateWeights()
{
	tabulateFollows();
	tabulateParent();
	tabulateModifies();
	tabulateUses();
}

void tabulateFollows()
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

void tabulateParent()
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


void tabulateModifies()
{
	PKB pkb = PKB::getInstance();

	vector<int> stmtsThatModifies = pkb.getModifiesLhs();
	vector<int> variableThatIsModified = pkb.getModifiesRhs();

	// Modifies(s,v)
	int modifiesS_V = stmtsThatModifies.size() + variableThatIsModified.size();

	// Modifies(a,v)
	int modifiesA_V = pkb.getStmtNumForType("assign").size() + variableThatIsModified.size(); 

	// Modifies(if,v)
	vector<int> if_stmts = pkb.getStmtNumForType("if");
	int mostVariablesModifiedByIf = 0;
	for(vector<int>::iterator it = if_stmts.begin(); it != if_stmts.end(); ++it) {
		if(pkb.getModVarForStmt(*it).size() > mostVariablesModifiedByIf) 
			mostVariablesModifiedByIf = *it;
	}
	int modifiesIF_V = if_stmts.size() + mostVariablesModifiedByIf;

	// Modifies(while,v)
	vector<int> while_stmts = pkb.getStmtNumForType("while");
	int mostVariablesModifiedByWhile = 0;
	for(vector<int>::iterator it = while_stmts.begin(); it != while_stmts.end(); ++it) {
		if(pkb.getModVarForStmt(*it).size() > mostVariablesModifiedByWhile) 
			mostVariablesModifiedByWhile = *it;
	}
	int modifiesW_V = while_stmts.size() + mostVariablesModifiedByWhile;

	// Modifies(proc,v) & (TODO)Modifies(call,v) 
	vector<int> procedures = pkb.getAllProcIndex();
	int mostVariablesModifiedByProcedure = 0;
	for(vector<int>::iterator it = procedures.begin(); it != procedures.end(); ++it) {
		if(pkb.getModVarForProc(*it).size() > mostVariablesModifiedByProcedure) 
			mostVariablesModifiedByProcedure = *it;
	}
	int modifiesP_V = procedures.size() + mostVariablesModifiedByProcedure;
}

void tabulateUses()
{
	PKB pkb = PKB::getInstance();


}