#include "QueryOptimiser.h"

/*
cost: 
All * relation 
Modifies: 1
Uses: 1
Follows*: 3
Parent*: 2
Call*: 1.5
Next*: 3.5
Affects: 3
Affects*: 5
*/

/**
	@brief Namespace containing functions for the query optimisation

*/

namespace QueryOptimiser
{
	double COST_MODIFIES = 1;
	double COST_USES = 1;
	double COST_FOLLOWS = 1;
	double COST_PARENT = 1;
	double COST_CALLS = 1;
	double COST_NEXT = 1;
	double COST_AFFECTS = 0;
	double COST_AFFECTSS = 0;
	double COST_FOLLOWSS = 0;
	double COST_PARENTS = 0;
	double COST_CALLSS = 0;
	double COST_NEXTS = 0;
	double COST_PATTERN = 0;
	double COST_WITH = 1;

	unordered_map<string, SYNONYM_TYPE> synonymNameToTypeMap;
	unordered_map<string, unsigned int> synonymsCount;  //Maps synonyms to the expected number of values
	set<string> mainTableSynonyms;  //Denotes synonyms that are already in the main table
	StatisticsTable* statsTable;

	QueryTree* optimiseQueryTree(QueryTree* qTreeRoot);
	void initialize(QueryTree* qTreeRoot);
	QueryTree* flattenQuery(QueryTree* qTreeRoot);
	QueryTree* optimiseClauses(QueryTree* qTreeRoot);
	QNode* optimiseClausesNode(QNode* clausesNode);

	pair<vector<QNode*>, vector<QNode*>> splitConstantClauses(QNode* clausesNode);
	vector<QNode*> reorderNonConstantClauses(vector<QNode*> clauses);
	QNode* getNextSmallestAndUpdate(vector<QNode*> &clauses);
	pair<int, DIRECTION> findIndexAndDirectionWithSmallestCost(vector<QNode*> clauses);
	pair<Synonym, Synonym> getClauseArguments(QNode* clause);

	int getExpectedCount(QNODE_TYPE rel_type, SYNONYM_TYPE type_probe, SYNONYM_TYPE type_output);
	double getReductionFactor(QNODE_TYPE rel_type, SYNONYM_TYPE typeLHS, SYNONYM_TYPE typeRHS, DIRECTION direction);
	double calculateCost(QNODE_TYPE rel_type, double numberOfValues);
	inline bool isContainedInMain(string wantedName);
	void updateSynonymsCount(string name, int expectedCount);
	void reduceSynonymsCount(string name, double reductionFactor);
	QNode* convertToTree(vector<QNode*> constantClauses, vector<QNode*> nonConstantClauses);

	/**
	* Method that is public to optimise the query tree
	* @param qTreeRoot the root of the query tree
	* @return a QueryTree object that contains the optimised query tree
	*/
	QueryTree* optimiseQueryTree(QueryTree* qTreeRoot)
	{
		initialize(qTreeRoot);

		QueryTree* flattenedQueryTree = flattenQuery(qTreeRoot);
		return optimiseClauses(flattenedQueryTree);
	}

	/**
	* Method to initialize
	* @param qTreeRoot the root of the query tree
	*/
	void initialize(QueryTree* qTreeRoot)
	{
		statsTable = new StatisticsTable();
		synonymNameToTypeMap = qTreeRoot->getSynonymsMap();
		int numberOfSynonyms = synonymNameToTypeMap.size();

		for (auto itr = synonymNameToTypeMap.begin(); itr != synonymNameToTypeMap.end(); ++itr) {
			synonymsCount[itr->first] = (unsigned int)statsTable->getCountForType(itr->second);
		}
	}

	/**
	* Method to remove redundant clause and to rewrite the clauses
	* @param qTreeRoot the root of the query tree
	* @return a QueryTree object that contains the flattened query tree
	*/
	QueryTree* flattenQuery(QueryTree* qTreeRoot)
	{
		//TODO: Remove redundant clauses
		//Rewrite with clauses
		return qTreeRoot;
	}

