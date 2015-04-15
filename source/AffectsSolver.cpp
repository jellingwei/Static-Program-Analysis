#pragma once

#include<stdio.h>
#include <iostream>
#include <string>
#include <vector>
#include <unordered_map>
#include <algorithm>
#include <queue>
#include <unordered_set>

#include "boost/dynamic_bitset.hpp"

#include "AffectsSolver.h"
#include "PKB.h"


BOOLEAN_ isVisitedBefore(unordered_map<int, boost::dynamic_bitset<> > variableTested, CNode* nextNode, pair<CNode*, boost::dynamic_bitset<>> nodePair, set<pair<CNode*, boost::dynamic_bitset<> >> visited, boost::dynamic_bitset<> variablesToMatch) {
	if (!nextNode) {
		return false;
	}

	bool isVisitedBefore = false;
	if (variableTested.count(nextNode->getProcLineNumber()) != 0) {
		if (variablesToMatch.is_subset_of(variableTested.at(nextNode->getProcLineNumber()) ) ) {
			isVisitedBefore = true;
		}
	}

	return isVisitedBefore;
}


// assumes that each node has only 1 non-inside node directly connected After it
CNode* getMandatoryNextNode(CNode* node, CFG* cfg, boost::dynamic_bitset<> variablesToMatch) {
	PKB pkb = PKB::getInstance();

	CNODE_TYPE type = node->getNodeType();
	vector<CNode*>* possibleNextNodes = node->getAfter();

	// handle special case for if statement
	if (node->getNodeType() == If_C && node->getVariablesInside2().size() > 0 && (node->getVariablesInside2() & variablesToMatch).none()) {
		// the else stmtList does not contain the variable,
		// skip to the statement Following the node in the AST

		vector<int> follows = pkb.getStmtFollowedFrom(node->getProcLineNumber());
		if (follows.size() > 0 ) {
			int stmtNumFollowingNode = follows.at(0);
			return pkb.cfgNodeTable.at(stmtNumFollowingNode);
		} else {
			return NULL;
		}
	}
	
	for (auto iter = possibleNextNodes->begin(); iter != possibleNextNodes->end(); ++iter) {
		if (cfg->isInsideNode(node, *iter) || (*iter)->getNodeType() == Proc_C || (*iter)->getNodeType() == EndProc_C) {
			continue;
		}
		
		return *iter;	
	}

	return NULL;
}

// assumes that each node has only 1 non-inside node directly connected  Before it
CNode* getMandatoryPrevNode(CNode* node, CFG* cfg, boost::dynamic_bitset<> variablesToMatch) {
	PKB pkb = PKB::getInstance();

	CNODE_TYPE type = node->getNodeType();
	vector<CNode*>* possiblePrevNodes = node->getBefore();

	// handle special case for end if statement
	if (node->getNodeType() == EndIf_C && node->getVariablesInside2().size() > 0 && (node->getVariablesInside2() & variablesToMatch).none()) {
		// the else stmtList does not contain the variable,
		// skip to the statement Following the node in the AST
		vector<int> follows = pkb.getStmtFollowedTo(node->getProcLineNumber());
		if (follows.size() > 0) {
			int stmtNumFollowedByNode = follows.at(0);
			return pkb.cfgNodeTable.at(stmtNumFollowedByNode);
		} else {
			return NULL;
		}
	}
	
	for (auto iter = possiblePrevNodes->begin(); iter != possiblePrevNodes->end(); ++iter) {
		if (cfg->isInsideNode(node, *iter) || (*iter)->getNodeType() == Proc_C || (*iter)->getNodeType() == EndProc_C) {
			continue;
		}

		return *iter;	
	}

	return NULL;
}

// assumes that each node has only 1 inside node directly connected to it
CNode* getInsideNextNode(CNode* node, CFG* cfg, boost::dynamic_bitset<> variablesToMatch) {
	PKB pkb = PKB::getInstance();

	CNODE_TYPE type = node->getNodeType();
	vector<CNode*>* possibleNextNodes = node->getAfter();

	boost::dynamic_bitset<> variablesToCompareForNode;
	
	variablesToCompareForNode = node->getVariablesInside();
	
	
	
	if (node->getVariablesInside().size() > 0 && (node->getVariablesInside() & variablesToMatch).none()) {
		// this branch will not match variablesToMatch, therefore do an early return
		return NULL;
	}
	
	for (auto iter = possibleNextNodes->begin(); iter != possibleNextNodes->end(); ++iter) {
		if (cfg->isInsideNode(node, *iter) && (*iter)->getNodeType() != Proc_C && (*iter)->getNodeType() != EndProc_C) {
			return *iter;
		}	
	}

	return NULL;
}

