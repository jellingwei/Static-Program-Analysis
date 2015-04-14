#include <stdio.h>
#include <iostream>
#include <string>
#include <vector>
#include <algorithm>

#include "Contain.h"
#include "common.h"
using namespace std;
vector<TNode*> getOperatorNodes(TNode* currentNode, TNODE_TYPE operatorType, bool transitiveClosure);
vector<int> checkForProgCase(TNode* currentNode, TNODE_TYPE descendentType, bool transitiveClosure);
vector<int> checkForProcCase(TNode* currentNode, TNODE_TYPE descendentType, bool transitiveClosure);
vector<int> checkForWhileCase(TNode* currentNode, TNODE_TYPE descendentType, bool transitiveClosure);
vector<int> checkForIfCase(TNode* currentNode, TNODE_TYPE descendentType, bool transitiveClosure);
vector<int> checkForStmtLstCase(TNode* currentNode, TNODE_TYPE descendentType, bool transitiveClosure);
vector<int> checkForAssignCase(TNode* currentNode, TNODE_TYPE descendentType, bool transitiveClosure);
vector<int> checkForOtherCase(TNode* currentNode, TNODE_TYPE descendentType, bool transitiveClosure);
vector<TNode*> checkForWhileCase2(TNode* currentNode, TNODE_TYPE descendentType, bool transitiveClosure);
vector<TNode*> checkForIfCase2(TNode* currentNode, TNODE_TYPE descendentType, bool transitiveClosure);
vector<TNode*> checkForStmtLstCase2(TNode* currentNode, TNODE_TYPE descendentType, bool transitiveClosure);
vector<TNode*> checkForAssignCase2(TNode* currentNode, TNODE_TYPE descendentType, bool transitiveClosure);
vector<TNode*> checkForOtherCase2(TNode* currentNode, TNODE_TYPE descendentType, bool transitiveClosure);
TNODE_TYPE convertFromSynonym(SYNONYM_TYPE type);