	/*QueryTree* rewriteWithClauses(QueryTree* qTreeRoot)
	{
	unordered_map<string, bool> selectSynonyms;

	QNode* resultNode = qTreeRoot->getResultNode();
	QNode* resultChildNode = resultNode->getChild();
	int numberOfSynonyms = resultNode->getNumberOfChildren();

	//Populate the select synonyms map
	for (int i = 0; i < numberOfSynonyms; i++) {
	Synonym wantedSynonym = resultChildNode->getArg1();
	if (wantedSynonym.getType() == BOOLEAN) {
	break;
	}
	selectSynonyms[wantedSynonym.getName()] = true;
	resultChildNode = resultNode->getNextChild();
	}

	QNode* clausesNode = qTreeRoot->getClausesNode();
	QNode* clauseNode = clausesNode->getChild();
	int numberOfClauses = clausesNode->getNumberOfChildren();

	for (int i = 0; i < numberOfClauses; i++) {
	if (clauseNode->getNodeType() == With) {
	;
	}
	}*/

	/**
	* Method that optimised clauses and constructs a new query tree
	* @param qTreeRoot the root of the query tree
	* @return a QueryTree object that contains the optimised query tree
	*/
	QueryTree* optimiseClauses(QueryTree* qTreeRoot)
	{
		QNode* resultNode = qTreeRoot->getResultNode();
		QNode* optimisedClausesNode = optimiseClausesNode(qTreeRoot->getClausesNode());

		//Instead of creating a new query tree, re-use the old query tree root
		qTreeRoot->setResultNode(resultNode);
		qTreeRoot->setClausesNode(optimisedClausesNode);
		return qTreeRoot;
	}

	/**
	* Method to optimise the CLAUSES subtree
	* @param clausesNode the start of the CLAUSES subtree
	* @return a QNode object that contains the optimised clauses
	*/
	QNode* optimiseClausesNode(QNode* clausesNode)
	{
		pair<vector<QNode*>, vector<QNode*>> clauses = splitConstantClauses(clausesNode);
		vector<QNode*> constantClauses = clauses.first;
		vector<QNode*> nonConstantClauses = clauses.second;
		nonConstantClauses = reorderNonConstantClauses(nonConstantClauses);
		return convertToTree(constantClauses, nonConstantClauses);
	}

	/**
	* Method to split the clauses into clauses containing constants and those that do not
	* @param clausesNode the start of the CLAUSES subtree
	* @return a pair of QNode objects that represent the clauses
	*/
	pair<vector<QNode*>, vector<QNode*>> splitConstantClauses(QNode* clausesNode)
	{
		vector<QNode*> constantClauses;
		vector<QNode*> nonConstantClauses;
		QNode* clauseNode = clausesNode->getChild();
		int numberOfClauses = clausesNode->getNumberOfChildren();

		for (int i = 0; i < numberOfClauses; i++) {
			QNODE_TYPE qnode_type = clauseNode->getNodeType();
			Synonym LHS;
			Synonym RHS;
			if (qnode_type == Pattern) {
				LHS = clauseNode->getArg0();
				RHS = clauseNode->getArg1();
			} else {
				LHS = clauseNode->getArg1();
				RHS = clauseNode->getArg2();
			}
			SYNONYM_TYPE typeLHS = LHS.getType();
			SYNONYM_TYPE typeRHS = RHS.getType();

			//If it is a constant, set the direction
			if (typeLHS == STRING_INT || typeLHS == STRING_CHAR || typeLHS == STRING_PATTERNS) {
				clauseNode->setDirection(LeftToRight);
				double reductionFactor = getReductionFactor(qnode_type, typeLHS, typeRHS, LeftToRight);
				reduceSynonymsCount(LHS.getName(), reductionFactor);  //Set the new expected count
			} else if (typeRHS == STRING_INT || typeRHS == STRING_CHAR || typeRHS == STRING_PATTERNS) {
				clauseNode->setDirection(RightToLeft);
				double reductionFactor = getReductionFactor(qnode_type, typeLHS, typeRHS, LeftToRight);
				reduceSynonymsCount(RHS.getName(), reductionFactor);  //Set the new expected count
			} else {
				nonConstantClauses.push_back(clauseNode);
				clauseNode = clausesNode->getNextChild();
				continue;
			}				

			constantClauses.push_back(clauseNode);
			clauseNode = clausesNode->getNextChild();
		}
		return make_pair(constantClauses, nonConstantClauses);
	}

	/**
	* Method to reorder the non-constant clauses
	* @param A vector of clauses that is to be reordered
	* @return a vector of QNode objects that are re-ordered
	*/
	vector<QNode*> reorderNonConstantClauses(vector<QNode*> clauses)
	{
		//TODO: Check that the vector is not of size zero
		vector<QNode*> reorderedClauses;

		while (clauses.size() != 0) {
			QNode* smallestClause = getNextSmallestAndUpdate(clauses);
			reorderedClauses.push_back(smallestClause);
		}

		return reorderedClauses;
	}

