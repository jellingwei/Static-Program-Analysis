#pragma once

#include<stdio.h>
#include <iostream>
#include <string>
#include <vector>

using namespace std;

#include "PKB.h"
#include "TNode.h"
#include "Synonym.h"


PKB::PKB() {
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
	nextTable = new NextTable();
	affectsTable = new AffectsTable();

	nextBipTable = new NextBipTable();
	affectsBipTable = new AffectsBipTable();
}

//AST

/**
* @param astNodeType  the node type which can be Procedure, Assign, Plus, Variable, StmtLst, While, If, Constant, Minus, Times, Program, Call
* @param stmtNum  the statement number of the TNode
* @param value  the value of the TNode
* @return a TNode for the given design entity together with its statement number and value. 
* @exception exception if stmtNum is negative or 0 or value is negative.
*/
TNode* PKB::createTNode(TNODE_TYPE astNodeType, STATEMENT stmtNum, VAR_INDEX value) 
{
	return ast->createTNode(astNodeType, stmtNum, value);
}

/**
* @return the root node of the AST.
*/
TNode* PKB::getRoot() 
{
	return ast->getRoot();
}

/**
* @param link  the relationship which can be Parent, Right_Sibling, Left_Sibling, Child
* @param fromNode  the node that connects to another node
* @param toNode  the node that receives a connection from another node
* @return TRUE if the link between the fromNode to toNode is created successfully. 
*		  FALSE if the link between the fromNode to toNode is not created successfully.
* @exception exception if link is invalid, or fromNode and toNode is NULL.
*/
STATUS PKB::createLink(LINK_TYPE link, TNode* fromNode, TNode* toNode) 
{
	return ast->createLink(link, fromNode, toNode);
}

/**
* @param parent  a node which may or may not have children nodes
* @return the total number of children the parent TNode has. 
* @exception exception if parent is NULL.
*/
INTEGER PKB::getChildrenSize(TNode* parent) 
{
	return ast->getChildrenSize(parent);
}

/**
* @param parent  a node which may or may not have children nodes
* @return the list of all the children nodes the parent TNode has.
* If there are no children nodes, return an empty list.
* @exception exception if parent is NULL.
*/
TNODE_LIST PKB::getChildrenNode(TNode* parent) 
{
	return ast->getChildrenNode(parent);
}

/**
* @param parent  a node which may or may not have the 'child' node
* @param child  a node whose parent node may or may not be the 'parent' node 
* @return TRUE if child TNode is a child node of parent TNode. 
*		  FALSE if child TNode is not a child node of parent TNode.
* @exception exception if parent or child is NULL.
*/
BOOLEAN_ PKB::isChildNode(TNode* parent, TNode* child) 
{
	return ast->isChildNode(parent, child);
}

/**
* @param node  an AST node
* @return TRUE if node exists. 
		  FALSE if node does not exist.
* @exception exception if node is NULL.
*/
BOOLEAN_ PKB::isExists(TNode* node) 
{
	return ast->isExists(node);
}

/**
 * @return the total number of nodes in the the AST.
 */
INTEGER PKB::getASTSize() 
{
	return ast->getSize();
}

/**
 * Pattern matching for assign statements.
 * @param RHS  the expression query with a suitable subtree
 * @return a vector of statement numbers which are assign stmts, and uses the input RHS as its right substree.
 */
STATEMENT_LIST PKB::patternMatchAssign(EXPRESSION RHS) 
{
	return ast->patternMatchAssign(RHS);
}

/**
 * Pattern matching for while statements.
 * @param LHS  the name of the variable that acts as the control variable for the while statements we are interested in
 * @return a vector of statement numbers which are while statements, and uses the input LHS as its control variable.
 */
STATEMENT_LIST PKB::patternMatchWhile(VARNAME LHS) 
{
	return ast->patternMatchWhile(LHS);
}

/**
 * Pattern matching for if statements.
 * @param LHS  the name of the variable that acts as the control variable for the if statements we are interested in
 * @return a vector of statement numbers which are in if statements, and uses the input LHS as its control variable.
 */
STATEMENT_LIST PKB::patternMatchIf(VARNAME LHS) 
{
	return ast->patternMatchIf(LHS);
}

/**
 * Obtain the index of control variable of a while loop. 
 * @param stmtNum  the statement number of the while loop
 * @return -1 if any of the following conditions hold: 
 *	   1. an invalid statement number is provided
 *     2. the statement indicated by the stmtNum is not a While statement
 *     3. the AST is poorly formed and the while loop's node is in an invalid state
 * Otherwise, return the index of the control variable.
 */
VAR_INDEX PKB::getControlVariable(STATEMENT stmtNum) 
{
	return ast->getControlVariable(stmtNum);
}

// VarTable

/**
 * If varName is not in the VarTable, inserts it into the VarTable with the
 * given statement number stmtNum and return its index. Otherwise, return its INDEX
 * and the table remains unchanged.
 * @param varName  the name of the variable
 * @param stmtNum  the statement number the variable belongs to
 * @return index of variable.
 * @exception exception if varName is empty or stmtNum is negative or 0.
 */
VAR_INDEX PKB::insertVar(VARNAME varName, STATEMENT stmtNum) 
{
	return varTable->insertVar(varName, stmtNum);
}

/**
 * @return the total number of variables in the the VarTable.
 */
INTEGER PKB::getVarTableSize() 
{
	return varTable->getVarTableSize();
}

/**
 * @param index  the index of the variable
 * @return the name of the variable in the VarTable with the given index.
 * If index is out of range, return an empty string.
 */
VARNAME PKB::getVarName(VAR_INDEX index) 
{
	return varTable->getVarName(index);
}

/**
 * If varName is in the VarTable, return its index. Otherwise, return -1 to
 * indicate there is no such variable in the the VarTable or if the varName is empty or null
 * @param varName  the name of the variable
 * @return index of variable or -1 if the following conditions hold:
 *			1. varName is empty or null
 *			2. there is no such variable in the the VarTable
 * @exception exception if varName is empty
 */
VAR_INDEX PKB::getVarIndex(VARNAME varName) 
{
	return varTable->getVarIndex(varName);
}

/**
 * @return all the index of the variables in the the VarTable.
 * If there is no answer, return an empty list.
 */
VARINDEX_LIST PKB::getAllVarIndex() 
{
	return varTable->getAllVarIndex();
}


// ProcTable

/**
 * If procName is not in the ProcTable, inserts it into the ProcTable and 
 * return its index. Otherwise, return its INDEX and the table remains unchanged.
 * @param procName  the name of the procedure
 * @return index of procedure
 * @exception exception if procName is empty.
 */
PROC_INDEX PKB::insertProc(PROCNAME procName) 
{
	return procTable->insertProc(procName);
}

/**
 * @return the total number of procedures in the the ProcTable.
 */
INTEGER PKB::getProcTableSize() 
{
	return procTable->getProcTableSize();;
}