vector<pair<int, vector<int>>> Contain::contains(SYNONYM_TYPE predecessorType, SYNONYM_TYPE descendentType, bool transitiveClosure) {
	PKB pkb = PKB::getInstance();
	vector<pair<int, vector<int>>> results;
	vector<int> LHS;
	vector<TNode*>* allToCheck;
	vector<int> allToCheckInt;
	vector<TNode*> queue;
	TNode* currentNode;
	vector<int> temp;
	//TNODE_TYPE childType;
	TNODE_TYPE predecessorType2, descendentType2;

	predecessorType2 = convertFromSynonym(predecessorType);
	descendentType2 = convertFromSynonym(descendentType);

	//switch(parentNode->getNodeType()) {
	switch(predecessorType2) {
		case Program: {
			vector<TNode*> sample;
			allToCheck = &sample;
			allToCheck->push_back(pkb.getRoot());
			LHS = checkForProgCase(pkb.getRoot(), descendentType2, transitiveClosure);
			if(LHS.size() != 0) results.push_back(make_pair(0, LHS));
			break;
		}

		case Procedure: {
			allToCheck = pkb.getRoot()->getChildren();
			for(int i=0; i<(int)allToCheck->size(); i++) {
				currentNode = allToCheck->at(i);
				LHS = checkForProcCase(currentNode, descendentType2, transitiveClosure);
				if(LHS.size() != 0) results.push_back(make_pair(currentNode->getNodeValueIdx(), LHS));
				LHS.clear();
			}
			break;
		}

		case While: {
			allToCheckInt = pkb.getStmtNumForType("while");
			for(int i=0 ; i<(int)allToCheckInt.size(); i++) {
				currentNode = pkb.getNodeForStmt(allToCheckInt.at(i));
				LHS = checkForWhileCase(currentNode, descendentType2, transitiveClosure);
				if(LHS.size() != 0) results.push_back(make_pair(currentNode->getStmtNumber(), LHS));
				LHS.clear();
			}
			break;
		}

		case If: {
			allToCheckInt = pkb.getStmtNumForType("if");
			for(int i=0 ; i<(int)allToCheckInt.size(); i++) {
				currentNode = pkb.getNodeForStmt(allToCheckInt.at(i));
				LHS = checkForIfCase(currentNode, descendentType2, transitiveClosure);
				if(LHS.size() != 0) results.push_back(make_pair(currentNode->getStmtNumber(), LHS));
				LHS.clear();
			}
			break;
		}

		case StmtLst: {
			//@todo
			//procedue
			for(int i=0; i<(int)pkb.getRoot()->getChildren()->size(); i++) {
				currentNode = pkb.getRoot()->getChildren()->at(i)->getChildren()->at(0);
				temp = checkForStmtLstCase(currentNode, descendentType2, transitiveClosure);
				LHS.insert( LHS.end(), temp.begin(), temp.end() );

				if(LHS.size() != 0) results.push_back(make_pair(currentNode->getChildren()->at(0)->getStmtNumber(), LHS));  //return first Stmt in StmtLst
				LHS.clear();
			}

			//while
			allToCheckInt = pkb.getStmtNumForType("while");
			for(int i=0 ; i<(int)allToCheckInt.size(); i++) {
				currentNode = pkb.getNodeForStmt(allToCheckInt.at(i))->getChildren()->at(1);
				temp = checkForStmtLstCase(currentNode, descendentType2, transitiveClosure);
				LHS.insert( LHS.end(), temp.begin(), temp.end() );

				if(LHS.size() != 0) results.push_back(make_pair(currentNode->getChildren()->at(0)->getStmtNumber(), LHS));  //return first Stmt in StmtLst
				LHS.clear();
			}

			//if x2 stmtLst
			allToCheckInt = pkb.getStmtNumForType("if");
			for(int i=0 ; i<(int)allToCheckInt.size(); i++) {
				currentNode = pkb.getNodeForStmt(allToCheckInt.at(i))->getChildren()->at(1);
				temp = checkForStmtLstCase(currentNode, descendentType2, transitiveClosure);
				LHS.insert( LHS.end(), temp.begin(), temp.end() );

				if(LHS.size() != 0) results.push_back(make_pair(currentNode->getChildren()->at(0)->getStmtNumber(), LHS));  //return first Stmt in StmtLst
				LHS.clear();

				currentNode = pkb.getNodeForStmt(allToCheckInt.at(i))->getChildren()->at(2);
				temp = checkForStmtLstCase(currentNode, descendentType2, transitiveClosure);
				LHS.insert( LHS.end(), temp.begin(), temp.end() );

				if(LHS.size() != 0) results.push_back(make_pair(currentNode->getChildren()->at(0)->getStmtNumber(), LHS));  //return first Stmt in StmtLst
				LHS.clear();
			}

			break;
		}

		case Assign: {
			allToCheckInt = pkb.getStmtNumForType("assign");
			for(int i=0 ; i<(int)allToCheckInt.size(); i++) {
				currentNode = pkb.getNodeForStmt(allToCheckInt.at(i));
				LHS = checkForAssignCase(currentNode, descendentType2, transitiveClosure);
				if(LHS.size() != 0) results.push_back(make_pair(currentNode->getStmtNumber(), LHS));
				LHS.clear();
			}
			break;
		}

		//@todo - Enum Stmt
		case Stmt: {
			//while
			allToCheckInt = pkb.getStmtNumForType("while");
			for(int i=0 ; i<(int)allToCheckInt.size(); i++) {
				currentNode = pkb.getNodeForStmt(allToCheckInt.at(i));
				LHS = checkForWhileCase(currentNode, descendentType2, transitiveClosure);
				if(LHS.size() != 0) results.push_back(make_pair(currentNode->getStmtNumber(), LHS));
				LHS.clear();
			}

			//if
			allToCheckInt = pkb.getStmtNumForType("if");
			for(int i=0 ; i<(int)allToCheckInt.size(); i++) {
				currentNode = pkb.getNodeForStmt(allToCheckInt.at(i));
				LHS = checkForIfCase(currentNode, descendentType2, transitiveClosure);
				if(LHS.size() != 0) results.push_back(make_pair(currentNode->getStmtNumber(), LHS));
				LHS.clear();
			}

			//assign
			allToCheckInt = pkb.getStmtNumForType("assign");
			for(int i=0 ; i<(int)allToCheckInt.size(); i++) {
				currentNode = pkb.getNodeForStmt(allToCheckInt.at(i));
				LHS = checkForAssignCase(currentNode, descendentType2, transitiveClosure);
				if(LHS.size() != 0) results.push_back(make_pair(currentNode->getStmtNumber(), LHS));
				LHS.clear();
			}

			break;		   
		}

		case Plus: case Minus: case Times: {
			//@todo
			allToCheckInt = pkb.getStmtNumForType("assign");
			TNode* checkThisOperator;
			for(int i=0 ; i<(int)allToCheckInt.size(); i++) {
				currentNode = pkb.getNodeForStmt(allToCheckInt.at(i));
				if(currentNode->getDescendent() > 2) {
					queue = getOperatorNodes(currentNode->getChildren()->at(1), predecessorType2, transitiveClosure);
					for(int j=0; j<(int)queue.size(); j++) {
						checkThisOperator = queue.at(j);
						LHS = checkForOtherCase(checkThisOperator, descendentType2, transitiveClosure);
						if(LHS.size() != 0) results.push_back(make_pair(checkThisOperator->getStmtNumber(), LHS));
						LHS.clear();
					}
				} 
			}
		}

		//Invalid
		case Variable: case Constant: case Call: {
			break;
		}

		default:
			break;
	}

	return results;
}