	/**
	* Method that updates the relevant information after getting the appropriate smallest cost clause
	* @param A vector of clauses that is to be reordered
	* @return a QNode object that is suitably small
	*/
	QNode* getNextSmallestAndUpdate(vector<QNode*> &clauses)
	{
		pair<int, DIRECTION> indexDirectionPair = findIndexAndDirectionWithSmallestCost(clauses);
		int smallestIndex = indexDirectionPair.first;  //Set index 0 as the smallest cost clause temporarily
		DIRECTION direction = indexDirectionPair.second;

		QNode* smallestNode = clauses[smallestIndex];
		smallestNode->setDirection(direction);
		clauses.erase(clauses.begin() + smallestIndex);
		QNODE_TYPE qnode_type = smallestNode->getNodeType();

		pair<Synonym, Synonym> argumentPair = getClauseArguments(smallestNode);
		Synonym LHS = argumentPair.first;
		Synonym RHS = argumentPair.second;
		SYNONYM_TYPE typeLHS = LHS.getType();
		SYNONYM_TYPE typeRHS = RHS.getType();
		string nameLHS = LHS.getName();
		string nameRHS = RHS.getName();

		double reductionFactor = getReductionFactor(qnode_type, typeLHS, typeRHS, direction);

		if (direction == LeftToRight) {
			reduceSynonymsCount(nameRHS, reductionFactor);
		} else {
			reduceSynonymsCount(nameLHS, reductionFactor);
		}

		mainTableSynonyms.insert(nameLHS);
		mainTableSynonyms.insert(nameRHS);
		return smallestNode;
	}

	/**
	* Method that decides which is the smallest clause to be taken out of the given vector of clauses
	* Also decides which direction is better for evaluation
	* @param A vector of clauses that is to be reordered
	* @return The index of the clause in the vector and the direction
	*/
	pair<int, DIRECTION> findIndexAndDirectionWithSmallestCost(vector<QNode*> clauses)
	{
		int smallestIndex = 0;  //Set index 0 as the smallest cost clause temporarily
		double smallestCost = UINT_MAX;  //Set the maximum for smallest cost
		bool isJoinClauseFound = false;
		DIRECTION direction = LeftToRight;

		for (unsigned int i = 0; i < clauses.size(); i++) {
			QNODE_TYPE qnode_type = clauses[i]->getNodeType();
			pair<Synonym, Synonym> argumentPair = getClauseArguments(clauses[i]);
			Synonym LHS = argumentPair.first;
			Synonym RHS = argumentPair.second;
			SYNONYM_TYPE typeLHS = LHS.getType();
			SYNONYM_TYPE typeRHS = RHS.getType();
			string nameLHS = LHS.getName();
			string nameRHS = RHS.getName();
			bool hasJoinSynonym = (isContainedInMain(nameLHS) || isContainedInMain(nameRHS));

			if (isJoinClauseFound && !hasJoinSynonym) {
				continue;
			}

			if (hasJoinSynonym) {
				isJoinClauseFound = true;
			}

			//Check left to right direction
			double numberOfValues = synonymsCount[nameLHS];
			double cost = calculateCost(qnode_type, numberOfValues);

			if (cost < smallestCost) {
				smallestCost = cost;
				smallestIndex = i;
				direction = LeftToRight;
			}

			//Check right to left direction
			numberOfValues = synonymsCount[nameRHS];
			cost = calculateCost(qnode_type, numberOfValues);

			if (cost < smallestCost) {
				smallestCost = cost;
				smallestIndex = i;
				direction = RightToLeft;
			}
		}
		return make_pair(smallestIndex, direction);
	}

	/**
	* Method that sets the LHS and RHS appropriately given a clause
	* e.g. LHS and RHS for usual clauses is at arg1 and arg 2
	* e.g. LHS and RHS for pattern is at arg0 and arg1
	* @param A QNode* object representing a clause, LHS and RHS Synonym objects
	*/
	void setClauseArguments(QNode* clause, Synonym LHS, Synonym RHS)
	{
		QNODE_TYPE qnode_type = clause->getNodeType();
		if (qnode_type == Pattern) {
			clause->setArg0(LHS);
			clause->setArg1(RHS);
		} else {
			clause->setArg1(LHS);
			clause->setArg2(LHS);
		}
	}

