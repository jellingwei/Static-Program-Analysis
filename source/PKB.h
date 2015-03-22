#pragma once

#include<stdio.h>
#include <iostream>
#include <string>
#include <vector>
#include "VarTable.h"
#include "ProcTable.h"
#include "StmtTable.h"
#include "ModifiesTable.h"
#include "UsesTable.h"
#include "FollowsTable.h"
#include "ParentTable.h"
#include "ConstantTable.h"
#include "CallsTable.h"
#include "NextTable.h"
#include "AffectsTable.h"
#include "CFG.h"
#include "AST.h"
#include "TNode.h"
#include "Synonym.h"
#include "common.h"

using namespace std;

class TNode;

class VarTable;  // no need to #include "VarTable.h" as all I need is pointer
class StmtTable;


class PKB {
public:
	// @cond todo
	VarTable* varTable; 
	ProcTable* procTable;
	StmtTable* stmtTable;
	ConstantTable* constantTable;
	CallsTable* callsTable;
	ModifiesTable* modifiesTable;
	UsesTable* usesTable;
	FollowsTable* followsTable;
	ParentTable* parentTable;
	AST* ast;

	// @endcond

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
	vector<int> patternMatchIf(string LHS);
	int getControlVariable(int stmtNum);

	// VarTable methods
	int insertVar(string varName, int stmtNum);
	int getVarTableSize();
	string getVarName(int index);
	int getVarIndex(string varName);
	int getStmtNum(string varName);
	vector<int> getAllVarIndex();

	// ProcTable methods
	int insertProc(string procName);
	int getProcTableSize();
	string getProcName(int index);
	int getProcIndex(string procName);
	vector<int> getAllProcIndex();

	// ConstantTable methods
	bool insertConstant(int constant, int stmtNum);
	int getConstantTableSize();
	int getConstant(int index);
	int getConstantIndex(int constant);
	vector<int> getStmtNum(int constant);
	bool isConstant(int number);
	vector<int> getAllConstant();

	// CallsTable methods
	bool setCalls(int procIndex1, int procIndex2);
	bool isCalls(int procIndex1, int procIndex2, bool transitiveClosure = false);
	bool isCallsS(int procIndex1, int procIndex2);
	vector<int> getProcsCalling(int procIndex2, bool transitiveClosure = false);
	vector<int> getProcsCallingS(int procIndex2);
	vector<int> getProcsCalledBy(int procIndex1, bool transitiveClosure = false);
	vector<int> getProcsCalledByS(int procIndex1);
	pair<vector<int>, vector<int>> getAllCallsPairs(bool transitiveClosure = false);
	pair<vector<int>, vector<int>> getAllCallsPairsS();
	string getProcNameCalledByStatement(int);
	vector<int> getCallsLhs();
	vector<int> getCallsRhs();

	// StmtTable methods
	bool insertStmt(int, string, TNode*, int);
	string getType(int);
	// @cond todo
	vector<int> getStmtNumForType(string);
	// @endcond todo
	vector<int> getStmtNumForType(SYNONYM_TYPE);
	int getProcIndexForStmt(int stmtNo);
	bool isAssign(int);
	bool isWhile(int);
	bool isIf(int);
	bool isCall(int);
	int getStmtTableSize();
	TNode* getNodeForStmt(int);

	// Parent Table methods
	bool setParent(TNode* stmtNum1, TNode* stmtNum2);
	vector<int> getParent(int stmtNum2, bool transitiveClosure = false);
	vector<int> getParentS(int stmtNum2);
	vector<int> getChild(int stmtNum1, bool transitiveClosure = false);
	vector<int> getChildS(int stmtNum1);
	bool isParent(int stmtNum1, int stmtNum2, bool transitiveClosure = false);
	bool isParentS(int stmtNum1, int stmtNum2);
	pair<vector<int>, vector<int>> getAllParentPairs(bool transitiveClosure = false);
	pair<vector<int>, vector<int>> getAllParentPairsS();
	vector<int> getParentLhs();  // get LHS of Parent(_, _)
	vector<int> getParentRhs(); // get RHS of Parent(_, _)