vector<TNode*> getOperatorNodes(TNode* currentNode, TNODE_TYPE operatorType, bool transitiveClosure) {
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
			temp = getOperatorNodes(testNode, operatorType, transitiveClosure);
			queue.insert( queue.end(), temp.begin(), temp.end() );
		}

		testNode2 = currentNode->getChildren()->at(1);
		if(testNode2->getNodeType() != Constant && testNode2->getNodeType() != Variable) {
			temp = getOperatorNodes(testNode2, operatorType, transitiveClosure);
			queue.insert( queue.end(), temp.begin(), temp.end() );
		}
	}
	return queue;
}

vector<int> checkForProgCase(TNode* currentNode, TNODE_TYPE descendentType, bool transitiveClosure) {
	vector<TNode*>* allToCheck;
	vector<int> LHS;
	vector<int> temp;

	allToCheck = currentNode->getChildren();
	for(int i=0; i<(int)allToCheck->size(); i++) {
		if(allToCheck->at(i)->getNodeType() == descendentType) {
			LHS.push_back(allToCheck->at(i)->getNodeValueIdx());  // ProcIdx
		}
		if(transitiveClosure) {
			temp = checkForProcCase(allToCheck->at(i), descendentType, transitiveClosure);
			//LHS.insert( LHS.end(), temp.begin(), temp.end() );
			for(int i=0; i<temp.size(); i++) {
				//to avoid duplicate entry of stmtNo, remove this condition if not needed
				if(find(LHS.begin(), LHS.end(), temp.at(i)) == LHS.end())  
					LHS.push_back(temp.at(i));
			}
		}
	}

	return LHS;
}

