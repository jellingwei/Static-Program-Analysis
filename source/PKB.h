#pragma once

#include<stdio.h>
#include <iostream>
#include <string>
#include <vector>
#include <stack>

#include "VarTable.h"
#include "ProcTable.h"
#include "StmtTable.h"
#include "ModifiesTable.h"
#include "UsesTable.h"
#include "FollowsSolver.h"
#include "ParentSolver.h"
#include "ConstantTable.h"
#include "CallsTable.h"
#include "NextSolver.h"
#include "NextBipSolver.h"
#include "AffectsBipSolver.h"
#include "AffectsSolver.h"
#include "Contain.h"
#include "Siblings.h"
#include "CFG.h"
#include "CNode.h"
#include "AST.h"
#include "TNode.h"
#include "Synonym.h"
#include "common.h"
#include "common_list.h"


using namespace std;

class TNode;

class VarTable;  
class StmtTable;


class PKB {
public:
	static PKB& getInstance();

	// AST methods
	TNode* createTNode(TNODE_TYPE ast_node_type, STATEMENT stmtNo, VAR_INDEX idx);					
	TNode* getRoot();
	STATUS createLink(LINK_TYPE link, TNode* fromNode, TNode* toNode);
	INTEGER getChildrenSize(TNode* parent);	
	TNODE_LIST getChildrenNode(TNode* parent);
	BOOLEAN_ isChildNode(TNode* parent, TNode* child);
	BOOLEAN_ isExists(TNode* node);
	TNode* getLastImpt();
	INTEGER getASTSize();

	STATEMENT_LIST patternMatchAssign(EXPRESSION RHS);
	STATEMENT_LIST patternMatchWhile(VARNAME LHS);
	STATEMENT_LIST patternMatchIf(VARNAME LHS);
	VAR_INDEX getControlVariable(STATEMENT stmtNum);
	
	// Extended Pattern Matching for While and If
	PAIR_LIST patternMatchWhile(VARNAME LHS, SYNONYM_TYPE then);
	PAIR_LIST patternMatchIfThen(VARNAME LHS, SYNONYM_TYPE then);
	PAIR_LIST patternMatchIfElse(VARNAME LHS, SYNONYM_TYPE then);
	//STATEMENT_LIST PKB::patternMatchIf(VARNAME LHS, SYNONYM_TYPE thenS, SYNONYM_TYPE elseS);
	PAIR_OF_PAIR_LIST patternMatchIf(VARNAME LHS, SYNONYM_TYPE thenS, SYNONYM_TYPE elseS);


	// VarTable methods
	VAR_INDEX insertVar(VARNAME varName, STATEMENT stmtNum);
	INTEGER getVarTableSize();
	VARNAME getVarName(VAR_INDEX index);
	VAR_INDEX getVarIndex(VARNAME varName);
	VARINDEX_LIST getAllVarIndex();

	// ProcTable methods
	PROC_INDEX insertProc(PROCNAME procName);
	INTEGER getProcTableSize();
	PROCNAME getProcName(PROC_INDEX index);
	PROC_INDEX getProcIndex(PROCNAME procName);
	PROCINDEX_LIST getAllProcIndex();

	// ConstantTable methods
	STATUS insertConstant(VALUE constant, STATEMENT stmtNum);
	INTEGER getConstantTableSize();
	STATEMENT_LIST getStmtNum(VALUE constant);
	BOOLEAN_ isConstant(INTEGER number);
	CONSTANT_LIST getAllConstant();