/**
 * @param index  the index of the procedure
 * @return the name of the procedure in the ProcTable with the given index.
 * If index is out of range, return an empty string.
 */
PROCNAME PKB::getProcName(PROC_INDEX index) 
{
	return procTable->getProcName(index);
}

/**
 * If procName is in the ProcTable, return its index. Otherwise, return -1 to
 * indicate there is no such procedure in the the ProcTable or if procName is empty or null.
 * @return index of procedure or -1 if the following conditions hold:
 *			1. procName is empty or null
 *			2. there is no such procedure in the the ProcTable
 * @exception exception if procName is empty
 */
PROC_INDEX PKB::getProcIndex(PROCNAME procName) 
{
	return procTable->getProcIndex(procName);
}

/**
 * @return all the index of the variables in the the ProcTable.
 * If there is no answer, return an empty list.
 */
PROCINDEX_LIST PKB::getAllProcIndex() 
{
	return procTable->getAllProcIndex() ;
}

//Constant

/**
 * If constant is not in the ConstantTable, inserts it into the ConstantTable with the
 * given statement number stmtNum.
 * @param constant  a number that appears in the program
 * @param stmtNum  the statement number the constant belongs to
 * @return TRUE if constantTable is updated successfully.
 *		   FALSE  if constantTable is not updated successfully.
 * @exception exception if constant is empty or stmtNum is negative or 0.
 */
STATUS PKB::insertConstant(VALUE constant, STATEMENT stmtNum) 
{
	return constantTable->insertConstant(constant, stmtNum);
}

/**
 * @return the total number of constants in the the ConstantTable.
 */
INTEGER PKB::getConstantTableSize() 
{
	return constantTable->getSize();
}

/**
* @param constant  a number that appears in the program
* @return the statement number the constant is in the ConstantTable.
* @exception exception if constant is negative.
*/
STATEMENT_LIST PKB::getStmtNum(VALUE constant) 
{
	return constantTable->getStmtNum(constant);
}


/**
* Check if the number is a constant in the ConstantTable.
* @return TRUE if number is a constant in the ConstantTable.
*		  FALSE if number is not a constant in the ConstantTable.
*/
BOOLEAN_ PKB::isConstant(INTEGER number) 
{
	return constantTable->isConstant(number);
}

/**
* @return the list of all the constant in the ConstantTable.
*/
CONSTANT_LIST PKB::getAllConstant() 
{
	return constantTable->getAllConstant();
}

// StmtTable

/**
 * If stmtNum is not in the StmtTable, inserts it into the StmtTable with the
 * given type.
 * @param stmtNum  the statement number 
 * @param type  the type of statement which can be While, Assign, If, Call
 * @param node  the TNode in the ast that corresponds to this statement
 * @param procIndex  the index of the procedure that this statement is in
 * @return TRUE if StmtTable is updated successfully. 
		   FALSE if StmtTable is not updated successfully.
 * @exception exception if stmtNum is negative or 0, or type is not while/assign/if/call.
 */
STATUS PKB::insertStmt(STATEMENT stmtNum, STATEMENT_TYPE type, TNode* node, PROC_INDEX procIndex) 
{
	return stmtTable->insertStmt(stmtNum, type, node, procIndex);
}

/**
 * Get the type of statement, which can be while/assign/if/call.
 * @param stmtNum  the statement number 
 * @return the statement type in the StmtTable with the given statement number.
 * If stmtNum is out of range, return an empty string.
 * @exception exception if stmtNum is negative or 0.
 */
STATEMENT_TYPE PKB::getType(STATEMENT stmtNum) 
{
	return stmtTable->getType(stmtNum);
}

/**
 * Get all the statement numbers with the corresponding valid STATEMENT_TYPE which are while/assign/if/call.
 * @param type  the type of statement which is while/assign/if/call
 * @return all the statement number of the statement type in the the StmtTable. 
 * If there is no answer or if type is an invalid STATEMENT_TYPE, return an empty list.
 * @exception exception if type is not while/assign/if/call.
*/
STATEMENT_LIST PKB::getStmtNumForType(STATEMENT_TYPE type) 
{
	 // @todo remove the whole function
	return stmtTable->getStmtNumForType(type);
}

/**
 * Get the proc index that the statement is in
 * @param stmtNo  the statement number
 * @return the procIndex that the statement is in
 */
PROC_INDEX PKB::getProcIndexForStmt(STATEMENT stmtNum) {
	return stmtTable->getProcIndexForStmt(stmtNum);
}

/**
 * Get all the statement numbers with the corresponding valid STATEMENT_TYPE which are while/assign/if/call.
 * @param type  the type of statement which is while/assign/if/call
 * @return all the statement number of the statement type in the the StmtTable. 
 * If there is no answer or if type is an invalid STATEMENT_TYPE, return an empty list.
 * @exception exception if type is not while/assign/if/call.
*/
STATEMENT_LIST PKB::getStmtNumForType(SYNONYM_TYPE type) 
{
	return stmtTable->getStmtNumForType(Synonym::convertToString(type));
}

/**
* Check if the statment number is an Assign statement.
* @param stmtNum  the statement number 
* @return TRUE if stmtNum is of Assignment Type. 
*		  FALSE if stmtNum is not of Assignment Type or stmtNum is out of range.
*/
BOOLEAN_ PKB::isAssign(STATEMENT stmtNum) 
{
	return stmtTable->isAssign(stmtNum);
}

/**
* Check if the statment number is an While statement.
* @param stmtNum  the statement number 
* @return TRUE if stmtNum is of While Type. 
*		  FALSE if stmtNum is not of While Type or stmtNum is out of range.
*/
BOOLEAN_ PKB::isWhile(STATEMENT stmtNum) {
	return stmtTable->isWhile(stmtNum);
}

/**
* Check if the statment number is an If statement.
* @param stmtNum  the statement number 
* @return TRUE if stmtNum is of If Type. 
*		  FALSE if stmtNum is not of If Type or stmtNum is out of range.
*/
BOOLEAN_ PKB::isIf(STATEMENT stmtNum) {
	return stmtTable->isIf(stmtNum);
}

/**
* Check if the statment number is an Call statement.
* @param stmtNum  the statement number 
* @return TRUE if stmtNum is of Call Type. 
*		  FALSE if stmtNum is not of Call Type or stmtNum is out of range.
*/
BOOLEAN_ PKB::isCall(STATEMENT stmtNum) {
	return stmtTable->isCall(stmtNum);
}

/**
 * @return the total number of statements in the the StmtTable.
 */
INTEGER PKB::getStmtTableSize() {
	return stmtTable->getSize();
}

/**
 * @return the TNode in the AST for the statement number
 * @param stmtNum  the statement number to retrieve the TNode for
 */
TNode* PKB::getNodeForStmt(STATEMENT stmtNum) {
	return stmtTable->getNodeForStmt(stmtNum);
}

// Parent