// assumes that each node has only 1 inside node directly connected to it
CNode* getInsidePrevNode(CNode* node, CFG* cfg, boost::dynamic_bitset<> variablesToMatch) {
	PKB pkb = PKB::getInstance();

	CNODE_TYPE type = node->getNodeType();
	vector<CNode*>* possiblePrevNodes = node->getBefore();

	boost::dynamic_bitset<> variablesToCompareForNode;
	if (type == EndIf_C) {
		variablesToCompareForNode = node->getVariablesInside();
	} else if (type == While_C) {
		variablesToCompareForNode = pkb.getModVarInBitvectorForStmt(node->getProcLineNumber());
	}
	
	if (variablesToCompareForNode.size() > 0 && (variablesToCompareForNode & variablesToMatch).none()) {
		// this branch will not match variablesToMatch, therefore do an early return
		return NULL;
	}
	
	for (auto iter = possiblePrevNodes->begin(); iter != possiblePrevNodes->end(); ++iter) {
		if (cfg->isInsideNode(node, *iter) && (*iter)->getNodeType() != Proc_C && (*iter)->getNodeType() != EndProc_C) {
			return *iter;
		}	
	}

	return NULL;
}



/**
 * For use in the priority queue when traversing the CFG for Affects
 */
class CompareAffects {
    public:
		bool operator() (pair<CNode*, boost::dynamic_bitset<> >& pair1, pair<CNode*, boost::dynamic_bitset<> >& pair2) { 
			return pair1.first->getProcLineNumber() < pair2.first->getProcLineNumber(); 
		}
};

class CompareAffectsReverse {
    public:
		bool operator() (pair<CNode*, boost::dynamic_bitset<> >& pair1, pair<CNode*, boost::dynamic_bitset<> >& pair2) { 
			return pair1.first->getProcLineNumber() > pair2.first->getProcLineNumber(); 
		}
};

BOOLEAN_ AffectsTable::canSkipNodesForwards(CNode* node) {
	int lineNum = node->getProcLineNumber();
	return (lineNum % 4 == 0 || node->getNodeType() == If_C || node->getNodeType() == While_C);
}

