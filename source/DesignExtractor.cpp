#pragma once

#include<stdio.h>
#include <iostream>
#include <string>
#include <vector>
#include <stack>
#include <unordered_set>
#include <numeric>
#include <algorithm>
#include <queue>
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

/**
* @param startProc the procedure index of a procedure calling other procedures
* @param allProcs a list of all unvisited procedures
* @param visited a list of all procedures visited
* @return a list of all procedures being called by startProc.
*/
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
 * @return a list of the called procedures in topological order
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
	 * Sorts the call statements by the order given in the constructor.
	 * @param i a TNode
	 * @param j a TNode
	 * @return TRUE if the call statements are sorted by the given order. 
	 *		   FALSE if the call statements are not sorted by the given order. 
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
 * Sorts the call statements in topological ordering.
 * @return a list of all the call statements in topological ordering.
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
 * For the input call statements nodes, set Modifies for them as well as all their ancestors.
 * This function writes into the PKB.
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
 * Comparator function for sorting procedure nodes in ascending order.
 * @param node1 a TNode of Procedure node type
 * @param node2 a TNode of Procedure node type
 * @return TRUE if node1 has a lower procedure index than node2.
 *		   FALSE if node1 does not have a lower procedure index than node2.
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

	/*
	vector<CNode*>* insideNodes = curCNode->getInside();
	for (auto iter = insideNodes->begin(); iter != insideNodes->end(); ++iter) {
		CNode* insideNode = *iter;
		cfg->createLink(Inside, IfEndNode, insideNode);
	}*/

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

		if (curStmt->getNodeType() == Assign || curStmt->getNodeType() == Call) {
			// for assign and call statements,
			// just create nodes and link together
			TNode* nextStmt = curStmt->getRightSibling();
			if (!nextStmt) {
				break;
			}

			CNode* nextCNode = createNextNode(nextStmt, cfg, startCNode);
			cfg->createLink(After, curCNode, nextCNode);  
			cfg->createLink(typeOfLinkToContainer, startCNode, nextCNode);

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
			cfg->createLink(typeOfLinkToContainer, startCNode, nextCNode);

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
			cfg->createLink(typeOfLinkToContainer, startCNode, nextCNode);
			
			curStmt = nextStmtNode;   
			curCNode = nextCNode;
		}
	}
	// return the last node created
	return curCNode;
}


/**
 * Build Control Flow Graph(CFG).
 * @return TRUE if the CFG is built successfully.
 *		   FALSE if the CFG is not built successfully.
 */
bool DesignExtractor::constructCfg() {
	// make a CFG(?) for every procedure
	PKB& pkb = PKB::getInstance();
	
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

/**
 * For use in the priority queue when traversing the CFG
 */
class CompareProglines {
    public:
    bool operator() (CNode* node1, CNode* node2) { 
       return node1->getProcLineNumber() < node2->getProcLineNumber(); 
    }
};


void addVariablesToMap(int progLineNum, boost::dynamic_bitset<> variables, unordered_map<int, set<int>>& reachingDefinitions) {
	int startingVarIndex = 1;
	for (size_t i = startingVarIndex; i < variables.size(); i++) {		
		if (variables[i] == 1) {
			reachingDefinitions[i].insert(progLineNum);
		}
	}
}

/**
 * Traverse through the CFG using priority queue, with progline numbers as priority, while maintaining the values for the definitions.
 * @param startNode the CNode used to start traversing the CFG
 */
void updateReachingDefinitionsThroughCfg(CNode* startNode) {
	PKB pkb = PKB::getInstance();
	
	priority_queue<CNode*, vector<CNode*>, CompareProglines> frontier;
 
	set<pair<CNode*, boost::dynamic_bitset<> >> visited;
	frontier.push(startNode);

	unordered_map<int, set<int>> reachingDefinition;

	//@todo tmr
	
}


void setVariablesInside() {
	PKB& pkb = PKB::getInstance();
	CFG* firstProcCfg = pkb.cfgTable.at(0); // can just get any cfg, since only using functions without needing state
	
	vector<int> containerStmt = pkb.getStmtNumForType(IF);
	vector<int> whileStmt = pkb.getStmtNumForType(WHILE);
	containerStmt.insert(containerStmt.end(), whileStmt.begin(), whileStmt.end());

	//@todo can be optimised very significantly... although there is no need
	for (auto iter = containerStmt.begin(); iter != containerStmt.end(); ++iter) {
		int container = *iter;

		CNode* containerNode = pkb.cfgNodeTable.at(container);
		vector<int> descendants = pkb.getChild(container, true);
		for (auto childrenIter = descendants.begin(); childrenIter != descendants.end(); ++childrenIter) {
			int descendant = *childrenIter;
			VARIABLES variables = pkb.getUseVarInBitvectorForStmt(descendant);

			CNode* node = pkb.cfgNodeTable.at(descendant);
			if (firstProcCfg->isInsideNode(containerNode, node)) {
				boost::dynamic_bitset<> updatedInsideVariables = containerNode->getVariablesInside().size() > 0? 
															   variables | containerNode->getVariablesInside() :
															   variables;
				
				containerNode->setVariablesInside(updatedInsideVariables);
				
				cout << "setting ... " << containerNode->getProcLineNumber() << " to " << variables << endl;
			}
			if (firstProcCfg->isInsideElseNode(containerNode, node)) {
				boost::dynamic_bitset<> updatedInsideVariables = containerNode->getVariablesInside2().size() > 0? 
															   variables | containerNode->getVariablesInside2() :
															   variables;
				containerNode->setVariablesInside2(updatedInsideVariables);
				cout << "setting 2 ... " << containerNode->getProcLineNumber() << " to " << variables << endl;
			} 

		}
	}

}


/**
 * Precomputes additional information to be stored for Affects and Affects*
 */
void DesignExtractor::precomputeInformationForAffects() {
	PKB& pkb = PKB::getInstance();
	
	// set variables inside..
	setVariablesInside();

	// set definitions reaching the dummy nodes
	/*for (int i = 0; i < pkb.cfgTable.size(); i++) {
		CFG* cfg = pkb.cfgTable.at(i); 

		// traverse through cfg and update reaching definitions
		updateReachingDefinitionsThroughCfg(cfg->getProcRoot());
	}*/



	// set first use of variables in container nodes
	//for (int i = 0; i < pkb.cfgTable.size(); i++) {
		//CFG* cfg = pkb.cfgTable.at(i); 

		// traverse through cfg and update reaching definitions
		//updateFirstUseThroughCfg(cfg->getProcRoot());
	//}

}