	// Follow Table methods
	bool setFollows(TNode* stmt1, TNode* stmt2);
	vector<int> getStmtFollowedTo(int stmtNum2, bool transitiveClosure = false);
	vector<int> getStmtFollowedToS(int stmtNum2);
	vector<int> getStmtFollowedFrom(int stmtNum1, bool transitiveClosure = false);
	vector<int> getStmtFollowedFromS(int stmtNum1);
	bool isFollows(int stmtNum1, int stmtNum2, bool transitiveClosure = false);
	bool isFollowsS(int stmtNum1, int stmtNum2);
	pair<vector<int>, vector<int>> getAllFollowsPairs(bool transitiveClosure = false);
	pair<vector<int>, vector<int>> getAllFollowsPairsS();
	vector<int> getFollowsLhs();
	vector<int> getFollowsRhs();

	// Modifies Table methods
	void initModifiesTable(int numVariables);
	bool setModifies(int stmtNum, int varIndex);
	bool isModifies(int stmtNum, int varIndex);
	vector<int> getModStmtNum(int varIndex);
	vector<int> getModVarForStmt(int stmtNum);
	pair<vector<int>, vector<int>> getAllModPair();
	vector<int> getModifiesLhs();
	vector<int> getModifiesRhs();

	boost::dynamic_bitset<> getModVarInBitvectorForStmt(int stmtNum);

	bool setModifiesProc(int procIndex, int varIndex);
	bool isModifiesProc(int procIndex, int varIndex);
	vector<int> getModProcIndex(int varIndex);
	vector<int> getModVarForProc(int procIndex);
	pair<vector<int>, vector<int>> getAllModProcPair();


	// UsesTable methods
	void initUsesTable(int numVariables);
	bool setUses(int stmtNum, int varIndex);
	bool isUses(int stmtNum, int varIndex);
	vector<int> getUsesStmtNum(int varIndex);
	vector<int> getUsesVarForStmt(int stmtNum);
	pair<vector<int>, vector<int>> getAllUsesPair();
	vector<int> getUsesLhs();
	vector<int> getUsesRhs();

	boost::dynamic_bitset<> getUseVarInBitvectorForStmt(int stmtNum);

	bool setUsesProc(int procIndex, int varIndex);
	bool isUsesProc(int procIndex, int varIndex);
	vector<int> getUsesProcIndex(int varIndex);
	vector<int> getUsesVarForProc(int procIndex);
	pair<vector<int>, vector<int>> getAllUsesProcPair();

	// cfg
	vector<int> getNextAfter(int progLine1, bool transitiveClosure = false);
	vector<int> getNextBefore(int progLine2, bool transitiveClosure = false);
	bool isNext(int progLine1, int progLine2, bool transitiveClosure = false);
	vector<int> getNextAfterS(int progLine1);
	vector<int> getNextBeforeS(int progLine2);
	bool isNextS(int progLine1, int progLine2);
	vector<int> getNextLhs();
	vector<int> getNextRhs();
	CNode* getCNodeForProgLine(int progLine);

	vector<CFG*> cfgTable;

	// affects
	bool isAffects(int progLine1, int progLine2, bool transitiveClosure = false);
	vector<int> getAffectedBy(int progLine1, bool transitiveClosure = false);
	vector<int> getAffecting(int progLine2, bool transitiveClosure = false);
	vector<int> getAffectsLhs();
	vector<int> getAffectsRhs();


	//@todo 
	// @cond todo
	unordered_map<int, CNode*> cfgNodeTable; //@todo nextTable?
	unordered_map<int, TNode*> nodeTable;   // @todo change calls to nodeTable to getNodeFromStmt
	unordered_map<int, int> stmtToProcMap;  // @todo change to getProcIndexForStmt from stmtTable
	unordered_map<int, int> stmtNumToProcLineMap; // @todo not needed anymore
	// @endcond

private:
	NextTable* nextTable;
	AffectsTable* affectsTable;
	PKB();
	
	
};
