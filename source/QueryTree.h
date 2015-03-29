#pragma once

#include <string>
#include "Synonym.h"
#include "QNode.h"
#include <iostream>
#include <unordered_map>

using std::endl;
using std::cout;
using std::string;
using std::unordered_map;

class QueryTree
{
public:
	QueryTree();
	QNode* createQNode(QNODE_TYPE QNODE_TYPE, Synonym arg0, Synonym arg1, Synonym arg2);
	bool linkNode(QNode* parent_node, QNode* child_node);
	void setResultNode(QNode* resultNode);
	void setClausesNode(QNode* clausesNode);

	QNode* getRoot();
	QNode* getResultNode();
	QNode* getClausesNode();
	void printTree();

	unordered_map<string, SYNONYM_TYPE> getSynonymsMap();
	void setSynonymsMap(unordered_map<string, SYNONYM_TYPE> synonymsMap);

private:
	void printNode(QNode* node);
	QNode _root;
	QNode _result;
	QNode _clauses;
	unordered_map<string, SYNONYM_TYPE> _synonymsMap;
};

