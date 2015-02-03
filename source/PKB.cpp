#pragma once

#include<stdio.h>
#include <iostream>
#include <string>
#include <vector>

using namespace std;

#include "PKB.h"
#include "TNode.h"


PKB::PKB() 
{
	varTable = new VarTable();
	procTable = new ProcTable();
	stmtTable = new StmtTable();
	constantTable = new ConstantTable();
	callsTable = new CallsTable();
	modifiesTable = new ModifiesTable();
	usesTable = new UsesTable();
	followsTable = new FollowsTable();
	parentTable = new ParentTable();
	ast = new AST();
}

/**
* @return a TNode for the given design entity together with its statement number and index. 
* @exception if stmtNo is negative or 0 or index is negative.
*/
TNode* PKB::createTNode(TNODE_TYPE ast_node_type, int stmtNo, int idx) 
{
	return ast->createTNode(ast_node_type, stmtNo, idx);
}

/**
* @return the root node of the AST.
*/
TNode* PKB::getRoot() 
{
	return ast->getRoot();
}

/**
* @return TRUE if the link between the fromNode to toNode is created successfully. Otherwise, return FALSE. 
* @exception if link is invalid, or fromNode and toNode is NULL.
*/
bool PKB::createLink(LINK_TYPE link, TNode* fromNode, TNode* toNode) 
{
	return ast->createLink(link, fromNode, toNode);
}

/**
* @return the total number of children the parent TNode has. 
* @exception if parent is NULL.
*/
int PKB::getChildrenSize(TNode* parent) 
{
	return ast->getChildrenSize(parent);
}

/**
* @return the list of all the children nodes the parent TNode has.
* If there is no answer, return an empty list.
* @exception if parent is NULL.
*/
vector<TNode*>* PKB::getChildrenNode(TNode* parent) 
{
	return ast->getChildrenNode(parent);
}

/**
* @return TRUE if child TNode is a child node of parent TNode. Otherwise, return FALSE.
* @exception if parent or child is NULL.
*/
bool PKB::isChildNode(TNode* parent, TNode* child) 
{
	return ast->isChildNode(parent, child);
}

/**
* @return TRUE if node exists. Otherwise, return FALSE.
* @exception if node is NULL.
*/
bool PKB::isExists(TNode* node) 
{
	return ast->isExists(node);
}
TNode* PKB::getLastImpt() 
{
	return ast->getLastImpt();
}

/**
 * @return the total number of nodes in the the AST.
 */
int PKB::getASTSize() 
{
	return ast->getSize();
}

/**
 * Pattern matching for assign statements.
 * @return a vector of statement numbers which are assign stmts, and uses the input RHS as its right substree.
 * @param RHS to match the expression query with a suitable subtree.
 */
vector<int> PKB::patternMatchAssign(string RHS) 
{
	return ast->patternMatchAssign(RHS);
}

/**
 * Pattern matching for while statements.
 * @return a vector of statement numbers which are while loops, and uses the input LHS as its control variable.
 * @param LHS  the name of the variable that acts as the control variable for the while statements we are interested in.
 */
vector<int> PKB::patternMatchWhile(string LHS) 
{
	return ast->patternMatchWhile(LHS);
}

vector<int> PKB::patternMatchIf(string LHS) 
{
	return ast->patternMatchIf(LHS);
}

/**
 * Obtain the index of control variable of a while loop. 
 * @param stmtNum the statement number of the while loop
 * @return -1 if 1. an invalid statement number is provided.
 *     2. the statement indicated by the stmtNum is not a While statement
 *     3. the AST is poorly formed and the while loop's node is in an invalid state
 * Otherwise, return the index of the control variable.
 */
int PKB::getControlVariable(int stmtNum) 
{
	return ast->getControlVariable(stmtNum);
}

// VarTable

/**
 * If varName is not in the VarTable, inserts it into the VarTable with the
 * given statement number stmtNum and return its index. Otherwise, return its INDEX
 * and the table remains unchanged.
 * @return index of variable
 * @exception if varName is empty or stmtNum is negative or 0.
 */
