#include "QueryOptimiser.h"

/**
@brief Namespace containing functions for the query optimisation

*/

namespace QueryOptimiser
{
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
	void scanAndRemoveRedundantSynonyms(vector<QNode*> &clauses, vector<vector<int>> adjacencyMatrix, unordered_map<string, int> name_index_map);
	pair<bool, vector<QNode*>> replaceSynonyms(vector<QNode*> clausesVector, Synonym original, Synonym replacement);

	QueryTree* optimiseClauses(QueryTree* qTreeRoot);
	QNode* optimiseClausesNode(QNode* clausesNode);
	pair<vector<QNode*>, vector<QNode*>> splitConstantClauses(QNode* clausesNode);
	vector<QNode*> reorderConstantClauses(vector<QNode*> constantClauses);
	vector<QNode*> reorderNonConstantClauses(vector<QNode*> clauses);

	QNode* getNextSmallestAndUpdate(vector<QNode*> &clauses);
	pair<int, DIRECTION> findIndexAndDirectionWithSmallestCost(vector<QNode*> clauses);
	pair<Synonym, Synonym> getClauseArguments(QNode* clause);
	void setClauseArguments(QNode* &clause, Synonym LHS, Synonym RHS);

	inline bool isContainedInMain(string wantedName);
	inline bool isSelectSynonym(string wantedName);

	QNode* createSubtree(QNODE_TYPE type, vector<QNode*> clausesVector);
	QNode* combineClausesVectors(vector<QNode*> clausesVector1, vector<QNode*> clausesVector2);
	pair<bool, DIRECTION> determineSupersetSubset(Synonym LHS, Synonym RHS);
	unordered_map<string, int> indexSynonymsReferenced(QNode* clausesNode);
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
		synonymNameToTypeMap = qTreeRoot->getSynonymsMap();
		statsTable = new StatisticsTable(synonymNameToTypeMap);
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

