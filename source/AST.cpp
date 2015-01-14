#include <stdio.h>
#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <cstring>
#include <regex>

using namespace std;

#include "AST.h"
#include "TNode.h"
#include "PKB.h"

AST::AST() {
	TNode* nullNode = createTNode(Procedure, 0, 0);
	_rootNode = createTNode(StmtLst, 0, 0);
	createLink(Child, nullNode, _rootNode);

}

TNode* AST::getRoot() {
	return _rootNode;
}

TNode* AST::createTNode(TNODE_TYPE ast_node_type, int stmtNo, int idx) {

	if(stmtNo < 0) {
		throw exception("AST error: Negative statment number");
	} else if(!(ast_node_type==0 || ast_node_type==1 || ast_node_type==2 || ast_node_type==3 || ast_node_type==4 || ast_node_type==5 || ast_node_type==6 || ast_node_type==7)) {
		throw exception("AST error: Invalid TNode_Type");
	}

	TNode* temp = new TNode(ast_node_type, stmtNo, idx);
	if (ast_node_type == Assign || ast_node_type == StmtLst || ast_node_type == While) _lastImpt = temp;
	allNodes.push_back(temp);
	return temp;
}

bool AST::createLink(LINK_TYPE link, TNode* fromNode, TNode* toNode) {
	if(!(link==0 || link==1 || link==2 || link==3)) {
		throw exception("AST error: Invalid Link_Type");
	} else if(fromNode==NULL || toNode==NULL) {
		throw exception("AST error: TNode* not referenced");
	}

	switch(link) {
		case Right_Sibling: {
			TNode& temp = *fromNode;
			temp.addRightSibling(toNode);
			TNode& temp2 = *toNode;
			temp2.addLeftSibling(fromNode);
			return true; }

		case Left_Sibling: {
			TNode& temp = *fromNode;
			temp.addLeftSibling(toNode);
			TNode temp2 = *toNode;
			temp2.addRightSibling(fromNode);
			return true; }

		case Child: {
			TNode& temp = *fromNode;
			temp.addChild(toNode);
			toNode->setParent(&temp);
			return true; }

		default:
			return false; 
	}
}
	
int AST::getChildrenSize(TNode* parent) {
	if(parent==NULL) {
		throw exception("AST error: TNode* not referenced");
	}

	TNode& temp = *parent;
	//return temp.TNode::getChildren().size();
	vector<TNode*> *pq = temp.getChildren();
	return pq->size();
}

vector<TNode*>* AST::getChildrenNode(TNode* parent) {
	if(parent==NULL) {
		throw exception("AST error: TNode* not referenced");
	}

	//TNode& par = *parent;
	return parent->getChildren();
}

bool AST::isChildNode(TNode* parent, TNode* child) {
	if(parent==NULL) {
		throw exception("AST error: TNode* not referenced");
	}

	TNode &chi = *child;
	if(chi.TNode::getParent() == parent) 
		return true;
	else return false;
}

bool AST::isExists(TNode* node) { 
	if(node==NULL) {
		throw exception("AST error: TNode* not referenced");
	}

	vector<TNode*>::iterator it;
	it = find(allNodes.begin(), allNodes.end(), node);
	if(it != allNodes.end())
		return true;
	else return false;
}

TNode* AST::getLastImpt() {
	return _lastImpt;
}

int AST::getSize() {
	return allNodes.size();
}


bool is_number(const std::string& s)
{
    std::string::const_iterator it = s.begin();
    while (it != s.end() && isdigit(*it)) ++it;
    return !s.empty() && it == s.end();
}

/**
 * Pattern matching for assign statements.
 * @return a vector of statement numbers which are assign stmts, and uses the input RHS as its right substree.
 * @param RHS, to match the expression query with a suitable subtree.
 */