int PKB::insertVar(string varName, int stmtNum) 
{
	return varTable->insertVar(varName, stmtNum);
}

/**
 * @return the total number of variables in the the VarTable.
 */
int PKB::getVarTableSize() 
{
	return varTable->getVarTableSize();
}
/**
 * @return the name of the variable in the VarTable with the given index.
 * If index is out of range, return an empty string.
 */
string PKB::getVarName(int index) 
{
	return varTable->getVarName(index);
}

/**
 * If varName is in the VarTable, return its index. Otherwise, return -1 to
 * indicate there is no such variable in the the VarTable. If varName is empty or null, -1 is
 * returned.
 * @return index of variable or -1 if varName is empty or null
 * @exception if varName is empty
 */
int PKB::getVarIndex(string varName) 
{
	return varTable->getVarIndex(varName);
}

/**
 * @return the statement number of the variable in the VarTable with the given
 * varName. Otherwise, return -1 to indicate there is no such statement number.
 * If there is no answer or if “varName” is empty or null, return an empty list.
 */
int PKB::getStmtNum(string varName) 
{
	return varTable->getStmtNum(varName);
}

/**
 * Return all the index of the variables in the the VarTable.
 * If there is no answer, return an empty list.
 */
vector<int> PKB::getAllVarIndex() 
{
	return varTable->getAllVarIndex();
}


// ProcTable

int PKB::insertProc(string procName) 
{
	return procTable->insertProc(procName);
}
int PKB::getProcTableSize() 
{
	return procTable->getProcTableSize();;
}
string PKB::getProcName(int index) 
{
	return procTable->getProcName(index);
}
int PKB::getProcIndex(string procName) 
{
	return procTable->getProcIndex(procName);
}

//Constant
/**
 * If constant is not in the ConstantTable, inserts it into the ConstantTable with the
 * given statement number stmtNum.
 * @return TRUE if constantTable got updated.
 * @exception if constant is empty or stmtNum is negative or 0.
 */
bool PKB::insertConstant(int constant, int stmtNum) 
{
	return constantTable->insertConstant(constant, stmtNum);
}

int PKB::getConstantTableSize() 
{
	return constantTable->getSize();
}
/**
 * @return the constant in the ConstantTable with the given index.
 * If index is out of range, return an empty string.
 */
int PKB::getConstant(int index) 
{
	return constantTable->getConstant(index);
}
/**
* @return the index of a constant in ConstantTable using the constant as the key.
*/
int PKB::getConstantIndex(int constant) 
{
	return constantTable->getConstantIndex(constant);
}
/**
* @return the statement number the constant is in the ConstantTable.
*/
vector<int> PKB::getStmtNum(int constant) 
{
	return constantTable->getStmtNum(constant);
}
/**
* Check if it is a constant in the ConstantTable.
* @return TRUE if number is a constant in the ConstantTable, otherwise FALSE
*/
bool PKB::isConstant(int number) 
{
	return constantTable->isConstant(number);
}

/**
* @return the list of all the constant in the ConstantTable.
*/
vector<int> PKB::getAllConstant() 
{
	return constantTable->getAllConstant();
}

// StmtTable
/**
* @return TRUE if the StmtTable is updated accordingly. Otherwise, return FALSE. 
* If stmtNum and type are already present in the StmtTable and are previously set, the StmtTable will not be updated.
* @exception if stmtNum is negative or 0, or type is not while/assign/if.
*/
bool PKB::insertStmt(int stmtNum, string type) 
{
	return stmtTable->insertStmt(stmtNum, type);
}
/**
 * @return the statement type in the StmtTable with the given statement number.
 * If stmtNum is out of range, return an empty string.
 * @exception if stmtNum is negative or 0.
 */
