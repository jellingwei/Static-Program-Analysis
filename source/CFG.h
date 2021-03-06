#pragma once

#include <vector>
#include <string>
#include <cassert>
#include "CNode.h"
#include "common.h"
#include "common_list.h"

using namespace std;

class CFG {
	private:		
		vector<CNode*> allNodes;
		CNode* _procNode;
		CNode* _endNode;

	public:
		CFG(TNode* procNode);														
		CNode* createCNode(CNODE_TYPE cfg_node_type, PROG_LINE_ procLineNo, CNode* header, TNode* ast);						
		CNode* getProcRoot();
		CNode* getProcEnd();
		STATUS createLink(CLINK_TYPE link, CNode* currNode, CNode* toNode);
		
		BOOLEAN_ hasInside(CNode* header);
		INTEGER getInsideSize(CNode* header);	
		CNODE_LIST getInsideNodes(CNode* header);
		BOOLEAN_ isInsideNode(CNode* parent, CNode* child);

		//use for If Else StmtLst
		BOOLEAN_ hasInsideElse(CNode* header);
		INTEGER getInsideElseSize(CNode* header);	
		CNODE_LIST getInsideElseNodes(CNode* header);
		BOOLEAN_ isInsideElseNode(CNode* parent, CNode* child);

		BOOLEAN_ hasBefore(CNode* curr);
		INTEGER getBeforeSize(CNode* curr);	
		CNODE_LIST getBeforeNodes(CNode* curr);
		BOOLEAN_ isBeforeNode(CNode* curr, CNode* before);

		BOOLEAN_ hasAfter(CNode* curr);
		INTEGER getAfterSize(CNode* curr);	
		CNODE_LIST getAfterNodes(CNode* curr);
		BOOLEAN_ isAfterNode(CNode* curr, CNode* after);

		BOOLEAN_ isExists(CNode* node);

		TNode* getASTref(CNode* node);

		void setEndNode(CNode* CFGlast);

		static CNODE_TYPE convertTNodeTypeToCNodeType(TNODE_TYPE tNodeType);
};