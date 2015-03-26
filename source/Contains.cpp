#include <stdio.h>
#include <iostream>
#include <string>
#include <vector>
#include <algorithm>

#include "Contains.h"

using namespace std;

vector<pair<TNode*, vector<TNode*>>> Contains::contains(TNODE_TYPE parentType, TNODE_TYPE childType, bool transitiveClosure) {
	PKB pkb = PKB::getInstance();
	vector<pair<TNode*, vector<TNode*>>> results;
	vector<TNode*> LHS;
	vector<TNode*>* allToCheck;
	vector<int> allToCheckInt;
	TNode* currentNode;
	vector<TNode*> temp;
	//TNODE_TYPE childType;

	//switch(parentNode->getNodeType()) {
	switch(parentType) {
		case Procedure: {
			allToCheck = pkb.getRoot()->getChildren();
			//childType = childNode->getNodeType();
			for(int i=0; i<(int)allToCheck->size(); i++) {
				currentNode = allToCheck->at(i);
				LHS = checkForProcCase(currentNode, childType, transitiveClosure);
				if(LHS.size() != 0) results.push_back(make_pair(currentNode, LHS));
			}
			break;
		}

		case While: {
			allToCheckInt = pkb.getStmtNumForType("while");
			for(int i=0 ; i<(int)allToCheckInt.size(); i++) {
				currentNode = pkb.getNodeForStmt(allToCheckInt.at(i));
				LHS = checkForWhileCase(currentNode, childType, transitiveClosure);
				if(LHS.size() != 0) results.push_back(make_pair(currentNode, LHS));
			}
			break;
		}

		case If: {
			allToCheckInt = pkb.getStmtNumForType("if");
			for(int i=0 ; i<(int)allToCheckInt.size(); i++) {
				currentNode = pkb.getNodeForStmt(allToCheckInt.at(i));
				LHS = checkForIfCase(currentNode, childType, transitiveClosure);
				if(LHS.size() != 0) results.push_back(make_pair(currentNode, LHS));
			}
			break;
		}

		case StmtLst: {
			//@todo
			//procedue
			for(int i=0; i<(int)pkb.getRoot()->getChildren()->size(); i++) {
				currentNode = pkb.getRoot()->getChildren()->at(i)->getChildren()->at(0);
				if(currentNode->getNodeType() == childType) {
					LHS.push_back(currentNode);
				} else {
					temp = checkForStmtLstCase(currentNode, childType, transitiveClosure);
					LHS.insert( LHS.end(), temp.begin(), temp.end() );
				}
				if(LHS.size() != 0) results.push_back(make_pair(currentNode, LHS));
				LHS.clear();
			}

			//while
			allToCheckInt = pkb.getStmtNumForType("while");
			for(int i=0 ; i<(int)allToCheckInt.size(); i++) {
				currentNode = pkb.getNodeForStmt(allToCheckInt.at(i))->getChildren()->at(1);
				if(currentNode->getNodeType() == childType) {
					LHS.push_back(currentNode);
				} else {
					temp = checkForStmtLstCase(currentNode, childType, transitiveClosure);
					LHS.insert( LHS.end(), temp.begin(), temp.end() );
				}
				if(LHS.size() != 0) results.push_back(make_pair(currentNode, LHS));
				LHS.clear();
			}

			//if x2 stmtLst
			allToCheckInt = pkb.getStmtNumForType("if");
			for(int i=0 ; i<(int)allToCheckInt.size(); i++) {
				currentNode = pkb.getNodeForStmt(allToCheckInt.at(i))->getChildren()->at(1);
				if(currentNode->getNodeType() == childType) {
					LHS.push_back(currentNode);
				} else {
					temp = checkForStmtLstCase(currentNode, childType, transitiveClosure);
					LHS.insert( LHS.end(), temp.begin(), temp.end() );
				}
				if(LHS.size() != 0) results.push_back(make_pair(currentNode, LHS));
				currentNode = pkb.getNodeForStmt(allToCheckInt.at(i))->getChildren()->at(2);
				if(currentNode->getNodeType() == childType) {
					LHS.push_back(currentNode);
				} else {
					temp = checkForStmtLstCase(currentNode, childType, transitiveClosure);
					LHS.insert( LHS.end(), temp.begin(), temp.end() );
				}
				if(LHS.size() != 0) results.push_back(make_pair(currentNode, LHS));
				LHS.clear();
			}

			break;
		}

		case Assign: {
			allToCheckInt = pkb.getStmtNumForType("assign");
			for(int i=0 ; i<(int)allToCheckInt.size(); i++) {
				currentNode = pkb.getNodeForStmt(allToCheckInt.at(i));
				LHS = checkForAssignCase(currentNode, childType, transitiveClosure);
				if(LHS.size() != 0) results.push_back(make_pair(currentNode, LHS));
			}
			break;
		}

		case Plus: case Variable: case Constant: case Minus: case Times: {
			//@todo
			LHS = checkForOtherCase(currentNode, childType, transitiveClosure);
			if(LHS.size() != 0) results.push_back(make_pair(currentNode, LHS));
			break;
		}

		default:
			break;
	}

	return results;
}

