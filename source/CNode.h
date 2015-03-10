#pragma once

#include <stdio.h>
#include <iostream>
#include <vector>
#include <string>
#include <utility>
#include <unordered_map>
#include <set>

#include <boost\dynamic_bitset.hpp>

#include "TNode.h"

using std::string;
using namespace std;

typedef boost::dynamic_bitset<> VARIABLES ;

enum CNODE_TYPE
{
	Assign_C, While_C, If_C, Call_C, Proc_C, EndProc_C, EndIf_C
};

enum CLINK_TYPE
{
	Before, After, Inside, Inside2
};

class CNode 
{
public:
	CNode();
	//CNode(CNODE_TYPE ast_node_type, int stmtNo);
	CNode(CNODE_TYPE cfg_node_type, int procLineNo, CNode* header, TNode* ast);
	
	int getProcLineNumber();
	CNODE_TYPE getNodeType();
	bool isDummy();
	unordered_map<int, set<int>> getReachingDefinitions();
	void setReachingDefinitions(unordered_map<int, set<int>>);
	unordered_map<int, set<int>> getFirstUseOfVariable();
	void setFirstUseOfVariable(unordered_map<int, set<int>>);
	
	vector<CNode*>* getBefore();
	void addBefore(CNode* node);
	bool hasBefore();
	
	vector<CNode*>* getAfter();
	void addAfter(CNode* node);
	bool hasAfter();
	

	void setHeader(CNode* header);
	CNode* getHeader();
	
	//for While & If Then StmtLst
	vector<CNode*>* getInside();
	void addInside(CNode* node);
	bool hasInside();
	VARIABLES getVariablesInside();
	void setVariablesInside(VARIABLES);

	//for If Else StmtLst
	vector<CNode*>* getInside2();
	void addInside2(CNode* node);
	bool hasInside2();
	VARIABLES getVariablesInside2();
	void setVariablesInside2(VARIABLES);

	TNode* getASTref();
	void setEnd();
	bool getEnd();

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
};