vector<int> AST::patternMatchAssign(string RHS) {

	//==========================================//
	//==== Stage 0 - Tidy query & matchExact ===//
	//==========================================//

	RHS.erase(std::remove(RHS.begin(), RHS.end(), ' '), RHS.end());		//remove whitespaces
	RHS.erase(std::remove(RHS.begin(), RHS.end(), '\"'), RHS.end());		//remove ""

	int underscore = RHS.find('_');
	bool matchExact = true;

	if(underscore!=-1) {
		matchExact = false;
		RHS.erase(underscore, 1);
		underscore = RHS.rfind('_');
		if(underscore!=-1) {
			RHS.erase(underscore, 1);
		}
	}

	char buffer[200];
	int length, subString;
	vector<string> pseudoAST;
	string bufferS;


	//=========================================//
	//==== Stage 1 - Build Pesudo-AST array ===//
	//=========================================//

	int	plusSign = RHS.rfind('+');
	// query contains AT LEAST a + operator
	if(plusSign != -1) {						
		if(plusSign != RHS.size()-1) {
			if(RHS.size() > 1) {
				length = RHS.size() - plusSign;						// means got Right factor next to last + operator "... + y"
				subString = RHS.copy(buffer, length, plusSign+1);	// copy whole factor up to 200chars long
				buffer[subString]='\0';
				bufferS = buffer;
				pseudoAST.push_back("+");
				pseudoAST.push_back(buffer);
				RHS.erase(plusSign, length);
			} else { 
				// query contains only Single + operator
				pseudoAST.push_back("+");
				RHS.erase(plusSign, 1);
			}

		} else {
			// special case: "x + y +" ending with a +
			pseudoAST.push_back("+");
			pseudoAST.push_back("");
			RHS.erase(plusSign, 1);
		}

		while(RHS.size() >= 1) {
			if(plusSign > 0) {			//means got L factor next to old +
				plusSign = RHS.rfind('+');
				if(plusSign != -1) {		 //-1 means not found, no more +	
					if(plusSign != RHS.size()-1) {
						length = RHS.size() - plusSign; //means got R factor next to current +
						subString = RHS.copy(buffer, length, plusSign+1);
						buffer[subString]='\0';
						bufferS = buffer;
						pseudoAST.push_back("+");
						pseudoAST.push_back(buffer);
						RHS.erase(plusSign, length);
					}
				}
			} else if(plusSign == 0) {
				pseudoAST.push_back("+");
				RHS.erase(plusSign, 1);
			} else if(plusSign == -1) {
				length = RHS.size();
				subString = RHS.copy(buffer, length, 0);
				buffer[subString]='\0';
				bufferS = buffer;
				pseudoAST.push_back(buffer);
				RHS.erase(0, length);
			}
			plusSign = RHS.rfind('+');
		} // end while


	} else {
		// query contains only Single factor
		pseudoAST.push_back(RHS);
	}


	//============================================//
	//==== Stage 2 - Scour AST Assign Subtrees ===//
	//============================================//

	vector<int> assignTable = PKB::getInstance().getStmtNumForType("assign");

	TNode *assignN, *firstChildNode, *rightChildNode, *leftChildNode, *nextPlus;
	vector<TNode*> *childrenList;
	vector<int> results, tempResults;
	TNODE_TYPE assignType = Assign, plusType = Plus, constType = Constant, varType = Variable;
	bool rightY = false, leftY = false, isNum, useFirstPlus = false;
	unsigned int current = 0;
	string tempHolder;
	
	// Special case: Query is "_" => All Stmts Returned
	if(pseudoAST.size()==1 && pseudoAST.at(0)=="") {
		return assignTable;
	}

	// For each assign stmt
	for(size_t i=0; i<assignTable.size(); i++) {
		assignN = PKB::getInstance().nodeTable.at(assignTable.at(i));
		current = 0;

		if(assignType == assignN->getNodeType()) {

			// Begin from last '+' operator in pseudo-AST
			if(pseudoAST.at(0) == "+") {	

				childrenList = assignN->getChildren();					// Children of Assign Node in AST
				firstChildNode = childrenList->at(1);					// '+' Node is Right Child

				if(plusType == firstChildNode->getNodeType()) {		
					current+=1;

					while(current < pseudoAST.size()) {
						childrenList = firstChildNode->getChildren();

						// Check '+' operator contains 2 children => contains Left and Right Child Nodes
						if(childrenList->size() == 2) {
							rightChildNode = childrenList->at(1);
							leftChildNode = childrenList->at(0);

							// Left & Right Child Nodes can be either Variable or Constant Type
							if((varType==rightChildNode->getNodeType() || constType==rightChildNode->getNodeType()) 
											&& (varType==leftChildNode->getNodeType() || constType==leftChildNode->getNodeType())) {

								// **Final Check: PseudoAST left just last 2 factors to parse (eg. psuedoAST size 3; index is 1)
								if(current == pseudoAST.size()-2) {

									// Check Right Node Type and Value
									if(varType==rightChildNode->getNodeType()) {
										rightY = rightChildNode->getNodeValueIdx() == PKB::getInstance().getVarIndex(pseudoAST.at(current));
									} else if(constType==rightChildNode->getNodeType()) {
										isNum = is_number(pseudoAST.at(current));
										if(isNum) {
											rightY = rightChildNode->getNodeValueIdx() == stoi(pseudoAST.at(current));
										}
									}

									// Check Left Node Type and Value
									if(varType==leftChildNode->getNodeType()) {
										leftY = leftChildNode->getNodeValueIdx() == PKB::getInstance().getVarIndex(pseudoAST.at(current+1));
									} else if(constType==leftChildNode->getNodeType()) {
										isNum = is_number(pseudoAST.at(current+1));
										if(isNum) {
											leftY = leftChildNode->getNodeValueIdx() == stoi(pseudoAST.at(current+1));
										}
									}
									
									// Both Left & Right are Valid and value matches pattern => **ADD Stmt No to Returned List
									if(rightY && leftY) {
										results.push_back(assignN->getStmtNumber());
									}
									rightY = leftY = false;
									break;

								} else {
									rightY = leftY = false;
									break;
								}

							// Left Child Node IS '+' operator and Right Child Nodes can be either Variable or Constant Type
							} else if((varType == rightChildNode->getNodeType() || constType==rightChildNode->getNodeType()) 
																					&& plusType == leftChildNode->getNodeType()) {

								if(varType==rightChildNode->getNodeType()) {
									rightY = rightChildNode->getNodeValueIdx() == PKB::getInstance().getVarIndex(pseudoAST.at(current));
								} else if(constType==rightChildNode->getNodeType()) {
									isNum = is_number(pseudoAST.at(current));
									if(isNum) {
										rightY = rightChildNode->getNodeValueIdx() == stoi(pseudoAST.at(current));
									}
								}
								

								// *Takes note of the first-next Plus Node after the top Plus Node, for subsequent reference
								if(!useFirstPlus) {
									nextPlus = leftChildNode;
									useFirstPlus = true;
								}

								// Eg: If pseudo-AST is size 5, index[0-4] and index is 1 => remains 2 other factors for checking
								if(rightY && current<=pseudoAST.size()-4) {

									// *No more factors for checking 
									// => Hence, restart search from with first-next Plus Node ref & restart from top in PseudoAST
									if(matchExact==false && current==pseudoAST.size()-2) {
										firstChildNode = nextPlus;
										useFirstPlus = false;
										current = 1;
										rightY = leftY = false;
									} else {
										// Remains 2 other factors for checking => Update Current idx in pseudoAST
										firstChildNode = leftChildNode;
										current+=2;
										rightY = leftY = false;
									}

								} else if(rightY==false && matchExact==false && current<=pseudoAST.size()-4) {
									// Restart from top in PseudoAST, Update to next Plus Node in AST
									firstChildNode = leftChildNode;
									current = 1;
									rightY = leftY = false;

								} else {
									if(rightY) {
										// *Restart search from with first-next Plus Node ref & restart from top in PseudoAST
										// Eg: When reached end of Pseudo-AST, but AST still contains Node => Hence restart from first-next Plus Node
										firstChildNode = nextPlus;
										useFirstPlus = false;
										current = 1;
										rightY = leftY = false;
									} else {
										rightY = leftY = false;
										break;
									}
								}

							} else {
								rightY = leftY = false;
								break;
							}
						}
					} // End While


				}			

			} else {	
				// Pseudo-AST does not contain any '+' operator => Single factor
				childrenList = assignN->getChildren();
				firstChildNode = childrenList->at(1);		// Variable/Constant is Right Child Node

				// MatchExact is True
				//		Right Child Node is Variable Type
				if(matchExact==true && varType == firstChildNode->getNodeType()) { 
					if(firstChildNode->getNodeValueIdx() == PKB::getInstance().getVarIndex(pseudoAST.at(0))) {
						results.push_back(assignN->getStmtNumber());
					}
				} 

				//		Right Child Node is Constant Type
				else if(matchExact==true && constType == firstChildNode->getNodeType()) {
					isNum = is_number(pseudoAST.at(0));
					if(isNum && (firstChildNode->getNodeValueIdx()==stoi(pseudoAST.at(0)))) {
						results.push_back(assignN->getStmtNumber());
					}
				} 

				// *MatchExact is False
				else {	
					isNum = is_number(pseudoAST.at(0));
					if(!isNum) {
						// Right Child Node is Variable Type
						UsesTable* usesTable = PKB::getInstance().usesTable;
						if(matchExact == false) {
							tempResults = usesTable->getUsesStmtNum(PKB::getInstance().getVarIndex(pseudoAST.at(0)));
							for(size_t i=0; i<tempResults.size(); i++) {
								if(PKB::getInstance().isAssign(tempResults.at(i))) {
									results.push_back(tempResults.at(i));
								}
							}
							break;
						}
					} else {
						// Right Child Node is Constant Type
						if(matchExact == false && PKB::getInstance().isConstant(stoi(pseudoAST.at(0)))) {
							tempResults = PKB::getInstance().getStmtNum(stoi(pseudoAST.at(0)));
							for(size_t i=0; i<tempResults.size(); i++) {
								if(PKB::getInstance().isAssign(tempResults.at(i))) {
									results.push_back(tempResults.at(i));
								}
							}
						}
					}
				}
			} // End Else Single Factor
		} // End If assign
	} // End for

	return results;
}