// Return Procedure Index
vector<int> checkForProcCase(TNode* currentNode, TNODE_TYPE descendentType, bool transitiveClosure) {
	vector<TNode*>* allToCheck;
	vector<int> LHS;
	vector<int> temp;

	allToCheck = currentNode->getChildren();
	for(int i=0; i<(int)allToCheck->size(); i++) {
		if(allToCheck->at(i)->getNodeType() == descendentType) {
			LHS.push_back(allToCheck->at(i)->getChildren()->at(0)->getStmtNumber());		// StmtLst Type return this first stmt
		}
		if(transitiveClosure) {
			temp = checkForStmtLstCase(allToCheck->at(i), descendentType, transitiveClosure);
			//LHS.insert( LHS.end(), temp.begin(), temp.end() );
			for(int i=0; i<temp.size(); i++) {
				//to avoid duplicate entry of stmtNo, remove this condition if not needed
				if(find(LHS.begin(), LHS.end(), temp.at(i)) == LHS.end())  
					LHS.push_back(temp.at(i));
			}
		}
	}

	return LHS;
}

vector<int> checkForWhileCase(TNode* currentNode, TNODE_TYPE descendentType, bool transitiveClosure) {
	vector<TNode*>* allToCheck;
	vector<int> LHS;
	vector<int> temp;

	allToCheck = currentNode->getChildren();
	if(descendentType!= Procedure && descendentType!= Program) {
		//Control Variable
		if(allToCheck->at(0)->getNodeType() == descendentType) {
			LHS.push_back(allToCheck->at(0)->getNodeValueIdx());  // Variable Idx
		}
		//StmtLst
		for(int i=1; i<(int)allToCheck->size(); i++) {
			if(allToCheck->at(i)->getNodeType() == descendentType) {
				LHS.push_back(allToCheck->at(i)->getChildren()->at(0)->getStmtNumber());	// StmtLst Type return this first stmt
			}
			if(transitiveClosure) {
				temp = checkForStmtLstCase(allToCheck->at(i), descendentType, transitiveClosure);
				//LHS.insert( LHS.end(), temp.begin(), temp.end() );
				for(int i=0; i<temp.size(); i++) {
					//to avoid duplicate entry of stmtNo, remove this condition if not needed
					if(find(LHS.begin(), LHS.end(), temp.at(i)) == LHS.end())  
						LHS.push_back(temp.at(i));
				}
			} 
		}	
	}
	return LHS;
}

vector<int> checkForIfCase(TNode* currentNode, TNODE_TYPE descendentType, bool transitiveClosure) {
	vector<TNode*>* allToCheck;
	vector<int> LHS;
	vector<int> temp;

	allToCheck = currentNode->getChildren();
	if(descendentType!= Procedure && descendentType!= Program) {
		//Control Variable
		if(allToCheck->at(0)->getNodeType() == descendentType) {
			LHS.push_back(allToCheck->at(0)->getNodeValueIdx());  // Variable Idx
		}
		//StmtLst x2
		for(int i=1; i<(int)allToCheck->size(); i++) {
			if(allToCheck->at(i)->getNodeType() == descendentType) {
				LHS.push_back(allToCheck->at(i)->getChildren()->at(0)->getStmtNumber());	// StmtLst Type return this first stmt
			}
			if(transitiveClosure) {
				temp = checkForStmtLstCase(allToCheck->at(i), descendentType, transitiveClosure);
				//LHS.insert( LHS.end(), temp.begin(), temp.end() );
				for(int i=0; i<temp.size(); i++) {
					//to avoid duplicate entry of stmtNo, remove this condition if not needed
					if(find(LHS.begin(), LHS.end(), temp.at(i)) == LHS.end())  
						LHS.push_back(temp.at(i));
				}
			}
		}
	}
	return LHS;
}

