#include <stdio.h>
#include <iostream>
#include <string>
#include <vector>
#include <algorithm>

#include "Siblings.h"
#include "common.h"
using namespace std;
vector<int> getAllPairs(TNode* currentNode, TNode* parentNode, TNODE_TYPE targetType);
vector<TNode*> getOperatorNodes(TNode* currentNode, TNODE_TYPE operatorType);
vector<TNode*> getOperandNodes(TNode* currentNode, TNODE_TYPE operatorType);

vector<pair<int, vector<int>>> Siblings::siblings(TNODE_TYPE first_siblingType, TNODE_TYPE second_siblingType) {
	PKB pkb = PKB::getInstance();
	vector<pair<int, vector<int>>> results;
	vector<int> LHS;
	vector<TNode*>* allToCheck;
	vector<int> allToCheckInt;
	vector<TNode*> queue;
	vector<TNode*> tempQueue;
	TNode* currentNode;
	TNode* parentNode;
	vector<int> temp;
	vector<int> RHS;

	switch(first_siblingType) {

		case Procedure: {
			parentNode = pkb.getRoot();
			if(first_siblingType == second_siblingType) {
				allToCheckInt = pkb.getAllProcIndex();
				for(int i=0; i<allToCheckInt.size(); i++) {
					currentNode = parentNode->getChildren()->at(i);
					temp = getAllPairs(currentNode, parentNode, second_siblingType);
					//LHS.insert( LHS.end(), temp.begin(), temp.end() );

					if(temp.size() != 0) results.push_back(make_pair(currentNode->getNodeValueIdx(), temp));
					LHS.clear();
				}
			}	
			break;
		}
		
		// If - then & else StmtLst
		case StmtLst: {
			if(second_siblingType == StmtLst) {
				allToCheckInt = pkb.getStmtNumForType("if");
				//if(second_siblingType == Assign || second_siblingType == While || second_siblingType == If) {
					for(int i=0 ; i<(int)allToCheckInt.size(); i++) {		
						currentNode = pkb.getNodeForStmt(allToCheckInt.at(i));
					
						RHS.push_back(currentNode->getChildren()->at(2)->getChildren()->at(0)->getStmtNumber());
						results.push_back(make_pair(currentNode->getChildren()->at(1)->getChildren()->at(0)->getStmtNumber(), RHS));
						RHS.clear();
					}		
				//}
			}
			break;
		}

		case While: {
			allToCheckInt = pkb.getStmtNumForType("while");
			if(second_siblingType == Assign || second_siblingType == While || second_siblingType == If) {
				for(int i=0 ; i<(int)allToCheckInt.size(); i++) {
					currentNode = pkb.getNodeForStmt(allToCheckInt.at(i));
					parentNode = currentNode->getParent();
					temp = getAllPairs(currentNode, parentNode, second_siblingType);

					if(temp.size() != 0) results.push_back(make_pair(currentNode->getStmtNumber(), temp));
					LHS.clear();
				}		
			}
			break;
		}

		case If: {
			allToCheckInt = pkb.getStmtNumForType("if");
			if(second_siblingType == Assign || second_siblingType == While || second_siblingType == If) {
				for(int i=0 ; i<(int)allToCheckInt.size(); i++) {
					currentNode = pkb.getNodeForStmt(allToCheckInt.at(i));
					parentNode = currentNode->getParent();
					temp = getAllPairs(currentNode, parentNode, second_siblingType);

					if(temp.size() != 0) results.push_back(make_pair(currentNode->getStmtNumber(), temp));
					LHS.clear();
				}		
			}
			break;
		}

		case Assign: {
			allToCheckInt = pkb.getStmtNumForType("assign");
			if(second_siblingType == Assign || second_siblingType == While || second_siblingType == If) {
				for(int i=0 ; i<(int)allToCheckInt.size(); i++) {
					currentNode = pkb.getNodeForStmt(allToCheckInt.at(i));
					parentNode = currentNode->getParent();
					temp = getAllPairs(currentNode, parentNode, second_siblingType);

					if(temp.size() != 0) results.push_back(make_pair(currentNode->getStmtNumber(), temp));
					LHS.clear();
				}		
			}
			break;
		}

		case Plus: case Minus: case Times: {
			allToCheckInt = pkb.getStmtNumForType("assign");
			TNode* checkThisOperator;
			for(int i=0 ; i<(int)allToCheckInt.size(); i++) {
				currentNode = pkb.getNodeForStmt(allToCheckInt.at(i));
				if(second_siblingType == Plus || second_siblingType == Minus || second_siblingType == Times || second_siblingType == Constant || second_siblingType == Variable) {
					//if(currentNode->getDescendent() > 2) {
						queue = getOperatorNodes(currentNode->getChildren()->at(1), first_siblingType);
						for(int j=0; j<(int)queue.size(); j++) {
							checkThisOperator = queue.at(j);
							parentNode = checkThisOperator->getParent();
							temp = getAllPairs(checkThisOperator, parentNode, second_siblingType);

							if(temp.size() != 0) results.push_back(make_pair(checkThisOperator->getStmtNumber(), temp));
							LHS.clear();
						}
					//}
				}
			}
			break;
		}

		/*case Constant: {
			allToCheckInt = pkb.getAllConstant();
			//vector<int> getStmtNum(int constant);
			break;
		}*/

		case Variable: case Constant: {
			allToCheckInt = pkb.getStmtNumForType("assign");
			TNode* checkThisOperand;
			for(int i=0 ; i<(int)allToCheckInt.size(); i++) {
				currentNode = pkb.getNodeForStmt(allToCheckInt.at(i));
				if(second_siblingType == Plus || second_siblingType == Minus || second_siblingType == Times || second_siblingType == Constant || second_siblingType == Variable) {
						if(first_siblingType == Variable) queue.push_back(currentNode->getChildren()->at(0));
					//if(currentNode->getDescendent() > 2) {
						tempQueue = getOperandNodes(currentNode->getChildren()->at(1), first_siblingType);
						queue.insert( queue.end(), tempQueue.begin(), tempQueue.end() );
		/*cout << "StmtNo " << currentNode->getStmtNumber() << " Inside tempQueue [ ";
		for(int q=0; q<(int)tempQueue.size(); q++) {
			cout << tempQueue.at(q)->getNodeValueIdx() << ", ";
		}
		cout << "] " << endl;*/
						for(int j=0; j<(int)queue.size(); j++) {
							checkThisOperand = queue.at(j);
							parentNode = checkThisOperand->getParent();
							temp = getAllPairs(checkThisOperand, parentNode, second_siblingType);

							if(temp.size() != 0) results.push_back(make_pair(checkThisOperand->getNodeValueIdx(), temp));
							LHS.clear();
						}
						queue.clear();
					//}
				}
			}
			
			
			//allToCheckInt = pkb.getAllVarIndex();
			//string getVarName(int index);
			//int getStmtNum(string varName);

			//vector<int> getUsesVarForStmt(int stmtNum);
			//vector<int> getModVarForStmt(int stmtNum);
			break;
		}

		//Invalid
		case Program: case Call: {
			break;
		}

		default:
			break;
	}

	return results;
}

