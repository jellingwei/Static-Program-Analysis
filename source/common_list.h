#pragma once

class CNode;
class TNode;
class QNode;
class Synonym;

typedef vector<Synonym> SYNONYM_LIST;
typedef vector<TNode*>* TNODE_LIST;
typedef vector<CNode*>* CNODE_LIST;
typedef vector<QNode*> QNODE_LIST;
typedef vector<std::pair<int, vector<int>>> PAIR_LIST;
typedef vector<std::pair<int, std::pair<int, int>>> PAIR_OF_PAIR_LIST;

typedef std::pair<CNode*, std::stack<CNode*> > NEXTBIP_STATE;