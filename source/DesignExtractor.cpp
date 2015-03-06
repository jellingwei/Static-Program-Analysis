#pragma once

#include<stdio.h>
#include <iostream>
#include <string>
#include <vector>
#include <stack>
#include <unordered_set>
#include <numeric>
#include <algorithm>
#include <cassert>

#include "DesignExtractor.h"
#include "TNode.h"
#include "PKB.h"


using namespace std;


DesignExtractor::DesignExtractor() {
	PKB pkb = PKB::getInstance();
	// assert that the parser did not do a bad job
	assert(pkb.getRoot()->getChildren()->size() == pkb.getProcTableSize());

	pkb.initUsesTable(pkb.getAllVarIndex().size() + 1);
	pkb.initModifiesTable(pkb.getAllVarIndex().size() + 1);
}

vector<int> dfsForProcedures(int startProc, vector<int>* allProcs, unordered_set<int>* visited) {
	PKB pkb = PKB::getInstance();

	vector<int> result;
	
	deque<int> frontier;
	frontier.push_back(startProc);

	while (!frontier.empty()) {
		int curProc = frontier.back(); frontier.pop_back();
		visited->insert(curProc);
		result.push_back(curProc);

		// update list of procedures to only contain unvisited procs
		auto position = find(allProcs->begin(), allProcs->end(), curProc);
		if (position != allProcs->end()) {
			allProcs->erase(position);
		}

		// add new procs to frontier
		vector<int> intermediateProcs = pkb.getProcsCalledBy(curProc, false);
		for (auto iter = intermediateProcs.begin(); iter != intermediateProcs.end(); ++iter) {
			if (visited->count(*iter) != 0 || find(frontier.begin(), frontier.end(), *iter) != frontier.end()) {
				// skip if already visited, or if already in the frontier
				continue;
			}
			frontier.push_back(*iter);
		}
	}

	reverse(result.begin(), result.end());
	return result;
}

/**
 * Return the called procedures in topological order
 */
vector<int> getCallsInTopologicalOrder() {
	PKB pkb = PKB::getInstance();
	vector<int> result;
	
	int startProc = 0;

	vector<int> allProcs = pkb.getAllProcIndex();; // used to check which procs hasn't been included
	unordered_set<int> visited;

	result = dfsForProcedures(startProc, &allProcs, &visited);
	
	if (!allProcs.empty()) {
		startProc = allProcs.front();
		vector<int> newResult = dfsForProcedures(startProc, &allProcs, &visited);
		result.insert(result.end(), newResult.begin(), newResult.end()); // append newResult behind result
	}

	return result;
}

class CallComparator {
public:
	CallComparator(vector<int> topoOrder) {
		this->topoOrder = topoOrder; 
	}

	/**
	 * Sorts the call statements by the order given in the constructor
	 */
	bool operator() (TNode* i, TNode* j) { 
		assert(i->getNodeType() == Call);
		assert(j->getNodeType() == Call);

		PKB pkb = PKB::getInstance();
		int iProc = pkb.stmtToProcMap.at(i->getStmtNumber());
		int jProc = pkb.stmtToProcMap.at(j->getStmtNumber());

		int iPosition = find(topoOrder.begin(), topoOrder.end(), iProc) - topoOrder.begin();
		int jPosition = find(topoOrder.begin(), topoOrder.end(), jProc) - topoOrder.begin();
		return iPosition < jPosition;
	}
private:
	vector<int> topoOrder;
};

/**
 * Sorts the call statements in topological ordering
 */
vector<TNode*> DesignExtractor::obtainCallStatementsInTopologicalOrder() {
	vector<int> topologicalOrder = getCallsInTopologicalOrder();

	PKB pkb = PKB::getInstance();

	vector<int> allCallStatementsNum = pkb.getStmtNumForType(CALL);
	
	// obtain every call statement node
	vector<TNode*> callStatementNodes;
	for (auto iter = allCallStatementsNum.begin(); iter != allCallStatementsNum.end(); ++iter) {
		TNode* node = pkb.getNodeForStmt(*iter);  assert(node != NULL);
		callStatementNodes.push_back(node);
	}

	// sort all call statements 
	CallComparator compare(topologicalOrder);
	sort(callStatementNodes.begin(), callStatementNodes.end(), compare);
	
	return callStatementNodes;
}

/**
 * For the input call statements nodes, set Modifies for them as well as all their ancestors
 * This function writes into the PKB
 * @param callStmt a vector of sorted TNodes in order of setting Modifies on them
 */