string PKB::getType(int stmtNum) 
{
	return stmtTable->getType(stmtNum);
}
/**
 * @return all the statement number of the statement type in the the StmtTable. 
 * If there is no answer or if type is an invalid STATEMENT_TYPE, return an empty list.
*/
vector<int> PKB::getStmtNumForType(string type) 
{
	return stmtTable->getStmtNumForType(type);
}
/**
* @return TRUE if stmtNo is of Assignment Type. Otherwise, return FALSE. 
* If stmtNo is out of range, return FALSE.
*/
bool PKB::isAssign(int stmtNum) 
{
	return stmtTable->isAssign(stmtNum);
}
/**
* @return TRUE if stmtNo is of While Type. Otherwise, return FALSE. 
* If stmtNo is out of range, return FALSE.
*/
bool PKB::isWhile(int stmtNum) 
{
	return stmtTable->isWhile(stmtNum);
}

/**
* @return TRUE if stmtNo is of If Type. Otherwise, return FALSE. 
* If stmtNo is out of range, return FALSE.
*/
bool PKB::isIf(int stmtNum) 
{
	return stmtTable->isIf(stmtNum);
}

/**
* @return TRUE if stmtNo is of Call Type. Otherwise, return FALSE. 
* If stmtNo is out of range, return FALSE.
*/
bool PKB::isCall(int stmtNum) 
{
	return stmtTable->isCall(stmtNum);
}


/**
 * @return the total number of statements in the the StmtTable.
 */
int PKB::getStmtTableSize() 
{
	return stmtTable->getSize();
}

// Parent
bool PKB::setParent(TNode* stmtNum1, TNode* stmtNum2) 
{
	return parentTable->setParent(stmtNum1, stmtNum2);
}
	
vector<int> PKB::getParent(int stmtNum2, bool transitiveClosure) 
{
	return parentTable->getParent(stmtNum2, transitiveClosure);
}
vector<int> PKB::getParentS(int stmtNum2) 
{
	bool transitiveClosure = true;
	return parentTable->getParent(stmtNum2, transitiveClosure);
}
vector<int> PKB::getChild(int stmtNum1, bool transitiveClosure) 
{
	return parentTable->getChild(stmtNum1, transitiveClosure);
}
vector<int> PKB::getChildS(int stmtNum1) 
{
	bool transitiveClosure = true;
	return parentTable->getChild(stmtNum1, transitiveClosure);
}

bool PKB::isParent(int stmtNum1, int stmtNum2, bool transitiveClosure) 
{
	return parentTable->isParent(stmtNum1, stmtNum2, transitiveClosure);
}
bool PKB::isParentS(int stmtNum1, int stmtNum2) 
{
	bool transitiveClosure = true;
	return parentTable->isParent(stmtNum1, stmtNum2, transitiveClosure);
}

pair<vector<int>, vector<int>> PKB::getAllParentPairs(bool transitiveClosure) 
{
	return parentTable->getAllParentPairs(transitiveClosure);
}
pair<vector<int>, vector<int>> PKB::getAllParentPairsS() 
{
	bool transitiveClosure = true;
	return parentTable->getAllParentPairs(transitiveClosure);
}


bool PKB::setFollows(TNode* stmt1, TNode* stmt2) 
{
	return followsTable->setFollows(stmt1, stmt2);
}	
vector<int> PKB::getStmtFollowedTo(int stmtNum2, bool transitiveClosure) 
{
	return followsTable->getStmtFollowedTo(stmtNum2, transitiveClosure);
}
vector<int> PKB::getStmtFollowedToS(int stmtNum2) 
{
	bool transitiveClosure = true;
	return followsTable->getStmtFollowedTo(stmtNum2, transitiveClosure);
}
vector<int> PKB::getStmtFollowedFrom(int stmtNum1 , bool transitiveClosure) 
{
	return followsTable->getStmtFollowedFrom(stmtNum1, transitiveClosure);
}
vector<int> PKB::getStmtFollowedFromS(int stmtNum1) 
{
	bool transitiveClosure = true;
	return followsTable->getStmtFollowedFrom(stmtNum1, transitiveClosure);
}
bool PKB::isFollows(int stmtNum1, int stmtNum2, bool transitiveClosure) 
{
	return followsTable->isFollows(stmtNum1, stmtNum2, transitiveClosure);
}
bool PKB::isFollowsS(int stmtNum1, int stmtNum2) 
{
	bool transitiveClosure = true;
	return followsTable->isFollows(stmtNum1, stmtNum2, transitiveClosure);
}
pair<vector<int>, vector<int>> PKB::getAllFollowsPairs(bool transitiveClosure) 
{
	return followsTable->getAllFollowsPairs(transitiveClosure);
}