vector<int> getAllPairs(TNode* currentNode, TNode* parentNode, TNODE_TYPE targetType) {
	vector<TNode*>* allToCheck;
	vector<int> result;
	TNode* test;

	allToCheck = parentNode->getChildren();
	for(int i=0; i<(int)allToCheck->size(); i++) {
		test = allToCheck->at(i);
		if(test != currentNode && test->getNodeType()==targetType) {
			if(targetType == Procedure || targetType == Constant || targetType == Variable) {
				result.push_back(test->getNodeValueIdx());
			} else {
				result.push_back(test->getStmtNumber());
			}
		}
	}
	return result;
}

vector<TNode*> getOperatorNodes(TNode* currentNode, TNODE_TYPE operatorType) {
	TNode* testNode;
	TNode* testNode2;
	vector<TNode*> queue;
	vector<TNode*> temp;
	if(currentNode->getDescendent() >= 2) {
		if(currentNode->getNodeType() == operatorType) {
			queue.push_back(currentNode);
		}

		testNode = currentNode->getChildren()->at(0);
		if(testNode->getNodeType() != Constant && testNode->getNodeType() != Variable) {
			temp = getOperatorNodes(testNode, operatorType);
			queue.insert( queue.end(), temp.begin(), temp.end() );
		}

		testNode2 = currentNode->getChildren()->at(1);
		if(testNode2->getNodeType() != Constant && testNode2->getNodeType() != Variable) {
			temp = getOperatorNodes(testNode2, operatorType);
			queue.insert( queue.end(), temp.begin(), temp.end() );
		}
	}
	return queue;
}

vector<TNode*> getOperandNodes(TNode* currentNode, TNODE_TYPE operandType) {
	TNode* testNode;
	TNode* testNode2;
	vector<TNode*> queue;
	vector<TNode*> temp;
	
	if(currentNode->getDescendent() >= 2) {
		//if(currentNode->getNodeType() == operandType) {
		//	queue.push_back(currentNode);
		//}
//cout << "currentNode Type " << currentNode->getNodeType() << " idx " << currentNode->getNodeValueIdx() << endl;

		testNode = currentNode->getChildren()->at(0);
		if(testNode->getNodeType() == operandType) {
			queue.push_back(testNode);
		} else if(testNode->getNodeType() != Constant && testNode->getNodeType() != Variable) {
//cout << "no :(" << endl;
			temp = getOperandNodes(testNode, operandType);
			queue.insert( queue.end(), temp.begin(), temp.end() );
		}
//cout << "tsk :(" << endl;
		testNode2 = currentNode->getChildren()->at(1);
		if(testNode2->getNodeType() == operandType) {
			queue.push_back(testNode2);
		} else if(testNode2->getNodeType() != Constant && testNode2->getNodeType() != Variable) {
			temp = getOperandNodes(testNode2, operandType);
			queue.insert( queue.end(), temp.begin(), temp.end() );
		}
	} else {
		if(currentNode->getNodeType() == operandType) queue.push_back(currentNode);
	}
	return queue;
}