void DesignExtractor::setModifiesForCallStatements(vector<TNode*> callStmt) {
	PKB pkb = PKB::getInstance();

	for (auto stmt = callStmt.begin(); stmt != callStmt.end(); ++stmt) {
		int procCalled = (*stmt)->getNodeValueIdx();
		int stmtNumber = (*stmt)->getStmtNumber();
		int currentProc = pkb.stmtToProcMap.at(stmtNumber);

		vector<int> varModifiedByProcCalled = pkb.getModVarForProc(procCalled);

		// set modifies for the (current statement, all variables modified by the function called)
		for (auto iter = varModifiedByProcCalled.begin(); iter != varModifiedByProcCalled.end(); ++iter) {
			pkb.setModifies(stmtNumber, *iter);
		}
		
		// for ancestors, set (parent, all variabled modified by the function called)
		while (pkb.getParent(stmtNumber).size()) 
		{
			stmtNumber = pkb.getParent(stmtNumber).at(0);
			if (stmtNumber <= 0) {
				continue;
			}
			for (auto iter = varModifiedByProcCalled.begin(); iter != varModifiedByProcCalled.end(); ++iter) {
				pkb.setModifies(stmtNumber, *iter);
			}			
		}
	}
}

/**
 * For the input call statements nodes, set Uses for them as well as all their ancestors.
 * This function writes into the PKB
 * @param callStmt a vector of sorted TNodes in order of setting Uses on them
 */
void DesignExtractor::setUsesForCallStatements(vector<TNode*> callStmt) {

	PKB pkb = PKB::getInstance();

	for (auto stmt = callStmt.begin(); stmt != callStmt.end(); ++stmt) {
		int procCalled = (*stmt)->getNodeValueIdx();
		int stmtNumber = (*stmt)->getStmtNumber();
		int currentProc = pkb.stmtToProcMap.at(stmtNumber);

		vector<int> varUsedByProcCalled = pkb.getUsesVarForProc(procCalled);

		// set uses for the (current statement, all variables modified by the function called)
		for (auto iter = varUsedByProcCalled.begin(); iter != varUsedByProcCalled.end(); ++iter) {
			pkb.setUses(stmtNumber, *iter);
		}
		
		// for ancestors, set (parent, all variabled modified by the function called)
		while (pkb.getParent(stmtNumber).size()) {
			stmtNumber = pkb.getParent(stmtNumber).at(0);
			if (stmtNumber <= 0) {
				continue;
			}
			for (auto iter = varUsedByProcCalled.begin(); iter != varUsedByProcCalled.end(); ++iter) {
				pkb.setUses(stmtNumber, *iter);
			}			
		}
	}
}

/**
 * Comparator function for sorting procedure nodes in ascending order
 */
bool procNodesCompare(TNode* node1, TNode* node2) {
	assert(node1->getNodeType() == Procedure);
	assert(node2->getNodeType() == Procedure);

	// node value of proc nodes is its proc index
	return node1->getNodeValueIdx() < node2->getNodeValueIdx();
}


CNode* createNextNode(TNode* nextStmt, CFG* cfg, CNode* header = NULL) {
	PKB& pkb = PKB::getInstance();

	CNODE_TYPE cNodeType = CFG::convertTNodeTypeToCNodeType(nextStmt->getNodeType());
	//int progLine = pkb.stmtNumToProcLineMap.at(nextStmt->getStmtNumber());
	int progLine = nextStmt->getStmtNumber(); 
	CNode* nextCNode = cfg->createCNode(cNodeType, 
										progLine,
										header, nextStmt);
	
	if (cNodeType == While_C || cNodeType == If_C || 
	cNodeType == Assign_C || cNodeType == Call_C) {

		pair<int, CNode*> progLineToNodePair(progLine, nextCNode);
		pkb.cfgNodeTable.insert(progLineToNodePair);
		
	}

	return nextCNode;
}

CNode* createDummyEndIfNode(CFG* cfg, TNode* ifStmtListNode, CNode* lastNodeInIf, CNode* lastNodeInElse, CNode* curCNode) {
	CNode* IfEndNode = cfg->createCNode(EndIf_C, 
										NULL,
										NULL, ifStmtListNode);

			
	cfg->createLink(After, lastNodeInIf, IfEndNode);
	if (lastNodeInElse) {
		// link the last node in the else stmtlist, if it exists
		cfg->createLink(After, lastNodeInElse, IfEndNode);
	} else {
		// otherwise, link the if node itself
		cfg->createLink(After, curCNode, IfEndNode);
	}
	return IfEndNode;
}

/**
 * Construct cfg for a stmtlist. 
 * 
 * @param cfg a cfg that will be modified in this function.
 * @return the last CNode created in the stmtlist
 */