	// CallsTable methods
	STATUS setCalls(PROC_INDEX procIndex1, PROC_INDEX procIndex2);
	BOOLEAN_ isCalls(PROC_INDEX procIndex1, PROC_INDEX procIndex2, TRANS_CLOSURE transitiveClosure = false);
	BOOLEAN_ isCallsS(PROC_INDEX procIndex1, PROC_INDEX procIndex2);
	PROCINDEX_LIST getProcsCalling(PROC_INDEX procIndex2, TRANS_CLOSURE transitiveClosure = false);
	PROCINDEX_LIST getProcsCallingS(PROC_INDEX procIndex2);
	PROCINDEX_LIST getProcsCalledBy(PROC_INDEX procIndex1, TRANS_CLOSURE transitiveClosure = false);
	PROCINDEX_LIST getProcsCalledByS(PROC_INDEX procIndex1);
	pair<PROGLINE_LIST, PROGLINE_LIST> getAllCallsPairs(TRANS_CLOSURE transitiveClosure = false);
	pair<PROGLINE_LIST, PROGLINE_LIST> getAllCallsPairsS();
	PROCNAME getProcNameCalledByStatement(STATEMENT stmtNum);
	PROCINDEX_LIST getCallsLhs();
	PROCINDEX_LIST getCallsRhs();
	BOOLEAN_ isCallsValid();

	// StmtTable methods
	STATUS insertStmt(STATEMENT stmtNum, STATEMENT_TYPE type, TNode* node, PROC_INDEX procIndex);
	STATEMENT_TYPE getType(STATEMENT stmtNum);
	// @cond todo
	STATEMENT_LIST getStmtNumForType(STATEMENT_TYPE type);
	// @endcond todo
	STATEMENT_LIST getStmtNumForType(SYNONYM_TYPE);
	PROC_INDEX getProcIndexForStmt(STATEMENT stmtNum);
	BOOLEAN_ isAssign(STATEMENT stmtNum);
	BOOLEAN_ isWhile(STATEMENT stmtNum);
	BOOLEAN_ isIf(STATEMENT stmtNum);
	BOOLEAN_ isCall(STATEMENT stmtNum);
	INTEGER getStmtTableSize();
	TNode* getNodeForStmt(STATEMENT stmtNum);

	// Parent Table methods
	STATUS setParent(TNode* stmtNum1, TNode* stmtNum2);
	STATEMENT_LIST getParent(STATEMENT stmtNum2, TRANS_CLOSURE transitiveClosure = false);
	STATEMENT_LIST getParentS(STATEMENT stmtNum2);
	STATEMENT_LIST getChild(STATEMENT stmtNum1, TRANS_CLOSURE transitiveClosure = false);
	STATEMENT_LIST getChildS(STATEMENT stmtNum1);
	BOOLEAN_ isParent(STATEMENT stmtNum1, STATEMENT stmtNum2, TRANS_CLOSURE transitiveClosure = false);
	BOOLEAN_ isParentS(STATEMENT stmtNum1, STATEMENT stmtNum2);
	pair<STATEMENT_LIST, STATEMENT_LIST> getAllParentPairs(TRANS_CLOSURE transitiveClosure = false);
	pair<STATEMENT_LIST, STATEMENT_LIST> getAllParentPairsS();
	STATEMENT_LIST getParentLhs();  // get LHS of Parent(_, _)
	STATEMENT_LIST getParentRhs(); // get RHS of Parent(_, _)
	BOOLEAN_ isParentValid();

	// Follow Table methods
	STATUS setFollows(TNode* stmt1, TNode* stmt2);
	STATEMENT_LIST getStmtFollowedTo(STATEMENT stmtNum2, TRANS_CLOSURE transitiveClosure = false);
	STATEMENT_LIST getStmtFollowedToS(STATEMENT stmtNum2);
	STATEMENT_LIST getStmtFollowedFrom(STATEMENT stmtNum1, TRANS_CLOSURE transitiveClosure = false);
	STATEMENT_LIST getStmtFollowedFromS(STATEMENT stmtNum1);
	BOOLEAN_ isFollows(STATEMENT stmtNum1, STATEMENT stmtNum2, TRANS_CLOSURE transitiveClosure = false);
	BOOLEAN_ isFollowsS(STATEMENT stmtNum1, STATEMENT stmtNum2);
	pair<STATEMENT_LIST, STATEMENT_LIST> getAllFollowsPairs(TRANS_CLOSURE transitiveClosure = false);
	pair<STATEMENT_LIST, STATEMENT_LIST> getAllFollowsPairsS();
	STATEMENT_LIST getFollowsLhs();
	STATEMENT_LIST getFollowsRhs();
	BOOLEAN_ isFollowsValid();