vector<TNode*> Contains::checkForProcCase(TNode* currentNode, TNODE_TYPE childType, bool transitiveClosure) {
	vector<TNode*>* allToCheck;
	vector<TNode*> LHS;
	vector<TNode*> temp;

	allToCheck = currentNode->getChildren();
	for(int i=0; i<(int)allToCheck->size(); i++) {
		if(allToCheck->at(i)->getNodeType() == childType) {
			LHS.push_back(allToCheck->at(i));
		} else {
			temp = checkForStmtLstCase(allToCheck->at(i), childType, transitiveClosure);
			LHS.insert( LHS.end(), temp.begin(), temp.end() );
		}
	}

	return LHS;
}

vector<TNode*> Contains::checkForWhileCase(TNode* currentNode, TNODE_TYPE childType, bool transitiveClosure) {
	vector<TNode*>* allToCheck;
	vector<TNode*> LHS;
	vector<TNode*> temp;

	allToCheck = currentNode->getChildren();
	if(childType!= Procedure && childType!= Program) {
		//Control Variable
		if(allToCheck->at(0)->getNodeType() == childType) {
			LHS.push_back(allToCheck->at(0));
		}
		//StmtLst
		for(int i=1; i<(int)allToCheck->size(); i++) {
			if(allToCheck->at(i)->getNodeType() == childType) {
				LHS.push_back(allToCheck->at(i));
			} else {
				temp = checkForStmtLstCase(allToCheck->at(i), childType, transitiveClosure);
				LHS.insert( LHS.end(), temp.begin(), temp.end() );
			}
		}
	}
	return LHS;
}

vector<TNode*> Contains::checkForIfCase(TNode* currentNode, TNODE_TYPE childType, bool transitiveClosure) {
	vector<TNode*>* allToCheck;
	vector<TNode*> LHS;
	vector<TNode*> temp;

	allToCheck = currentNode->getChildren();
	if(childType!= Procedure && childType!= Program) {
		//Control Variable
		if(allToCheck->at(0)->getNodeType() == childType) {
			LHS.push_back(allToCheck->at(0));
		}
		//StmtLst x2
		for(int i=1; i<(int)allToCheck->size(); i++) {
			if(allToCheck->at(i)->getNodeType() == childType) {
				LHS.push_back(allToCheck->at(i));
			} else {
				temp = checkForStmtLstCase(allToCheck->at(i), childType, transitiveClosure);
				LHS.insert( LHS.end(), temp.begin(), temp.end() );
			}
		}
	}
	return LHS;
}

vector<TNode*> Contains::checkForStmtLstCase(TNode* currentNode, TNODE_TYPE childType, bool transitiveClosure) {
	vector<TNode*>* allToCheck;
	vector<TNode*> LHS;
	vector<TNode*> temp;

	allToCheck = currentNode->getChildren();
	if(childType!= Procedure && childType!= Program) {
		for(int i=0; i<(int)allToCheck->size(); i++) {
			if(allToCheck->at(i)->getNodeType() == childType) {
				LHS.push_back(allToCheck->at(i));
			} else {
				if(allToCheck->at(i)->getNodeType() == While) {
					temp = checkForWhileCase(allToCheck->at(i), childType, transitiveClosure);
					LHS.insert( LHS.end(), temp.begin(), temp.end() );
				} else if(allToCheck->at(i)->getNodeType() == If) {
					temp = checkForIfCase(allToCheck->at(i), childType, transitiveClosure);
					LHS.insert( LHS.end(), temp.begin(), temp.end() );
				} else if(allToCheck->at(i)->getNodeType() == Assign) {
					temp = checkForAssignCase(allToCheck->at(i), childType, transitiveClosure);
					LHS.insert( LHS.end(), temp.begin(), temp.end() );
				} else {
					temp = checkForOtherCase(allToCheck->at(i), childType, transitiveClosure);
					LHS.insert( LHS.end(), temp.begin(), temp.end() );
				}
			}
		}
	}
	return LHS;
}

vector<TNode*> Contains::checkForAssignCase(TNode* currentNode, TNODE_TYPE childType, bool transitiveClosure) {
	vector<TNode*>* allToCheck;
	vector<TNode*> LHS;
	vector<TNode*> temp;

	allToCheck = currentNode->getChildren();
	if(childType!= Procedure && childType!= Program && childType!=While && childType!=If && childType!=StmtLst) {
		//RHS
		if(allToCheck->at(0)->getNodeType() == childType) {
			LHS.push_back(allToCheck->at(0));
		}
		//LHS
		for(int i=1; i<(int)allToCheck->size(); i++) {
			if(allToCheck->at(i)->getNodeType() == childType) {
				LHS.push_back(allToCheck->at(i));
			}
			temp = checkForOtherCase(allToCheck->at(i), childType, transitiveClosure);
			LHS.insert( LHS.end(), temp.begin(), temp.end() );
		}
	}
	return LHS;
}
	
vector<TNode*> Contains::checkForOtherCase(TNode* currentNode, TNODE_TYPE childType, bool transitiveClosure) {
	vector<TNode*>* allToCheck;
	vector<TNode*> LHS;
	vector<TNode*> temp;

	allToCheck = currentNode->getChildren();
	if(childType!= Procedure && childType!= Program && childType!=While && childType!=If && childType!=StmtLst && childType!=Assign) {
		for(int i=1; i<(int)allToCheck->size(); i++) {
			if(allToCheck->at(i)->getNodeType() == childType) {
				LHS.push_back(allToCheck->at(i));
			}
			temp = checkForOtherCase(allToCheck->at(i), childType, transitiveClosure);
			LHS.insert( LHS.end(), temp.begin(), temp.end() );
		}
	}
	return LHS;
}