vector<int> checkForStmtLstCase(TNode* currentNode, TNODE_TYPE descendentType, bool transitiveClosure) {
	vector<TNode*>* allToCheck;
	vector<int> LHS;
	vector<int> temp;

	allToCheck = currentNode->getChildren();
	if(descendentType!= Procedure && descendentType!= Program) {
		for(int i=0; i<(int)allToCheck->size(); i++) {
			if(allToCheck->at(i)->getNodeType() == descendentType) {
				LHS.push_back(allToCheck->at(i)->getStmtNumber());
			}

			if(descendentType == Stmt) {
				if(allToCheck->at(i)->getNodeType() == While || allToCheck->at(i)->getNodeType() == If || allToCheck->at(i)->getNodeType() == Assign)
					LHS.push_back(allToCheck->at(i)->getStmtNumber());
			}

			if(transitiveClosure) {
				if(allToCheck->at(i)->getNodeType() == While) {
					temp = checkForWhileCase(allToCheck->at(i), descendentType, transitiveClosure);
					//LHS.insert( LHS.end(), temp.begin(), temp.end() );
						for(int i=0; i<temp.size(); i++) {
						//to avoid duplicate entry of stmtNo, remove this condition if not needed
						if(find(LHS.begin(), LHS.end(), temp.at(i)) == LHS.end())  
							LHS.push_back(temp.at(i));
					}
				} else if(allToCheck->at(i)->getNodeType() == If) {
					temp = checkForIfCase(allToCheck->at(i), descendentType, transitiveClosure);
					//LHS.insert( LHS.end(), temp.begin(), temp.end() );
					for(int i=0; i<temp.size(); i++) {
						//to avoid duplicate entry of stmtNo, remove this condition if not needed
						if(find(LHS.begin(), LHS.end(), temp.at(i)) == LHS.end())  
							LHS.push_back(temp.at(i));
					}
				} else if(allToCheck->at(i)->getNodeType() == Assign) {
					temp = checkForAssignCase(allToCheck->at(i), descendentType, transitiveClosure);
					//LHS.insert( LHS.end(), temp.begin(), temp.end() );
					for(int i=0; i<temp.size(); i++) {
						//to avoid duplicate entry of stmtNo, remove this condition if not needed
						if(find(LHS.begin(), LHS.end(), temp.at(i)) == LHS.end())  
							LHS.push_back(temp.at(i));
					}
				} else {
					temp = checkForOtherCase(allToCheck->at(i), descendentType, transitiveClosure);
					//LHS.insert( LHS.end(), temp.begin(), temp.end() );
					for(int i=0; i<temp.size(); i++) {
						//to avoid duplicate entry of stmtNo, remove this condition if not needed
						if(find(LHS.begin(), LHS.end(), temp.at(i)) == LHS.end())  
							LHS.push_back(temp.at(i));
					}
				}
			}
		}
	}
	return LHS;
}



vector<int> checkForAssignCase(TNode* currentNode, TNODE_TYPE descendentType, bool transitiveClosure) {
	vector<TNode*>* allToCheck;
	vector<int> LHS;
	vector<int> temp;

	allToCheck = currentNode->getChildren();
	if(descendentType!= Procedure && descendentType!= Program && descendentType!=While && descendentType!=If && descendentType!=StmtLst) {
		//RHS
		if(allToCheck->at(0)->getNodeType() == descendentType) {
			LHS.push_back(allToCheck->at(0)->getNodeValueIdx());
		}
		//LHS
		for(int i=1; i<(int)allToCheck->size(); i++) {
			if(allToCheck->at(i)->getNodeType() == descendentType) {
				if(descendentType == Variable || descendentType == Constant) LHS.push_back(allToCheck->at(i)->getNodeValueIdx()); //*Important eg a=2
				else LHS.push_back(allToCheck->at(i)->getStmtNumber());
			}
			if(transitiveClosure) {
				temp = checkForOtherCase(allToCheck->at(i), descendentType, transitiveClosure);
				//LHS.insert( LHS.end(), temp.begin(), temp.end() );
				for(int i=0; i<temp.size(); i++) {
					//to avoid duplicate entry of stmtNo, remove this condition if not needed
					if(find(LHS.begin(), LHS.end(), temp.at(i)) == LHS.end())  
						LHS.push_back(temp.at(i));
				}
			}
		}
	}
	return LHS;
}