PROGLINE_LIST AffectsTable::getProgLinesAffectedBy(PROG_LINE_ progLine1, TRANS_CLOSURE transitiveClosure, bool terminateOnOneResult) {
	PKB pkb = PKB::getInstance();
	// verify that progLine1 is a program line and is an assignment statement
	if (pkb.cfgNodeTable.count(progLine1) == 0) {
		return vector<int>();
	}
	CNode* node = pkb.cfgNodeTable.at(progLine1);
	if (node->getNodeType() != Assign_C) {
		return vector<int>();
	}

	vector<int> result;

	// initialise variablesToMatch and the priority queue
	boost::dynamic_bitset<> variablesToMatch = pkb.getModVarInBitvectorForStmt(progLine1);
	priority_queue<pair<CNode*, boost::dynamic_bitset<> >, vector<pair<CNode*, boost::dynamic_bitset<> > >, CompareAffects> frontier;

	CNode* startNode = getMandatoryNextNode(node, pkb.cfgTable.at(0), variablesToMatch);
	if (startNode) {
		frontier.push(make_pair<CNode*, boost::dynamic_bitset<> >(startNode,variablesToMatch));
	}
	set<pair<CNode*, boost::dynamic_bitset<> >> visited;
	unordered_map<int, boost::dynamic_bitset<> > variableTested;
	set<int> addedToAnswer;

	while (!frontier.empty()) {
		node = frontier.top().first;
		variablesToMatch = frontier.top().second;
		//visited.insert(frontier.top());

		frontier.pop();


		bool isNodeVisitedBefore = isVisitedBefore(variableTested, node,  pair<CNode*, boost::dynamic_bitset<> >(node,variablesToMatch), 
			                          visited, variablesToMatch);


		if (variableTested.count(node->getProcLineNumber()) != 0) {
			variableTested[node->getProcLineNumber()] |=  variablesToMatch;
		} else {
			variableTested[node->getProcLineNumber()] =  variablesToMatch;
		}

		if (isNodeVisitedBefore) {
			continue;
		}

		if (node->getNodeType() == Assign_C || node->getNodeType() == Call_C) {
			boost::dynamic_bitset<> variablesModified = pkb.getModVarInBitvectorForStmt(node->getProcLineNumber());
			boost::dynamic_bitset<> variablesUsed = pkb.getUseVarInBitvectorForStmt(node->getProcLineNumber());
			
			// test for results
			bool isAssignment = node->getNodeType() == Assign_C ;
			bool isResultsModified = false;
			bool isAlreadyAddedToResults = find(addedToAnswer.begin(), addedToAnswer.end(), node->getProcLineNumber()) != addedToAnswer.end();
			if (!((variablesToMatch & variablesUsed).none()) && isAssignment) {
				if (!isAlreadyAddedToResults) {
					result.push_back(node->getProcLineNumber());
					addedToAnswer.insert(node->getProcLineNumber());
				}
				isResultsModified = true;

				// early return. this is used if the presense of one result is sufficient for the query
				if (terminateOnOneResult) {
					return result;
				}

			}
			// reset any re-defined variables
			variablesToMatch &= ~variablesModified;

			// set variables for indrectly affected stmts
			if (transitiveClosure && isResultsModified) {
				variablesToMatch |= variablesModified;
			}

			// if there are no more variables to match, don't explore this path further
			if (variablesToMatch.none()) {
				continue;
			}
		} 

		// skip to future nodes if there is the required information attached
		bool isFirstUseAttached = canSkipNodesForwards(node);
		if (isFirstUseAttached) {
			unordered_map<int, set<int> > currentFirstUse = node->getFirstUseOfVariable();
			
			for (size_t i = 0; i < variablesToMatch.size(); i++) {
				if (variablesToMatch[i] == 0 || !node->isVariableLive(i)) {
					continue;
				}

				set<int> procLinesToSkipTo = currentFirstUse[i];
				for (auto skipIter = procLinesToSkipTo.begin(); skipIter != procLinesToSkipTo.end(); ++skipIter) {
					CNode* skipToNode = pkb.cfgNodeTable.at(*skipIter);
					boost::dynamic_bitset<> variableForSkipping(pkb.getVarTableSize() + 1);
					variableForSkipping.set(i);

					pair<CNode*, boost::dynamic_bitset<>> nodePair = make_pair<CNode*, boost::dynamic_bitset<> >(skipToNode, variableForSkipping);
					
					if (skipToNode ) {
						frontier.push(nodePair);
						
					}
				}		
			}

			continue;
		}

		CNode* nextNode = getMandatoryNextNode(node, pkb.cfgTable.at(0), variablesToMatch);
		pair<CNode*, boost::dynamic_bitset<>> nodePair = make_pair<CNode*, boost::dynamic_bitset<> >(nextNode, variablesToMatch);
		

		if (nextNode ) {
			frontier.push(make_pair<CNode*, boost::dynamic_bitset<> >(nextNode, variablesToMatch));

			
		}

		CNode* possibleNode = getInsideNextNode(node, pkb.cfgTable.at(0), variablesToMatch);
		nodePair = make_pair<CNode*, boost::dynamic_bitset<> >(possibleNode, variablesToMatch);
		
		
		if (possibleNode ) {
			frontier.push(make_pair<CNode*, boost::dynamic_bitset<> >(possibleNode, variablesToMatch));
			
		}
	}


	return result;
}

BOOLEAN_ AffectsTable::canSkipNodesBackwards(CNode* node) {
	int lineNum = node->getProcLineNumber();

	return (lineNum % 4 == 0 || node->getNodeType() == EndIf_C || node->getNodeType() == While_C );
}