/**
* Sets the parent relation.
* @param stmtNum1  the statement number of the parent node
* @param stmtNum2  the statement number of the child node
* @return TRUE if the AST is updated accordingly. 
*		  FALSE if the AST is not updated accordingly. 
* If stmtNum1 and stmtNum2 were already previously set, the AST will not be updated.
* @exception exception if stmtNum1 or stmtNum2 is NULL.
*/
STATUS PKB::setParent(TNode* stmtNum1, TNode* stmtNum2) {
	return parentTable->setParent(stmtNum1, stmtNum2);
}

/**
* @param stmtNum2  the statement number of the child
* @param transitiveClosure  a flag to indicate the computation of Parent or Parent* relation
* @return if transitiveClosure is false : a list of statement numbers of the parent of stmtNum2, based on the Parent relation.
*		  if transitiveClosure is true: a list of statement numbers of the ancestors of stmtNum2, based on the Parent* relation.
*		  if stmtNum2 is out of range, an empty list will be returned.
*/
STATEMENT_LIST PKB::getParent(STATEMENT stmtNum2, TRANS_CLOSURE transitiveClosure) {
	return parentTable->getParent(stmtNum2, transitiveClosure);
}

/**
* @param stmtNum2  the statement number of the child
* @return a list of statement numbers of the ancestors of stmtNum2, based on the Parent* relation.
* 		  if stmtNum2 is out of range, an empty list will be returned.
*/
STATEMENT_LIST PKB::getParentS(STATEMENT stmtNum2) {
	bool transitiveClosure = true;
	return parentTable->getParent(stmtNum2, transitiveClosure);
}

/**
* @param stmtNum1  the statement number of the parent
* @param transitiveClosure  a flag to indicate the computation of Parent or Parent* relation
* @return if transitiveClosure is false : a list of statement numbers of the child of stmtNum1, based on the Parent relation.
*		  if transitiveClosure is true: a list of statement numbers of the descendants of stmtNum1, based on the Parent* relation.
*		  if stmtNum1 is out of range, an empty list will be returned.
*/
STATEMENT_LIST PKB::getChild(STATEMENT stmtNum1, TRANS_CLOSURE transitiveClosure) {
	return parentTable->getChild(stmtNum1, transitiveClosure);
}

/**
* @param stmtNum1  the statement number of the parent
* @return a list of statement numbers of the descendants of stmtNum1, based on the Parent* relation.
*		  if stmtNum1 is out of range, an empty list will be returned.
*/
STATEMENT_LIST PKB::getChildS(STATEMENT stmtNum1) {
	bool transitiveClosure = true;
	return parentTable->getChild(stmtNum1, transitiveClosure);
}

/**
 * @param stmtNum1  the statement number of the parent
 * @param stmtNum2  the statement number of the child
 * @param transitiveClosure  a flag to indicate the computation of Parent or Parent* relation
 * @return if transitiveClosure is false: TRUE if Parent(stmtNum1, stmtNum2) holds. FALSE if Parent(stmtNum1, stmtNum2) does not hold.
 *		   if transitiveClosure is true: TRUE if Parent*(stmtNum1, stmtNum2) holds. 
 *										 FALSE if Parent*(stmtNum1, stmtNum2) does not hold.
 *		   FALSE if either stmtNum1 or stmtNum2 is invalid.
 */
BOOLEAN_ PKB::isParent(STATEMENT stmtNum1, STATEMENT stmtNum2, TRANS_CLOSURE transitiveClosure) {
	return parentTable->isParent(stmtNum1, stmtNum2, transitiveClosure);
}

/**
 * @param stmtNum1  the statement number of the parent
 * @param stmtNum2  the statement number of the child
 * @return TRUE if Parent*(stmtNum1, stmtNum2) holds. 
 *		   FALSE if Parent*(stmtNum1, stmtNum2) does not hold or either stmtNum1 or stmtNum2 is invalid.
 */
BOOLEAN_ PKB::isParentS(STATEMENT stmtNum1, STATEMENT stmtNum2) {
	bool transitiveClosure = true;
	return parentTable->isParent(stmtNum1, stmtNum2, transitiveClosure);
}

/**
* @param transitiveClosure  a flag to indicate the computation of Parent or Parent* relation
* @return if transitiveClosure is false: all pairs of stmtNum1, stmtNum2 where Parent(stmtNum1, stmtNum2) is satisfied.
*		  if transitiveClosure is true: all pairs of stmtNum1, stmtNum2 where Parent*(stmtNum1, stmtNum2) is satisfied.
*/
pair<STATEMENT_LIST, STATEMENT_LIST> PKB::getAllParentPairs(TRANS_CLOSURE transitiveClosure) {
	return parentTable->getAllParentPairs(transitiveClosure);
}

/**
* @return all pairs of stmtNum1, stmtNum2 where Parent*(stmtNum1, stmtNum2) is satisfied.
*/
pair<STATEMENT_LIST, STATEMENT_LIST> PKB::getAllParentPairsS() {
	bool transitiveClosure = true;
	return parentTable->getAllParentPairs(transitiveClosure);
}

/**
 * @return a list of all statement numbers, stmt, where Parent(stmt, _) is true.
 */
STATEMENT_LIST PKB::getParentLhs(){
	return parentTable->getAllParents();
}

/**
 * @return a list of all statement numbers, stmt, where Parent(_, stmt) is true.
 */
STATEMENT_LIST PKB::getParentRhs(){
	return parentTable->getAllChildren();
}

BOOLEAN_ PKB::isParentValid() {
	return parentTable->isValid();
}

// FollowsTable 

/**
* Sets the Follows relation.
* @param stmtNum1  the statement number of the following node
* @param stmtNum2  the statement number of the follower node
* @return TRUE if the AST is updated accordingly. 
*		  FALSE if the AST is not updated accordingly.
* If stmtNum1 and stmtNum2 were already previously set, the AST will not be updated.
* @exception exception if stmtNum1 or stmtNum2 is NULL.
*/
STATUS PKB::setFollows(TNode* stmt1, TNode* stmt2) {
	return followsTable->setFollows(stmt1, stmt2);
}	

/**
* @param stmtNum2  the statement number of the follower node
* @param transitiveClosure  a flag to indicate the computation of Follow or Follow* relation
* @return if transitiveClosure is false: a list of statement numbers, stmtNum, where Follows(stmtNum, stmtNum2) is satisfied.
*		  if transitiveClosure is true: a list of statement numbers, stmtNum, where Follows*(stmtNum, stmtNum2) is satisfied.
*		  an empty list if stmtNum2 is invalid.
*/
STATEMENT_LIST PKB::getStmtFollowedTo(STATEMENT stmtNum2, TRANS_CLOSURE transitiveClosure) {
	return followsTable->getStmtFollowedTo(stmtNum2, transitiveClosure);
}

/**
* @param stmtNum2  the statement number of the follower node
* @return a list of statement numbers, stmtNum, where Follows*(stmtNum, stmtNum2) is satisfied.
*		  an empty list if stmtNum2 is invalid.
*/
STATEMENT_LIST PKB::getStmtFollowedToS(STATEMENT stmtNum2) {
	bool transitiveClosure = true;
	return followsTable->getStmtFollowedTo(stmtNum2, transitiveClosure);
}

