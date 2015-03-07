#include <vector>
#include <assert.h>
#include <unordered_map>
using std::string;
using std::vector;
#include "TNode.h"

/**
 * The DesignExtractor handles the setting of information that could not be set during parsing.
 * For the functions in the DesignExtractor, at any point where a failure occurs, an exception will be thrown
 * therefore the functions for modifying data in the PKB should not return any value.
 */
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
		void precomputeInformationForAffects();
		void constructStatisticsTable();

};