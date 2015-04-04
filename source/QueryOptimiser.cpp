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
	unordered_map<string, int> selectSynonyms;  //Maps the synonyms that are being selected to the index
	unordered_map<string, unsigned int> synonymsCount;  //Maps synonyms to the expected number of values
	set<string> mainTableSynonyms;  //Denotes synonyms that are already in the main table after clause evaluation
	StatisticsTable* statsTable;

	void initialize(QueryTree* qTreeRoot);

	QueryTree* flattenQuery(QueryTree* qTreeRoot);
	QueryTree* rewriteWithClauses(QueryTree* qTreeRoot);
	vector<QNode*> populateSelectSynonyms(QNode* resultNode);
	pair<vector<QNode*>, vector<QNode*>> splitWithClauses(QNode* clausesNode);
	pair<bool, DIRECTION> isWithClauseRewritable(Synonym LHS, Synonym RHS);

	QueryTree* removeReduantClausesAndSynonyms(QueryTree* qTreeRoot);
	QNode* scanAndReplaceRedundantItems(QNode* clausesNode, vector<vector<int>> adjacencyMatrix, unordered_map<string, int> name_index_map);
	void replaceRedundantSynonymInClauses(QNode* &clausesNode, string name);
	vector<QNode*> replaceSynonyms(vector<QNode*> clausesVector, Synonym original, Synonym replacement);

	QueryTree* optimiseClauses(QueryTree* qTreeRoot);
	QNode* optimiseClausesNode(QNode* clausesNode);
	pair<vector<QNode*>, vector<QNode*>> splitConstantClauses(QNode* clausesNode);
	vector<QNode*> reorderNonConstantClauses(vector<QNode*> clauses);

	QNode* getNextSmallestAndUpdate(vector<QNode*> &clauses);
	pair<int, DIRECTION> findIndexAndDirectionWithSmallestCost(vector<QNode*> clauses);
	pair<Synonym, Synonym> getClauseArguments(QNode* clause);
	void setClauseArguments(QNode* &clause, Synonym LHS, Synonym RHS);

	int getExpectedCount(QNODE_TYPE rel_type, SYNONYM_TYPE type_probe, SYNONYM_TYPE type_output);
	double calculateCost(QNODE_TYPE rel_type, double numberOfValues);
	inline bool isContainedInMain(string wantedName);
	inline bool isSelectSynonym(string wantedName);
	void reduceSynonymsCount(string name, double reductionFactor);

	QNode* createSubtree(QNODE_TYPE type, vector<QNode*> clausesVector);
	QNode* combineClausesVectors(vector<QNode*> clausesVector1, vector<QNode*> clausesVector2);
	pair<bool, DIRECTION> determineSupersetSubset(Synonym LHS, Synonym RHS);
	unordered_map<string, int> indexSynonymsReferenced(QNode* resultNode, QNode* clausesNode);
	vector<vector<int>> createSynonymGraph(QNode* clausesNode, unordered_map<string, int> name_index_map);
	set<int> scanSynonymsReachable(vector<vector<int>> adjacencyMatrix);

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
		qTreeRoot = rewriteWithClauses(qTreeRoot);
		qTreeRoot = removeReduantClausesAndSynonyms(qTreeRoot);
		return qTreeRoot;
	}

	QueryTree* rewriteWithClauses(QueryTree* qTreeRoot)
	{
		vector<QNode*> resultClauses = populateSelectSynonyms(qTreeRoot->getResultNode());
		pair<vector<QNode*>, vector<QNode*>> clauses_with_pair = splitWithClauses(qTreeRoot->getClausesNode());
		vector<QNode*> clauses = clauses_with_pair.first;
		vector<QNode*> withClauses = clauses_with_pair.second;
		vector<QNode*> finalWithClauses;

		for (unsigned int i = 0; i < withClauses.size(); i++) {
			QNode* singleWithClause = withClauses[i];
			Synonym LHS = singleWithClause->getArg1();
			Synonym RHS = singleWithClause->getArg2();
			pair<bool, DIRECTION> isRewriteable_direction_pair = isWithClauseRewritable(LHS, RHS);
			bool isRewritable = isRewriteable_direction_pair.first;

			if (isRewritable) {
				DIRECTION direction = isRewriteable_direction_pair.second;
				Synonym superset;
				Synonym subset;
				if (direction == LeftToRight) {
					superset = LHS;
					subset = RHS;
				} else {
					superset = RHS;
					subset = LHS;
				}
				resultClauses = replaceSynonyms(resultClauses, superset, subset);
				clauses = replaceSynonyms(clauses, superset, subset);
				withClauses.erase(withClauses.begin() + i);
				i--;
				withClauses = replaceSynonyms(withClauses, superset, subset);
			} else {
				finalWithClauses.push_back(singleWithClause);
			}
		}
		QNode* resultSubtree = createSubtree(RESULT, resultClauses);
		QNode* clausesSubtree = combineClausesVectors(clauses, finalWithClauses);
		qTreeRoot->setResultNode(resultSubtree);
		qTreeRoot->setClausesNode(clausesSubtree);
		return qTreeRoot;
	}

	pair<vector<QNode*>, vector<QNode*>> splitWithClauses(QNode* clausesNode)
	{
		vector<QNode*> clauses;
		vector<QNode*> withClauses;
		QNode* clauseNode = clausesNode->getChild();
		int numberOfClauses = clausesNode->getNumberOfChildren();

		for (int i = 0; i < numberOfClauses; i++) {
			QNODE_TYPE qnode_type = clauseNode->getNodeType();
			if (qnode_type == With) {
				withClauses.push_back(clauseNode);
			} else {
				clauses.push_back(clauseNode);
			}
			clauseNode = clausesNode->getNextChild();
		}
		return make_pair(clauses, withClauses);
	}

	pair<vector<QNode*>, vector<QNode*>> splitRedundantClauses(QNode* clausesNode, set<int> synonymsReachable, unordered_map<string, int> name_index_map)
	{
		vector<QNode*> redundantVector;
		vector<QNode*> clausesVector;
		int numberOfClauses = clausesNode->getNumberOfChildren();
		QNode* childNode = clausesNode->getChild();

		for (int i = 0; i < numberOfClauses; i++) {
			pair<Synonym, Synonym> synonymPair = getClauseArguments(childNode);
			Synonym LHS = synonymPair.first;
			Synonym RHS = synonymPair.second;
			string nameLHS = LHS.getName();
			string nameRHS = RHS.getName();
			int indexLHS = -1;
			int indexRHS = -1;

			if (name_index_map.count(nameLHS) == 1) {
				//This synonym is referenced in the query
				indexLHS = name_index_map[nameLHS];
				if (synonymsReachable.count(indexLHS) == 0) {
					//todo: assert that RHS is also not reachable
					//Since LHS is not reachable, RHS must also not be reachable
					//This clause might be redundant
					redundantVector.push_back(childNode);
					childNode = clausesNode->getNextChild();
					continue;
				} else {
					//LHS is reachable
					clausesVector.push_back(childNode);
					childNode = clausesNode->getNextChild();
					continue;
				}
			} else {
				//LHS is a constant
				//Do nothing and check the RHS
			}

			if (name_index_map.count(nameRHS) == 1) {
				//This synonym is referenced in the query
				indexRHS = name_index_map[nameRHS];
				if (synonymsReachable.count(indexRHS) == 0) {
					//todo: assert that RHS is also not reachable
					//Since RHS is not reachable, LHS must also not be reachable
					//This clause might be redundant
					redundantVector.push_back(childNode);
					childNode = clausesNode->getNextChild();
					continue;
				} else {
					//RHS is reachable
					clausesVector.push_back(childNode);
					childNode = clausesNode->getNextChild();
					continue;
				}
			} else {
				//LHS and RHS are constants
				//They are possibly redundant
				//Push to redundant vector
				redundantVector.push_back(childNode);
				childNode = clausesNode->getNextChild();
				continue;
			}
		}
		return make_pair(clausesVector, redundantVector);
	}

	QueryTree* removeReduantClausesAndSynonyms(QueryTree* qTreeRoot)
	{
		QNode* resultNode = qTreeRoot->getResultNode();
		QNode* clausesNode = qTreeRoot->getClausesNode();
		unordered_map<string, int> name_index_map = indexSynonymsReferenced(resultNode, clausesNode);
		vector<vector<int>> adjacencyMatrix = createSynonymGraph(clausesNode, name_index_map);
		set<int> synonymIndexReachable = scanSynonymsReachable(adjacencyMatrix);

		clausesNode = scanAndReplaceRedundantItems(clausesNode, adjacencyMatrix, name_index_map);
		qTreeRoot->setResultNode(resultNode);
		qTreeRoot->setClausesNode(clausesNode);
		return qTreeRoot;
	}

	QNode* scanAndReplaceRedundantItems(QNode* clausesNode, vector<vector<int>> adjacencyMatrix, unordered_map<string, int> name_index_map)
	{
		QueryEvaluator::resetValues(synonymNameToTypeMap);
		set<int> synonymsReachable = scanSynonymsReachable(adjacencyMatrix);
		pair<vector<QNode*>, vector<QNode*>> clausesPair = splitRedundantClauses(clausesNode, synonymsReachable, name_index_map);
		vector<QNode*> clausesVector = clausesPair.first;
		vector<QNode*> redundantVector = clausesPair.second;  //A vector of potentially redundant clauses

		scanAndRemoveRedundantSynonyms(redundantVector, adjacencyMatrix, name_index_map);
		QNode* redundantClauses = createSubtree(CLAUSES, redundantVector);
		redundantClauses = optimiseClausesNode(redundantClauses);
		bool isRedundantClausesValid = QueryEvaluator::processClausesNode(redundantClauses);

		if (!isRedundantClausesValid) {
			throw exception();
		} else {
			scanAndRemoveRedundantSynonyms(clausesVector, adjacencyMatrix, name_index_map);
			return createSubtree(CLAUSES, clausesVector);
		}

		/*for (auto itr = name_index_map.begin(); itr != name_index_map.end(); ++itr) {
			string name = itr->first;
			int index = itr->second;
			int count = 0;
			for (unsigned int i = 0; i < adjacencyMatrix.size(); i++) {
				count += adjacencyMatrix[index][i];
			}

			if (count == 0 || count == 1) {
				if (isSelectSynonym(name)) {
					continue;  //Cannot replace a synonym that is being selected
				} else {
					//Since this synonym can be replaced, attempt to replace it in the clauses
					replaceRedundantSynonymInClauses(clausesNode, name);
				}
			}
		}*/
		throw exception();
	}

	void scanAndRemoveRedundantSynonyms(vector<QNode*> clauses, vector<vector<int>> adjacencyMatrix, unordered_map<string, int> name_index_map)
	{
		Synonym undefined(UNDEFINED, "_");
		unordered_map<string, int> synonymOccurrence;

		for (unsigned int i = 0; i < clauses.size(); i++) {
			QNode* clause = clauses[i];
			QNODE_TYPE query_type = clause->getNodeType();
			SYNONYM_TYPE type = synonymNameToTypeMap[name];
			Synonym redundant(type, name);

			pair<Synonym, Synonym> synonymPair = getClauseArguments(clause);
			Synonym LHS = synonymPair.first;
			Synonym RHS = synonymPair.second;

			if (query_type == With || query_type == Pattern || query_type == Parent || 
				query_type == ParentT || query_type == Follows || query_type == FollowsT) {
				continue;
			}

			//Check for LHS if it is redundant but it cannot be a With, Modifies or Uses
			if (LHS == redundant) {
				if (query_type != ModifiesS && query_type != ModifiesP && 
					query_type != UsesS && query_type != UsesP) {
						LHS = undefined;
				}
			}

			if (RHS == redundant) {
				RHS = undefined;
			}

			setClauseArguments(clause, LHS, RHS);
		}
	}

	vector<QNode*> replaceSynonyms(vector<QNode*> clausesVector, Synonym toBeReplaced, Synonym replacement)
	{
		vector<QNode*> finalClauses;

		for (unsigned int i = 0; i < clausesVector.size(); i++) {
			QNode* singleClause = clausesVector[i];
			pair<Synonym, Synonym> synonymPair = getClauseArguments(singleClause);
			Synonym LHS = synonymPair.first;
			Synonym RHS = synonymPair.second;

			if (LHS == toBeReplaced) {
				LHS = replacement;
			}
			if (RHS == toBeReplaced) {
				RHS = replacement;
			}
			setClauseArguments(singleClause, LHS, RHS);
			finalClauses.push_back(singleClause);
		}
		return finalClauses;
	}

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
		return combineClausesVectors(constantClauses, nonConstantClauses);
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
			pair<Synonym, Synonym> argumentPair = getClauseArguments(clauseNode);
			Synonym LHS = argumentPair.first;
			Synonym RHS = argumentPair.second;
			SYNONYM_TYPE typeLHS = LHS.getType();
			SYNONYM_TYPE typeRHS = RHS.getType();

			//If it is a constant, set the direction
			if (typeLHS == STRING_INT || typeLHS == STRING_CHAR || typeLHS == STRING_PATTERNS || typeRHS == UNDEFINED) {
				clauseNode->setDirection(LeftToRight);
				double reductionFactor = statsTable->getReductionFactor(qnode_type, typeLHS, typeRHS, LeftToRight);
				reduceSynonymsCount(LHS.getName(), reductionFactor);  //Set the new expected count
			} else if (typeRHS == STRING_INT || typeRHS == STRING_CHAR || typeRHS == STRING_PATTERNS || typeLHS == UNDEFINED) {
				clauseNode->setDirection(RightToLeft);
				double reductionFactor = statsTable->getReductionFactor(qnode_type, typeLHS, typeRHS, RightToLeft);
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

		double reductionFactor = statsTable->getReductionFactor(qnode_type, typeLHS, typeRHS, direction);

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
	void setClauseArguments(QNode* &clause, Synonym LHS, Synonym RHS)
	{
		QNODE_TYPE qnode_type = clause->getNodeType();
		if (qnode_type == Pattern) {
			clause->setArg0(LHS);
			clause->setArg1(RHS);
		} else {
			clause->setArg1(LHS);
			clause->setArg2(RHS);
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

	pair<bool, DIRECTION> determineSupersetSubset(Synonym LHS, Synonym RHS)
	{
		SYNONYM_TYPE typeLHS = LHS.getType();
		SYNONYM_TYPE typeRHS = RHS.getType();
		SYNONYM_ATTRIBUTE attributeLHS = LHS.getAttribute();
		SYNONYM_ATTRIBUTE attributeRHS = RHS.getAttribute();

		if (typeLHS == STRING_CHAR) {
			if (typeRHS == VARIABLE || typeRHS == PROCEDURE) {
				return make_pair(true, RightToLeft);  //Do not accept call stmts as a superset
			} else {
				return make_pair(false, RightToLeft);
			}
		} else if (typeLHS == STRING_INT) {
			if (attributeRHS == stmtNo) {
				return make_pair(true, RightToLeft);
			} else {
				return make_pair(false, RightToLeft);
			}
		} else if (typeLHS == STMT) {
			if (typeRHS == ASSIGN || typeRHS == CALL || typeRHS == WHILE || typeRHS == IF || typeRHS == STRING_INT) {
				return make_pair(true, LeftToRight);  //If RHS is also STMT, does it count? 
			} else {
				return make_pair(false, LeftToRight);
			}
		} else if (typeLHS == ASSIGN || typeLHS == CALL || typeLHS == WHILE || 
			typeLHS == IF || typeLHS == PROG_LINE || typeLHS == CONSTANT) {
				if (typeRHS == STRING_INT) {
					return make_pair(true, LeftToRight);
				} else {
					return make_pair(false, LeftToRight);
				}
		} else if (typeLHS == PROCEDURE || typeLHS == VARIABLE) {
			if (typeRHS == STRING_CHAR) {
				return make_pair(true, LeftToRight);
			} else {
				return make_pair(false, LeftToRight);
			}
		} else {
			return make_pair(false, LeftToRight);
		}
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

	vector<QNode*> populateSelectSynonyms(QNode* resultNode)
	{
		vector<QNode*> resultSynonyms;
		QNode* resultChildNode = resultNode->getChild();
		int numberOfSynonyms = resultNode->getNumberOfChildren();

		//Populate the select synonyms map
		for (int i = 0; i < numberOfSynonyms; i++) {
			Synonym wantedSynonym = resultChildNode->getArg1();
			if (wantedSynonym.getType() != BOOLEAN) {
				selectSynonyms[wantedSynonym.getName()] = i;
			}
			resultSynonyms.push_back(resultChildNode);
			resultChildNode = resultNode->getNextChild();
		}
		return resultSynonyms;
	}

	pair<bool, DIRECTION> isWithClauseRewritable(Synonym LHS, Synonym RHS)
	{
		pair<bool, DIRECTION> superset_subset_pair = determineSupersetSubset(LHS, RHS);
		bool isSupersetSubset = superset_subset_pair.first;

		if (!isSupersetSubset) {
			return superset_subset_pair;
		}

		Synonym superset;
		Synonym subset;
		if (superset_subset_pair.second == LeftToRight) {
			superset = LHS;
			subset = RHS;
		} else {
			superset = RHS;
			subset = LHS;
		}
		string nameSuperset = superset.getName();
		SYNONYM_TYPE typeSubset = subset.getType();

		if (typeSubset == STRING_INT || typeSubset == STRING_CHAR) {
			if (isSelectSynonym(nameSuperset)) {
				return make_pair(false, LeftToRight);  //Cannot replace a select synonym with constants
			}
		}
		return superset_subset_pair;
	}

	inline bool isContainedInMain(string wantedName)
	{
		if (mainTableSynonyms.count(wantedName) == 1) {
			return true;
		} else {
			return false;
		}
	}

	inline bool isSelectSynonym(string wantedName)
	{
		if (selectSynonyms.count(wantedName) == 1) {
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

	unordered_map<string, int> indexSynonymsReferenced(QNode* resultNode, QNode* clausesNode)
	{
		unordered_map<string, int> name_index_map = selectSynonyms;
		int index = selectSynonyms.size();
		/*
		int numberOfClauses = resultNode->getNumberOfChildren();
		QNode* childNode = resultNode->getChild();

		for (int i = 0; i < numberOfClauses; i++) {
			Synonym synonym = childNode->getArg1();
			if (synonym.getType() == BOOLEAN) {
				break;
			} else {
				string name = synonym.getName();
				if (name_index_map.count(name) == 0) {
					name_index_map[name] = index;
					index++;
				}
				childNode = resultNode->getNextChild();
			}
		}*/

		int numberOfClauses = clausesNode->getNumberOfChildren();
		QNode* childNode = clausesNode->getChild();

		for (int i = 0; i < numberOfClauses; i++) {
			pair<Synonym, Synonym> synonymPair = getClauseArguments(childNode);
			Synonym LHS = synonymPair.first;
			Synonym RHS = synonymPair.second;
			SYNONYM_TYPE typeLHS = LHS.getType();
			SYNONYM_TYPE typeRHS = RHS.getType();

			if (typeLHS != STRING_CHAR && typeLHS != STRING_INT && typeLHS != STRING_PATTERNS && typeLHS != UNDEFINED) {
				string name = LHS.getName();
				if (name_index_map.count(name) == 0) {
					name_index_map[name] = index;
					index++;
				}
			}

			if (typeRHS != STRING_CHAR && typeRHS != STRING_INT && typeRHS != STRING_PATTERNS && typeRHS != UNDEFINED) {
				string name = RHS.getName();
				if (name_index_map.count(name) == 0) {
					name_index_map[name] = index;
					index++;
				}
			}
			childNode = clausesNode->getNextChild();
		}
		return name_index_map;
	}

	vector<vector<int>> createSynonymGraph(QNode* clausesNode, unordered_map<string, int> name_index_map)
	{
		//todo: need to mark itself inside the graph?
		vector<vector<int>> adjacencyMatrix;
		int numberOfSynonyms = name_index_map.size();

		adjacencyMatrix.resize(numberOfSynonyms);
		for (int i = 0; i < numberOfSynonyms; i++) {
			adjacencyMatrix[i].resize(numberOfSynonyms, 0);
		}

		int numberOfClauses = clausesNode->getNumberOfChildren();
		QNode* childNode = clausesNode->getChild();

		for (int i = 0; i < numberOfClauses; i++) {
			pair<Synonym, Synonym> synonymPair = getClauseArguments(childNode);
			Synonym LHS = synonymPair.first;
			Synonym RHS = synonymPair.second;
			SYNONYM_TYPE typeLHS = LHS.getType();
			SYNONYM_TYPE typeRHS = RHS.getType();
			int indexLHS;
			int indexRHS;

			if (typeLHS != STRING_CHAR && typeLHS != STRING_INT && typeLHS != STRING_PATTERNS && typeLHS != UNDEFINED) {
				indexLHS = name_index_map[LHS.getName()];
			} else {
				childNode = clausesNode->getNextChild();
				continue;
			}

			if (typeRHS != STRING_CHAR && typeRHS != STRING_INT && typeRHS != STRING_PATTERNS && typeRHS != UNDEFINED) {
				indexRHS = name_index_map[RHS.getName()];
			} else {
				childNode = clausesNode->getNextChild();
				continue;
			}
			adjacencyMatrix[indexLHS][indexRHS] = 1;
			adjacencyMatrix[indexRHS][indexLHS] = 1;
			childNode = clausesNode->getNextChild();
		}
		return adjacencyMatrix;
	}

	set<int> scanSynonymsReachable(vector<vector<int>> adjacencyMatrix)
	{
		int numberOfSynonyms = adjacencyMatrix.size();
		set<int> synonymsReachable;

		for (auto itr = selectSynonyms.begin(); itr != selectSynonyms.end(); ++itr) {
			string name = itr->first;
			int index = selectSynonyms[name];

			bool* visited = new bool[numberOfSynonyms];
			for (int i = 0; i < numberOfSynonyms; i++) {
				visited[i] = false;
			}

			stack<int> dfsStack;
			dfsStack.push(index);

			while (!dfsStack.empty()) {
				int synonymIndex = dfsStack.top();
				dfsStack.pop();
				synonymsReachable.insert(synonymIndex);
				visited[synonymIndex] = true;

				for (int i = 0; i < numberOfSynonyms; i++) {
					if (adjacencyMatrix[synonymIndex][i] == 1 && visited[i] == false) {
						dfsStack.push(i);
					}
				}
			}
			delete visited;
		}
		return synonymsReachable;
	}

	QNode* createSubtree(QNODE_TYPE type, vector<QNode*> clausesVector)
	{
		Synonym empty;
		QNode* clausesNode = new QNode(type, empty, empty, empty, empty);
		for (unsigned int i = 0; i < clausesVector.size(); i++) {
			QNode* clause = clausesVector[i];
			clausesNode->setChild(clause);
			clause->setParent(clausesNode);
		}
		return clausesNode;
	}

	/**
	* Method to join constant clauses and non-constant clauses into one subtree
	* @param a vector of constant clauses
	* @param a vector of non-constant clauses
	* @return A QNode object representing the subtree
	*/
	QNode* combineClausesVectors(vector<QNode*> clausesVector1, vector<QNode*> clausesVector2)
	{
		Synonym empty;
		QNode* clausesNode = new QNode(CLAUSES, empty, empty, empty, empty);

		for (unsigned int i = 0; i < clausesVector1.size(); i++) {
			QNode* clause = clausesVector1[i];
			clausesNode->setChild(clause);
			clause->setParent(clausesNode);
		}

		for (unsigned int i = 0; i < clausesVector2.size(); i++) {
			QNode* clause = clausesVector2[i];
			clausesNode->setChild(clause);
			clause->setParent(clausesNode);
		}
		return clausesNode;
	}
}
