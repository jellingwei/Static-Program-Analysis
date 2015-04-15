#pragma once

#include <vector>
#include <string>
#include <cassert>
#include "TNode.h"
#include "common.h"
#include "common_list.h"

using namespace std;

class AST {
	private:		
		vector<TNode*> allNodes;
		TNode* _rootNode;

	public:
		AST();														
		TNode* createTNode(TNODE_TYPE ast_node_type, STATEMENT stmtNo, VAR_INDEX idx);						
		TNode* getRoot();
		STATUS createLink(LINK_TYPE link, TNode* fromNode, TNode* toNode);
		
		INTEGER getChildrenSize(TNode* parent);	
		TNODE_LIST getChildrenNode(TNode* parent);
		BOOLEAN_ isChildNode(TNode* parent, TNode* child);

		BOOLEAN_ isExists(TNode* node);
		INTEGER getSize();

		STATEMENT_LIST patternMatchAssign(EXPRESSION RHS);
		STATEMENT_LIST patternMatchWhile(VARNAME LHS);
		STATEMENT_LIST patternMatchIf(VARNAME LHS);

		vector<pair<int, vector<int>>> patternMatchWhile(VARNAME LHS, SYNONYM_TYPE then);
		vector<pair<int, vector<int>>> patternMatchIfThen(VARNAME LHS, SYNONYM_TYPE thenS);
		vector<pair<int, vector<int>>> patternMatchIfElse(VARNAME LHS, SYNONYM_TYPE thenS);
		//STATEMENT_LIST patternMatchIf(VARNAME LHS, SYNONYM_TYPE thenS, SYNONYM_TYPE elseS);
		vector<pair<int, vector<pair<int, int>>>> patternMatchIf(VARNAME LHS, SYNONYM_TYPE thenS, SYNONYM_TYPE elseS);

		VAR_INDEX getControlVariable(STATEMENT stmtNum);

		INTEGER getDescendent(TNode* curr);
};