		if (clauses.size() != 0) {
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
					//Replace the clauses in the results subtree
					pair<bool, vector<QNode*>> status_clauses_pair = replaceSynonyms(resultClauses, superset, subset);
					resultClauses = status_clauses_pair.second;

					//Replace the clauses in the clauses subtree
					status_clauses_pair = replaceSynonyms(clauses, superset, subset);
					bool isWithRemovable = status_clauses_pair.first;
					clauses = status_clauses_pair.second;

					if (isWithRemovable) {
						//Only remove in the with subtree if it is removable
						withClauses.erase(withClauses.begin() + i);
						i--;
						withClauses = replaceSynonyms(withClauses, superset, subset).second;
					} else {
						finalWithClauses.push_back(singleWithClause);
					}
				} else {
					finalWithClauses.push_back(singleWithClause);
				}
			}
		} else {
			//Cannot remove with clauses if there are no other clauses
			finalWithClauses = withClauses;
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

			if (LHS.isSynonym() && name_index_map.count(nameLHS) == 1) {
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

			if (RHS.isSynonym() && name_index_map.count(nameRHS) == 1) {
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
				//LHS and RHS are constants or _
				//Possibly redundant
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
		populateSelectSynonyms(resultNode);
		unordered_map<string, int> name_index_map = indexSynonymsReferenced(clausesNode);
		vector<vector<int>> adjacencyMatrix = createSynonymGraph(clausesNode, name_index_map);

		clausesNode = scanAndReplaceRedundantItems(clausesNode, adjacencyMatrix, name_index_map);
		qTreeRoot->setResultNode(resultNode);
		qTreeRoot->setClausesNode(clausesNode);
		return qTreeRoot;
	}

	QNode* scanAndReplaceRedundantItems(QNode* clausesNode, vector<vector<int>> adjacencyMatrix, unordered_map<string, int> name_index_map)
	{
		set<int> synonymsReachable = scanSynonymsReachable(adjacencyMatrix);
		pair<vector<QNode*>, vector<QNode*>> clausesPair = splitRedundantClauses(clausesNode, synonymsReachable, name_index_map);
		vector<QNode*> clausesVector = clausesPair.first;
		vector<QNode*> redundantVector = clausesPair.second;  //A vector of potentially redundant clauses

		scanAndRemoveRedundantSynonyms(redundantVector, adjacencyMatrix, name_index_map);
		QNode* redundantClauses = createSubtree(CLAUSES, redundantVector);
		redundantClauses = optimiseClausesNode(redundantClauses);
		QueryEvaluator::resetValues(synonymNameToTypeMap);
		bool isRedundantClausesValid = QueryEvaluator::processClausesNode(redundantClauses);

		if (!isRedundantClausesValid) {
			Synonym LHS(STRING_INT, 0);
			Synonym RHS(STRING_CHAR, "0");
			QNode* falseNode = new QNode(With, LHS, LHS, RHS, RHS);  //Create a false node object that will be false
			vector<QNode*> falseNodes;
			falseNodes.push_back(falseNode);
			return createSubtree(CLAUSES, falseNodes);  //Return an invalid clause
		} else {
			scanAndRemoveRedundantSynonyms(clausesVector, adjacencyMatrix, name_index_map);
			return createSubtree(CLAUSES, clausesVector);
		}
	}

	void scanAndRemoveRedundantSynonyms(vector<QNode*> &clauses, vector<vector<int>> adjacencyMatrix, unordered_map<string, int> name_index_map)
	{
		unordered_map<string, int> synonymOccurrence;

		for (unsigned int i = 0; i < clauses.size(); i++) {
			QNode* clause = clauses[i];
			QNODE_TYPE query_type = clause->getNodeType();

			if (query_type == With || query_type == Sibling || 
				query_type == Contains || query_type == ContainsT) {
					continue;  //Cannot replace synonyms in these clauses
			}

			Synonym undefined(UNDEFINED, "_");
			pair<Synonym, Synonym> synonymPair = getClauseArguments(clause);
			Synonym LHS = synonymPair.first;
			Synonym RHS = synonymPair.second;
			string nameLHS = LHS.getName();
			string nameRHS = RHS.getName();

			//Check LHS and cannot replace it if it is going to be selected
			if (LHS.isSynonym() && !isSelectSynonym(nameLHS)) {
				if (synonymOccurrence.count(nameLHS) == 0) {
					//Count the number of occurrences this synonym appears in the clause
					int index = name_index_map[nameLHS];
					int count = 0;
					for (unsigned int i = 0; i < adjacencyMatrix.size(); i++) {
						count += adjacencyMatrix[index][i];
					}
					synonymOccurrence[nameLHS] = count;
				} 

				int count = synonymOccurrence[nameLHS];
				if (count == 0 || count == 1) {
					//This synonym is likely to be redundant
					if (query_type == Parent || query_type == ParentT || query_type == Follows || 
						query_type == FollowsT || query_type == Next || query_type == NextT) {
							if (LHS.getType() == STMT || LHS.getType() == PROG_LINE) {
								LHS = undefined;  //Only stmt/progline can be replaced in the above clauses
							}
					} else if (query_type != ModifiesS && query_type != ModifiesP && 
						query_type != UsesS && query_type != UsesP && query_type != Pattern) {
							LHS = undefined;  //LHS cannot be "_" in the these clauses
					}
				}
			}

			//Check RHS and cannot replace it if it is going to be selected
			if (RHS.isSynonym() && !isSelectSynonym(nameRHS)) {
				if (synonymOccurrence.count(nameRHS) == 0) {
					//Count the number of occurrences this synonym appears in the clause
					int index = name_index_map[nameRHS];
					int count = 0;
					for (unsigned int i = 0; i < adjacencyMatrix.size(); i++) {
						count += adjacencyMatrix[index][i];
					}
					synonymOccurrence[nameRHS] = count;
				} 

				int count = synonymOccurrence[nameRHS];
				if (count == 0 || count == 1) {
					if (query_type == Parent || query_type == ParentT || query_type == Follows || 
						query_type == FollowsT || query_type == Next || query_type == NextT) {
							if (RHS.getType() == STMT || RHS.getType() == PROG_LINE) {
								RHS = undefined;
							}
					} else {
						RHS = undefined;  //Can replace RHS directly if clause is not one of the above
					}
				}
			}
			setClauseArguments(clause, LHS, RHS);
			swap(clause, clauses[i]);
		}
	}

	pair<bool, vector<QNode*>> replaceSynonyms(vector<QNode*> clausesVector, Synonym toBeReplaced, Synonym replacement)
	{
		vector<QNode*> finalClauses;
		bool isRemovable = true;  //Denote that the with clause can be removed

		for (unsigned int i = 0; i < clausesVector.size(); i++) {
			QNode* singleClause = clausesVector[i];
			pair<Synonym, Synonym> synonymPair = getClauseArguments(singleClause);
			Synonym LHS = synonymPair.first;
			Synonym RHS = synonymPair.second;
			QNODE_TYPE qnode_type = singleClause->getNodeType();

			if (LHS == toBeReplaced) {
				if (qnode_type == Pattern || qnode_type == Sibling ||
					qnode_type == Contains || qnode_type == ContainsT) {
					isRemovable = false;  //Cannot rewrite these clauses
				} else {
					LHS = replacement;
				}
			}

			if (RHS == toBeReplaced) {
				if (qnode_type == Sibling || qnode_type == Contains || qnode_type == ContainsT) {
					isRemovable = false;  //Cannot rewrite these clauses
				} else {
					RHS = replacement;
				}
			}
			setClauseArguments(singleClause, LHS, RHS);
			finalClauses.push_back(singleClause);
		}
		return make_pair(isRemovable, finalClauses);
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
		constantClauses = reorderConstantClauses(constantClauses);
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

			if (LHS.isUndefined() && RHS.isUndefined()) {
				clauseNode->setDirection(LeftToRight);
			} else if (LHS.isConstant()) {
				clauseNode->setDirection(LeftToRight);
				statsTable->reduceCount(qnode_type, LHS, RHS, LeftToRight);  //Set the new expected count
			} else if (RHS.isConstant()) {
				clauseNode->setDirection(RightToLeft);
				statsTable->reduceCount(qnode_type, LHS, RHS, RightToLeft);  //Set the new expected count
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

	vector<QNode*> reorderConstantClauses(vector<QNode*> clauses)
	{
		//TODO: Check that the vector is not of size zero
		vector<QNode*> bothConstants;
		vector<QNode*> oneConstant;

		for (unsigned int i = 0; i < clauses.size(); i++) {
			QNode* clause = clauses[i];
			QNODE_TYPE qnode_type = clause->getNodeType();

			pair<Synonym, Synonym> argumentPair = getClauseArguments(clause);
			Synonym LHS = argumentPair.first;
			Synonym RHS = argumentPair.second;

			if (LHS.isConstant() && RHS.isConstant()) {
				bothConstants.push_back(clause);
				continue;
			}

			if (LHS.isUndefined() && RHS.isUndefined()) {
				bothConstants.push_back(clause);
				continue;
			}

			oneConstant.push_back(clause);
		}

		vector<QNode*> reorderedClauses;
		reorderedClauses.insert(reorderedClauses.end(), bothConstants.begin(), bothConstants.end());
		reorderedClauses.insert(reorderedClauses.end(), oneConstant.begin(), oneConstant.end());
		return reorderedClauses;
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
		string nameLHS = LHS.getName();
		string nameRHS = RHS.getName();

		statsTable->reduceCount(qnode_type, LHS, RHS, direction);

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
			string nameLHS = LHS.getName();
			string nameRHS = RHS.getName();
			bool hasJoinSynonym = (isContainedInMain(nameLHS) || isContainedInMain(nameRHS));

			if (isJoinClauseFound && !hasJoinSynonym) {
				continue;  //Do not take other clauses that have no synonym to join on
			}

			if (hasJoinSynonym && !isJoinClauseFound) {
				isJoinClauseFound = true;
				double costLHS = statsTable->calculateCost(qnode_type, LHS, RHS, LeftToRight);
				double costRHS = statsTable->calculateCost(qnode_type, LHS, RHS, RightToLeft);

				if (costLHS <= costRHS) {
					smallestCost = costLHS;
					smallestIndex = i;
					direction = LeftToRight;
				} else {
					smallestCost = costRHS;
					smallestIndex = i;
					direction = RightToLeft;
				}
				continue;
			}

			//Check left to right direction
			double cost = statsTable->calculateCost(qnode_type, LHS, RHS, LeftToRight);
			if (cost <= smallestCost) {
				smallestCost = cost;
				smallestIndex = i;
				direction = LeftToRight;
			}

			//Check right to left direction
			cost = statsTable->calculateCost(qnode_type, LHS, RHS, RightToLeft);
			if (cost <= smallestCost) {
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
				BOOLEAN_ isSubset = ValuesHandler::isValueSubset(typeRHS, LHS.getName());
				return make_pair(isSubset, RightToLeft);  //Do not accept call stmts as a superset
			} else {
				return make_pair(false, RightToLeft);
			}
		} else if (typeLHS == STRING_INT) {
			if (attributeRHS == stmtNo) {
				//Only stmt, assign, call, while, if synonyms have attribute as stmtNo
				BOOLEAN_ isSubset = ValuesHandler::isValueSubset(typeRHS, stoi(LHS.getName()));
				return make_pair(isSubset, RightToLeft);
			} else {
				return make_pair(false, RightToLeft);
			}
		} else if (typeLHS == STMT || typeLHS == PROG_LINE) {
			if (typeRHS == ASSIGN || typeRHS == CALL || typeRHS == WHILE || typeRHS == IF || typeRHS == STRING_INT) {
				return make_pair(true, LeftToRight);  //Cannot replace a stmt with another stmt
			} else {
				return make_pair(false, LeftToRight);
			}
		} else if (typeLHS == ASSIGN || typeLHS == CALL || typeLHS == WHILE || typeLHS == IF || typeLHS == CONSTANT) {
			if (typeRHS == STRING_INT) {
				BOOLEAN_ isSubset = ValuesHandler::isValueSubset(typeLHS, stoi(RHS.getName()));
				return make_pair(isSubset, LeftToRight);
			} else {
				return make_pair(false, LeftToRight);
			}
		} else if (typeLHS == PROCEDURE || typeLHS == VARIABLE) {
			if (typeRHS == STRING_CHAR) {
				BOOLEAN_ isSubset = ValuesHandler::isValueSubset(typeLHS, RHS.getName());
				return make_pair(isSubset, LeftToRight);
			} else {
				return make_pair(false, LeftToRight);
			}
		} else {
			return make_pair(false, LeftToRight);
		}
	}

	vector<QNode*> populateSelectSynonyms(QNode* resultNode)
	{
		vector<QNode*> resultSynonyms;
		QNode* resultChildNode = resultNode->getChild();
		int numberOfSynonyms = resultNode->getNumberOfChildren();
		selectSynonyms.clear();

		//Populate the select synonyms map
		for (int i = 0; i < numberOfSynonyms; i++) {
			Synonym wantedSynonym = resultChildNode->getArg1();
			string name = wantedSynonym.getName();
			if (wantedSynonym.getType() != BOOLEAN && selectSynonyms.count(name) == 0) {
				selectSynonyms[name] = i;
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

		if (isSelectSynonym(nameSuperset) && subset.isConstant()) {
			return make_pair(false, LeftToRight);  //Cannot replace a select synonym with constants
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

	unordered_map<string, int> indexSynonymsReferenced(QNode* clausesNode)
	{
		unordered_map<string, int> name_index_map = selectSynonyms;  //Index the selected synonyms first
		int index = selectSynonyms.size();  //Initialize the index value to the number of select synonyms
		int numberOfClauses = clausesNode->getNumberOfChildren();
		QNode* childNode = clausesNode->getChild();

		for (int i = 0; i < numberOfClauses; i++) {
			pair<Synonym, Synonym> synonymPair = getClauseArguments(childNode);
			Synonym LHS = synonymPair.first;
			Synonym RHS = synonymPair.second;

			if (LHS.isSynonym()) {
				string name = LHS.getName();
				if (name_index_map.count(name) == 0) {
					name_index_map[name] = index;
					index++;
				}
			}

			if (RHS.isSynonym()) {
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
			int indexLHS;
			int indexRHS;

			if (LHS.isSynonym() && RHS.isSynonym()) {
				indexLHS = name_index_map[LHS.getName()];
				indexRHS = name_index_map[RHS.getName()];
				adjacencyMatrix[indexLHS][indexRHS]++;
				adjacencyMatrix[indexRHS][indexLHS]++;
			} else if (LHS.isSynonym()) {
				indexLHS = name_index_map[LHS.getName()];
				adjacencyMatrix[indexLHS][indexLHS]++;
			} else if (RHS.isSynonym()) {
				indexRHS = name_index_map[RHS.getName()];
				adjacencyMatrix[indexRHS][indexRHS]++;
			}
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
					if (adjacencyMatrix[synonymIndex][i] >= 1 && visited[i] == false) {
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