/**
* @param stmtNum1  the statement number of the following node
* @param transitiveClosure  a flag to indicate the computation of Follow or Follow* relation
* @return if transitiveClosure is false: a list of statement numbers, stmtNum2, where Follows(stmtNum1, stmtNum2) is satisfied.
*		  if transitiveClosure is true: a list of statement numbers, stmtNum2, where Follows*(stmtNum1, stmtNum2) is satisfied.
*		  an empty list if stmtNum1 is invalid.
*/
STATEMENT_LIST PKB::getStmtFollowedFrom(STATEMENT stmtNum1 , TRANS_CLOSURE transitiveClosure) {
	return followsTable->getStmtFollowedFrom(stmtNum1, transitiveClosure);
}

/**
* @param stmtNum1  the statement number of the following node
* @return a list of statement numbers, stmtNum2, where Follows*(stmtNum1, stmtNum2) is satisfied.
*		  an empty list if stmtNum1 is invalid.
*/
STATEMENT_LIST PKB::getStmtFollowedFromS(STATEMENT stmtNum1) {
	bool transitiveClosure = true;
	return followsTable->getStmtFollowedFrom(stmtNum1, transitiveClosure);
}

/**
 * @param stmtNum1  the statement number of the following node
 * @param stmtNum2  the statement number of the follower node
 * @param transitiveClosure  a flag to indicate the computation of Follow or Follow* relation
 * @return if transitiveClosure is false: TRUE if Follow(stmtNum1, stmtNum2) is satisfied. FALSE if Follow(stmtNum1, stmtNum2) is not satisfied.
 *		   if transitiveClosure is true: TRUE if Follow*(stmtNum1, stmtNum2) is satisfied. 
 *										 FALSE if Follow*(stmtNum1, stmtNum2) is not satisfied.
 *		   FALSE if either stmtNum1 or stmtNum2 is invalid.
 */
BOOLEAN_ PKB::isFollows(STATEMENT stmtNum1, STATEMENT stmtNum2, TRANS_CLOSURE transitiveClosure) {
	return followsTable->isFollows(stmtNum1, stmtNum2, transitiveClosure);
}
/**
* @param stmtNum1  the statement number of the following node
* @param stmtNum2  the statement number of the follower node
* @return TRUE if Follows*(stmtNum1, stmtNum2) is satisfied. 
*		  FALSE if Follows*(stmtNum1, stmtNum2) is not satisfied or either stmtNum1 or stmtNum2 is invalid.
*/
BOOLEAN_ PKB::isFollowsS(STATEMENT stmtNum1, STATEMENT stmtNum2) {
	bool transitiveClosure = true;
	return followsTable->isFollows(stmtNum1, stmtNum2, transitiveClosure);
}

/**
* @param transitiveClosure  a flag to indicate the computation of Follow or Follow* relation
* @return if transitiveClosure is false: all pairs of stmtNum1, stmtNum2 where Follow(stmtNum1, stmtNum2) is satisfied.
*		  if transitiveClosure is true: all pairs of stmtNum1, stmtNum2 where Follow*(stmtNum1, stmtNum2) is satisfied.
*/
pair<STATEMENT_LIST, STATEMENT_LIST> PKB::getAllFollowsPairs(TRANS_CLOSURE transitiveClosure) {
	return followsTable->getAllFollowsPairs(transitiveClosure);
}

/**
* @return all pairs of stmtNum1, stmtNum2 where Follows*(stmtNum1, stmtNum2) is satisfied.
*/
pair<STATEMENT_LIST, STATEMENT_LIST> PKB::getAllFollowsPairsS() {
	bool transitiveClosure = true;
	return followsTable->getAllFollowsPairs(transitiveClosure);
}

/**
 * @return a list of all statement numbers, stmt, where Follows(stmt, _) is true.
 */
STATEMENT_LIST PKB::getFollowsLhs() {
	return followsTable->getLhs();
}

/**
 * @return a list of all statement numbers, stmt, where Follows(_, stmt) is true.
 */
STATEMENT_LIST PKB::getFollowsRhs() {
	return followsTable->getRhs();
}

BOOLEAN_ PKB::isFollowsValid() {
	return followsTable->isValid();
}

// CallsTable methods

/**
* Sets the Calls relation.
* @param procIndex1  the procedure index of the caller procedure
* @param procIndex2  the procedure index of the callee procedure
* @return TRUE if the CallsTable is updated accordingly. 
*		  FALSE if the CallsTable is not updated accordingly.
* If procIndex1 and procIndex2 were already previously set, the CallsTable will not be updated.
* @exception exception if if procIndex1 or procIndex2 is negative or 0.
*/
STATUS PKB::setCalls(PROC_INDEX procIndex1, PROC_INDEX procIndex2){
	return callsTable->setCalls(procIndex1, procIndex2);
}

/**
* @param procIndex1  the procedure index of the caller procedure
* @param procIndex2  the procedure index of the callee procedure
* @param transitiveClosure  a flag to indicate the computation of Calls or Calls* relation
* @return if transitiveClosure is false: TRUE if Calls(procIndex1, procIndex2) is satisfied. FALSE if Calls(procIndex1, procIndex2) is not satisfied.
*		   if transitiveClosure is true: TRUE if Calls*(procIndex1, procIndex2) is satisfied. 
*										 FALSE if Calls*(procIndex1, procIndex2) is not satisfied.
*		   FALSE if either procIndex1 or procIndex2 is negative or 0.
*/
BOOLEAN_ PKB::isCalls(PROC_INDEX procIndex1, PROC_INDEX procIndex2, TRANS_CLOSURE transitiveClosure) {
	return callsTable->isCalls(procIndex1, procIndex2, transitiveClosure);
}

/**
* @param procIndex1  the procedure index of the caller procedure
* @param procIndex2  the procedure index of the callee procedure
* @return TRUE if Calls*(procIndex1, procIndex2) is satisfied. 
*		  FALSE if Calls*(procIndex1, procIndex2) is not satisfied or either procIndex1 or procIndex2 is negative or 0.
*/
BOOLEAN_ PKB::isCallsS(PROC_INDEX procIndex1, PROC_INDEX procIndex2) {
	bool transitiveClosure = true;
	return callsTable->isCalls(procIndex1, procIndex2, transitiveClosure);
}

/**
 * @param procIndex2  the procedure index of the callee procedure
 * @param transitiveClosure  a flag to indicate the computation of Calls or Calls* relation
 * @return if transitiveClosure is false: a list of procedure indexes, procIndex1, where Calls(procIndex1, procIndex2) is satisfied.
 *		   if transitiveClosure is true: a list of procedure indexes, procIndex1, where Calls*(procIndex1, procIndex2) is satisfied.
 *		   an empty list if procIndex2 is invalid or negative or 0.
 */
