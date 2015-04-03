#pragma once

#include <stdio.h>
#include <iostream>
#include <vector>
#include <string>
#include <utility>
#include <unordered_map>
#include <set>

#include <boost\dynamic_bitset.hpp>

#include "common.h"
#include "common_list.h"
#include "TNode.h"

using std::string;
using namespace std;

typedef boost::dynamic_bitset<> VARIABLES ;

class CNode 
{
public:
	CNode();
	//CNode(CNODE_TYPE ast_node_type, int stmtNo);
	CNode(CNODE_TYPE cfg_node_type, int procLineNo, CNode* header, TNode* ast);
	
	PROG_LINE_ getProcLineNumber();
	CNODE_TYPE getNodeType();
	BOOLEAN_ isDummy();
	unordered_map<int, set<int>> getReachingDefinitions();
	void setReachingDefinitions(unordered_map<int, set<int>>);
	unordered_map<int, set<int>> getFirstUseOfVariable();
	void setFirstUseOfVariable(unordered_map<int, set<int>>);
	
	CNODE_LIST getBefore();
	void addBefore(CNode* node);
	BOOLEAN_ hasBefore();
	
	CNODE_LIST getAfter();
	void addAfter(CNode* node);
	BOOLEAN_ hasAfter();
	

	void setHeader(CNode* header);
	CNode* getHeader();
	
	//for While & If Then StmtLst
	CNODE_LIST getInside();
	void addInside(CNode* node);
	BOOLEAN_ hasInside();
	VARIABLES getVariablesInside();
	void setVariablesInside(VARIABLES);

	//for If Else StmtLst
	CNODE_LIST getInside2();
	void addInside2(CNode* node);
	BOOLEAN_ hasInside2();
	VARIABLES getVariablesInside2();
	void setVariablesInside2(VARIABLES);

	BOOLEAN_ isVariableLive(VAR_INDEX varIndex);
	BOOLEAN_ isVariableDefinedBefore(VAR_INDEX varIndex);
	

	TNode* getASTref();
	void setEnd();
	BOOLEAN_ getEnd();

	bool operator<(const CNode& b);

private:
	CNODE_TYPE _nodeType;
	int _procLineNumber;
	
	vector<CNode*> _before;
	vector<CNode*> _after;

	CNode* _header;
	vector<CNode*> _inside;
	vector<CNode*> _inside2;

	TNode* _ASTref;
	bool isEnd;

	unordered_map<int, set<int>> _reachingDefinitions;
	unordered_map<int, set<int>> _firstUseOfVariable;
	VARIABLES _variablesInside;
	VARIABLES _variablesInside2;

	VARIABLES* _reachingDefinitionsVariables;
	VARIABLES* _liveVariables;
};