// Return Variable or Constant Index
vector<int> checkForOtherCase(TNode* currentNode, TNODE_TYPE descendentType, bool transitiveClosure) {
	vector<TNode*>* allToCheck;
	vector<int> LHS;
	vector<int> temp;

	allToCheck = currentNode->getChildren();
	if(descendentType!= Procedure && descendentType!= Program && descendentType!=While && descendentType!=If && descendentType!=StmtLst && descendentType!=Assign && descendentType!=Stmt) {
		for(int i=0; i<(int)allToCheck->size(); i++) {
			if(allToCheck->at(i)->getNodeType() == descendentType) {
				if(descendentType == Times || descendentType == Plus || descendentType == Minus) {
					LHS.push_back(allToCheck->at(i)->getStmtNumber());	 // Operator --> StmtNo
				} else if (descendentType == Constant || descendentType == Variable) {
					LHS.push_back(allToCheck->at(i)->getNodeValueIdx()); // Operands --> Idx Value
				}
			}
			if(transitiveClosure) {
				temp = checkForOtherCase(allToCheck->at(i), descendentType, transitiveClosure);
				//LHS.insert( LHS.end(), temp.begin(), temp.end() );

				for(int i=0; i<temp.size(); i++) {
					//to avoid duplicate entry of stmtNo, remove this condition if not needed
					if(find(LHS.begin(), LHS.end(), temp.at(i)) == LHS.end())  
						LHS.push_back(temp.at(i));
				}
			}
		}
	}
	return LHS;
}

// For Pattern Match Extension

vector<int> Contain::checkForWhileThen(TNode* currentNode, SYNONYM_TYPE descendentType, bool transitiveClosure) {
	TNODE_TYPE descendentType2;
	descendentType2 = convertFromSynonym(descendentType);

	vector<TNode*> test = checkForStmtLstCase2(currentNode, descendentType2, transitiveClosure);

	vector<int> result;
	for(int i=0; i<test.size(); i++) {
		//to avoid duplicate entry of stmtNo, remove this condition if not needed
		if(find(result.begin(), result.end(), test.at(i)->getStmtNumber()) == result.end())  
			result.push_back(test.at(i)->getStmtNumber());
	}

	return result;
	//return checkForStmtLstCase(currentNode, descendentType, transitiveClosure);
}

vector<int> Contain::checkForIfThenElse(TNode* currentNode, SYNONYM_TYPE descendentType, bool transitiveClosure) {
	TNODE_TYPE descendentType2;
	descendentType2 = convertFromSynonym(descendentType);

	vector<TNode*> test = checkForStmtLstCase2(currentNode, descendentType2, transitiveClosure);

	vector<int> result;
	for(int i=0; i<test.size(); i++) {
		//to avoid duplicate entry of stmtNo, remove this condition if not needed
		if(find(result.begin(), result.end(), test.at(i)->getStmtNumber()) == result.end())  
			result.push_back(test.at(i)->getStmtNumber());
	}

	return result;
	//return checkForStmtLstCase(currentNode, descendentType, transitiveClosure);
}

vector<pair<int, int>> Contain::checkForIf(TNode* thenNode, SYNONYM_TYPE thenS, TNode* elseNode, SYNONYM_TYPE elseS) {
	vector<int> result1, result2;
	vector<pair<int, int>> finalResult;

	result1 = checkForStmtLstCase(thenNode, convertFromSynonym(thenS), true);
	result2 = checkForStmtLstCase(elseNode, convertFromSynonym(elseS), true);

	for(int i=0; i<(int)result1.size(); i++) {
		for(int j=0; j<(int)result2.size(); j++) {
			finalResult.push_back(make_pair(result1.at(i), result2.at(j)));
		}
	}
	return finalResult;
}

// Returns List of TNodes* instead of StmtNo/VarIdx