PROCINDEX_LIST PKB::getProcsCalling(PROC_INDEX procIndex2, TRANS_CLOSURE transitiveClosure) {
	return callsTable->getProcsCalling(procIndex2, transitiveClosure);
}

/**
 * @param procIndex2  the procedure index of the callee procedure
 * @return a list of procedure indexes, procIndex1, where Calls*(procIndex1, procIndex2) is satisfied.
 *		   an empty list if procIndex2 is invalid or negative or 0.
 */
PROCINDEX_LIST PKB::getProcsCallingS(PROC_INDEX procIndex2) {
	bool transitiveClosure = true;
	return callsTable->getProcsCalling(procIndex2, transitiveClosure);
}

/**
 * @param procIndex1  the procedure index of the caller procedure
 * @param transitiveClosure  a flag to indicate the computation of Calls or Calls* relation
 * @return if transitiveClosure is false: a list of procedure indexes, procIndex2, where Calls(procIndex1, procIndex2) is satisfied.
 *		   if transitiveClosure is true: a list of procedure indexes, procIndex2, where Calls*(procIndex1, procIndex2) is satisfied.
 *		   an empty list if procIndex1 is invalid or negative or 0.
 */
PROCINDEX_LIST PKB::getProcsCalledBy(PROC_INDEX procIndex1, TRANS_CLOSURE transitiveClosure) {
	return callsTable->getProcsCalledBy(procIndex1, transitiveClosure);
}

/**
 * @param procIndex1  the procedure index of the caller procedure
 * @return a list of procedure indexes, procIndex2, where Calls*(procIndex1, procIndex2) is satisfied.
 *		   an empty list if procIndex1 is invalid or negative or 0.
 */
PROCINDEX_LIST PKB::getProcsCalledByS(PROC_INDEX procIndex1) {
	bool transitiveClosure = true;
	return callsTable->getProcsCalledBy(procIndex1, transitiveClosure);
}

/**
 * @param transitiveClosure  a flag to indicate the computation of Calls or Calls* relation
 * @return if transitiveClosure is false: all pairs of procIndex1, procIndex2 where Calls(procIndex1, procIndex2) is satisfied.
 *		   if transitiveClosure is true: all pairs of procIndex1, procIndex2 where Calls*(procIndex1, procIndex2) is satisfied.
 */
pair<PROCINDEX_LIST, PROCINDEX_LIST> PKB::getAllCallsPairs(TRANS_CLOSURE transitiveClosure) {
	return callsTable->getAllCallsPairs(transitiveClosure);
}

/**
 * @return all pairs of procIndex1, procIndex2 where Calls*(procIndex1, procIndex2) is satisfied.
 */
pair<PROCINDEX_LIST, PROCINDEX_LIST> PKB::getAllCallsPairsS() 
{
	bool transitiveClosure = true;
	return callsTable->getAllCallsPairs(transitiveClosure);
}

/**
 * @param stmtNum  the statement number of the Call statement
 * @return the procedure name that is called by the statement number stmtNum.
 * @exception exception if stmtNum is negative or 0.
 */
PROCNAME PKB::getProcNameCalledByStatement(STATEMENT stmtNum) {
	return callsTable->getProcNameCalledByStatement(stmtNum);
}

/**
 * @return a list of all procedure indexes, procIndex1, where Calls(procIndex1, procIndex2) is true.
 */
PROCINDEX_LIST PKB::getCallsLhs() {
	return callsTable->getLhs();
}

/**
 * @return a list of all procedure indexes, procIndex2, where Calls(procIndex1, procIndex2) is true.
 */
PROCINDEX_LIST PKB::getCallsRhs() {
	return callsTable->getRhs();
}

BOOLEAN_ PKB::isCallsValid() {
	return callsTable->isValid();
}


// ModifiesTable methods

/**
* Initialise the Modifies Table.
* @param numVariables number of variables
* @exception exception if numVariables is negative or 0.
*/
void PKB::initModifiesTable(INTEGER numVariables) {
	if (numVariables <= 0) {
		throw runtime_error("initModifiesTable: invalid number of variables");
	}
	modifiesTable->init(numVariables);
}

/**
* Sets the Modifies relation for statements.
* @param stmtNum  the statement number that modifies a particular variable
* @param varIndex  the index of a variable
* @return TRUE if the ModifiesTable is updated accordingly. 
*		  FALSE if the ModifiesTable is not updated accordingly.
* If stmtNum and varIndex were already previously set, the ModifiesTable will not be updated.
* @exception exception if either stmtNum or varIndex is negative or 0.
*/
STATUS PKB::setModifies(STATEMENT stmtNum, VAR_INDEX varIndex) {
	int procIndex = stmtToProcMap.at(stmtNum);
	
	bool updatedProc = modifiesTable->setModifiesProc(procIndex, varIndex);
	bool updatedStmt = modifiesTable->setModifies(stmtNum, varIndex);
	return updatedStmt || updatedProc;
}

/**
* @param stmtNum  the statement number that modifies a particular variable
* @param varIndex  the index of a variable
* @return TRUE if Modifies(stmtNum, varIndex) is satisfied.
*		  FALSE if Modifies(stmtNum, varIndex) is not satisfied or either stmtNum or varIndex is negative or 0.
*/
BOOLEAN_ PKB::isModifies(STATEMENT stmtNum, VAR_INDEX varIndex) {
	return modifiesTable->isModifies(stmtNum, varIndex);
}

/**
* @param varIndex  the index of a variable
* @return the list of all the statements that modifies the variable with variable index varIndex. 
* If there is no statement that modifies the variable, or if varIndex is invalid, return an empty list.
*/
STATEMENT_LIST PKB::getModStmtNum(VAR_INDEX varIndex) {
	return modifiesTable->getModStmtNum(varIndex);
}

/**
* @param stmtNum  the statement number that modifies a particular variable
* @return the list of all the variables that is modified by the statement number. 
* If there is no answer, or if stmtNum is negative or 0, return an empty list.
*/
VARINDEX_LIST PKB::getModVarForStmt(STATEMENT stmtNum) {
	return modifiesTable->getModVarForStmt(stmtNum);
}

/**
* @param stmtNum the statement number that modifies a particular variable
* @exception exception if stmtNum is negative or 0.
*/
boost::dynamic_bitset<> PKB::getModVarInBitvectorForStmt(STATEMENT stmtNum) {
	return modifiesTable->getModVarInBitvectorForStmt(stmtNum);
}

/**
* @return all pairs of statement numbers, stmtNum, and variable indexes, varIndex, where Modifies(stmtNum, varIndex) is satisfied.
*/
pair<STATEMENT_LIST, VARINDEX_LIST> PKB::getAllModPair() {
	return modifiesTable->getAllModPair();
}

/**
* @return a list of all statement numbers, stmtNum, where Modifies(stmtNum, varIndex) is true.
*/
STATEMENT_LIST PKB::getModifiesLhs() {
	return modifiesTable->getLhs();
}

/**
 * @return a list of all variable indexes, varIndex, where Modifies(stmtNum, varIndex) is true.
 */