CNode* constructCfgForStmtList(TNode* stmtListNode, CNode* startCNode, CFG* cfg, bool isInsideElse = false) {
	PKB pkb = PKB::getInstance();
	assert(stmtListNode->getNodeType() == StmtLst);

	// based on CNode, use Inside2 if is inside Else.
	// @todo think about if distinguishing between if and else is really needed
	CLINK_TYPE typeOfLinkToContainer = (isInsideElse)? Inside2 : Inside;

	// link startCNode to the first node in the stmtlist
	TNode* curStmt = stmtListNode->getChildren()->at(0);
	CNode* curCNode = startCNode;
	CNode* firstCNode = createNextNode(curStmt, cfg);

    cfg->createLink(After, curCNode, firstCNode);
	cfg->createLink(typeOfLinkToContainer, curCNode, firstCNode);
	
	curCNode = firstCNode;
	// iterate over all stmts in the stmtlist
	while (curStmt != NULL) {
		cfg->createLink(typeOfLinkToContainer, curCNode, firstCNode);

		if (curStmt->getNodeType() == Assign || curStmt->getNodeType() == Call) {
			// for assign and call statements,
			// just create nodes and link together
			TNode* nextStmt = curStmt->getRightSibling();
			if (!nextStmt) {
				break;
			}

			CNode* nextCNode = createNextNode(nextStmt, cfg, startCNode);
			cfg->createLink(After, curCNode, nextCNode);  

			curStmt = nextStmt;
			curCNode = nextCNode;
		} else if (curStmt->getNodeType() == If) {
			// create cnodes for the statements in the ifthen stmtlist
			TNode* ifStmtListNode = curStmt->getChildren()->at(1);
			CNode* lastNodeInIf = constructCfgForStmtList(ifStmtListNode, curCNode, cfg);

			// create cnodes for the statements in the else stmtlist
			CNode* lastNodeInElse = NULL;
			if (curStmt->getChildren()->size() == 3) {
				TNode* elseStmtListNode = curStmt->getChildren()->at(2);
				bool isProgLinesInElse = true;
				lastNodeInElse = constructCfgForStmtList(elseStmtListNode, curCNode, cfg, isProgLinesInElse);
			}

			// create a dummy CNode to terminate the if statement with.
			// this is to keep things predictable and not allow any node to have too many nodes "before" it
			// can also be used to store additional information
			CNode* IfEndNode = createDummyEndIfNode(cfg, ifStmtListNode, lastNodeInIf, lastNodeInElse, curCNode);
			assert(IfEndNode->getBefore()->size() == 2);

			// link the dummy end node to the next stmt, if it exist
			TNode* nextStmtNode = curStmt->getRightSibling(); 
			if (!nextStmtNode) {
				curCNode = IfEndNode;
				break;
			}
			CNode* nextCNode = createNextNode(nextStmtNode, cfg, startCNode);
			cfg->createLink(After, IfEndNode, nextCNode);

			curStmt = nextStmtNode;
			curCNode = nextCNode;
		} else if (curStmt->getNodeType() == While) {
			// create cnodes for the statements in the while stmtlist
			TNode* whileStmtListNode = curStmt->getChildren()->at(1);
			CNode* lastNodeInWhile = constructCfgForStmtList(whileStmtListNode, curCNode, cfg);

			// link last node back to original While stmt
			cfg->createLink(After, lastNodeInWhile, curCNode);

			// link the end node to the next stmt, if it exist
			TNode* nextStmtNode = curStmt->getRightSibling();
			if (!nextStmtNode) {
				break;
			}
			CNode* nextCNode = createNextNode(nextStmtNode, cfg, startCNode);
			cfg->createLink(After, curCNode, nextCNode);
			
			curStmt = nextStmtNode;   
			curCNode = nextCNode;
		}
	}
	// return the last node created
	return curCNode;
}

/**
 * Build Control Flow Graph
 */
bool DesignExtractor::constructCfg() {
	// make a CFG(?) for every procedure
	PKB pkb = PKB::getInstance();
	
	TNode* rootNode = pkb.getRoot();
	assert(rootNode->getNodeType() == Program);

	vector<TNode*>* procNodes = rootNode->getChildren(); // all procedure nodes are children of the root program node
	  
	// construct cfg for every procedures in order of increasing procIndex
	sort(procNodes->begin(), procNodes->end(), procNodesCompare);
	for (auto procNode = procNodes->begin(); procNode != procNodes->end(); ++procNode) {
		CFG* cfg = new CFG(*procNode);
		TNode* procStmtListNode = (*procNode)->getChildren()->at(0);
		assert(procStmtListNode->getNodeType() == StmtLst);

		CNode* lastNode = constructCfgForStmtList(procStmtListNode, cfg->getProcRoot(), cfg);
		cfg->setEndNode(lastNode);
		
		pkb.cfgTable.push_back(cfg);
	}

	return true;
}

