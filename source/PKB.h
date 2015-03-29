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
#include "common_list.h"

using namespace std;

class TNode;

class VarTable;  // no need to #include "VarTable.h" as all I need is pointer
class StmtTable;


class PKB {
public:
	// @cond todo


	// @endcond

	static PKB& getInstance();

	// AST methods
	TNode* createTNode(TNODE_TYPE ast_node_type, STATEMENT stmtNo, VAR_INDEX idx);					
	TNode* getRoot();
	STATUS createLink(LINK_TYPE link, TNode* fromNode, TNode* toNode);
	INTEGER getChildrenSize(TNode* parent);	
	vector<TNode*>* getChildrenNode(TNode* parent);
	BOOLEAN_ isChildNode(TNode* parent, TNode* child);
	BOOLEAN_ isExists(TNode* node);
	TNode* getLastImpt();
	INTEGER getASTSize();

	STATEMENT_LIST patternMatchAssign(EXPRESSION RHS);
	STATEMENT_LIST patternMatchWhile(VARNAME LHS);
	STATEMENT_LIST patternMatchIf(VARNAME LHS);
	VAR_INDEX getControlVariable(STATEMENT stmtNum);

	// VarTable methods
	VAR_INDEX insertVar(VARNAME varName, STATEMENT stmtNum);
	INTEGER getVarTableSize();
	VARNAME getVarName(VAR_INDEX index);
	VAR_INDEX getVarIndex(VARNAME varName);
	STATEMENT getStmtNum(VARNAME varName);
	vector<int> getAllVarIndex();

	// ProcTable methods
	PROC_INDEX insertProc(PROCNAME procName);
	INTEGER getProcTableSize();
	PROCNAME getProcName(PROC_INDEX index);
	PROC_INDEX getProcIndex(PROCNAME procName);
	PROGLINE_LIST getAllProcIndex();

	// ConstantTable methods
	STATUS insertConstant(VALUE constant, STATEMENT stmtNum);
	INTEGER getConstantTableSize();
	VALUE getConstant(VAR_INDEX index);
	VAR_INDEX getConstantIndex(VALUE constant);
	STATEMENT_LIST getStmtNum(VALUE constant);
	BOOLEAN_ isConstant(INTEGER number);
	CONSTANT_LIST getAllConstant();

	// CallsTable methods
	STATUS setCalls(PROC_INDEX procIndex1, PROC_INDEX procIndex2);
	BOOLEAN_ isCalls(PROC_INDEX procIndex1, PROC_INDEX procIndex2, bool transitiveClosure = false);
	BOOLEAN_ isCallsS(PROC_INDEX procIndex1, PROC_INDEX procIndex2);
	PROGLINE_LIST getProcsCalling(PROC_INDEX procIndex2, bool transitiveClosure = false);
	PROGLINE_LIST getProcsCallingS(PROC_INDEX procIndex2);
	PROGLINE_LIST getProcsCalledBy(PROC_INDEX procIndex1, bool transitiveClosure = false);
	PROGLINE_LIST getProcsCalledByS(PROC_INDEX procIndex1);
	pair<PROGLINE_LIST, PROGLINE_LIST> getAllCallsPairs(bool transitiveClosure = false);
	pair<PROGLINE_LIST, PROGLINE_LIST> getAllCallsPairsS();
	string getProcNameCalledByStatement(STATEMENT);
	PROGLINE_LIST getCallsLhs();
	PROGLINE_LIST getCallsRhs();

	// StmtTable methods
	STATUS insertStmt(STATEMENT, STATEMENT_TYPE, TNode*, PROC_INDEX);
	STATEMENT_TYPE getType(STATEMENT);
	// @cond todo
	STATEMENT_LIST getStmtNumForType(STATEMENT_TYPE);
	// @endcond todo
	STATEMENT_LIST getStmtNumForType(SYNONYM_TYPE);
	PROC_INDEX getProcIndexForStmt(STATEMENT stmtNo);
	BOOLEAN_ isAssign(STATEMENT);
	BOOLEAN_ isWhile(STATEMENT);
	BOOLEAN_ isIf(STATEMENT);
	BOOLEAN_ isCall(STATEMENT);
	INTEGER getStmtTableSize();
	TNode* getNodeForStmt(STATEMENT);

	// Parent Table methods
	STATUS setParent(TNode* stmtNum1, TNode* stmtNum2);
	STATEMENT_LIST getParent(STATEMENT stmtNum2, bool transitiveClosure = false);
	STATEMENT_LIST getParentS(STATEMENT stmtNum2);
	STATEMENT_LIST getChild(STATEMENT stmtNum1, bool transitiveClosure = false);
	STATEMENT_LIST getChildS(STATEMENT stmtNum1);
	BOOLEAN_ isParent(STATEMENT stmtNum1, STATEMENT stmtNum2, bool transitiveClosure = false);
	BOOLEAN_ isParentS(STATEMENT stmtNum1, STATEMENT stmtNum2);
	pair<STATEMENT_LIST, STATEMENT_LIST> getAllParentPairs(bool transitiveClosure = false);
	pair<STATEMENT_LIST, STATEMENT_LIST> getAllParentPairsS();
	STATEMENT_LIST getParentLhs();  // get LHS of Parent(_, _)
	STATEMENT_LIST getParentRhs(); // get RHS of Parent(_, _)