STATEMENT_LIST PKB::getModifiesRhs() {
	return modifiesTable->getRhs();
}

/**
* Sets the Modifies relation for procedures.
* @param procIndex  the index of a procedure that modifies a particular variable
* @param varIndex  the index of a variable
* @return TRUE if the ModifiesTable is updated accordingly. 
*		  FALSE if the ModifiesTable is not updated accordingly.
* If procIndex and varIndex were already previously set, the ModifiesTable will not be updated.
* @exception exception if procIndex or varIndex is negative or 0.
*/
STATUS PKB::setModifiesProc(PROC_INDEX procIndex, VAR_INDEX varIndex) {
	return modifiesTable->setModifiesProc(procIndex, varIndex);
}

/**
* @param procIndex  the index of a procedure that modifies a particular variable
* @param varIndex  the index of a variable
* @return TRUE if Modifies(procIndex, varIndex) is satisfied.
*		  FALSE if Modifies(procIndex, varIndex) is not satisfied or either procIndex or varIndex is negative or 0.
*/
BOOLEAN_ PKB::isModifiesProc(PROC_INDEX procIndex, VAR_INDEX varIndex) {
	return modifiesTable->isModifiesProc(procIndex, varIndex);
}

/**
* @param varIndex  the index of a variable
* @return the list of all the procedures that modifies the variable with variable index varIndex. 
* If there is no procedure that modifies the variable, or if varIndex is invalid, return an empty list.
*/
PROCINDEX_LIST PKB::getModProcIndex(VAR_INDEX varIndex) {
	return modifiesTable->getModProcIndex(varIndex);
}

/**
* @param procIndex  the index of a procedure that modifies a particular variable
* @return the list of all the variables that is modified by the procedure with procedure index procIndex. 
* If there is no variables that is modified by the procedure, or if procIndex is invalid, return an empty list.
*/
VARINDEX_LIST PKB::getModVarForProc(PROC_INDEX procIndex) {
	return modifiesTable->getModVarForProc(procIndex);
}

/**
* @return all pairs of procedure indexes, procIndex, and variable indexes, varIndex, where Modifies(procIndex, varIndex) is satisfied.
*/
pair<PROCINDEX_LIST, VARINDEX_LIST> PKB::getAllModProcPair() {
	return modifiesTable->getAllModProcPair();
}


// UsesTable methods

/**
* Initialise the Uses Table.
* @param numVariables number of variables
* @exception exception if numVariables is negative or 0.
*/
void PKB::initUsesTable(INTEGER numVariables) {
	if (numVariables <= 0) {
		throw runtime_error("initUsesTable: invalid number of variables");
	}
	usesTable->init(numVariables);
}

/**
* Sets the Uses relation for statements.
* @param stmtNum  the statement number that uses a particular variable
* @param varIndex  the index of a variable
* @return TRUE if the UsesTable is updated accordingly. 
*		  FALSE if the UsesTable is not updated accordingly.
* If stmtNum and varIndex were already previously set, the UsesTable will not be updated.
* @exception exception if either stmtNum or varIndex is negative or 0.
*/
STATUS PKB::setUses(STATEMENT stmtNum, VAR_INDEX varIndex) {
	int procIndex = stmtToProcMap.at(stmtNum);

	bool updateProc = usesTable->setUsesProc(procIndex, varIndex);
	bool updateStmt = usesTable->setUses(stmtNum, varIndex);
	return updateStmt || updateProc;
}

/**
* @param stmtNum  the statement number that uses a particular variable
* @param varIndex  the index of a variable
* @return TRUE if Uses(stmtNum,varIndex) is satisfied.
*		  FALSE if Uses(stmtNum,varIndex) is not satisfied or either stmtNum or varIndex is negative or 0.
*/
BOOLEAN_ PKB::isUses(STATEMENT stmtNum, VAR_INDEX varIndex) {
	return usesTable->isUses(stmtNum, varIndex);
}

/**
* @param varIndex  the index of a variable
* @return the list of all the statements that uses the variable with variable index varIndex. 
* If there is no statement that uses the variable, or if varIndex is invalid, return an empty list.
*/
STATEMENT_LIST PKB::getUsesStmtNum(VAR_INDEX varIndex) {
	return usesTable->getUsesStmtNum(varIndex);
}

/**
* @param stmtNum  the statement number that uses a particular variable
* @return the list of all the variables that is used by the statement number stmtNum. 
* If there is no variable that is used by the statement number, or if stmtNum is negative or 0, return an empty list.
*/
VARINDEX_LIST PKB::getUsesVarForStmt(STATEMENT stmtNum) {
	return usesTable->getUsesVarForStmt(stmtNum);
}

/**
* @param stmtNum the statement number that uses a particular variable
* @exception exception if stmtNum is negative or 0.
*/
boost::dynamic_bitset<> PKB::getUseVarInBitvectorForStmt(STATEMENT stmtNum) {
	return usesTable->getUseVarInBitvectorForStmt(stmtNum);
}

/**
* @return all pairs of statement numbers, stmtNum, and variable indexes, varIndex, where Uses(stmtNum, varIndex) is satisfied.
*/
pair<STATEMENT_LIST, VARINDEX_LIST> PKB::getAllUsesPair() {
	return usesTable->getAllUsesPair();
}

/**
* @return a list of all statement numbers, stmtNum, where Modifies(stmtNum, varIndex) is true.
*/
STATEMENT_LIST PKB::getUsesLhs() {
	return usesTable->getLhs();
}

/**
* @return a list of all variable indexes, varIndex, where Modifies(stmtNum, varIndex) is true.
*/
STATEMENT_LIST PKB::getUsesRhs() {
	return usesTable->getRhs();
}


BOOLEAN_ PKB::isUsesValid() {
	return usesTable->isValid();
}

/**
* Sets the Uses relation for procedures.
* @param procIndex  the index of a procedure that uses a particular variable
* @param varIndex  the index of a variable
* @return TRUE if the UsesTable is updated accordingly. 
*		  FALSE if the UsesTable is not updated accordingly.
* If procIndex and varIndex were already previously set, the UsesTable will not be updated.
* @exception exception if procIndex or varIndex is negative or 0.
*/
STATUS PKB::setUsesProc(PROC_INDEX procIndex, VAR_INDEX varIndex) {
	return usesTable->setUsesProc(procIndex, varIndex);
}

/**
* @param procIndex  the index of a procedure that uses a particular variable
* @param varIndex  the index of a variable
* @return TRUE if Uses(procIndex,varIndex) is satisfied.
*		  FALSE if Uses(procIndex,varIndex) is not satisfied or either procIndex or varIndex is negative or 0.
*/
BOOLEAN_ PKB::isUsesProc(PROC_INDEX procIndex, VAR_INDEX varIndex) {
	return usesTable->isUsesProc(procIndex, varIndex);
}