void DesignExtractor::constructStatisticsTable() {
	throw exception("Not implemented yet");
}

/**
 * Sets Modifies for assignment statements, and propagate the changes to their ancestors
 */
void DesignExtractor::setModifiesForAssignmentStatements() {

	PKB pkb = PKB::getInstance();

	vector<int> assignStmts = pkb.getStmtNumForType(ASSIGN);
	for (auto iter = assignStmts.begin(); iter != assignStmts.end(); ++iter) {
		int stmtNumber = *iter;

		// find variable modified
		TNode* node = pkb.getNodeForStmt(stmtNumber);
		assert(node != NULL);
		assert(node->getChildren()->size() == 2);

		int varIndex = node->getChildren()->at(0)->getNodeValueIdx();
		pkb.setModifies(stmtNumber, varIndex);

		// for ancestors, set (parent, all variable)
		while (pkb.getParent(stmtNumber).size()) {
			stmtNumber = pkb.getParent(stmtNumber).at(0);
			if (stmtNumber <= 0) {
				continue;
			}
			pkb.setModifies(stmtNumber, varIndex);	
		}
	}
}

vector<int> obtainVarUsedInExpression(TNode* node) {
	vector<int> varUsed;

	deque<TNode*> frontier;
	frontier.push_back(node->getChildren()->at(1));
	TNode* curNode;
	// bfs
	while (!frontier.empty()) {
		curNode = frontier.back(); frontier.pop_back();

		if (curNode->hasChild()) {
			vector<TNode*>* children = curNode->getChildren();
			// add new nodes to frontier
			for (auto iter = children->begin(); iter != children->end(); ++iter) {
				frontier.push_back(*iter);
			}
		}
		// add any variables encountered to varUsed
		if (curNode->getNodeType() == Variable) {
			varUsed.push_back(curNode->getNodeValueIdx());
		}
	}
	return varUsed;
}

/**
 * Sets Uses for assignment statements, and propagate the changes to their ancestors
 */
void DesignExtractor::setUsesForAssignmentStatements() {

	PKB pkb = PKB::getInstance();

	vector<int> assignStmts = pkb.getStmtNumForType(ASSIGN);
	for (auto iter = assignStmts.begin(); iter != assignStmts.end(); ++iter) {
		int stmtNumber = *iter;

		// find variable used
		TNode* node = pkb.getNodeForStmt(stmtNumber);
		assert(node != NULL);
		vector<int> varIndexesUsed = obtainVarUsedInExpression(node);
		for (auto varIter = varIndexesUsed.begin(); varIter != varIndexesUsed.end(); ++varIter) {
			pkb.setUses(stmtNumber, *varIter);
		}

		// for ancestors, set (parent, all variable)
		while (pkb.getParent(stmtNumber).size()) {
			stmtNumber = pkb.getParent(stmtNumber).at(0);
			if (stmtNumber <= 0) {
				continue;
			}
			for (auto varIter = varIndexesUsed.begin(); varIter != varIndexesUsed.end(); ++varIter) {
				pkb.setUses(stmtNumber, *varIter);
			}		
		}
	}
}

/**
 * Sets Uses for While and If statements, and propagate the changes to their ancestors
 */
void DesignExtractor::setUsesForContainerStatements() {
	PKB pkb = PKB::getInstance();

	vector<int> containerStmts = pkb.getStmtNumForType(IF);
	vector<int> whileStmts = pkb.getStmtNumForType(WHILE);

	containerStmts.insert(containerStmts.end(), whileStmts.begin(), whileStmts.end());

	for (auto iter = containerStmts.begin(); iter != containerStmts.end(); ++iter) {
		int stmtNumber = *iter;

		// find variable used
		TNode* node = pkb.getNodeForStmt(stmtNumber);
		assert(node != NULL);
		assert(node->getChildren()->size() == 2 || node->getChildren()->size() == 3);

		int varIndex = node->getChildren()->at(0)->getNodeValueIdx();
		pkb.setUses(stmtNumber, varIndex);

		// for ancestors, set (parent, all variable)
		while (pkb.getParent(stmtNumber).size()) {
			stmtNumber = pkb.getParent(stmtNumber).at(0);
			if (stmtNumber <= 0) {
				continue;
			}
			
			pkb.setUses(stmtNumber, varIndex);
		}
	}
}