	// Modifies Table methods
	void initModifiesTable(INTEGER numVariables);
	STATUS setModifies(STATEMENT stmtNum, VAR_INDEX varIndex);
	BOOLEAN_ isModifies(STATEMENT stmtNum, VAR_INDEX varIndex);
	STATEMENT_LIST getModStmtNum(VAR_INDEX varIndex);
	VARINDEX_LIST getModVarForStmt(STATEMENT stmtNum);
	pair<STATEMENT_LIST, VARINDEX_LIST> getAllModPair();
	STATEMENT_LIST getModifiesLhs();
	STATEMENT_LIST getModifiesRhs();

	boost::dynamic_bitset<> getModVarInBitvectorForStmt(STATEMENT stmtNum);

	STATUS setModifiesProc(PROC_INDEX procIndex, VAR_INDEX varIndex);
	BOOLEAN_ isModifiesProc(PROC_INDEX procIndex, VAR_INDEX varIndex);
	PROCINDEX_LIST getModProcIndex(VAR_INDEX varIndex);
	VARINDEX_LIST getModVarForProc(PROC_INDEX procIndex);
	pair<PROCINDEX_LIST, VARINDEX_LIST> getAllModProcPair();


	// UsesTable methods
	void initUsesTable(INTEGER numVariables);
	STATUS setUses(STATEMENT stmtNum, VAR_INDEX varIndex);
	BOOLEAN_ isUses(STATEMENT stmtNum, VAR_INDEX varIndex);
	STATEMENT_LIST getUsesStmtNum(VAR_INDEX varIndex);
	VARINDEX_LIST getUsesVarForStmt(STATEMENT stmtNum);
	pair<STATEMENT_LIST, VARINDEX_LIST> getAllUsesPair();
	STATEMENT_LIST getUsesLhs();
	STATEMENT_LIST getUsesRhs();
	BOOLEAN_ isUsesValid();

	boost::dynamic_bitset<> getUseVarInBitvectorForStmt(STATEMENT stmtNum);

	STATUS setUsesProc(PROC_INDEX procIndex, VAR_INDEX varIndex);
	BOOLEAN_ isUsesProc(PROC_INDEX procIndex, VAR_INDEX varIndex);
	PROCINDEX_LIST getUsesProcIndex(VAR_INDEX varIndex);
	VARINDEX_LIST getUsesVarForProc(PROC_INDEX procIndex);
	pair<PROCINDEX_LIST, VARINDEX_LIST> getAllUsesProcPair();

	// cfg
	PROGLINE_LIST getNextAfter(PROG_LINE_ progLine1, TRANS_CLOSURE transitiveClosure = false);
	PROGLINE_LIST getNextBefore(PROG_LINE_ progLine2, TRANS_CLOSURE transitiveClosure = false);
	BOOLEAN_ isNext(PROG_LINE_ progLine1, PROG_LINE_ progLine2, TRANS_CLOSURE transitiveClosure = false);
	PROGLINE_LIST getNextAfterS(PROG_LINE_ progLine1);
	PROGLINE_LIST getNextBeforeS(PROG_LINE_ progLine2);
	BOOLEAN_ isNextS(PROG_LINE_ progLine1, PROG_LINE_ progLine2);
	PROGLINE_LIST getNextLhs();
	PROGLINE_LIST getNextRhs();
	CNode* getCNodeForProgLine(PROG_LINE_ progLine);
	BOOLEAN_ isNextValid();
	BOOLEAN_ isNextLhsValid(PROG_LINE_);
	BOOLEAN_ isNextRhsValid(PROG_LINE_);

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
	void setNextLhs(int);
	void setNextRhs(int);

	vector<CFG*> cfgTable;

