#include <vector>
#include <assert.h>
#include <unordered_map>
using std::string;
using std::vector;
#include "TNode.h"

class DesignExtractor {
	public:
		DesignExtractor();
		vector<TNode*> obtainCallStatementsInTopologicalOrder();
		void setModifiesForCallStatements(vector<TNode*>);
		void setUsesForCallStatements(vector<TNode*>);
		void setModifiesForAssignmentStatements();
		void setUsesForAssignmentStatements();
		void setUsesForContainerStatements();
		
		bool constructCfg();
		void constructStatisticsTable();
};