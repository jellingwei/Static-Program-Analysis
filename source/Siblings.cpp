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
TNODE_TYPE convertFromSynonym2(SYNONYM_TYPE type);

vector<pair<int, vector<int>>> Siblings::siblings(SYNONYM_TYPE first_siblingType, SYNONYM_TYPE second_siblingType) {
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
	TNODE_TYPE first_siblingType2, second_siblingType2;

	first_siblingType2 = convertFromSynonym2(first_siblingType);
	second_siblingType2 = convertFromSynonym2(second_siblingType);
	
	switch(first_siblingType2) {

		case Procedure: {
			parentNode = pkb.getRoot();
			if(first_siblingType2 == second_siblingType2) {
				allToCheckInt = pkb.getAllProcIndex();
				for(int i=0; i<(int)allToCheckInt.size(); i++) {
					currentNode = parentNode->getChildren()->at(i);
					temp = getAllPairs(currentNode, parentNode, second_siblingType2);
					//LHS.insert( LHS.end(), temp.begin(), temp.end() );

					if(temp.size() != 0) results.push_back(make_pair(currentNode->getNodeValueIdx(), temp));
					temp.clear();
				}
			}	
			break;
		}
		
		// If - then & else StmtLst
		case StmtLst: {
			/*if(second_siblingType2 == StmtLst) {
				allToCheckInt = pkb.getStmtNumForType("if");
				//if(second_siblingType == Assign || second_siblingType == While || second_siblingType == If) {
					for(int i=0 ; i<(int)allToCheckInt.size(); i++) {		
						currentNode = pkb.getNodeForStmt(allToCheckInt.at(i));
					
						RHS.push_back(currentNode->getChildren()->at(2)->getChildren()->at(0)->getStmtNumber());
						results.push_back(make_pair(currentNode->getChildren()->at(1)->getChildren()->at(0)->getStmtNumber(), RHS));
						RHS.clear();
					}		
				//}
			}*/
			if(second_siblingType2 == StmtLst || second_siblingType2 == Variable) {
				allToCheckInt = pkb.getStmtNumForType("if");
				for(int i=0 ; i<(int)allToCheckInt.size(); i++) {
					currentNode = pkb.getNodeForStmt(allToCheckInt.at(i))->getChildren()->at(1);
					parentNode = currentNode->getParent();
					temp = getAllPairs(currentNode, parentNode, second_siblingType2);

					if(temp.size() != 0) results.push_back(make_pair(currentNode->getChildren()->at(0)->getStmtNumber(), temp));
					temp.clear();

					currentNode = pkb.getNodeForStmt(allToCheckInt.at(i))->getChildren()->at(2);
					parentNode = currentNode->getParent();
					temp = getAllPairs(currentNode, parentNode, second_siblingType2);

					if(temp.size() != 0) results.push_back(make_pair(currentNode->getChildren()->at(0)->getStmtNumber(), temp));
					temp.clear();
				}

				allToCheckInt = pkb.getStmtNumForType("while");
				for(int i=0 ; i<(int)allToCheckInt.size(); i++) {
					currentNode = pkb.getNodeForStmt(allToCheckInt.at(i))->getChildren()->at(1);
					parentNode = currentNode->getParent();
					temp = getAllPairs(currentNode, parentNode, second_siblingType2);

					if(temp.size() != 0) results.push_back(make_pair(currentNode->getChildren()->at(0)->getStmtNumber(), temp));
					temp.clear();
				}	
			}

			break;
		}

		case While: {
			allToCheckInt = pkb.getStmtNumForType("while");
			if(second_siblingType2 == Assign || second_siblingType2 == While || second_siblingType2 == If || second_siblingType2 == Stmt  || second_siblingType2 == Call) {
				for(int i=0 ; i<(int)allToCheckInt.size(); i++) {
					currentNode = pkb.getNodeForStmt(allToCheckInt.at(i));
					parentNode = currentNode->getParent();
					temp = getAllPairs(currentNode, parentNode, second_siblingType2);

					if(temp.size() != 0) results.push_back(make_pair(currentNode->getStmtNumber(), temp));
					temp.clear();
				}		
			}
			break;
		}

		case If: {
			allToCheckInt = pkb.getStmtNumForType("if");
			if(second_siblingType2 == Assign || second_siblingType2 == While || second_siblingType2 == If || second_siblingType2 == Stmt || second_siblingType2 == Call) {
				for(int i=0 ; i<(int)allToCheckInt.size(); i++) {
					currentNode = pkb.getNodeForStmt(allToCheckInt.at(i));
					parentNode = currentNode->getParent();
					temp = getAllPairs(currentNode, parentNode, second_siblingType2);

					if(temp.size() != 0) results.push_back(make_pair(currentNode->getStmtNumber(), temp));
					temp.clear();
				}		
			}
			break;
		}

		case Assign: {
			allToCheckInt = pkb.getStmtNumForType("assign");
			if(second_siblingType2 == Assign || second_siblingType2 == While || second_siblingType2 == If || second_siblingType2 == Stmt || second_siblingType2 == Call) {
				for(int i=0 ; i<(int)allToCheckInt.size(); i++) {
					currentNode = pkb.getNodeForStmt(allToCheckInt.at(i));
					parentNode = currentNode->getParent();
					temp = getAllPairs(currentNode, parentNode, second_siblingType2);

					if(temp.size() != 0) results.push_back(make_pair(currentNode->getStmtNumber(), temp));
					temp.clear();
				}		
			}
			break;
		}

		case Stmt: {
			//assign
			allToCheckInt = pkb.getStmtNumForType("assign");
			if(second_siblingType2 == Assign || second_siblingType2 == While || second_siblingType2 == If || second_siblingType2 == Stmt || second_siblingType2 == Call) {
				for(int i=0 ; i<(int)allToCheckInt.size(); i++) {
					currentNode = pkb.getNodeForStmt(allToCheckInt.at(i));
					parentNode = currentNode->getParent();
					temp = getAllPairs(currentNode, parentNode, second_siblingType2);

					if(temp.size() != 0) results.push_back(make_pair(currentNode->getStmtNumber(), temp));
					temp.clear();
				}		
			}

			//if
			allToCheckInt = pkb.getStmtNumForType("if");
			if(second_siblingType2 == Assign || second_siblingType2 == While || second_siblingType2 == If || second_siblingType2 == Stmt || second_siblingType2 == Call) {
				for(int i=0 ; i<(int)allToCheckInt.size(); i++) {
					currentNode = pkb.getNodeForStmt(allToCheckInt.at(i));
					parentNode = currentNode->getParent();
					temp = getAllPairs(currentNode, parentNode, second_siblingType2);

					if(temp.size() != 0) results.push_back(make_pair(currentNode->getStmtNumber(), temp));
					temp.clear();
				}		
			}

			//while
			allToCheckInt = pkb.getStmtNumForType("while");
			if(second_siblingType2 == Assign || second_siblingType2 == While || second_siblingType2 == If || second_siblingType2 == Stmt || second_siblingType2 == Call) {
				for(int i=0 ; i<(int)allToCheckInt.size(); i++) {
					currentNode = pkb.getNodeForStmt(allToCheckInt.at(i));
					parentNode = currentNode->getParent();
					temp = getAllPairs(currentNode, parentNode, second_siblingType2);

					if(temp.size() != 0) results.push_back(make_pair(currentNode->getStmtNumber(), temp));
					temp.clear();
				}		
			}

			//call
			allToCheckInt = pkb.getStmtNumForType("call");
			if(second_siblingType2 == Assign || second_siblingType2 == While || second_siblingType2 == If || second_siblingType2 == Stmt || second_siblingType2 == Call) {
				for(int i=0 ; i<(int)allToCheckInt.size(); i++) {
					currentNode = pkb.getNodeForStmt(allToCheckInt.at(i));
					parentNode = currentNode->getParent();
					temp = getAllPairs(currentNode, parentNode, second_siblingType2);

					if(temp.size() != 0) results.push_back(make_pair(currentNode->getStmtNumber(), temp));
					temp.clear();
				}		
			}
			break;
		}

		case Plus: case Minus: case Times: {
			allToCheckInt = pkb.getStmtNumForType("assign");
			TNode* checkThisOperator;
			for(int i=0 ; i<(int)allToCheckInt.size(); i++) {
				currentNode = pkb.getNodeForStmt(allToCheckInt.at(i));
				if(second_siblingType2 == Plus || second_siblingType2 == Minus || second_siblingType2 == Times || second_siblingType2 == Constant || second_siblingType2 == Variable) {
					//if(currentNode->getDescendent() > 2) {
						queue = getOperatorNodes(currentNode->getChildren()->at(1), first_siblingType2);
						for(int j=0; j<(int)queue.size(); j++) {
							checkThisOperator = queue.at(j);
							parentNode = checkThisOperator->getParent();
							temp = getAllPairs(checkThisOperator, parentNode, second_siblingType2);

							if(temp.size() != 0) results.push_back(make_pair(checkThisOperator->getStmtNumber(), temp));
							temp.clear();
						}
					//}
				}
			}
			break;
		}

		case Variable: case Constant: {
			allToCheckInt = pkb.getStmtNumForType("assign");
			TNode* checkThisOperand;
			for(int i=0 ; i<(int)allToCheckInt.size(); i++) {
				currentNode = pkb.getNodeForStmt(allToCheckInt.at(i));
				if(second_siblingType2 == Plus || second_siblingType2 == Minus || second_siblingType2 == Times || second_siblingType2 == Constant || second_siblingType2 == Variable) {
						if(first_siblingType2 == Variable) queue.push_back(currentNode->getChildren()->at(0));
					//if(currentNode->getDescendent() > 2) {
						tempQueue = getOperandNodes(currentNode->getChildren()->at(1), first_siblingType2);
						queue.insert( queue.end(), tempQueue.begin(), tempQueue.end() );
		/*cout << "StmtNo " << currentNode->getStmtNumber() << " Inside tempQueue [ ";
		for(int q=0; q<(int)tempQueue.size(); q++) {
			cout << tempQueue.at(q)->getNodeValueIdx() << ", ";
		}
		cout << "] " << endl;*/
						for(int j=0; j<(int)queue.size(); j++) {
							checkThisOperand = queue.at(j);
							parentNode = checkThisOperand->getParent();
							temp = getAllPairs(checkThisOperand, parentNode, second_siblingType2);

							if(temp.size() != 0) results.push_back(make_pair(checkThisOperand->getNodeValueIdx(), temp));
							temp.clear();
						}
						queue.clear();
					//}
				}
			}
			
			if(second_siblingType2 == StmtLst) {
				allToCheckInt = pkb.getStmtNumForType("if");
				for(int i=0 ; i<(int)allToCheckInt.size(); i++) {
					currentNode = pkb.getNodeForStmt(allToCheckInt.at(i))->getChildren()->at(0);
					parentNode = currentNode->getParent();
					temp = getAllPairs(currentNode, parentNode, second_siblingType2);

					if(temp.size() != 0) results.push_back(make_pair(currentNode->getNodeValueIdx(), temp));
					temp.clear();;
				}

				allToCheckInt = pkb.getStmtNumForType("while");
				for(int i=0 ; i<(int)allToCheckInt.size(); i++) {
					currentNode = pkb.getNodeForStmt(allToCheckInt.at(i))->getChildren()->at(0);
					parentNode = currentNode->getParent();
					temp = getAllPairs(currentNode, parentNode, second_siblingType2);

					if(temp.size() != 0) results.push_back(make_pair(currentNode->getNodeValueIdx(), temp));
					temp.clear();
				}
			}
			
			//allToCheckInt = pkb.getAllVarIndex();
			//string getVarName(int index);
			//int getStmtNum(string varName);

			//vector<int> getUsesVarForStmt(int stmtNum);
			//vector<int> getModVarForStmt(int stmtNum);
			break;
		}

		case Call: {
			allToCheckInt = pkb.getStmtNumForType("call");
			if(second_siblingType2 == Assign || second_siblingType2 == While || second_siblingType2 == If || second_siblingType2 == Stmt || second_siblingType2 == Call) {
				for(int i=0 ; i<(int)allToCheckInt.size(); i++) {
					currentNode = pkb.getNodeForStmt(allToCheckInt.at(i));
					parentNode = currentNode->getParent();
					temp = getAllPairs(currentNode, parentNode, second_siblingType2);

					if(temp.size() != 0) results.push_back(make_pair(currentNode->getStmtNumber(), temp));
					temp.clear();
				}		
			}
			break;
		}

		//Invalid
		case Program: {
			break;
		}

		default:
			break;
	}

	return results;
}

