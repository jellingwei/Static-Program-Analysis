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
#include "ExpressionParser.h"
#include "PatternMatch.h"

AST::AST() {
	//*TNode* nullNode = createTNode(Procedure, 0, 0);
	//*_rootNode = createTNode(StmtLst, 0, 0);
	//*createLink(Child, nullNode, _rootNode);
	_rootNode = createTNode(Program, 0, 0);

}

TNode* AST::getRoot() {
	return _rootNode;
}

TNode* AST::createTNode(TNODE_TYPE ast_node_type, int stmtNo, int idx) {

	if(stmtNo < 0) {
		throw exception("AST error: Negative statment number");
	} else if(!(ast_node_type==0 || ast_node_type==1 || ast_node_type==2 || ast_node_type==3 || ast_node_type==4 || ast_node_type==5 || ast_node_type==6 || ast_node_type==7 || ast_node_type==8 || ast_node_type==9 || ast_node_type==10 || ast_node_type==11)) {
		throw exception("AST error: Invalid TNode_Type");
	}

	TNode* temp = new TNode(ast_node_type, stmtNo, idx);
	//if (ast_node_type == Assign || ast_node_type == StmtLst || ast_node_type == While) _lastImpt = temp;
	allNodes.push_back(temp);
	return temp;
}

/**
* @return TRUE if the link between the fromNode to toNode is created successfully. Otherwise, return FALSE. 
* @exception if link is invalid, or fromNode and toNode is NULL.
*/
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
			TNode& temp2 = *toNode;
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

/**
* @return the total number of children the parent TNode has. 
* @exception if parent is NULL.
*/
int AST::getChildrenSize(TNode* parent) {
	if(parent==NULL) {
		throw exception("AST error: TNode* not referenced");
	}

	TNode& temp = *parent;
	//return temp.TNode::getChildren().size();
	vector<TNode*> *pq = temp.getChildren();
	return pq->size();
}

/**
* @return the list of all the children nodes the parent TNode has.
* If there is no answer, return an empty list.
* @exception if parent is NULL.
*/
vector<TNode*>* AST::getChildrenNode(TNode* parent) {
	if(parent==NULL) {
		throw exception("AST error: TNode* not referenced");
	}

	//TNode& par = *parent;
	return parent->getChildren();
}

/**
* @return TRUE if child TNode is a child node of parent TNode. Otherwise, return FALSE.
* @exception if parent or child is NULL.
*/
bool AST::isChildNode(TNode* parent, TNode* child) {
	if(parent==NULL) {
		throw exception("AST error: TNode* not referenced");
	}

	TNode &chi = *child;
	if(chi.TNode::getParent() == parent) 
		return true;
	else return false;
}

/**
* @return TRUE if node exists. Otherwise, return FALSE.
* @exception if node is NULL.
*/
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


/**
 * @return the total number of nodes in the the AST.
 */
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
 * @param RHS to match the expression query with a suitable subtree.
 */
vector<int> AST::patternMatchAssign(string RHS) {
//cout << "received [" << RHS << "]" << endl;
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

	//@todo - change to boolean
	string isExact;
	if(matchExact) isExact = "*";
	else isExact = ",";
	
	vector<string> vRHS;
	vector<int> results;
	PKB pkb = PKB::getInstance();
//cout << "[" << RHS << "]" << endl;
	if(RHS.empty()) {
		results = pkb.getStmtNumForType("assign");
		return results;
	}

	// remove blocks of multiple whitespace
	string multipleSpaces = "[\\s]+";
	regex separatorRegex(multipleSpaces);
	sregex_token_iterator reg_end;
	sregex_token_iterator rs(RHS.begin(), RHS.end(), separatorRegex, -1);
	
	// tokenise words and operators
	string operators = "([\\w\\d]+|[*\\-+=;{}\\(\\)])";
	regex operRegex(operators);

	for (; rs != reg_end; ++rs) 
	{
		std::smatch match;
		string res(rs->str());
		while (std::regex_search(res, match, operRegex)) 
		{
			if (match.empty()) 
			{
				break;
			}
			vRHS.push_back(match[0]);
			res = match.suffix().str();
		} 
	}

	/*cout << "inside vRHS " << endl;
	for (int i=0; i<vRHS.size();i++)
		cout << "[" << vRHS[i] << "]" << " ";
	cout << " " << endl;
cout << "myvector has " << vRHS.size() << " elements" << endl;
cout << "=======" << endl;*/

	try {
		ExpressionParser exprParser;
		TNode* top = exprParser.parseExpressionForQuerying(vRHS);
		PatternMatch pattern;
		results = pattern.PatternMatchAssign(top, isExact);
	} catch(const runtime_error& e) {
		return results;
	}
	
//cout << "rQ is " << top->getNodeType() << endl;
	//vector<int> temp = pattern.PatternMatchAssign(top, isExact);
	
	
/*cout << "ok " << endl;
	for(int i =0; i< results.size(); i++)
		cout << "[" << results[i] << "] ";
	cout << " " << endl;
	cout << "=======" << endl;*/
	return results;
}

/**
 * Obtain the index of control variable of a while or if statement. 
 * @param stmtNum the statement number of the while or if statement
 * @return -1 if 1. an invalid statement number is provided.
 *     2. the statement indicated by the stmtNum is not a While statement or If statement
 *     3. the AST is poorly formed and the while or if's node is in an invalid state
 * Otherwise, return the index of the control variable.
 */
int AST::getControlVariable(int stmtNum) {
	if (stmtNum <= 0) {
		return -1;
	}

	PKB pkb = PKB::getInstance();

	if (pkb.getNodeForStmt(stmtNum) == NULL) {
		return -1;
	}
	TNode* node = pkb.getNodeForStmt(stmtNum);
	if (node->getNodeType() != While && node->getNodeType() != If) {
		return -1;
	}

	assert(node->getChildren()->size() >= 2); // < 2 children is an invalid state for a while or if node
	if (node->getChildren()->size() < 1) {
		return -1;
	}
	int controlVariable = node->getChildren()->at(0)->getNodeValueIdx();

	return controlVariable;
}

/**
 * Pattern matching for while statements.
 * @return a vector of statement numbers which are while loops, and uses the input LHS as its control variable.
 * @param LHS  the name of the variable that acts as the control variable for the while statements we are interested in.
 */
vector<int> AST::patternMatchWhile(string LHS) {
	
	PatternMatch pattern;
	vector<int> result;
	result = pattern.patternMatchParentStmt(LHS, While);

	return result;
}
/**
 * Pattern matching for if statements.
 * @return a vector of statement numbers which are if statements, and uses the input LHS as its control variable.
 * @param LHS  the name of the variable that acts as the control variable for the if statements we are interested in.
 */
vector<int> AST::patternMatchIf(string LHS) {

	PatternMatch pattern;
	vector<int> result;
	result = pattern.patternMatchParentStmt(LHS, If);

	return result;
}