	// Follow Table methods
	STATUS setFollows(TNode* stmt1, TNode* stmt2);
	STATEMENT_LIST getStmtFollowedTo(STATEMENT stmtNum2, bool transitiveClosure = false);
	STATEMENT_LIST getStmtFollowedToS(STATEMENT stmtNum2);
	STATEMENT_LIST getStmtFollowedFrom(STATEMENT stmtNum1, bool transitiveClosure = false);
	STATEMENT_LIST getStmtFollowedFromS(STATEMENT stmtNum1);
	BOOLEAN_ isFollows(STATEMENT stmtNum1, STATEMENT stmtNum2, bool transitiveClosure = false);
	BOOLEAN_ isFollowsS(STATEMENT stmtNum1, STATEMENT stmtNum2);
	pair<STATEMENT_LIST, STATEMENT_LIST> getAllFollowsPairs(bool transitiveClosure = false);
	pair<STATEMENT_LIST, STATEMENT_LIST> getAllFollowsPairsS();
	STATEMENT_LIST getFollowsLhs();
	STATEMENT_LIST getFollowsRhs();

	// Modifies Table methods
	void initModifiesTable(INTEGER numVariables);
	STATUS setModifies(STATEMENT stmtNum, VAR_INDEX varIndex);
	STATUS isModifies(STATEMENT stmtNum, VAR_INDEX varIndex);
	STATEMENT_LIST getModStmtNum(VAR_INDEX varIndex);
	STATEMENT_LIST getModVarForStmt(STATEMENT stmtNum);
	pair<STATEMENT_LIST, STATEMENT_LIST> getAllModPair();
	STATEMENT_LIST getModifiesLhs();
	STATEMENT_LIST getModifiesRhs();

	boost::dynamic_bitset<> getModVarInBitvectorForStmt(STATEMENT stmtNum);

	STATUS setModifiesProc(PROC_INDEX procIndex, VAR_INDEX varIndex);
	BOOLEAN_ isModifiesProc(PROC_INDEX procIndex, VAR_INDEX varIndex);
	vector<int> getModProcIndex(VAR_INDEX varIndex);
	vector<int> getModVarForProc(PROC_INDEX procIndex);
	pair<vector<int>, vector<int>> getAllModProcPair();


	// UsesTable methods
	void initUsesTable(INTEGER numVariables);
	STATUS setUses(STATEMENT stmtNum, VAR_INDEX varIndex);
	BOOLEAN_ isUses(STATEMENT stmtNum, VAR_INDEX varIndex);
	STATEMENT_LIST getUsesStmtNum(VAR_INDEX varIndex);
	vector<int> getUsesVarForStmt(STATEMENT stmtNum);
	pair<STATEMENT_LIST, vector<int>> getAllUsesPair();
	STATEMENT_LIST getUsesLhs();
	STATEMENT_LIST getUsesRhs();

	boost::dynamic_bitset<> getUseVarInBitvectorForStmt(STATEMENT stmtNum);

	STATUS setUsesProc(PROC_INDEX procIndex, VAR_INDEX varIndex);
	BOOLEAN_ isUsesProc(PROC_INDEX procIndex, VAR_INDEX varIndex);
	vector<int> getUsesProcIndex(VAR_INDEX varIndex);
	vector<int> getUsesVarForProc(PROC_INDEX procIndex);
	pair<vector<int>, vector<int>> getAllUsesProcPair();

	// cfg
	vector<int> getNextAfter(PROG_LINE_ progLine1, bool transitiveClosure = false);
	vector<int> getNextBefore(PROG_LINE_ progLine2, bool transitiveClosure = false);
	BOOLEAN_ isNext(PROG_LINE_ progLine1, PROG_LINE_ progLine2, bool transitiveClosure = false);
	vector<int> getNextAfterS(PROG_LINE_ progLine1);
	vector<int> getNextBeforeS(PROG_LINE_ progLine2);
	BOOLEAN_ isNextS(PROG_LINE_ progLine1, PROG_LINE_ progLine2);
	vector<int> getNextLhs();
	vector<int> getNextRhs();
	CNode* getCNodeForProgLine(PROG_LINE_ progLine);

	// functions for precomputations for next
	int getFirstProgLineInProc(int);
	int getLastProgLineInProc(int);
	void setFirstProgLineInProc(int, int);
	void setLastProgLineInProc(int, int);

	int getFirstProgLineInContainer(int);
	int getLastProgLineInContainer(int);
	void setFirstProgLineInElse(int, int);
	void setLastProgLineInContainer(int, int);
	void setProgLineInWhile(int);

	vector<CFG*> cfgTable;

	// affects
	BOOLEAN_ isAffects(PROG_LINE_ progLine1, PROG_LINE_ progLine2, bool transitiveClosure = false);
	vector<int> getAffectedBy(PROG_LINE_ progLine1, bool transitiveClosure = false);
	vector<int> getAffecting(PROG_LINE_ progLine2, bool transitiveClosure = false);
	vector<int> getAffectsLhs();
	vector<int> getAffectsRhs();

	static bool canSkipNodesBackwards(CNode* node);
	static bool canSkipNodesForwards(CNode* node);


	//@todo 
	// @cond todo
	unordered_map<int, CNode*> cfgNodeTable; //@todo nextTable?
	unordered_map<int, TNode*> nodeTable;   // @todo change calls to nodeTable to getNodeFromStmt
	unordered_map<int, int> stmtToProcMap;  // @todo change to getProcIndexForStmt from stmtTable
	unordered_map<int, int> stmtNumToProcLineMap; // @todo not needed anymore
	// @endcond

private:
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
	NextTable* nextTable;
	AffectsTable* affectsTable;
	PKB();
	
	
};