// CallsTable methods
bool PKB::setCalls(int procIndex1, int procIndex2)
{
	return callsTable->setCalls(procIndex1, procIndex2);
}
bool PKB::isCalls(int procIndex1, int procIndex2, bool transitiveClosure) 
{
	return callsTable->isCalls(procIndex1, procIndex2, transitiveClosure);;
}
vector<int> PKB::getProcsCalling(int procIndex2, bool transitiveClosure) 
{
	return callsTable->getProcsCalling(procIndex2, transitiveClosure);
}
vector<int> PKB::getProcsCalledBy(int procIndex1, bool transitiveClosure) 
{
	return callsTable->getProcsCalledBy(procIndex1, transitiveClosure);
}
pair<vector<int>, vector<int>> PKB::getAllCallsPairs(bool transitiveClosure) 
{
	return callsTable->getAllCallsPairs(transitiveClosure);
}


// ModifiesTable methods
bool PKB::setModifies(int stmtNum, int varIndex) 
{
	return modifiesTable->setModifies(stmtNum, varIndex);
}
bool PKB::isModifies(int stmtNum, int varIndex) 
{
	return modifiesTable->isModifies(stmtNum, varIndex);
}
vector<int> PKB::getModStmtNum(int varIndex) 
{
	return modifiesTable->getModStmtNum(varIndex);
}
vector<int> PKB::getModVarForStmt(int stmtNum) 
{
	return modifiesTable->getModVarForStmt(stmtNum);
}
pair<vector<int>, vector<int>> PKB::getAllModPair() 
{
	return modifiesTable->getAllModPair();
}

bool PKB::setModifiesProc(int procIndex, int varIndex) 
{
	return modifiesTable->setModifiesProc(procIndex, varIndex);
}
bool PKB::isModifiesProc(int procIndex, int varIndex) 
{
	return modifiesTable->isModifiesProc(procIndex, varIndex);
}
vector<int> PKB::getModProcIndex(int varIndex) 
{
	return modifiesTable->getModProcIndex(varIndex);
}
vector<int> PKB::getModVarForProc(int procIndex) 
{
	return modifiesTable->getModVarForProc(procIndex);
}
pair<vector<int>, vector<int>> PKB::getAllModProcPair() 
{
	return modifiesTable->getAllModProcPair();
}


// UsesTable methods
bool PKB::setUses(int stmtNum, int varIndex) 
{
	return usesTable->setUses(stmtNum, varIndex);
}
bool PKB::isUses(int stmtNum, int varIndex) 
{
	return usesTable->isUses(stmtNum, varIndex);
}
vector<int> PKB::getUsesStmtNum(int varIndex) 
{
	return usesTable->getUsesStmtNum(varIndex);
}
vector<int> PKB::getUsesVarForStmt(int stmtNum) 
{
	return usesTable->getUsesVarForStmt(stmtNum);
}
pair<vector<int>, vector<int>> PKB::getAllUsesPair() 
{
	return usesTable->getAllUsesPair();
}

bool PKB::setUsesProc(int procIndex, int varIndex) 
{
	return usesTable->setUsesProc(procIndex, varIndex);
}
bool PKB::isUsesProc(int procIndex, int varIndex) 
{
	return usesTable->isUsesProc(procIndex, varIndex);
}
vector<int> PKB::getUsesProcIndex(int varIndex) 
{
	return usesTable->getUsesProcIndex(varIndex);
}
vector<int> PKB::getUsesVarForProc(int procIndex) 
{
	return usesTable->getUsesVarForProc(procIndex);
}
pair<vector<int>, vector<int>> PKB::getAllUsesProcPair() 
{
	return usesTable->getAllUsesProcPair();
}


/*
** Singleton implementation: for Query Evaluator to get an instance of PKB
*/
PKB& PKB::getInstance() 
{
	static PKB instance;
	return instance;
}