PROGLINE_LIST AffectsTable::getProgLinesAffecting(PROG_LINE_ progLine2, bool transitiveClosure, bool terminateOnOneResult) {
	PKB pkb = PKB::getInstance();
	// verify that progLine2 is a program line and is an assignment statement
	if (pkb.cfgNodeTable.count(progLine2) == 0) {
		return vector<int>();
	}
	CNode* node = pkb.cfgNodeTable.at(progLine2);
	if (node->getNodeType() != Assign_C) {
		return vector<int>();
	}

	vector<int> result;

	// initialise variablesToMatch and the priority queue
	boost::dynamic_bitset<> variablesToMatch = pkb.getUseVarInBitvectorForStmt(progLine2);
	priority_queue<pair<CNode*, boost::dynamic_bitset<> >, vector<pair<CNode*, boost::dynamic_bitset<> > >, CompareAffectsReverse> frontier;

	CNode* startNode = getMandatoryPrevNode(node, pkb.cfgTable.at(0), variablesToMatch);
	if (startNode) {
		frontier.push(make_pair<CNode*, boost::dynamic_bitset<> >(startNode, variablesToMatch));
	}
	set<pair<CNode*, boost::dynamic_bitset<> >> visited;
	unordered_map<int, boost::dynamic_bitset<> > variableTested;

	set<int> addedToAnswer;

	while (!frontier.empty()) {
		node = frontier.top().first;
		variablesToMatch = frontier.top().second;	
		//visited.insert(frontier.top());
		frontier.pop();

		bool isNodeVisitedBefore = isVisitedBefore(variableTested, node,  pair<CNode*, boost::dynamic_bitset<> >(node,variablesToMatch), 
			                          visited, variablesToMatch);

		if (variableTested.count(node->getProcLineNumber()) != 0) {
			variableTested[node->getProcLineNumber()] |=  variablesToMatch;
		} else {
			variableTested[node->getProcLineNumber()] =  variablesToMatch;
		}

		if (isNodeVisitedBefore) {
			continue;
		}
		

		if (node->getNodeType() == Assign_C || node->getNodeType() == Call_C) {
			boost::dynamic_bitset<> variablesModified = pkb.getModVarInBitvectorForStmt(node->getProcLineNumber());
			boost::dynamic_bitset<> variablesUsed = pkb.getUseVarInBitvectorForStmt(node->getProcLineNumber());
		
			// test for results
			bool lineIsAssignment = node->getNodeType() == Assign_C;
			bool isResultsModified = false;
			bool isAlreadyAddedToResults = find(addedToAnswer.begin(), addedToAnswer.end(), node->getProcLineNumber()) != addedToAnswer.end();
			if (!((variablesToMatch & variablesModified).none()) && lineIsAssignment) {
				if (!isAlreadyAddedToResults) {
					result.push_back(node->getProcLineNumber());
					addedToAnswer.insert(node->getProcLineNumber());
				}
				isResultsModified = true;

				// early return. this is used if the presense of one result is sufficient for the query
				if (terminateOnOneResult) {
					return result;
				}
			}
			// reset any re-defined variables
			variablesToMatch &= ~variablesModified;

			// set variables for indrectly affected stmts
			if (transitiveClosure && isResultsModified) {
				variablesToMatch |= variablesUsed;
			}

			// if there are no more variables to match, don't explore this path further
			if (variablesToMatch.none()) {
				continue;
			}
		} 

		// skip to future nodes if there is the required information attached
		bool isReachingDefinitionAttached = canSkipNodesBackwards(node);
		if (isReachingDefinitionAttached) {
			unordered_map<int, set<int> > currentReachingDefs = node->getReachingDefinitions();
			
			for (size_t i = 0; i < variablesToMatch.size(); i++) {
				if (variablesToMatch[i] == 0 || !node->isVariableDefinedBefore(i)) {
					continue;
				}

				set<int> procLinesToSkipTo = currentReachingDefs[i];
				for (auto skipIter = procLinesToSkipTo.begin(); skipIter != procLinesToSkipTo.end(); ++skipIter) {
					CNode* skipToNode = pkb.cfgNodeTable.at(*skipIter);

					boost::dynamic_bitset<> variableForSkipping(pkb.getVarTableSize() + 1);
					variableForSkipping.set(i);
					pair<CNode*, boost::dynamic_bitset<>> nodePair = make_pair<CNode*, boost::dynamic_bitset<> >(skipToNode, variableForSkipping);
					
					if (skipToNode ) {
						frontier.push(nodePair);
					}
				}		
			}

			continue;
		}

		CNode* prevNode = getMandatoryPrevNode(node, pkb.cfgTable.at(0), variablesToMatch);
		pair<CNode*, boost::dynamic_bitset<>> nodePair = make_pair<CNode*, boost::dynamic_bitset<> >(prevNode, variablesToMatch);

		
		if (prevNode  ) {
			frontier.push(nodePair);

		}

		CNode* possibleNode = getInsidePrevNode(node, pkb.cfgTable.at(0), variablesToMatch);
		nodePair = make_pair<CNode*, boost::dynamic_bitset<> >(possibleNode, variablesToMatch);


		if (possibleNode ) {
			frontier.push(nodePair);
		}
		
	}

	return result;
}