/**
 * Pattern matching for while statements.
 * @return a vector of statement numbers which are while loops, and uses the input LHS as its control variable.
 * @param LHS, the name of the variable that acts as the control variable for the while statements we are interested in.
 */
vector<int> AST::patternMatchWhile(string LHS) {
	// strip leading and trailing space
	LHS.erase(0, LHS.find_first_not_of(" "));
	LHS.erase(LHS.find_last_not_of(" ") + 1);
	
	if (LHS.empty()) {
		vector<int> emptyVector;
		return emptyVector;
	}

	int varIndex = PKB::getInstance().getVarIndex(LHS);
	if (varIndex <= 0) {
		vector<int> emptyVector;
		return emptyVector;
	}
	
	// Get statements using the variable
	vector<int> candidateList = PKB::getInstance().getUsesStmtNum(varIndex);

	// Filter non-while statements from these statements
	vector<int> result;
	for (auto stmt = candidateList.begin(); stmt != candidateList.end(); ++stmt) {
		bool isWhile = PKB::getInstance().isWhile(*stmt); 
		if (isWhile) {
			TNode* stmtNode = PKB::getInstance().nodeTable.at(*stmt);

			assert(stmtNode->getChildren()->size() >= 2); // < 2 children is an invalid state for a while node
			if (stmtNode->getChildren()->size() < 1) {
				continue;
			}
			int controlVariable = stmtNode->getChildren()->at(0)->getNodeValueIdx();

			if (controlVariable == varIndex) {
				result.push_back(*stmt);
			}
		}
	}

	return result;
}

/**
 * Obtain the index of control variable of a while loop. 
 * @param stmtNum, the statement number of the while loop
 * @return -1 if 1. an invalid statement number is provided.
 *     2. the statement indicated by the stmtNum is not a While statement
 *     3. the AST is poorly formed and the while loop's node is in an invalid state
 * Otherwise, return the index of the control variable.
 */
int AST::getControlVariable(int stmtNum) {
	if (stmtNum <= 0) {
		return -1;
	}

	if (PKB::getInstance().nodeTable.count(stmtNum) <= 0) {
		return -1;
	}
	TNode* whileNode = PKB::getInstance().nodeTable.at(stmtNum);
	if (whileNode->getNodeType() != While) {
		return -1;
	}

	assert(whileNode->getChildren()->size() >= 2); // < 2 children is an invalid state for a while node
	if (whileNode->getChildren()->size() < 1) {
		return -1;
	}
	int controlVariable = whileNode->getChildren()->at(0)->getNodeValueIdx();

	return controlVariable;
}