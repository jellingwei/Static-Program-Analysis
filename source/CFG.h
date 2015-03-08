#pragma once

#include <vector>
#include <string>
#include <cassert>
#include "CNode.h"

using namespace std;

class CFG {
	private:		
		vector<CNode*> allNodes;
		CNode* _procNode;
		CNode* _endNode;

	public:
		CFG(TNode* procNode);														
		CNode* createCNode(CNODE_TYPE cfg_node_type, int procLineNo, CNode* header, TNode* ast);						
		CNode* getProcRoot();
		CNode* getProcEnd();
		bool createLink(CLINK_TYPE link, CNode* currNode, CNode* toNode);
		
		bool hasInside(CNode* header);
		int getInsideSize(CNode* header);	
		vector<CNode*>* getInsideNodes(CNode* header);
		bool isInsideNode(CNode* parent, CNode* child);

		//use for If Else StmtLst
		bool hasInsideElse(CNode* header);
		int getInsideElseSize(CNode* header);	
		vector<CNode*>* getInsideElseNodes(CNode* header);
		bool isInsideElseNode(CNode* parent, CNode* child);

		bool hasBefore(CNode* curr);
		int getBeforeSize(CNode* curr);	
		vector<CNode*>* getBeforeNodes(CNode* curr);
		bool isBeforeNode(CNode* curr, CNode* before);

		bool hasAfter(CNode* curr);
		int getAfterSize(CNode* curr);	
		vector<CNode*>* getAfterNodes(CNode* curr);
		bool isAfterNode(CNode* curr, CNode* after);

		bool isExists(CNode* node);

		TNode* getASTref(CNode* node);

		void setEndNode(CNode* CFGlast);

		static CNODE_TYPE convertTNodeTypeToCNodeType(TNODE_TYPE tNodeType);
};