	// affects
	BOOLEAN_ isAffects(PROG_LINE_ progLine1, PROG_LINE_ progLine2, TRANS_CLOSURE transitiveClosure = false);
	PROGLINE_LIST getAffectedBy(PROG_LINE_ progLine1, TRANS_CLOSURE transitiveClosure = false);
	PROGLINE_LIST getAffecting(PROG_LINE_ progLine2, TRANS_CLOSURE transitiveClosure = false);
	PROGLINE_LIST getAffectsLhs();
	PROGLINE_LIST getAffectsRhs();
	BOOLEAN_ isAffectsValid();
	BOOLEAN_ isAffectsLhsValid(PROG_LINE_);
	BOOLEAN_ isAffectsRhsValid(PROG_LINE_);

	static BOOLEAN_ canSkipNodesBackwards(CNode* node);
	static BOOLEAN_ canSkipNodesForwards(CNode* node);

	//NextBip
	BOOLEAN_ isNextBip(PROG_LINE_ progLine1, PROG_LINE_ progLine2, TRANS_CLOSURE transitiveClosure);
 	PROGLINE_LIST getNextBipAfter(PROG_LINE_ progLine1, TRANS_CLOSURE transitiveClosure = false);
	PROGLINE_LIST getNextBipBefore(PROG_LINE_ progLine2, TRANS_CLOSURE transitiveClosure = false);
	set<NEXTBIP_STATE> getNextBipAfterWithState(CNode* progLine1, stack<CNode*> afterCall);
	set<NEXTBIP_STATE> getNextBipBeforeWithState(CNode* progLine2, stack<CNode*> afterCall);
	PROGLINE_LIST getNextBipLhs();
	PROGLINE_LIST getNextBipRhs();

	//AffectsBip
	BOOLEAN_ isAffectsBip(PROG_LINE_ progLine1, PROG_LINE_ progLine2, TRANS_CLOSURE transitiveClosure);
	PROGLINE_LIST getAffectsBipAfter(PROG_LINE_ progLine1, TRANS_CLOSURE transitiveClosure = false);
	PROGLINE_LIST getAffectsBipBefore(PROG_LINE_ progLine2, TRANS_CLOSURE transitiveClosure = false);
	PROGLINE_LIST getAffectsBipLhs();
	PROGLINE_LIST getAffectsBipRhs();


	//@todo 
	// @cond todo
	unordered_map<int, CNode*> cfgNodeTable; //@todo nextTable?
	unordered_map<int, TNode*> nodeTable;   // @todo change calls to nodeTable to getNodeFromStmt
	unordered_map<int, int> stmtToProcMap;  // @todo change to getProcIndexForStmt from stmtTable
	unordered_map<int, int> stmtNumToProcLineMap; // @todo not needed anymore
	// @endcond

	// contains 
	PAIR_LIST contains(SYNONYM_TYPE parentType, SYNONYM_TYPE childType, bool transitiveClosure);
	VALUE_LIST contains(int stmtNo, SYNONYM_TYPE childType, bool transitiveClosure);
	VALUE_LIST contains(SYNONYM_TYPE predecessorType, int stmtNo, bool transitiveClosure);

	// siblings
	PAIR_LIST siblings(SYNONYM_TYPE first_siblingType, SYNONYM_TYPE second_siblingType);
	VALUE_LIST siblings(int stmtNo, SYNONYM_TYPE second_siblingType);
	VALUE_LIST siblings(SYNONYM_TYPE first_siblingType, int stmtNo);


private:
	VarTable* varTable; 
	ProcTable* procTable;
	StmtTable* stmtTable;
	ConstantTable* constantTable;
	CallsTable* callsTable;
	ModifiesTable* modifiesTable;
	UsesTable* usesTable;
	FollowsSolver* followsTable;
	ParentSolver* parentTable;
	AST* ast;
	NextSolver* nextTable;
	AffectsSolver* affectsTable;
	NextBipSolver* nextBipTable;
	AffectsBipSolver* affectsBipTable;
	class Contain* contain;
	class Siblings* sibling;
	PKB();
	
	
};