vector<TNode*> checkForStmtLstCase2(TNode* currentNode, TNODE_TYPE descendentType, bool transitiveClosure) {
	vector<TNode*>* allToCheck;
	vector<TNode*> LHS;
	vector<TNode*> temp;

	allToCheck = currentNode->getChildren();
	if(descendentType!= Procedure && descendentType!= Program) {
		for(int i=0; i<(int)allToCheck->size(); i++) {
			if(allToCheck->at(i)->getNodeType() == descendentType) {
				LHS.push_back(allToCheck->at(i));
			}
			if(transitiveClosure) {
				if(allToCheck->at(i)->getNodeType() == While) {
					temp = checkForWhileCase2(allToCheck->at(i), descendentType, transitiveClosure);
					LHS.insert( LHS.end(), temp.begin(), temp.end() );
				} else if(allToCheck->at(i)->getNodeType() == If) {
					temp = checkForIfCase2(allToCheck->at(i), descendentType, transitiveClosure);
					LHS.insert( LHS.end(), temp.begin(), temp.end() );
				} else if(allToCheck->at(i)->getNodeType() == Assign) {
					temp = checkForAssignCase2(allToCheck->at(i), descendentType, transitiveClosure);
					LHS.insert( LHS.end(), temp.begin(), temp.end() );
				} else {
					temp = checkForOtherCase2(allToCheck->at(i), descendentType, transitiveClosure);
					LHS.insert( LHS.end(), temp.begin(), temp.end() );
				}
			}
		}
	}
	return LHS;
}

vector<TNode*> checkForWhileCase2(TNode* currentNode, TNODE_TYPE descendentType, bool transitiveClosure) {
	vector<TNode*>* allToCheck;
	vector<TNode*> LHS;
	vector<TNode*> temp;

	allToCheck = currentNode->getChildren();
	if(descendentType!= Procedure && descendentType!= Program) {
		//Control Variable
		if(allToCheck->at(0)->getNodeType() == descendentType) {
			LHS.push_back(allToCheck->at(0));
		}
		//StmtLst
		for(int i=1; i<(int)allToCheck->size(); i++) {
			if(allToCheck->at(i)->getNodeType() == descendentType) {
				LHS.push_back(allToCheck->at(i));
			}
			if(transitiveClosure) {
				temp = checkForStmtLstCase2(allToCheck->at(i), descendentType, transitiveClosure);
				LHS.insert( LHS.end(), temp.begin(), temp.end() );
			} 
		}	
	}
	return LHS;
}

vector<TNode*> checkForIfCase2(TNode* currentNode, TNODE_TYPE descendentType, bool transitiveClosure) {
	vector<TNode*>* allToCheck;
	vector<TNode*> LHS;
	vector<TNode*> temp;

	allToCheck = currentNode->getChildren();
	if(descendentType!= Procedure && descendentType!= Program) {
		//Control Variable
		if(allToCheck->at(0)->getNodeType() == descendentType) {
			LHS.push_back(allToCheck->at(0));
		}
		//StmtLst x2
		for(int i=1; i<(int)allToCheck->size(); i++) {
			if(allToCheck->at(i)->getNodeType() == descendentType) {
				LHS.push_back(allToCheck->at(i));
			}
			if(transitiveClosure) {
				temp = checkForStmtLstCase2(allToCheck->at(i), descendentType, transitiveClosure);
				LHS.insert( LHS.end(), temp.begin(), temp.end() );
			}
		}
	}
	return LHS;
}

vector<TNode*> checkForAssignCase2(TNode* currentNode, TNODE_TYPE descendentType, bool transitiveClosure) {
	vector<TNode*>* allToCheck;
	vector<TNode*> LHS;
	vector<TNode*> temp;

	allToCheck = currentNode->getChildren();
	if(descendentType!= Procedure && descendentType!= Program && descendentType!=While && descendentType!=If && descendentType!=StmtLst) {
		//RHS
		if(allToCheck->at(0)->getNodeType() == descendentType) {
			LHS.push_back(allToCheck->at(0));
		}
		//LHS
		for(int i=1; i<(int)allToCheck->size(); i++) {
			if(allToCheck->at(i)->getNodeType() == descendentType) {
				LHS.push_back(allToCheck->at(i));
			}
			if(transitiveClosure) {
				temp = checkForOtherCase2(allToCheck->at(i), descendentType, transitiveClosure);
				LHS.insert( LHS.end(), temp.begin(), temp.end() );
			}
		}
	}
	return LHS;
}
	