	/**
	* Method that returns the appropriate LHS and RHS given a clause
	* e.g. LHS and RHS for usual clauses is at arg1 and arg 2
	* e.g. LHS and RHS for pattern is at arg0 and arg1
	* @param A QNode* object representing a clause
	* @return The pair of LHS and RHS synonyms that are in the clause
	*/
	pair<Synonym, Synonym> getClauseArguments(QNode* clause)
	{
		Synonym LHS;
		Synonym RHS;
		QNODE_TYPE qnode_type = clause->getNodeType();
		if (qnode_type == Pattern) {
			LHS = clause->getArg0();
			RHS = clause->getArg1();
		} else {
			LHS = clause->getArg1();
			RHS = clause->getArg2();
		}
		return make_pair(LHS, RHS);
	}

	/**
	* Method to get the reduction factor of a given clause
	* @return The reduction factor
	*/
	double getReductionFactor(QNODE_TYPE rel_type, SYNONYM_TYPE typeLHS, SYNONYM_TYPE typeRHS, DIRECTION direction)
	{
		return statsTable->getReductionFactor(rel_type, typeLHS, typeRHS, direction);
	}

	/**
	* Method to get the estimated cost of evaluating this relation
	* @param The relationship type
	* @param The estimated number of times this relation will be called
	*/
	double calculateCost(QNODE_TYPE rel_type, double numberOfValues)
	{
		switch (rel_type) {
		case ModifiesS:
		case ModifiesP:
			return numberOfValues * COST_MODIFIES;
		case UsesS:
		case UsesP:
			return numberOfValues * COST_USES;
		case Parent:
			return numberOfValues * COST_PARENT;
		case ParentT:
			if (COST_PARENTS == 0) {
				COST_PARENTS = statsTable->getParentSCost();
			}
			return numberOfValues * COST_PARENTS;
		case Follows:
			return numberOfValues * COST_FOLLOWS;
		case FollowsT:
			if (COST_FOLLOWSS == 0) {
				COST_FOLLOWSS = statsTable->getFollowsSCost();
			}
			return numberOfValues * COST_FOLLOWSS;
		case Calls:
			return numberOfValues * COST_CALLS;
		case CallsT:
			if (COST_CALLSS == 0) {
				COST_CALLSS = statsTable->getCallsSCost();
			}
			return numberOfValues * COST_CALLSS;
		case Next:
			return numberOfValues * COST_NEXT;
		case NextT:
			if (COST_NEXTS == 0) {
				COST_NEXTS = statsTable->getNextSCost();
			}
			return numberOfValues * COST_NEXTS;
		case Affects:
			if (COST_AFFECTS == 0) {
				COST_AFFECTS = statsTable->getAffectsCost();
			}
			return numberOfValues * COST_AFFECTS;
		case AffectsT:
			if (COST_AFFECTSS == 0) {
				COST_AFFECTSS = statsTable->getAffectsSCost();
			}
			return numberOfValues * COST_AFFECTSS;
		case Pattern:
			if (COST_PATTERN == 0) {
				COST_PATTERN = statsTable->getPatternCost();
			}
			return numberOfValues * COST_PATTERN;
		case With:
			return numberOfValues * COST_WITH;
		default:
			return 1;  //It should never reach here
		}
	}

	inline bool isContainedInMain(string wantedName)
	{
		if (mainTableSynonyms.count(wantedName) == 1) {
			return true;
		} else {
			return false;
		}
	}

	/**
	* Method to reduce the synonym count by the reduction factor
	* @param name The name of the synonym
	* @param reductionFactor the reduction factor that is given
	*/
	void reduceSynonymsCount(string name, double reductionFactor)
	{
		auto mapItr = synonymsCount.find(name);
		//Only set if the synonym exists in the map
		if (mapItr != synonymsCount.end()) {
			unsigned int currentCount = synonymsCount[name];
			synonymsCount[name] = (unsigned int)ceil(currentCount * reductionFactor);
		}
	}

	/**
	* Method to join constant clauses and non-constant clauses into one subtree
	* @param a vector of constant clauses
	* @param a vector of non-constant clauses
	* @return A QNode object representing the subtree
	*/
	QNode* convertToTree(vector<QNode*> constantClauses, vector<QNode*> nonConstantClauses)
	{
		Synonym empty;
		QNode* clausesNode = new QNode(CLAUSES, empty, empty, empty);

		for (unsigned int i = 0; i < constantClauses.size(); i++) {
			QNode* clause = constantClauses[i];
			clausesNode->setChild(clause);
			clause->setParent(clausesNode);
		}

		for (unsigned int i = 0; i < nonConstantClauses.size(); i++) {
			QNode* clause = nonConstantClauses[i];
			clausesNode->setChild(clause);
			clause->setParent(clausesNode);
		}
		return clausesNode;
	}
}
