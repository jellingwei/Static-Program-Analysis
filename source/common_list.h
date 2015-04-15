#pragma once

class CNode;
class TNode;
class Synonym;

typedef vector<Synonym> SYNONYM_LIST;
typedef vector<TNode*>* TNODE_LIST;
typedef vector<CNode*>* CNODE_LIST;

typedef std::pair<CNode*, std::stack<CNode*> > NEXTBIP_STATE;