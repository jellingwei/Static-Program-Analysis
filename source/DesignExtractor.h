#include <vector>
#include <assert.h>
#include <unordered_map>
using std::string;
using std::vector;
#include "TNode.h"

class DesignExtractor {
	public:
		DesignExtractor();
		vector<int> getCallsInTopologicalOrder();
		vector<TNode*> obtainCallStatementsInTopologicalOrder(vector<int>);
		void setModifiesForCallStatements(vector<TNode*>);
		void setUsesForCallStatements(vector<TNode*>);
		
		bool constructCfg();
};