vector<TNode*> checkForOtherCase2(TNode* currentNode, TNODE_TYPE descendentType, bool transitiveClosure) {
	vector<TNode*>* allToCheck;
	vector<TNode*> LHS;
	vector<TNode*> temp;

	allToCheck = currentNode->getChildren();
	if(descendentType!= Procedure && descendentType!= Program && descendentType!=While && descendentType!=If && descendentType!=StmtLst && descendentType!=Assign) {
		for(int i=0; i<(int)allToCheck->size(); i++) {
			if(allToCheck->at(i)->getNodeType() == descendentType) {
				LHS.push_back(allToCheck->at(i));
			}
			if(transitiveClosure) {
				temp = checkForOtherCase2(allToCheck->at(i), descendentType, transitiveClosure);
				LHS.insert( LHS.end(), temp.begin(), temp.end() );
			}
		}
	}
	return LHS;
}

TNODE_TYPE convertFromSynonym(SYNONYM_TYPE type) {
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
		case STMT: case PROG_LINE:
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
			returnType = Constant;
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

/*bool Contain::contains(int stmtNo, int stmtNo2, bool transitiveClosure) {
	vector<pair<int, vector<int>>> toCompare;
	toCompare = contains(STMT, STMT, true);

	for(int i=0; i<(int)toCompare.size(); i++) {
		pair <int, vector<int>> tester = toCompare.at(i);
		if(tester.first == stmtNo) {
			for(int j=0; j<(int)tester.second.size(); j++) {
				if(tester.second.at(j) == stmtNo2) return true;
			}
		}
	}

	return false;
}*/

vector<int> Contain::contains(int stmtNo, SYNONYM_TYPE descendentType, bool transitiveClosure) {
	PKB pkb = PKB::getInstance();
	string stmtType = pkb.getType(stmtNo);
	TNode* currentNode = pkb.getNodeForStmt(stmtNo);
	vector<int> temp;
	vector<pair<int, vector<int>>> results;

	if(stmtType == "assign") {
		temp = checkForAssignCase(currentNode, convertFromSynonym(descendentType), true);
	} else if(stmtType == "while") {
		temp = checkForWhileCase(currentNode, convertFromSynonym(descendentType), true);
	} else if(stmtType == "if") {
		temp = checkForIfCase(currentNode, convertFromSynonym(descendentType), true);
	}

	//if(temp.size() != 0) results.push_back(make_pair(stmtNo, temp));
	
	return temp;
}

vector<int> Contain::contains(SYNONYM_TYPE predecessorType, int stmtNo, bool transitiveClosure) {
	PKB pkb = PKB::getInstance();
	string stmtType = pkb.getType(stmtNo);
	TNODE_TYPE descendentType;
	//class Contain* contain;
	vector<int> temp;
	vector<pair<int, vector<int>>> result;
	//vector<pair<int, vector<int>>> results;

	if(predecessorType == PROGRAM || predecessorType == PROCEDURE || predecessorType == STMTLST || predecessorType == STMT || predecessorType == WHILE || predecessorType == IF) {
		if(stmtType == "assign") {
			result = contains(predecessorType, ASSIGN, true);
		} else if(stmtType == "while") {
			result = contains(predecessorType, WHILE, true);
		} else if(stmtType == "if") {
			result = contains(predecessorType, IF, true);
		} else if(stmtType == "call") {
			result = contains(predecessorType, CALL, true);
		}
	}

	for(int i=0; i<result.size(); i++) {
		pair <int, vector<int>> tester = result.at(i);
		for(int j=0; j<tester.second.size(); j++) {
			if(tester.second.at(j) == stmtNo)	temp.push_back(tester.first);
		}
		//if(temp.size() != 0) results.push_back(make_pair(tester.first, temp));
		//temp.clear();
	}

	return temp;
}