/**
* @param varIndex  the index of a variable
* @return the list of all the procedures that uses the variable with variable index varIndex. 
* If there is no procedure that uses the variable, or if varIndex is invalid, return an empty list.
*/
PROCINDEX_LIST PKB::getUsesProcIndex(VAR_INDEX varIndex) {
	return usesTable->getUsesProcIndex(varIndex);
}

/**
* @param procIndex  the index of a procedure that uses a particular variable
* @return the list of all the variables that is used by the procedure with procedure index procIndex. 
* If there is no variable that is used by the procedure, or if procIndex is negative, return an empty list.
*/
VARINDEX_LIST PKB::getUsesVarForProc(PROC_INDEX procIndex) {
	return usesTable->getUsesVarForProc(procIndex);
}

/**
* @return all pairs of procedure indexes, procIndex, and variable index, varIndex, where Uses(procIndex, varIndex) is satisfied.
*/
pair<PROCINDEX_LIST, VARINDEX_LIST> PKB::getAllUsesProcPair() {
	return usesTable->getAllUsesProcPair();
}


// NextTable methods


/**
* @param progLine1  the program line that is the predecessor of progLine2
* @param progLine2  the program line that is the successor of progLine1
* @param transitiveClosure  a flag to indicate the computation of Next or Next* relation
* @return if transitiveClosure is false: TRUE if Next(progLine1, progLine2) is satisfied. FALSE if Next(progLine1, progLine2) is not satisfied.
*		  if transitiveClosure is true: TRUE if Next*(progLine1, progLine2) is satisfied. 
*										 FALSE if Next*(progLine1, progLine2) is not satisfied.
*		   FALSE if either progLine1 or progLine2 is negative or 0.
*/
BOOLEAN_ PKB::isNext(PROG_LINE_ progLine1, PROG_LINE_ progLine2, TRANS_CLOSURE transitiveClosure) {
	return nextTable->isNext(progLine1, progLine2, transitiveClosure);
}

/**
* @param progLine1  the program line that is the predecessor of progLine2
* @param progLine2  the program line that is the successor of progLine1
* @return TRUE if Next*(progLine1, progLine2) is satisfied. 
*		  FALSE if Next*(progLine1, progLine2) is not satisfied or if either progLine1 or progLine2 is negative or 0.
*/
BOOLEAN_ PKB::isNextS(PROG_LINE_ progLine1, PROG_LINE_ progLine2) {
	bool transClosure = true;
	return nextTable->isNext(progLine1, progLine2, transClosure);
}

/**
* @param progLine2  the program line that is the successor of progLine1
* @param transitiveClosure  a flag to indicate the computation of Next or Next* relation
* @return if transitiveClosure is false: a list of program lines, progLine1, where Next(progLine1, progLine2) is satisfied.
*		  if transitiveClosure is true: a list of program lines, progLine1, where Next*(progLine1, progLine2) is satisfied.
*		  an empty list if progLine2 is invalid or negative or 0.
*/
PROGLINE_LIST PKB::getNextBefore(PROG_LINE_ progLine2, TRANS_CLOSURE transitiveClosure) {
	return nextTable->getNextBefore(progLine2, transitiveClosure);
}

/**
* @param progLine2  the program line that is the successor of progLine1
* @return a list of program lines, progLine1, where Next*(progLine1, progLine2) is satisfied.
*		  an empty list if progLine2 is invalid or negative or 0.
*/
PROGLINE_LIST PKB::getNextBeforeS(PROG_LINE_ progLine2) {
	bool transClosure = true;
	return nextTable->getNextBefore(progLine2, transClosure);
}

/**
* @param progLine1  the program line that is the predecessor of progLine2
* @param transitiveClosure  a flag to indicate the computation of Next or Next* relation
* @return if transitiveClosure is false: a list of program lines, progLine2, where Next(progLine1, progLine2) is satisfied.
*		  if transitiveClosure is true: a list of program lines, progLine2, where Next*(progLine1, progLine2) is satisfied.
*		  an empty list if progLine1 is invalid or negative or 0.
*/
PROGLINE_LIST PKB::getNextAfter(PROG_LINE_ progLine1, TRANS_CLOSURE transitiveClosure) {
	return nextTable->getNextAfter(progLine1, transitiveClosure);
}

/**
* @param progLine1  the program line that is the predecessor of progLine2
* @return a list of program lines, progLine2, where Next*(progLine1, progLine2) is satisfied.
*		  an empty list if progLine1 is invalid or negative or 0.
*/
PROGLINE_LIST PKB::getNextAfterS(PROG_LINE_ progLine1) {
	bool transClosure = true;
	return nextTable->getNextAfter(progLine1, transClosure);
}

/**
* @return a list of all program lines, progLine1, where Next(progLine1, progLine2) is true.
*/
PROGLINE_LIST PKB::getNextLhs() {
	return nextTable->getLhs();
}

/**
* @return a list of all program lines, progLine2, where Next(progLine1, progLine2) is true.
*/
PROGLINE_LIST PKB::getNextRhs() {
	return nextTable->getRhs();
}

BOOLEAN_ PKB::isNextValid() {
	return nextTable->isValid();
}

BOOLEAN_ PKB::isNextLhsValid(PROG_LINE_ lhs) { 
	return nextTable->isLhsValid(lhs);
}

BOOLEAN_ PKB::isNextRhsValid(PROG_LINE_ rhs) {
	return nextTable->isRhsValid(rhs);
}

/**
* @param procIndex the index of a procedure
* @return the first program line in the procedure.
*/
PROG_LINE_ PKB::getFirstProgLineInProc(PROC_INDEX procIndex) {
	return nextTable->getFirstProgLineInProc(procIndex);
}

/**
* @param procIndex the index of a procedure
* @return the last program line in the procedure.
*/
PROG_LINE_ PKB::getLastProgLineInProc(PROC_INDEX procIndex) {
	return nextTable->getLastProgLineInProc(procIndex);
}

/**
* Sets the first program line in the procedure.
* @param procIndex the index of a procedure
* @param firstProgline the first program line
*/
void PKB::setFirstProgLineInProc(PROC_INDEX procIndex, PROG_LINE_ firstProgline) {
	nextTable->setFirstProgLineInProc(procIndex, firstProgline);
}

/**
* Sets the last program line in the procedure.
* @param procIndex the index of a procedure
* @param lastProgline the last program line
*/
void PKB::setLastProgLineInProc(PROC_INDEX procIndex, PROG_LINE_ lastProgline) {
	nextTable->setLastProgLineInProc(procIndex, lastProgline);
}

/**
* @param container the container statement
* @return the first program line in the container statement.
*/
PROG_LINE_ PKB::getFirstProgLineInContainer(CONTAINER_STATEMENT container) {
	return nextTable->getFirstProgLineInContainer(container);
}

/**
* @param container the container statement
* @return the last program line in the container statement.
*/
PROG_LINE_ PKB::getLastProgLineInContainer(CONTAINER_STATEMENT container) {
	return nextTable->getLastProgLineInContainer(container);
}

