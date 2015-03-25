#pragma once

#include<stdio.h>
#include <iostream>
#include <string>
#include <vector>

using namespace std;

#include "PKB.h"
#include "TNode.h"
#include "Synonym.h"


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
	nextTable = new NextTable();
	affectsTable = new AffectsTable();
}

//AST

/**
* @param astNodeType  the node type which can be Procedure, Assign, Plus, Variable, StmtLst, While, If, Constant, Minus, Times, Program, Call
* @param stmtNum  the statement number of the TNode
* @param value  the value of the TNode
* @return a TNode for the given design entity together with its statement number and value. 
* @exception exception if stmtNum is negative or 0 or value is negative.
*/
TNode* PKB::createTNode(TNODE_TYPE astNodeType, int stmtNo, int value) 
{
	return ast->createTNode(astNodeType, stmtNo, value);
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
bool PKB::createLink(LINK_TYPE link, TNode* fromNode, TNode* toNode) 
{
	return ast->createLink(link, fromNode, toNode);
}

/**
* @param parent  a node which may or may not have children nodes
* @return the total number of children the parent TNode has. 
* @exception exception if parent is NULL.
*/
int PKB::getChildrenSize(TNode* parent) 
{
	return ast->getChildrenSize(parent);
}

/**
* @param parent  a node which may or may not have children nodes
* @return the list of all the children nodes the parent TNode has.
* If there are no children nodes, return an empty list.
* @exception exception if parent is NULL.
*/
vector<TNode*>* PKB::getChildrenNode(TNode* parent) 
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
bool PKB::isChildNode(TNode* parent, TNode* child) 
{
	return ast->isChildNode(parent, child);
}

/**
* @param node  an AST node
* @return TRUE if node exists. 
		  FALSE if node does not exist.
* @exception exception if node is NULL.
*/
bool PKB::isExists(TNode* node) 
{
	return ast->isExists(node);
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
 * @param RHS  the expression query with a suitable subtree
 * @return a vector of statement numbers which are assign stmts, and uses the input RHS as its right substree.
 */
vector<int> PKB::patternMatchAssign(string RHS) 
{
	return ast->patternMatchAssign(RHS);
}

/**
 * Pattern matching for while statements.
 * @param LHS  the name of the variable that acts as the control variable for the while statements we are interested in
 * @return a vector of statement numbers which are while statements, and uses the input LHS as its control variable.
 */
vector<int> PKB::patternMatchWhile(string LHS) 
{
	return ast->patternMatchWhile(LHS);
}

/**
 * Pattern matching for if statements.
 * @param LHS  the name of the variable that acts as the control variable for the if statements we are interested in
 * @return a vector of statement numbers which are in if statements, and uses the input LHS as its control variable.
 */
vector<int> PKB::patternMatchIf(string LHS) 
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
int PKB::getControlVariable(int stmtNum) 
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
 * @param index  the index of the variable
 * @return the name of the variable in the VarTable with the given index.
 * If index is out of range, return an empty string.
 */
string PKB::getVarName(int index) 
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
int PKB::getVarIndex(string varName) 
{
	return varTable->getVarIndex(varName);
}

/**
 * @return all the index of the variables in the the VarTable.
 * If there is no answer, return an empty list.
 */
vector<int> PKB::getAllVarIndex() 
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
int PKB::insertProc(string procName) 
{
	return procTable->insertProc(procName);
}

/**
 * @return the total number of procedures in the the ProcTable.
 */
int PKB::getProcTableSize() 
{
	return procTable->getProcTableSize();;
}

/**
 * @param index  the index of the procedure
 * @return the name of the procedure in the ProcTable with the given index.
 * If index is out of range, return an empty string.
 */
string PKB::getProcName(int index) 
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
int PKB::getProcIndex(string procName) 
{
	return procTable->getProcIndex(procName);
}

/**
 * @return all the index of the variables in the the ProcTable.
 * If there is no answer, return an empty list.
 */
vector<int> PKB::getAllProcIndex() 
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
bool PKB::insertConstant(int constant, int stmtNum) 
{
	return constantTable->insertConstant(constant, stmtNum);
}

/**
 * @return the total number of constants in the the ConstantTable.
 */
int PKB::getConstantTableSize() 
{
	return constantTable->getSize();
}

/**
* @param constant  a number that appears in the program
* @return the index of a constant in ConstantTable using the constant as the key.
* @exception exception if constant is negative.
*/
int PKB::getConstantIndex(int constant) 
{
	return constantTable->getConstantIndex(constant);
}

/**
* @param constant  a number that appears in the program
* @return the statement number the constant is in the ConstantTable.
* @exception exception if constant is negative.
*/
vector<int> PKB::getStmtNum(int constant) 
{
	return constantTable->getStmtNum(constant);
}


/**
* Check if the number is a constant in the ConstantTable.
* @return TRUE if number is a constant in the ConstantTable.
*		  FALSE if number is not a constant in the ConstantTable.
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
bool PKB::insertStmt(int stmtNum, string type, TNode* node, int procIndex) 
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
string PKB::getType(int stmtNum) 
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
vector<int> PKB::getStmtNumForType(string type) 
{
	 // @todo remove the whole function
	return stmtTable->getStmtNumForType(type);
}

/**
 * Get the proc index that the statement is in
 * @param stmtNo  the statement number
 * @return the procIndex that the statement is in
 */
int PKB::getProcIndexForStmt(int stmtNo) {
	return stmtTable->getProcIndexForStmt(stmtNo);
}

/**
 * Get all the statement numbers with the corresponding valid STATEMENT_TYPE which are while/assign/if/call.
 * @param type  the type of statement which is while/assign/if/call
 * @return all the statement number of the statement type in the the StmtTable. 
 * If there is no answer or if type is an invalid STATEMENT_TYPE, return an empty list.
 * @exception exception if type is not while/assign/if/call.
*/
vector<int> PKB::getStmtNumForType(SYNONYM_TYPE type) 
{
	return stmtTable->getStmtNumForType(Synonym::convertToString(type));
}

/**
* Check if the statment number is an Assign statement.
* @param stmtNum  the statement number 
* @return TRUE if stmtNum is of Assignment Type. 
*		  FALSE if stmtNum is not of Assignment Type or stmtNum is out of range.
*/
bool PKB::isAssign(int stmtNum) 
{
	return stmtTable->isAssign(stmtNum);
}

/**
* Check if the statment number is an While statement.
* @param stmtNum  the statement number 
* @return TRUE if stmtNum is of While Type. 
*		  FALSE if stmtNum is not of While Type or stmtNum is out of range.
*/
bool PKB::isWhile(int stmtNum) {
	return stmtTable->isWhile(stmtNum);
}

/**
* Check if the statment number is an If statement.
* @param stmtNum  the statement number 
* @return TRUE if stmtNum is of If Type. 
*		  FALSE if stmtNum is not of If Type or stmtNum is out of range.
*/
bool PKB::isIf(int stmtNum) {
	return stmtTable->isIf(stmtNum);
}

/**
* Check if the statment number is an Call statement.
* @param stmtNum  the statement number 
* @return TRUE if stmtNum is of Call Type. 
*		  FALSE if stmtNum is not of Call Type or stmtNum is out of range.
*/
bool PKB::isCall(int stmtNum) {
	return stmtTable->isCall(stmtNum);
}

/**
 * @return the total number of statements in the the StmtTable.
 */
int PKB::getStmtTableSize() {
	return stmtTable->getSize();
}

/**
 * @return the TNode in the AST for the statement number
 * @param stmtNum  the statement number to retrieve the TNode for
 */
TNode* PKB::getNodeForStmt(int stmtNum) {
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
bool PKB::setParent(TNode* stmtNum1, TNode* stmtNum2) {
	return parentTable->setParent(stmtNum1, stmtNum2);
}

/**
* @param stmtNum2  the statement number of the child
* @param transitiveClosure  a flag to indicate the computation of Parent or Parent* relation
* @return if transitiveClosure is false : a list of statement numbers of the parent of stmtNum2, based on the Parent relation.
*		  if transitiveClosure is true: a list of statement numbers of the ancestors of stmtNum2, based on the Parent* relation.
*		  if stmtNum2 is out of range, an empty list will be returned.
*/
vector<int> PKB::getParent(int stmtNum2, bool transitiveClosure) {
	return parentTable->getParent(stmtNum2, transitiveClosure);
}

/**
* @param stmtNum2  the statement number of the child
* @return a list of statement numbers of the ancestors of stmtNum2, based on the Parent* relation.
* 		  if stmtNum2 is out of range, an empty list will be returned.
*/
vector<int> PKB::getParentS(int stmtNum2) {
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
vector<int> PKB::getChild(int stmtNum1, bool transitiveClosure) {
	return parentTable->getChild(stmtNum1, transitiveClosure);
}

/**
* @param stmtNum1  the statement number of the parent
* @return a list of statement numbers of the descendants of stmtNum1, based on the Parent* relation.
*		  if stmtNum1 is out of range, an empty list will be returned.
*/
vector<int> PKB::getChildS(int stmtNum1) {
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
bool PKB::isParent(int stmtNum1, int stmtNum2, bool transitiveClosure) {
	return parentTable->isParent(stmtNum1, stmtNum2, transitiveClosure);
}

/**
 * @param stmtNum1  the statement number of the parent
 * @param stmtNum2  the statement number of the child
 * @return TRUE if Parent*(stmtNum1, stmtNum2) holds. 
 *		   FALSE if Parent*(stmtNum1, stmtNum2) does not hold or either stmtNum1 or stmtNum2 is invalid.
 */
bool PKB::isParentS(int stmtNum1, int stmtNum2) {
	bool transitiveClosure = true;
	return parentTable->isParent(stmtNum1, stmtNum2, transitiveClosure);
}

/**
* @param transitiveClosure  a flag to indicate the computation of Parent or Parent* relation
* @return if transitiveClosure is false: all pairs of stmtNum1, stmtNum2 where Parent(stmtNum1, stmtNum2) is satisfied.
*		  if transitiveClosure is true: all pairs of stmtNum1, stmtNum2 where Parent*(stmtNum1, stmtNum2) is satisfied.
*/
pair<vector<int>, vector<int>> PKB::getAllParentPairs(bool transitiveClosure) {
	return parentTable->getAllParentPairs(transitiveClosure);
}

/**
* @return all pairs of stmtNum1, stmtNum2 where Parent*(stmtNum1, stmtNum2) is satisfied.
*/
pair<vector<int>, vector<int>> PKB::getAllParentPairsS() {
	bool transitiveClosure = true;
	return parentTable->getAllParentPairs(transitiveClosure);
}

/**
 * @return a list of all statement numbers, stmt, where Parent(stmt, _) is true.
 */
vector<int> PKB::getParentLhs(){
	return parentTable->getAllParents();
}

/**
 * @return a list of all statement numbers, stmt, where Parent(_, stmt) is true.
 */
vector<int> PKB::getParentRhs(){
	return parentTable->getAllChildren();
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
bool PKB::setFollows(TNode* stmt1, TNode* stmt2) {
	return followsTable->setFollows(stmt1, stmt2);
}	

/**
* @param stmtNum2  the statement number of the follower node
* @param transitiveClosure  a flag to indicate the computation of Follow or Follow* relation
* @return if transitiveClosure is false: a list of statement numbers, stmtNum, where Follows(stmtNum, stmtNum2) is satisfied.
*		  if transitiveClosure is true: a list of statement numbers, stmtNum, where Follows*(stmtNum, stmtNum2) is satisfied.
*		  an empty list if stmtNum2 is invalid.
*/
vector<int> PKB::getStmtFollowedTo(int stmtNum2, bool transitiveClosure) {
	return followsTable->getStmtFollowedTo(stmtNum2, transitiveClosure);
}

/**
* @param stmtNum2  the statement number of the follower node
* @return a list of statement numbers, stmtNum, where Follows*(stmtNum, stmtNum2) is satisfied.
*		  an empty list if stmtNum2 is invalid.
*/
vector<int> PKB::getStmtFollowedToS(int stmtNum2) {
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
vector<int> PKB::getStmtFollowedFrom(int stmtNum1 , bool transitiveClosure) {
	return followsTable->getStmtFollowedFrom(stmtNum1, transitiveClosure);
}

/**
* @param stmtNum1  the statement number of the following node
* @return a list of statement numbers, stmtNum2, where Follows*(stmtNum1, stmtNum2) is satisfied.
*		  an empty list if stmtNum1 is invalid.
*/
vector<int> PKB::getStmtFollowedFromS(int stmtNum1) {
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
bool PKB::isFollows(int stmtNum1, int stmtNum2, bool transitiveClosure) {
	return followsTable->isFollows(stmtNum1, stmtNum2, transitiveClosure);
}
/**
* @param stmtNum1  the statement number of the following node
* @param stmtNum2  the statement number of the follower node
* @return TRUE if Follows*(stmtNum1, stmtNum2) is satisfied. 
*		  FALSE if Follows*(stmtNum1, stmtNum2) is not satisfied or either stmtNum1 or stmtNum2 is invalid.
*/
bool PKB::isFollowsS(int stmtNum1, int stmtNum2) {
	bool transitiveClosure = true;
	return followsTable->isFollows(stmtNum1, stmtNum2, transitiveClosure);
}

/**
* @param transitiveClosure  a flag to indicate the computation of Follow or Follow* relation
* @return if transitiveClosure is false: all pairs of stmtNum1, stmtNum2 where Follow(stmtNum1, stmtNum2) is satisfied.
*		  if transitiveClosure is true: all pairs of stmtNum1, stmtNum2 where Follow*(stmtNum1, stmtNum2) is satisfied.
*/
pair<vector<int>, vector<int>> PKB::getAllFollowsPairs(bool transitiveClosure) {
	return followsTable->getAllFollowsPairs(transitiveClosure);
}

/**
* @return all pairs of stmtNum1, stmtNum2 where Follows*(stmtNum1, stmtNum2) is satisfied.
*/
pair<vector<int>, vector<int>> PKB::getAllFollowsPairsS() {
	bool transitiveClosure = true;
	return followsTable->getAllFollowsPairs(transitiveClosure);
}

/**
 * @return a list of all statement numbers, stmt, where Follows(stmt, _) is true.
 */
vector<int> PKB::getFollowsLhs() {
	return followsTable->getLhs();
}

/**
 * @return a list of all statement numbers, stmt, where Follows(_, stmt) is true.
 */
vector<int> PKB::getFollowsRhs() {
	return followsTable->getRhs();
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
bool PKB::setCalls(int procIndex1, int procIndex2){
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
bool PKB::isCalls(int procIndex1, int procIndex2, bool transitiveClosure) {
	return callsTable->isCalls(procIndex1, procIndex2, transitiveClosure);
}

/**
* @param procIndex1  the procedure index of the caller procedure
* @param procIndex2  the procedure index of the callee procedure
* @return TRUE if Calls*(procIndex1, procIndex2) is satisfied. 
*		  FALSE if Calls*(procIndex1, procIndex2) is not satisfied or either procIndex1 or procIndex2 is negative or 0.
*/
bool PKB::isCallsS(int procIndex1, int procIndex2) {
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
vector<int> PKB::getProcsCalling(int procIndex2, bool transitiveClosure) {
	return callsTable->getProcsCalling(procIndex2, transitiveClosure);
}

/**
 * @param procIndex2  the procedure index of the callee procedure
 * @return a list of procedure indexes, procIndex1, where Calls*(procIndex1, procIndex2) is satisfied.
 *		   an empty list if procIndex2 is invalid or negative or 0.
 */
vector<int> PKB::getProcsCallingS(int procIndex2) {
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
vector<int> PKB::getProcsCalledBy(int procIndex1, bool transitiveClosure) {
	return callsTable->getProcsCalledBy(procIndex1, transitiveClosure);
}

/**
 * @param procIndex1  the procedure index of the caller procedure
 * @return a list of procedure indexes, procIndex2, where Calls*(procIndex1, procIndex2) is satisfied.
 *		   an empty list if procIndex1 is invalid or negative or 0.
 */
vector<int> PKB::getProcsCalledByS(int procIndex1) {
	bool transitiveClosure = true;
	return callsTable->getProcsCalledBy(procIndex1, transitiveClosure);
}

/**
 * @param transitiveClosure  a flag to indicate the computation of Calls or Calls* relation
 * @return if transitiveClosure is false: all pairs of procIndex1, procIndex2 where Calls(procIndex1, procIndex2) is satisfied.
 *		   if transitiveClosure is true: all pairs of procIndex1, procIndex2 where Calls*(procIndex1, procIndex2) is satisfied.
 */
pair<vector<int>, vector<int>> PKB::getAllCallsPairs(bool transitiveClosure) {
	return callsTable->getAllCallsPairs(transitiveClosure);
}

/**
 * @return all pairs of procIndex1, procIndex2 where Calls*(procIndex1, procIndex2) is satisfied.
 */
pair<vector<int>, vector<int>> PKB::getAllCallsPairsS() 
{
	bool transitiveClosure = true;
	return callsTable->getAllCallsPairs(transitiveClosure);
}

/**
 * @param stmtNum  the statement number of the Call statement
 * @return the procedure name that is called by the statement number stmtNum.
 * @exception exception if stmtNum is negative or 0.
 */
string PKB::getProcNameCalledByStatement(int stmtNum) {
	return callsTable->getProcNameCalledByStatement(stmtNum);
}

/**
 * @return a list of all procedure indexes, procIndex1, where Calls(procIndex1, procIndex2) is true.
 */
vector<int> PKB::getCallsLhs() {
	return callsTable->getLhs();
}

/**
 * @return a list of all procedure indexes, procIndex2, where Calls(procIndex1, procIndex2) is true.
 */
vector<int> PKB::getCallsRhs() {
	return callsTable->getRhs();
}


// ModifiesTable methods

void PKB::initModifiesTable(int numVariables) {
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
bool PKB::setModifies(int stmtNum, int varIndex) {
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
bool PKB::isModifies(int stmtNum, int varIndex) {
	return modifiesTable->isModifies(stmtNum, varIndex);
}

/**
* @param varIndex  the index of a variable
* @return the list of all the statements that modifies the variable with variable index varIndex. 
* If there is no statement that modifies the variable, or if varIndex is invalid, return an empty list.
*/
vector<int> PKB::getModStmtNum(int varIndex) {
	return modifiesTable->getModStmtNum(varIndex);
}

/**
* @param stmtNum  the statement number that modifies a particular variable
* @return the list of all the variables that is modified by the statement number. 
* If there is no answer, or if stmtNum is negative or 0, return an empty list.
*/
vector<int> PKB::getModVarForStmt(int stmtNum) {
	return modifiesTable->getModVarForStmt(stmtNum);
}

boost::dynamic_bitset<> PKB::getModVarInBitvectorForStmt(int stmtNum) {
	return modifiesTable->getModVarInBitvectorForStmt(stmtNum);
}

/**
* @return all pairs of statement numbers, stmtNum, and variable indexes, varIndex, where Modifies(stmtNum, varIndex) is satisfied.
*/
pair<vector<int>, vector<int>> PKB::getAllModPair() {
	return modifiesTable->getAllModPair();
}

/**
* @return a list of all statement numbers, stmtNum, where Modifies(stmtNum, varIndex) is true.
*/
vector<int> PKB::getModifiesLhs() {
	return modifiesTable->getLhs();
}

/**
 * @return a list of all variable indexes, varIndex, where Modifies(stmtNum, varIndex) is true.
 */
vector<int> PKB::getModifiesRhs() {
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
bool PKB::setModifiesProc(int procIndex, int varIndex) {
	return modifiesTable->setModifiesProc(procIndex, varIndex);
}

/**
* @param procIndex  the index of a procedure that modifies a particular variable
* @param varIndex  the index of a variable
* @return TRUE if Modifies(procIndex, varIndex) is satisfied.
*		  FALSE if Modifies(procIndex, varIndex) is not satisfied or either procIndex or varIndex is negative or 0.
*/
bool PKB::isModifiesProc(int procIndex, int varIndex) {
	return modifiesTable->isModifiesProc(procIndex, varIndex);
}

/**
* @param varIndex  the index of a variable
* @return the list of all the procedures that modifies the variable with variable index varIndex. 
* If there is no procedure that modifies the variable, or if varIndex is invalid, return an empty list.
*/
vector<int> PKB::getModProcIndex(int varIndex) {
	return modifiesTable->getModProcIndex(varIndex);
}

/**
* @param procIndex  the index of a procedure that modifies a particular variable
* @return the list of all the variables that is modified by the procedure with procedure index procIndex. 
* If there is no variables that is modified by the procedure, or if procIndex is invalid, return an empty list.
*/
vector<int> PKB::getModVarForProc(int procIndex) {
	return modifiesTable->getModVarForProc(procIndex);
}

/**
* @return all pairs of procedure indexes, procIndex, and variable indexes, varIndex, where Modifies(procIndex, varIndex) is satisfied.
*/
pair<vector<int>, vector<int>> PKB::getAllModProcPair() {
	return modifiesTable->getAllModProcPair();
}


// UsesTable methods

void PKB::initUsesTable(int numVariables) {
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
bool PKB::setUses(int stmtNum, int varIndex) {
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
bool PKB::isUses(int stmtNum, int varIndex) {
	return usesTable->isUses(stmtNum, varIndex);
}

/**
* @param varIndex  the index of a variable
* @return the list of all the statements that uses the variable with variable index varIndex. 
* If there is no statement that uses the variable, or if varIndex is invalid, return an empty list.
*/
vector<int> PKB::getUsesStmtNum(int varIndex) {
	return usesTable->getUsesStmtNum(varIndex);
}

/**
* @param stmtNum  the statement number that uses a particular variable
* @return the list of all the variables that is used by the statement number stmtNum. 
* If there is no variable that is used by the statement number, or if stmtNum is negative or 0, return an empty list.
*/
vector<int> PKB::getUsesVarForStmt(int stmtNum) {
	return usesTable->getUsesVarForStmt(stmtNum);
}
boost::dynamic_bitset<> PKB::getUseVarInBitvectorForStmt(int stmtNum) {
	return usesTable->getUseVarInBitvectorForStmt(stmtNum);
}

/**
* @return all pairs of statement numbers, stmtNum, and variable indexes, varIndex, where Uses(stmtNum, varIndex) is satisfied.
*/
pair<vector<int>, vector<int>> PKB::getAllUsesPair() {
	return usesTable->getAllUsesPair();
}

/**
* @return a list of all statement numbers, stmtNum, where Modifies(stmtNum, varIndex) is true.
*/
vector<int> PKB::getUsesLhs() {
	return usesTable->getLhs();
}

/**
* @return a list of all variable indexes, varIndex, where Modifies(stmtNum, varIndex) is true.
*/
vector<int> PKB::getUsesRhs() {
	return usesTable->getRhs();
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
bool PKB::setUsesProc(int procIndex, int varIndex) {
	return usesTable->setUsesProc(procIndex, varIndex);
}

/**
* @param procIndex  the index of a procedure that uses a particular variable
* @param varIndex  the index of a variable
* @return TRUE if Uses(stmtNum,varIndex) is satisfied.
*		  FALSE if Uses(stmtNum,varIndex) is not satisfied or either procIndex or varIndex is negative or 0.
*/
bool PKB::isUsesProc(int procIndex, int varIndex) {
	return usesTable->isUsesProc(procIndex, varIndex);
}

/**
* @param varIndex  the index of a variable
* @return the list of all the procedures that uses the variable with variable index varIndex. 
* If there is no procedure that uses the variable, or if varIndex is invalid, return an empty list.
*/
vector<int> PKB::getUsesProcIndex(int varIndex) {
	return usesTable->getUsesProcIndex(varIndex);
}

/**
* @param procIndex  the index of a procedure that uses a particular variable
* @return the list of all the variables that is used by the procedure with procedure index procIndex. 
* If there is no variable that is used by the procedure, or if procIndex is negative, return an empty list.
*/
vector<int> PKB::getUsesVarForProc(int procIndex) {
	return usesTable->getUsesVarForProc(procIndex);
}

/**
* @return all pairs of procedure indexes, procIndex, and variable index, varIndex, where Uses(procIndex, varIndex) is satisfied.
*/
pair<vector<int>, vector<int>> PKB::getAllUsesProcPair() {
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
bool PKB::isNext(int progLine1, int progLine2, bool transitiveClosure) {
	return nextTable->isNext(progLine1, progLine2, transitiveClosure);
}

/**
* @param progLine1  the program line that is the predecessor of progLine2
* @param progLine2  the program line that is the successor of progLine1
* @return TRUE if Next*(progLine1, progLine2) is satisfied. 
*		  FALSE if Next*(progLine1, progLine2) is not satisfied or if either progLine1 or progLine2 is negative or 0.
*/
bool PKB::isNextS(int progLine1, int progLine2) {
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
vector<int> PKB::getNextBefore(int progLine2, bool transitiveClosure) {
	return nextTable->getNextBefore(progLine2, transitiveClosure);
}

/**
* @param progLine2  the program line that is the successor of progLine1
* @return a list of program lines, progLine1, where Next*(progLine1, progLine2) is satisfied.
*		  an empty list if progLine2 is invalid or negative or 0.
*/
vector<int> PKB::getNextBeforeS(int progLine2) {
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
vector<int> PKB::getNextAfter(int progLine1, bool transitiveClosure) {
	return nextTable->getNextAfter(progLine1, transitiveClosure);
}

/**
* @param progLine1  the program line that is the predecessor of progLine2
* @return a list of program lines, progLine2, where Next*(progLine1, progLine2) is satisfied.
*		  an empty list if progLine1 is invalid or negative or 0.
*/
vector<int> PKB::getNextAfterS(int progLine1) {
	bool transClosure = true;
	return nextTable->getNextAfter(progLine1, transClosure);
}

/**
* @return a list of all program lines, progLine1, where Next(progLine1, progLine2) is true.
*/
vector<int> PKB::getNextLhs() {
	return nextTable->getLhs();
}

/**
* @return a list of all program lines, progLine2, where Next(progLine1, progLine2) is true.
*/
vector<int> PKB::getNextRhs() {
	return nextTable->getRhs();
}


int PKB::getFirstProgLineInProc(int procIndex) {
	return nextTable->getFirstProgLineInProc(procIndex);
}
int PKB::getLastProgLineInProc(int procIndex) {
	return nextTable->getLastProgLineInProc(procIndex);
}
void PKB::setFirstProgLineInProc(int procIndex, int firstProgline) {
	nextTable->setFirstProgLineInProc(procIndex, firstProgline);
}
void PKB::setLastProgLineInProc(int procIndex, int lastProgline) {
	nextTable->setLastProgLineInProc(procIndex, lastProgline);
}


int PKB::getFirstProgLineInContainer(int container) {
	return nextTable->getFirstProgLineInContainer(container);
}
int PKB::getLastProgLineInContainer(int container) {
	return nextTable->getLastProgLineInContainer(container);
}
void PKB::setFirstProgLineInElse(int container, int progline) {
	nextTable->setFirstProgLineInElse(container, progline);
}
void PKB::setLastProgLineInContainer(int container, int progline) {
	nextTable->setLastProgLineInContainer(container, progline);
}

/**
* @param progLine the program line
* @return the CNode for the progline
*/
CNode* PKB::getCNodeForProgLine(int progLine) {
	return nextTable->getCNodeForProgLine(progLine);
}


bool PKB::isAffects(int progLine1, int progLine2, bool transitiveClosure) {
	//@todo optimise in future
	// do this now to prevent regressions, as Affects will change a lot in the next week
	vector<int> ans = getAffectedBy(progLine1, transitiveClosure);

	return find(ans.begin(), ans.end(), progLine2) != ans.end();

}

vector<int> PKB::getAffectedBy(int progLine1, bool transitiveClosure) {
	return affectsTable->getProgLinesAffectedBy(progLine1, transitiveClosure);
}

vector<int> PKB::getAffecting(int progLine2, bool transitiveClosure) {
	return affectsTable->getProgLinesAffecting(progLine2, transitiveClosure);
}

vector<int> PKB::getAffectsLhs() {
	return affectsTable->getLhs();
}
vector<int> PKB::getAffectsRhs() {
	return affectsTable->getRhs();
}

bool PKB::canSkipNodesBackwards(CNode* node) {
	return AffectsTable::canSkipNodesBackwards(node);
}

bool PKB::canSkipNodesForwards(CNode* node) {
	return AffectsTable::canSkipNodesForwards(node);
}


/*
** Singleton implementation: for Query Evaluator to get an instance of PKB
*/
PKB& PKB::getInstance() 
{
	static PKB instance;
	return instance;
}
