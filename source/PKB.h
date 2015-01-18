#pragma once

#include<stdio.h>
#include <iostream>
#include <string>
#include <vector>
#include "VarTable.h"
#include "StmtTable.h"
#include "ModifiesTable.h"
#include "UsesTable.h"
#include "FollowsTable.h"
#include "ParentTable.h"
#include "ConstantTable.h"
#include "AST.h"
#include "TNode.h"

using namespace std;
typedef short PROC;

class TNode;

class VarTable;  // no need to #include "VarTable.h" as all I need is pointer
class StmtTable;


class PKB {
public:
	// @Todo make all tables private so that PKB is a real facade
	VarTable* varTable; 
	StmtTable* stmtTable;
	ConstantTable* constantTable;
	ModifiesTable* modifiesTable;
	UsesTable* usesTable;
	FollowsTable* followsTable;
	ParentTable* parentTable;
	AST* ast;

	static PKB& getInstance();

	// AST methods
	TNode* createTNode(TNODE_TYPE ast_node_type, int stmtNo, int idx);					
	TNode* getRoot();
	bool createLink(LINK_TYPE link, TNode* fromNode, TNode* toNode);
	int getChildrenSize(TNode* parent);	
	vector<TNode*>* getChildrenNode(TNode* parent);
	bool isChildNode(TNode* parent, TNode* child);
	bool isExists(TNode* node);
	TNode* getLastImpt();
	int getASTSize();

	vector<int> patternMatchAssign(string RHS);
	vector<int> patternMatchWhile(string LHS);
	int getControlVariable(int stmtNum);

	// VarTable methods
	int insertVar(string varName, int stmtNum);
	int getVarTableSize();
	string getVarName(int index);
	int getVarIndex(string varName);
	int getStmtNum(string varName);
	vector<int> getAllVarIndex();

	// ConstantTable methods
	bool insertConstant(int constant, int stmtNum);
	int getConstantTableSize();
	int getConstant(int index);
	int getConstantIndex(int constant);
	vector<int> getStmtNum(int constant);
	bool isConstant(int number);
	vector<int> getAllConstant();

	// StmtTable methods
	bool insertStmt(int, string);
	string getType(int);
	vector<int> getStmtNumForType(string);
	bool isAssign(int);
	bool isWhile(int);
	int getStmtTableSize();

	// Parent Table methods
	bool setParent(TNode* stmtNum1, TNode* stmtNum2);
	vector<int> getParent(int stmtNum2, bool transitiveClosure = false);
	vector<int> getChild(int stmtNum1, bool transitiveClosure = false);
	bool isParent(int stmtNum1, int stmtNum2, bool transitiveClosure = false);
	pair<vector<int>, vector<int>> getAllParentPairs(bool transitiveClosure = false);

	// Follow Table methods
	bool setFollows(TNode* stmt1, TNode* stmt2);
	vector<int> getStmtFollowedTo(int stmtNum2, bool transitiveClosure = false);
	vector<int> getStmtFollowedFrom(int stmtNum1, bool transitiveClosure = false);
	bool isFollows(int stmtNum1, int stmtNum2, bool transitiveClosure = false);
	pair<vector<int>, vector<int>> getAllFollowsPairs(bool transitiveClosure = false);

	// ModifiesTable methods
	bool setModifies(int stmtNum, int varIndex);
	bool isModifies(int stmtNum, int varIndex);
	vector<int> getModStmtNum(int varIndex);
	vector<int> getModVarForStmt(int stmtNum);
	pair<vector<int>, vector<int>> getAllModPair();

	// UsesTable methods
	bool setUses(int stmtNum, int varIndex);
	bool isUses(int stmtNum, int varIndex);
	vector<int> getUsesStmtNum(int varIndex);
	vector<int> getUsesVarForStmt(int stmtNum);
	pair<vector<int>, vector<int>> getAllUsesPair();

	
	unordered_map<int, TNode*> nodeTable;

private:
	PKB();
	
	
};