TNODE_TYPE convertFromSynonym2(SYNONYM_TYPE type) {
	TNODE_TYPE returnType;
	switch(type) {
		case PROGRAM:
			returnType = Program;
			break;
		case PROCEDURE:
			returnType = Procedure;
			break;
		case WHILE:
			returnType = While;
			break;
		case IF:
			returnType = If;
			break;
		case STMTLST:
			returnType = StmtLst;
			break;
		case STMT:
			returnType= Stmt;
			break;
		case ASSIGN:
			returnType = Assign;
			break;
		case PLUS:
			returnType = Plus;
			break;
		case MINUS:
			returnType = Minus;
			break;
		case TIMES:
			returnType = Times;
			break;
		case CONSTANT:
			returnType= Constant;
			break;
		case VARIABLE:
			returnType = Variable;
			break;
		case CALL:
			returnType = Call;
			break;
		default:
			returnType = Nil;
			break;
	}
	return returnType;
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
			} else if(targetType == StmtLst) {
				result.push_back(test->getChildren()->at(0)->getStmtNumber());	// first stmt in StmtLst
			} else {
				result.push_back(test->getStmtNumber());
			}
		}

		if(test != currentNode && targetType == Stmt) {
			if(test->getNodeType() == Assign || test->getNodeType() == While || test->getNodeType() == If || test->getNodeType() == Call) {
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

vector<int> Siblings::siblings(int stmtNo, SYNONYM_TYPE second_siblingType) {
	PKB pkb = PKB::getInstance();
	string stmtType = pkb.getType(stmtNo);
	TNode* currentNode = pkb.getNodeForStmt(stmtNo);
	vector<int> temp;
	//vector<pair<int, vector<int>>> results;

	if(second_siblingType == ASSIGN || second_siblingType == IF || second_siblingType == WHILE || second_siblingType == CALL || second_siblingType == STMT) {
		temp = getAllPairs(currentNode, currentNode->getParent(), convertFromSynonym2(second_siblingType));
		//if(temp.size() != 0) results.push_back(make_pair(stmtNo, temp));
		//temp.clear();
	}
	return temp;
}

vector<int> Siblings::siblings(SYNONYM_TYPE first_siblingType, int stmtNo) {
	PKB pkb = PKB::getInstance();
	string stmtType = pkb.getType(stmtNo);
	TNode* currentNode = pkb.getNodeForStmt(stmtNo);
	vector<int> temp;
	//vector<pair<int, vector<int>>> result;
	//vector<pair<int, vector<int>>> results;

	if(first_siblingType == ASSIGN || first_siblingType == IF || first_siblingType == WHILE || first_siblingType == CALL || first_siblingType == STMT) {
		temp = getAllPairs(currentNode, currentNode->getParent(), convertFromSynonym2(first_siblingType));
		//if(temp.size() != 0) results.push_back(make_pair(stmtNo, temp));
		//temp.clear();
	}
	return temp;
}