pair<PROGLINE_LIST, PROGLINE_LIST> AffectsTable::getAllAffectsPairs(TRANS_CLOSURE transitiveClosure) {
	
	pair<vector<int>, vector<int>> results;

	return results;
}



PROGLINE_LIST AffectsTable::getLhs() {
	PKB pkb = PKB::getInstance();
	vector<int> assignments = pkb.getStmtNumForType(ASSIGN);
	vector<int> results;

	for (auto iter = assignments.begin(); iter != assignments.end(); ++iter) {
		vector<int> ans = getProgLinesAffectedBy(*iter, false, true); // no transitive closure, early termination
		if (!ans.empty()) {
			results.push_back(*iter);
		}
	}

	return results;
}

PROGLINE_LIST AffectsTable::getRhs() {

	PKB pkb = PKB::getInstance();
	vector<int> assignments = pkb.getStmtNumForType(ASSIGN);
	vector<int> results;

	for (auto iter = assignments.begin(); iter != assignments.end(); ++iter) {
		vector<int> ans = getProgLinesAffecting(*iter, false, true); // no transitive closure, early termination
		if (!ans.empty()) {
			results.push_back(*iter);
		}
	}

	return results;
}



BOOLEAN_ AffectsTable::isAffects(PROG_LINE_ progLine1, PROG_LINE_ progLine2, TRANS_CLOSURE transitiveClosure) {
	if (PKB::getInstance().stmtToProcMap[progLine1] != PKB::getInstance().stmtToProcMap[progLine2]) {
		return false;
	}

	PKB pkb = PKB::getInstance();
	// verify that progLine1 is a program line and is an assignment statement
	if (pkb.cfgNodeTable.count(progLine1) == 0) {
		return false;
	}
	CNode* node = pkb.cfgNodeTable.at(progLine1);
	if (node->getNodeType() != Assign_C) {
		return false;
	}

	vector<int> result;

	// initialise variablesToMatch and the priority queue
	boost::dynamic_bitset<> variablesToMatch = pkb.getModVarInBitvectorForStmt(progLine1);
	priority_queue<pair<CNode*, boost::dynamic_bitset<> >, vector<pair<CNode*, boost::dynamic_bitset<> > >, CompareAffects> frontier;

	CNode* startNode = getMandatoryNextNode(node, pkb.cfgTable.at(0), variablesToMatch);
	if (startNode) {
		frontier.push(make_pair<CNode*, boost::dynamic_bitset<> >(startNode,variablesToMatch));
	}
	set<pair<CNode*, boost::dynamic_bitset<> >> visited;
	unordered_map<int, boost::dynamic_bitset<> > variableTested;
	set<int> addedToAnswer;

	while (!frontier.empty()) {
		node = frontier.top().first;
		variablesToMatch = frontier.top().second;

		frontier.pop();


		bool isNodeVisitedBefore = isVisitedBefore(variableTested, node,  pair<CNode*, boost::dynamic_bitset<> >(node,variablesToMatch), 
			                          visited, variablesToMatch);


		if (variableTested.count(node->getProcLineNumber()) != 0) {
			variableTested[node->getProcLineNumber()] |=  variablesToMatch;
		} else {
			variableTested[node->getProcLineNumber()] =  variablesToMatch;
		}

		if (isNodeVisitedBefore) {
			continue;
		}

		if (node->getNodeType() == Assign_C || node->getNodeType() == Call_C) {
			boost::dynamic_bitset<> variablesModified = pkb.getModVarInBitvectorForStmt(node->getProcLineNumber());
			boost::dynamic_bitset<> variablesUsed = pkb.getUseVarInBitvectorForStmt(node->getProcLineNumber());
			
			// test for results
			bool isAssignment = node->getNodeType() == Assign_C ;
			bool isResultsModified = false;
			bool isAlreadyAddedToResults = find(addedToAnswer.begin(), addedToAnswer.end(), node->getProcLineNumber()) != addedToAnswer.end();
			if (!((variablesToMatch & variablesUsed).none()) && isAssignment) {
				if (!isAlreadyAddedToResults) {
					result.push_back(node->getProcLineNumber());
					addedToAnswer.insert(node->getProcLineNumber());

					if (node->getProcLineNumber() == progLine2) {
						return true;
					}
				}
				isResultsModified = true;
			}
			// reset any re-defined variables
			variablesToMatch &= ~variablesModified;

			// set variables for indrectly affected stmts
			if (transitiveClosure && isResultsModified) {
				variablesToMatch |= variablesModified;
			}

			// if there are no more variables to match, don't explore this path further
			if (variablesToMatch.none()) {
				continue;
			}
		} 

		// skip to future nodes if there is the required information attached
		bool isFirstUseAttached = canSkipNodesForwards(node);
		if (isFirstUseAttached) {
			unordered_map<int, set<int> > currentFirstUse = node->getFirstUseOfVariable();
			
			for (size_t i = 0; i < variablesToMatch.size(); i++) {
				if (variablesToMatch[i] == 0 || !node->isVariableLive(i)) {
					continue;
				}

				set<int> procLinesToSkipTo = currentFirstUse[i];
				for (auto skipIter = procLinesToSkipTo.begin(); skipIter != procLinesToSkipTo.end(); ++skipIter) {
					CNode* skipToNode = pkb.cfgNodeTable.at(*skipIter);
					boost::dynamic_bitset<> variableForSkipping(pkb.getVarTableSize() + 1);
					variableForSkipping.set(i);

					pair<CNode*, boost::dynamic_bitset<>> nodePair = make_pair<CNode*, boost::dynamic_bitset<> >(skipToNode, variableForSkipping);
					
					if (skipToNode ) {
						frontier.push(nodePair);
						
					}
				}		
			}

			continue;
		}

		CNode* nextNode = getMandatoryNextNode(node, pkb.cfgTable.at(0), variablesToMatch);
		pair<CNode*, boost::dynamic_bitset<>> nodePair = make_pair<CNode*, boost::dynamic_bitset<> >(nextNode, variablesToMatch);
		

		if (nextNode ) {
			frontier.push(make_pair<CNode*, boost::dynamic_bitset<> >(nextNode, variablesToMatch));
		}

		CNode* possibleNode = getInsideNextNode(node, pkb.cfgTable.at(0), variablesToMatch);
		nodePair = make_pair<CNode*, boost::dynamic_bitset<> >(possibleNode, variablesToMatch);
		
		
		if (possibleNode ) {
			frontier.push(make_pair<CNode*, boost::dynamic_bitset<> >(possibleNode, variablesToMatch));
			
		}
	}

	return false;
}