/**
* Sets the first program line in the else of the if statement
* @param container the else of the if statement
* @param progline the program line
*/
void PKB::setFirstProgLineInElse(CONTAINER_STATEMENT container, PROG_LINE_ progline) {
	nextTable->setFirstProgLineInElse(container, progline);
}

/**
* Sets the last program line in the else of the if statement
* @param container the else of the if statement
* @param progline the program line
*/
void PKB::setLastProgLineInContainer(CONTAINER_STATEMENT container, PROG_LINE_ progline) {
	nextTable->setLastProgLineInContainer(container, progline);
}

/**
* Sets the program line in the while statement
* @param progline the program line
*/
void PKB::setProgLineInWhile(PROG_LINE_ progLine) {
	nextTable->setProgLineInWhile(progLine);
}

/**
* @param progLine the program line
* @return the CNode for the program line.
*/
CNode* PKB::getCNodeForProgLine(PROG_LINE_ progLine) {
	//return nextTable->getCNodeForProgLine(progLine);
	return cfgNodeTable.at(progLine);
}

// Affects Table methods

/**
* @param progLine1  the program line that affects progline2
* @param progLine2  the program line that is affected by progline1
* @param transitiveClosure  a flag to indicate the computation of Affect or Affect* relation
* @return TRUE if Affect(progLine1,progLine2) is satisfied.
*		  FALSE if Affect(progLine1,progLine2) is not satisfied or either progline1 or progLine2 is negative or 0.
*/
BOOLEAN_ PKB::isAffects(PROG_LINE_ progLine1, PROG_LINE_ progLine2, TRANS_CLOSURE transitiveClosure) {
	//@todo optimise in future
	// do this now to prevent regressions, as Affects will change a lot in the next week
	vector<int> ans = getAffectedBy(progLine1, transitiveClosure);

	return find(ans.begin(), ans.end(), progLine2) != ans.end();

}

/**
* @param progLine1  the program line that affects progline2
* @param transitiveClosure  a flag to indicate the computation of Affect or Affect* relation
* @return if transitiveClosure is false: a list of program lines, progLine2, where Affect(progLine1, progLine2) is satisfied.
*		  if transitiveClosure is true: a list of program lines, progLine2, where Affect*(progLine1, progLine2) is satisfied.
*		  an empty list if progLine1 is invalid or negative or 0.
*/
PROGLINE_LIST PKB::getAffectedBy(PROG_LINE_ progLine1, TRANS_CLOSURE transitiveClosure) {
	return affectsTable->getProgLinesAffectedBy(progLine1, transitiveClosure);
}

/**
* @param progLine2  the program line that is affected by progline1
* @param transitiveClosure  a flag to indicate the computation of Affect or Affect* relation
* @return if transitiveClosure is false: a list of program lines, progLine1, where Affect(progLine1, progLine2) is satisfied.
*		  if transitiveClosure is true: a list of program lines, progLine1, where Affect*(progLine1, progLine2) is satisfied.
*		  an empty list if progLine2 is invalid or negative or 0.
*/
PROGLINE_LIST PKB::getAffecting(PROG_LINE_ progLine2, TRANS_CLOSURE transitiveClosure) {
	return affectsTable->getProgLinesAffecting(progLine2, transitiveClosure);
}

/**
* @return a list of all program lines, progLine1, where Affect(progLine1, progLine2) is true.
*/
PROGLINE_LIST PKB::getAffectsLhs() {
	return affectsTable->getLhs();
}

/**
* @return a list of all program lines, progLine2, where Affect(progLine1, progLine2) is true.
*/
PROGLINE_LIST PKB::getAffectsRhs() {
	return affectsTable->getRhs();
}


BOOLEAN_ PKB::isAffectsValid() {
	return affectsTable->isValid();
}

BOOLEAN_ PKB::isAffectsLhsValid(PROG_LINE_ lhs) { 
	return affectsTable->isLhsValid(lhs);
}

BOOLEAN_ PKB::isAffectsRhsValid(PROG_LINE_ rhs) {
	return affectsTable->isRhsValid(rhs);
}


/**
* Checks if the node has information about the variables modified previously on the control flow graph.
* @param node a CNode
* @return TRUE if the node contains the neccessary information to skip backwards.
*		  FALSE if the node does not contain the neccessary information to skip backwards.
*/
BOOLEAN_ PKB::canSkipNodesBackwards(CNode* node) {
	return AffectsTable::canSkipNodesBackwards(node);
}

/**
* Checks if the node has information about the variables used next on the control flow graph.
* @param node a CNode
* @return TRUE if the node contains the neccessary information to skip ahead.
*		  FALSE if the node does not contain the neccessary information to skip ahead.
*/
BOOLEAN_ PKB::canSkipNodesForwards(CNode* node) {
	return AffectsTable::canSkipNodesForwards(node);
}


//NextBip

BOOLEAN_ PKB::isNextBip(PROG_LINE_ progLine1, PROG_LINE_ progLine2, TRANS_CLOSURE transitiveClosure) {
	return false;
}

PROGLINE_LIST PKB::getNextBipAfter(PROG_LINE_ progline1, TRANS_CLOSURE transitiveClosure) {
	return nextBipTable->getNextBipAfter(progline1, transitiveClosure);
}


PROGLINE_LIST PKB::getAffectsBipAfter(PROG_LINE_ progLine1, TRANS_CLOSURE transitiveClosure) {
	return affectsBipTable->getProgLinesAffectsBipAfter(progLine1, transitiveClosure);
}

PROGLINE_LIST PKB::getAffectsBipBefore(PROG_LINE_ progLine2, TRANS_CLOSURE transitiveClosure) {
	return affectsBipTable->getProgLinesAffectsBipBefore(progLine2, transitiveClosure);
}

PROGLINE_LIST PKB::getNextBipBefore(PROG_LINE_ progline2, TRANS_CLOSURE transitiveClosure) {
	return nextBipTable->getNextBipBefore(progline2, transitiveClosure);
}


PROGLINE_LIST PKB::getNextBipLhs() {
	return vector<int>();
}

PROGLINE_LIST PKB::getNextBipRhs() {
	return vector<int>();
}

// AffectsBip

BOOLEAN_ PKB::isAffectsBip(PROG_LINE_ progLine1, PROG_LINE_ progLine2, TRANS_CLOSURE transitiveClosure) {

	return false;

}

PROGLINE_LIST PKB::getAffectsBipAfter(PROG_LINE_ progLine1, TRANS_CLOSURE transitiveClosure) {
	return vector<int>();
}

PROGLINE_LIST PKB::getAffectsBipBefore(PROG_LINE_ progLine2, TRANS_CLOSURE transitiveClosure) {
	return vector<int>();
}

PROGLINE_LIST PKB::getAffectsBipLhs() {
	return vector<int>();
}

PROGLINE_LIST PKB::getAffectsBipRhs() {
	return vector<int>();
}

/*
** Singleton implementation: for Query Evaluator to get an instance of PKB
*/
PKB& PKB::getInstance() 
{
	static PKB instance;
	return instance;
}
