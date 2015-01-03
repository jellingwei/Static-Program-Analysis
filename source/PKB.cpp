#pragma once

#include<stdio.h>
#include <iostream>
#include <string>
#include <vector>

using namespace std;

#include "PKB.h"
#include "TNode.h"


PKB::PKB() {
	varTable = new VarTable();
	stmtTable = new StmtTable();
	constantTable = new ConstantTable();
	modifiesTable = new ModifiesTable();
	usesTable = new UsesTable();
	followsTable = new FollowsTable();
	parentTable = new ParentTable();
	ast = new AST();
}

TNode* PKB::createTNode(TNODE_TYPE ast_node_type, int stmtNo, int idx) {
	return ast->createTNode(ast_node_type, stmtNo, idx);
}
TNode* PKB::getRoot() {
	return ast->getRoot();
}
bool PKB::createLink(LINK_TYPE link, TNode* fromNode, TNode* toNode) {
	return ast->createLink(link, fromNode, toNode);
}
		
int PKB::getChildrenSize(TNode* parent) {
	return ast->getChildrenSize(parent);
}
vector<TNode*>* PKB::getChildrenNode(TNode* parent) {
	return ast->getChildrenNode(parent);
}
bool PKB::isChildNode(TNode* parent, TNode* child) {
	return ast->isChildNode(parent, child);
}

bool PKB::isExists(TNode* node) {
	return ast->isExists(node);
}
TNode* PKB::getLastImpt() {
	return ast->getLastImpt();
}
int PKB::getASTSize() {
	return ast->getSize();
}

vector<int> PKB::patternMatchAssign(string RHS) {
	return ast->patternMatchAssign(RHS);
}

vector<int> PKB::patternMatchWhile(string LHS) {
	return ast->patternMatchWhile(LHS);
}

int PKB::getControlVariable(int stmtNum) {
	return ast->getControlVariable(stmtNum);
}

// VarTable

int PKB::insertVar(string varName, int stmtNum) {
	return varTable->insertVar(varName, stmtNum);
}
int PKB::getVarTableSize() {
	return varTable->getSize();
}
string PKB::getVarName(int index) {
	return varTable->getVarName(index);
}
int PKB::getVarIndex(string varName) {
	return varTable->getVarIndex(varName);
}
int PKB::getStmtNum(string varName) {
	return varTable->getStmtNum(varName);
}
vector<int> PKB::getAllVarIndex() {
	return varTable->getAllVarIndex();
}

//Constant
bool PKB::insertConstant(int constant, int stmtNum) {
	return constantTable->insertConstant(constant, stmtNum);
}
int PKB::getConstantTableSize() {
	return constantTable->getSize();
}
int PKB::getConstant(int index) {
	return constantTable->getConstant(index);
}
int PKB::getConstantIndex(int constant) {
	return constantTable->getConstantIndex(constant);
}
vector<int> PKB::getStmtNum(int constant) {
	return constantTable->getStmtNum(constant);
}
bool PKB::isConstant(int number) {
	return constantTable->isConstant(number);
}
vector<int> PKB::getAllConstant() {
	return constantTable->getAllConstant();
}

// StmtTable
bool PKB::insertStmt(int stmtNum, string type) {
	return stmtTable->insertStmt(stmtNum, type);
}
string PKB::getType(int stmtNum) {
	return stmtTable->getType(stmtNum);
}
vector<int> PKB::getStmtNumForType(string type) {
	return stmtTable->getStmtNumForType(type);
}
bool PKB::isAssign(int stmtNum) {
	return stmtTable->isAssign(stmtNum);
}
int PKB::getStmtTableSize() {
	return stmtTable->getSize();
}

// Parent
bool PKB::setParent(TNode* stmtNum1, TNode* stmtNum2) {
	return parentTable->setParent(stmtNum1, stmtNum2);
}
	
vector<int> PKB::getParent(int stmtNum2) {
	return parentTable->getParent(stmtNum2);
}
vector<int> PKB::getChild(int stmtNum1) {
	return parentTable->getChild(stmtNum1);
}

bool PKB::isParent(int stmtNum1, int stmtNum2) {
	return parentTable->isParent(stmtNum1, stmtNum2);
}

pair<vector<int>, vector<int>> PKB::getAllParentPairs(bool transitiveClosure) {
	return parentTable->getAllParentPairs(transitiveClosure);
}


bool PKB::setFollows(TNode* stmt1, TNode* stmt2) {
	return followsTable->setFollows(stmt1, stmt2);
}
	
vector<int> PKB::getStmtFollowedTo(int stmtNum2) {
	return followsTable->getStmtFollowedTo(stmtNum2);
}
vector<int> PKB::getStmtFollowedFrom(int stmtNum1) {
	return followsTable->getStmtFollowedFrom(stmtNum1);
}

bool PKB::isFollows(int stmtNum1, int stmtNum2) {
	return followsTable->isFollows(stmtNum1, stmtNum2);
}

pair<vector<int>, vector<int>> PKB::getAllFollowsPairs(bool transitiveClosure) {
	return followsTable->getAllFollowsPairs(transitiveClosure);
}

// ModifiesTable methods
bool PKB::setModifies(int stmtNum, int varIndex) {
	return modifiesTable->setModifies(stmtNum, varIndex);
}
bool PKB::isModifies(int stmtNum, int varIndex) {
	return modifiesTable->isModifies(stmtNum, varIndex);
}
vector<int> PKB::getModStmtNum(int varIndex) {
	return modifiesTable->getModStmtNum(varIndex);
}
vector<int> PKB::getModVarForStmt(int stmtNum) {
	return modifiesTable->getModVarForStmt(stmtNum);
}
pair<vector<int>, vector<int>> PKB::getAllModPair() {
	return modifiesTable->getAllModPair();
}

// UsesTable methods
bool PKB::setUses(int stmtNum, int varIndex) {
	return usesTable->setUses(stmtNum, varIndex);
}
bool PKB::isUses(int stmtNum, int varIndex) {
	return usesTable->isUses(stmtNum, varIndex);
}
vector<int> PKB::getUsesStmtNum(int varIndex) {
	return usesTable->getUsesStmtNum(varIndex);
}
vector<int> PKB::getUsesVarForStmt(int stmtNum) {
	return usesTable->getUsesVarForStmt(stmtNum);
}
pair<vector<int>, vector<int>> PKB::getAllUsesPair() {
	return usesTable->getAllUsesPair();
}


/*
** Singleton implementation: for Query Evaluator to get an instance of PKB
*/
PKB& PKB::getInstance() {
	static PKB instance;
	return instance;
}