BOOLEAN_ AffectsTable::isValid() {
	PKB pkb = PKB::getInstance();
	vector<int> assignments = pkb.getStmtNumForType(ASSIGN);
	
	for (auto iter = assignments.begin(); iter != assignments.end(); ++iter) {
		bool transitiveClosure = false;
		bool earlyTermination = true;
		vector<int> ans = getProgLinesAffectedBy(*iter, transitiveClosure, earlyTermination); 
		if (!ans.empty()) {
			return true;
		}
	}

	return false;
}

BOOLEAN_ AffectsTable::isLhsValid(PROG_LINE_ lhs) {
	if (PKB::getInstance().cfgNodeTable.count(lhs) == 0) {
		return false;
	}

	bool transClosure = false; 
	bool earlyTermination = true;
	vector<int> ans = getProgLinesAffectedBy(lhs, transClosure, earlyTermination); // no transitive closure, early termination

	return !ans.empty();
}

BOOLEAN_ AffectsTable::isRhsValid(PROG_LINE_ rhs) {
	if (PKB::getInstance().cfgNodeTable.count(rhs) == 0) {
		return false;
	}
	bool transClosure = false; 
	bool earlyTermination = true;
	vector<int> ans = getProgLinesAffecting(rhs, transClosure, earlyTermination); // no transitive closure, early termination

	return !ans.empty();
}