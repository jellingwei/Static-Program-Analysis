#pragma once

class CNode;
class TNode;
class QNode;
class Synonym;

typedef vector<Synonym> SYNONYM_LIST;
typedef vector<TNode*>* TNODE_LIST;
typedef vector<CNode*>* CNODE_LIST;
typedef vector<QNode*> QNODE_LIST;

typedef std::pair<CNode*, std::stack<CNode*> > NEXTBIP_STATE;