#pragma once

#include <vector>
#include <string>
#include <cassert>
#include "TNode.h"

using namespace std;

class AST {
	private:		
		vector<TNode*> allNodes;
		TNode* _rootNode;

	public:
		AST();														
		TNode* createTNode(TNODE_TYPE ast_node_type, int stmtNo, int idx);						
		TNode* getRoot();
		bool createLink(LINK_TYPE link, TNode* fromNode, TNode* toNode);
		
		int getChildrenSize(TNode* parent);	
		vector<TNode*>* getChildrenNode(TNode* parent);
		bool isChildNode(TNode* parent, TNode* child);

		bool isExists(TNode* node);
		int getSize();

		vector<int> patternMatchAssign(string RHS);
		vector<int> patternMatchWhile(string LHS);
		vector<int> patternMatchIf(string LHS);

		vector<int> patternMatchWhile(string LHS, TNODE_TYPE then);
		vector<int> patternMatchIfThen(string LHS, TNODE_TYPE thenS);
		vector<int> patternMatchIfElse(string LHS, TNODE_TYPE thenS);
		vector<int> patternMatchIf(string LHS, TNODE_TYPE thenS, TNODE_TYPE elseS);

		int getControlVariable(int stmtNum);

		int getDescendent(TNode* curr);
};