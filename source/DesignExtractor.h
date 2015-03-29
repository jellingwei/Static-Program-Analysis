#pragma once

#include <vector>
#include <assert.h>
#include <unordered_map>
using std::string;
using std::vector;
#include "TNode.h"
#include "PKB.h"

/**
 * The DesignExtractor handles the setting of information that could not be set during parsing.
 * For the functions in the DesignExtractor, at any point where a failure occurs, an exception will be thrown
 * therefore the functions for modifying data in the PKB should not return any value.
 */
class DesignExtractor {
	public:
		DesignExtractor();
		void setModifiesForCallStatements();
		void setUsesForCallStatements();
		void setModifiesForAssignmentStatements();
		void setUsesForAssignmentStatements();
		void setUsesForContainerStatements();
		
		STATUS constructCfg();
		void precomputeInformationForAffects();
		void precomputeInformationForNext();
		void constructStatisticsTable();

};