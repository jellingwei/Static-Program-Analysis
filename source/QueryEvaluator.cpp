#include <vector>
#include <string>
#include <set>
using std::set;
using std::vector;
using std::string;
using std::stoi;  //String to integer
using std::pair;
using std::make_pair;

#include "QueryEvaluator.h"
#include "PKB.h"
#include "ValuesHandler.h"
#include "AbstractWrapper.h"

/**
@brief Namespace containing functions for the evaluating of queries.

*/
namespace QueryEvaluator 
{
	//Private functions to evaluate the query tree
	vector<Synonym> processResultNode(QNode* resultNode, bool isValid);
	inline bool processClausesNode(QNode* clausesNode);
	inline bool processClause(QNode* clauseNode);

	//Functions to process clauses
	bool processModifies(Synonym LHS, Synonym RHS, DIRECTION direction);
	pair<vector<int>, vector<int>> evaluateModifiesByLHS(Synonym LHS, Synonym RHS);
	pair<vector<int>, vector<int>> evaluateModifiesByRHS(Synonym LHS, Synonym RHS);

	bool processUses(Synonym LHS, Synonym RHS, DIRECTION direction);
	pair<vector<int>, vector<int>> evaluateUsesByLHS(Synonym LHS, Synonym RHS);
	pair<vector<int>, vector<int>> evaluateUsesByRHS(Synonym LHS, Synonym RHS);

	bool processParentT(Synonym LHS, Synonym RHS, bool isTrans, DIRECTION direction);
	pair<vector<int>, vector<int>> evaluateParentByLHS(Synonym LHS, Synonym RHS, bool isTrans);
	pair<vector<int>, vector<int>> evaluateParentByRHS(Synonym LHS, Synonym RHS, bool isTrans);

	bool processFollowsT(Synonym LHS, Synonym RHS, bool isTrans, DIRECTION direction);
	pair<vector<int>, vector<int>> evaluateFollowsByLHS(Synonym LHS, Synonym RHS, bool isTrans);
	pair<vector<int>, vector<int>> evaluateFollowsByRHS(Synonym LHS, Synonym RHS, bool isTrans);

	bool processCallsT(Synonym LHS, Synonym RHS, bool isTrans, DIRECTION direction);
	pair<vector<int>, vector<int>> evaluateCallsByLHS(Synonym LHS, Synonym RHS, bool isTrans);
	pair<vector<int>, vector<int>> evaluateCallsByRHS(Synonym LHS, Synonym RHS, bool isTrans);

	bool processNextT(Synonym LHS, Synonym RHS, bool isTrans, DIRECTION direction);
	pair<vector<int>, vector<int>> evaluateNextByLHS(Synonym LHS, Synonym RHS, bool isTrans);
	pair<vector<int>, vector<int>> evaluateNextByRHS(Synonym LHS, Synonym RHS, bool isTrans);

	bool processAffectsT(Synonym LHS, Synonym RHS, bool isTrans, DIRECTION direction);
	pair<vector<int>, vector<int>> evaluateAffectsByLHS(Synonym LHS, Synonym RHS, bool isTrans);
	pair<vector<int>, vector<int>> evaluateAffectsByRHS(Synonym LHS, Synonym RHS, bool isTrans);

	//Functions to process pattern clauses
	inline bool processPatternClause(QNode* patternClause);
	bool processAssignPattern(Synonym arg0, Synonym LHS, Synonym RHS);
	bool processIfPattern(Synonym arg0, Synonym LHS, Synonym RHS);
	bool processWhilePattern(Synonym arg0, Synonym LHS, Synonym RHS);

	//Functions to process with clauses
	bool processWithClause(QNode* withClause);

	PKB pkb = PKB::getInstance();  //Get the instance of the PKB singleton

	/**
	* Processes the query tree given a query tree node.
	* @param qTreeRoot the root node of the query tree
	* @return an empty vector if the Such That or Pattern clauses are invalid.
	*/
	vector<Synonym> processQueryTree(QueryTree* qTreeRoot) 
	{
		ValuesHandler::initialize(qTreeRoot->getSynonymsMap());
		vector<Synonym> synonymResult;

		bool isValid = processClausesNode(qTreeRoot->getClausesNode());

		synonymResult = processResultNode(qTreeRoot->getResultNode(), isValid);
		return synonymResult;
	}

	/**
	* Processes the result node in the query tree.
	* @param resultNode
	* @param isValid
	* @return the wanted synonym in a vector
	*/
	vector<Synonym> processResultNode(QNode* resultNode, bool isValid) 
	{
		vector<Synonym> result;

		QNode* resultChildNode = resultNode->getChild();
		int numberOfSynonyms = resultNode->getNumberOfChildren();

		if (numberOfSynonyms == 1) {
			Synonym wantedSynonym = resultChildNode->getArg1();

			if (wantedSynonym.getType() == BOOLEAN && isValid) {
				Synonym s(BOOLEAN, "TRUE");
				result.push_back(s);
				return result;
			} else if (wantedSynonym.getType() == BOOLEAN && !isValid) {
				Synonym s(BOOLEAN, "FALSE");
				result.push_back(s);
				return result;
			} else if (!isValid) {
				return result;
			}

			string wantedSynonymName = wantedSynonym.getName();
			Synonym s = ValuesHandler::getSynonym(wantedSynonymName);
			result.push_back(s);
		} else if (numberOfSynonyms > 1 && isValid) {
			vector<string> wantedNames;

			for (int i = 0; i < numberOfSynonyms; i++) {
				Synonym wantedSynonym = resultChildNode->getArg1();
				string wantedSynonymName = wantedSynonym.getName();
				wantedNames.push_back(wantedSynonymName);
				resultChildNode = resultNode->getNextChild();
			}

			result = ValuesHandler::getSynonymTuples(wantedNames);
		} 
		return result;
	}

	/**
	* Processes the such that node in the query tree.
	* @param clausesNode
	* @eturn TRUE if the clauses are valid. FALSE if the clauses are not valid.
	*/
	inline bool processClausesNode(QNode* clausesNode) 
	{
		int numberOfClauses = clausesNode->getNumberOfChildren();
		QNode* clauseNode = clausesNode->getChild();

		for (int i = 0; i < numberOfClauses; i++) {
			bool isValid = processClause(clauseNode);

			if (!isValid || AbstractWrapper::GlobalStop) {
				return false;
			} else {
				clauseNode = clausesNode->getNextChild();
			}
		}
		return true;
	}

	/**
	* Processes each such that clause from the such that node.
	* @param clausesNode
	* @return TRUE if the clause is valid. FALSE if the clause is not valid.
	*/
	inline bool processClause(QNode* clauseNode) 
	{
		QNODE_TYPE qnode_type = clauseNode->getNodeType();
		DIRECTION direction = clauseNode->getDirection();
		Synonym LHS = clauseNode->getArg1();
		Synonym RHS = clauseNode->getArg2();

		switch (qnode_type) {
		case ModifiesP:
		case ModifiesS:
			return processModifies(LHS, RHS, direction);
		case UsesP:
		case UsesS:
			return processUses(LHS, RHS, direction);
		case Parent:
			return processParentT(LHS, RHS, false, direction);
		case ParentT:
			return processParentT(LHS, RHS, true, direction);
		case Follows:
			return processFollowsT(LHS, RHS, false, direction);
		case FollowsT:
			return processFollowsT(LHS, RHS, true, direction);
		case Calls:
			return processCallsT(LHS, RHS, false, direction);
		case CallsT:
			return processCallsT(LHS, RHS, true, direction);
		case Next:
			return processNextT(LHS, RHS, false, direction);
		case NextT:
			return processNextT(LHS, RHS, true, direction);
		case Affects:
			return processAffectsT(LHS, RHS, false, direction);
		case AffectsT:
			return processAffectsT(LHS, RHS, true, direction);
		case Pattern:
			return processPatternClause(clauseNode);
		case With:
			return processWithClause(clauseNode);
		default:
			return false;
		}
	}

	/**
	* Processes the modifies clause.
	* @param LHS
	* @param RHS
	* @return TRUE if the clause is valid. FALSE if the clause is not valid.
	*/
	bool processModifies(Synonym LHS, Synonym RHS, DIRECTION direction) 
	{
		SYNONYM_TYPE typeLHS = LHS.getType();
		SYNONYM_TYPE typeRHS = RHS.getType();
		string nameLHS = LHS.getName();
		string nameRHS = RHS.getName();

		//The arguments are valid, begin processing the arguments
		if (typeLHS == STRING_INT && typeRHS == STRING_CHAR) {
			return pkb.isModifies(stoi(nameLHS), pkb.getVarIndex(nameRHS));
		} else if (typeLHS == STRING_CHAR && typeRHS == STRING_CHAR) {
			return pkb.isModifiesProc(pkb.getProcIndex(nameLHS), pkb.getVarIndex(nameRHS));
		} else if (typeLHS == STRING_INT) {
			//LHS is the line number, find the variables that are modified
			vector<int> stmts = pkb.getModVarForStmt(stoi(nameLHS));
			RHS.setValues(stmts);
			return ValuesHandler::addAndProcessIntermediateSynonym(RHS);
		} else if (typeLHS == STRING_CHAR) {
			//LHS is the proc name, find the variables that are modified
			vector<int> stmts = pkb.getModVarForProc(pkb.getProcIndex(nameLHS));
			RHS.setValues(stmts);
			return ValuesHandler::addAndProcessIntermediateSynonym(RHS);
		} else if (typeRHS == STRING_CHAR) {
			//RHS is the variable that is modified, find the statements
			vector<int> stmts;
			if (typeLHS == PROCEDURE) {
				stmts = pkb.getModProcIndex(pkb.getVarIndex(nameRHS));
			} else {
				stmts = pkb.getModStmtNum(pkb.getVarIndex(nameRHS));
			}

			LHS.setValues(stmts);
			return ValuesHandler::addAndProcessIntermediateSynonym(LHS);
		} else if (typeRHS == UNDEFINED) {
			if (typeLHS == PROCEDURE) {
				LHS.setValues(pkb.getAllProcIndex());
			} else {
				LHS.setValues(pkb.getModifiesLhs());
			}
			return ValuesHandler::addAndProcessIntermediateSynonym(LHS);
		} else {
			if (direction == LeftToRight) {
				pair<vector<int>, vector<int>> modifiesPair = evaluateModifiesByLHS(LHS, RHS);
				LHS.setValues(modifiesPair.first);
				RHS.setValues(modifiesPair.second);
			} else {
				pair<vector<int>, vector<int>> modifiesPair = evaluateModifiesByRHS(LHS, RHS);
				LHS.setValues(modifiesPair.first);
				RHS.setValues(modifiesPair.second);
			}
			return ValuesHandler::addAndProcessIntermediateSynonyms(LHS, RHS);
		}
	}

	/**
	* @param LHS
	* @param RHS
	* @return A pair of vectors denoting the pairs of values returned by the relation
	*/
	pair<vector<int>, vector<int>> evaluateModifiesByLHS(Synonym LHS, Synonym RHS)
	{
		vector<int> valuesLHS = ValuesHandler::getSynonym(LHS.getName()).getValues();
		vector<int> acceptedLHS;
		vector<int> acceptedRHS;
		vector<int> stmts;

		for (unsigned int i = 0; i < valuesLHS.size(); i++) {
			if (LHS.getType() == PROCEDURE) {
				stmts = pkb.getModVarForProc(valuesLHS[i]);
			} else {
				stmts = pkb.getModVarForStmt(valuesLHS[i]);
			}

			for (unsigned int j = 0; j < stmts.size(); j++) {
				acceptedLHS.push_back(valuesLHS[i]);
				acceptedRHS.push_back(stmts[j]);
			}
		}
		return make_pair(acceptedLHS, acceptedRHS);
	}

	/**
	* @param LHS
	* @param RHS
	* @return A pair of vectors denoting the pairs of values returned by the relation
	*/
	pair<vector<int>, vector<int>> evaluateModifiesByRHS(Synonym LHS, Synonym RHS)
	{
		vector<int> valuesRHS = ValuesHandler::getSynonym(RHS.getName()).getValues();
		vector<int> acceptedLHS;
		vector<int> acceptedRHS;

		for (unsigned int i = 0; i < valuesRHS.size(); i++) {
			vector<int> stmts;
			if (LHS.getType() == PROCEDURE) {
				stmts = pkb.getModProcIndex(valuesRHS[i]);
			} else {
				stmts = pkb.getModStmtNum(valuesRHS[i]);
			}

			for (unsigned int j = 0; j < stmts.size(); j++) {
				acceptedLHS.push_back(stmts[j]);
				acceptedRHS.push_back(valuesRHS[i]);
			}
		}
		return make_pair(acceptedLHS, acceptedRHS);
	}

	/**
	* Processes the uses clause.
	* @param LHS
	* @param RHS
	* @return TRUE if the clause is valid. FALSE if the clause is not valid.
	*/
	bool processUses(Synonym LHS, Synonym RHS, DIRECTION direction) 
	{
		SYNONYM_TYPE typeLHS = LHS.getType();
		SYNONYM_TYPE typeRHS = RHS.getType();
		string nameLHS = LHS.getName();
		string nameRHS = RHS.getName();

		//The arguments are valid, begin processing the arguments
		if (typeLHS == STRING_INT && typeRHS == STRING_CHAR) {
			return pkb.isUses(stoi(nameLHS), pkb.getVarIndex(nameRHS));
		} else if (typeLHS == STRING_CHAR && typeRHS == STRING_CHAR) { 
			return pkb.isUsesProc(pkb.getProcIndex(nameLHS), pkb.getVarIndex(nameRHS));
		} else if (typeLHS == STRING_INT) {
			//LHS is the line number, find the variable that is used
			vector<int> vars = pkb.getUsesVarForStmt(stoi(nameLHS));
			RHS.setValues(vars);
			return ValuesHandler::addAndProcessIntermediateSynonym(RHS);
		} else if (typeLHS == STRING_CHAR) {
			vector<int> vars = pkb.getUsesVarForProc(pkb.getProcIndex(nameLHS));
			RHS.setValues(vars);
			return ValuesHandler::addAndProcessIntermediateSynonym(RHS);
		} else if (typeRHS == STRING_CHAR) {
			//RHS is the variable that is used, find the statements that uses it
			vector<int> stmts;
			if (typeLHS == PROCEDURE) {
				stmts = pkb.getUsesProcIndex(pkb.getVarIndex(nameRHS));
			} else {
				stmts = pkb.getUsesStmtNum(pkb.getVarIndex(nameRHS));
			}
			LHS.setValues(stmts);
			return ValuesHandler::addAndProcessIntermediateSynonym(LHS);
		} else if (typeRHS == UNDEFINED) {
			if (typeLHS == PROCEDURE) {
				LHS.setValues(pkb.getAllProcIndex());
			} else {
				LHS.setValues(pkb.getUsesLhs());
			}
			return ValuesHandler::addAndProcessIntermediateSynonym(LHS);
		} else {
			if (direction == LeftToRight) {
				pair<vector<int>, vector<int>> usesPair = evaluateUsesByLHS(LHS, RHS);
				LHS.setValues(usesPair.first);
				RHS.setValues(usesPair.second);
			} else {
				pair<vector<int>, vector<int>> usesPair = evaluateUsesByRHS(LHS, RHS);
				LHS.setValues(usesPair.first);
				RHS.setValues(usesPair.second);
			}
			return ValuesHandler::addAndProcessIntermediateSynonyms(LHS, RHS);
		}
	}

	/**
	* @param LHS
	* @param RHS
	* @return A pair of vectors denoting the pairs of values returned by the relation
	*/
	pair<vector<int>, vector<int>> evaluateUsesByLHS(Synonym LHS, Synonym RHS)
	{
		vector<int> valuesLHS = ValuesHandler::getSynonym(LHS.getName()).getValues();
		vector<int> acceptedLHS;
		vector<int> acceptedRHS;

		for (unsigned int i = 0; i < valuesLHS.size(); i++) {
			vector<int> stmts;
			if (LHS.getType() == PROCEDURE) {
				stmts = pkb.getUsesVarForProc(valuesLHS[i]);
			} else {
				stmts = pkb.getUsesVarForStmt(valuesLHS[i]);
			}

			for (unsigned int j = 0; j < stmts.size(); j++) {
				acceptedLHS.push_back(valuesLHS[i]);
				acceptedRHS.push_back(stmts[j]);
			}
		}
		return make_pair(acceptedLHS, acceptedRHS);
	}

	/**
	* @param LHS
	* @param RHS
	* @return A pair of vectors denoting the pairs of values returned by the relation
	*/
	pair<vector<int>, vector<int>> evaluateUsesByRHS(Synonym LHS, Synonym RHS)
	{
		vector<int> valuesRHS = ValuesHandler::getSynonym(RHS.getName()).getValues();
		vector<int> acceptedLHS;
		vector<int> acceptedRHS;

		for (unsigned int i = 0; i < valuesRHS.size(); i++) {
			vector<int> stmts;
			if (LHS.getType() == PROCEDURE) {
				stmts = pkb.getUsesProcIndex(valuesRHS[i]);
			} else {
				stmts = pkb.getUsesStmtNum(valuesRHS[i]);
			}

			for (unsigned int j = 0; j < stmts.size(); j++) {
				acceptedLHS.push_back(stmts[j]);
				acceptedRHS.push_back(valuesRHS[i]);
			}
		}
		return make_pair(acceptedLHS, acceptedRHS);
	}

	/**
	* Processes the parent clause.
	* @param LHS
	* @param RHS
	* @param isTrans a flag to indicate the computation of Parent or Parent* relation
	* @return TRUE if the clause is valid. FALSE if the clause is not valid.
	*/
	bool processParentT(Synonym LHS, Synonym RHS, bool isTrans, DIRECTION direction) 
	{
		SYNONYM_TYPE typeLHS = LHS.getType();
		SYNONYM_TYPE typeRHS = RHS.getType();
		string nameLHS = LHS.getName();
		string nameRHS = RHS.getName();

		if (typeLHS == STRING_INT && typeRHS == STRING_INT) {
			return pkb.isParent(stoi(nameLHS), stoi(nameRHS), isTrans);
		} else if (typeLHS == STRING_INT) {
			vector<int> stmts = pkb.getChild(stoi(nameLHS), isTrans);
			RHS.setValues(stmts);
			return ValuesHandler::addAndProcessIntermediateSynonym(RHS);
		} else if (typeRHS == STRING_INT) {
			vector<int> stmts = pkb.getParent(stoi(nameRHS), isTrans);
			LHS.setValues(stmts);
			return ValuesHandler::addAndProcessIntermediateSynonym(LHS);
		} else if (typeLHS == UNDEFINED && typeRHS == UNDEFINED) {
			//TODO: Shift the optimiser since this clause might be redundant
			LHS.setValues(pkb.getParentLhs());
			return ValuesHandler::addAndProcessIntermediateSynonym(LHS);
		} else if (typeLHS == UNDEFINED) {
			RHS.setValues(pkb.getParentRhs());
			return ValuesHandler::addAndProcessIntermediateSynonym(RHS);
		} else if (typeRHS == UNDEFINED) {
			LHS.setValues(pkb.getParentLhs());
			return ValuesHandler::addAndProcessIntermediateSynonym(LHS);
		} else {
			if (direction == LeftToRight) {
				pair<vector<int>, vector<int>> parentsPair = evaluateParentByLHS(LHS, RHS, isTrans);
				LHS.setValues(parentsPair.first);
				RHS.setValues(parentsPair.second);
			} else {
				pair<vector<int>, vector<int>> parentsPair = evaluateParentByRHS(LHS, RHS, isTrans);
				LHS.setValues(parentsPair.first);
				RHS.setValues(parentsPair.second);
			}
			return ValuesHandler::addAndProcessIntermediateSynonyms(LHS, RHS);
		}
	}

	/**
	* @param LHS
	* @param RHS
	* @param isTrans a flag to indicate the computation of Parent or Parent* relation
	* @return A pair of vectors denoting the pairs of values returned by the relation
	*/
	pair<vector<int>, vector<int>> evaluateParentByLHS(Synonym LHS, Synonym RHS, bool isTrans)
	{
		vector<int> valuesLHS = ValuesHandler::getSynonym(LHS.getName()).getValues();
		vector<int> acceptedLHS;
		vector<int> acceptedRHS;

		for (unsigned int i = 0; i < valuesLHS.size(); i++) {
			vector<int> stmts = pkb.getChild(valuesLHS[i], isTrans);

			for (unsigned int j = 0; j < stmts.size(); j++) {
				acceptedLHS.push_back(valuesLHS[i]);
				acceptedRHS.push_back(stmts[j]);
			}
		}
		return make_pair(acceptedLHS, acceptedRHS);
	}

	/**
	* @param LHS
	* @param RHS
	* @param isTrans a flag to indicate the computation of Parent or Parent* relation
	* @return A pair of vectors denoting the pairs of values returned by the relation
	*/
	pair<vector<int>, vector<int>> evaluateParentByRHS(Synonym LHS, Synonym RHS, bool isTrans)
	{
		vector<int> valuesRHS = ValuesHandler::getSynonym(RHS.getName()).getValues();
		vector<int> acceptedLHS;
		vector<int> acceptedRHS;

		for (unsigned int i = 0; i < valuesRHS.size(); i++) {
			vector<int> stmts = pkb.getParent(valuesRHS[i], isTrans);

			for (unsigned int j = 0; j < stmts.size(); j++) {
				acceptedLHS.push_back(stmts[j]);
				acceptedRHS.push_back(valuesRHS[i]);
			}
		}
		return make_pair(acceptedLHS, acceptedRHS);
	}

	/**
	* Processes the follows clause.
	* @param LHS
	* @param RHS
	* @param isTrans a flag to indicate the computation of Follows or Follows* relation
	* @return TRUE if the clause is valid. FALSE if the clause is not valid.
	*/
	bool processFollowsT(Synonym LHS, Synonym RHS, bool isTrans, DIRECTION direction) 
	{
		SYNONYM_TYPE typeLHS = LHS.getType();
		SYNONYM_TYPE typeRHS = RHS.getType();
		string nameLHS = LHS.getName();
		string nameRHS = RHS.getName();

		if (typeLHS == STRING_INT && typeRHS == STRING_INT) {
			return pkb.isFollows(stoi(nameLHS), stoi(nameRHS), isTrans);
		} else if (typeLHS == STRING_INT) {
			// Given stmtNum1, get stmtNum2 such that Follows(stmt1, stmt2) is satisfied
			vector<int> stmt = pkb.getStmtFollowedFrom(stoi(nameLHS), isTrans);
			RHS.setValues(stmt);
			return ValuesHandler::addAndProcessIntermediateSynonym(RHS);
		} else if (typeRHS == STRING_INT) {
			// Given stmtNum2, get stmtNum1 such that Follows(stmt1, stmt2) is satisfied
			vector<int> stmt = pkb.getStmtFollowedTo(stoi(nameRHS), isTrans);
			LHS.setValues(stmt);
			return ValuesHandler::addAndProcessIntermediateSynonym(LHS);
		} else if (typeLHS == UNDEFINED && typeRHS == UNDEFINED) {
			//TODO: Shift to optimiser since this might be a redundant clause
			LHS.setValues(pkb.getFollowsLhs());
			return ValuesHandler::addAndProcessIntermediateSynonym(LHS);
		} else if (typeLHS == UNDEFINED) {
			RHS.setValues(pkb.getFollowsRhs());
			return ValuesHandler::addAndProcessIntermediateSynonym(RHS);
		} else if (typeRHS == UNDEFINED) {
			LHS.setValues(pkb.getFollowsLhs());
			return ValuesHandler::addAndProcessIntermediateSynonym(LHS);
		} else {
			if (direction == LeftToRight) {
				pair<vector<int>, vector<int>> followsPair = evaluateFollowsByLHS(LHS, RHS, isTrans);
				LHS.setValues(followsPair.first);
				RHS.setValues(followsPair.second);
			} else {
				pair<vector<int>, vector<int>> followsPair = evaluateFollowsByRHS(LHS, RHS, isTrans);
				LHS.setValues(followsPair.first);
				RHS.setValues(followsPair.second);
			}
			return ValuesHandler::addAndProcessIntermediateSynonyms(LHS, RHS);
		}
	}

	/**
	* @param LHS
	* @param RHS
	* @param isTrans a flag to indicate the computation of Follows or Follows* relation
	* @return A pair of vectors denoting the pairs of values returned by the relation
	*/
	pair<vector<int>, vector<int>> evaluateFollowsByLHS(Synonym LHS, Synonym RHS, bool isTrans)
	{
		vector<int> valuesLHS = ValuesHandler::getSynonym(LHS.getName()).getValues();
		vector<int> acceptedLHS;
		vector<int> acceptedRHS;

		for (unsigned int i = 0; i < valuesLHS.size(); i++) {
			vector<int> stmts = pkb.getStmtFollowedFrom(valuesLHS[i], isTrans);

			for (unsigned int j = 0; j < stmts.size(); j++) {
				acceptedLHS.push_back(valuesLHS[i]);
				acceptedRHS.push_back(stmts[j]);
			}
		}
		return make_pair(acceptedLHS, acceptedRHS);
	}

	/**
	* @param LHS
	* @param RHS
	* @param isTrans a flag to indicate the computation of Follows or Follows* relation
	* @return A pair of vectors denoting the pairs of values returned by the relation
	*/
	pair<vector<int>, vector<int>> evaluateFollowsByRHS(Synonym LHS, Synonym RHS, bool isTrans)
	{
		vector<int> valuesRHS = ValuesHandler::getSynonym(RHS.getName()).getValues();
		vector<int> acceptedLHS;
		vector<int> acceptedRHS;

		for (unsigned int i = 0; i < valuesRHS.size(); i++) {
			vector<int> stmts = pkb.getStmtFollowedTo(valuesRHS[i], isTrans);

			for (unsigned int j = 0; j < stmts.size(); j++) {
				acceptedLHS.push_back(stmts[j]);
				acceptedRHS.push_back(valuesRHS[i]);
			}
		}
		return make_pair(acceptedLHS, acceptedRHS);
	}

	/**
	* Processes the Calls clause.
	* @param LHS
	* @param RHS
	* @param isTrans a flag to indicate the computation of Calls or Calls* relation
	* @return TRUE if the clause is valid. FALSE if the clause is not valid.
	*/
	bool processCallsT(Synonym LHS, Synonym RHS, bool isTrans, DIRECTION direction)
	{
		SYNONYM_TYPE typeLHS = LHS.getType();
		SYNONYM_TYPE typeRHS = RHS.getType();
		string nameLHS = LHS.getName();
		string nameRHS = RHS.getName();

		if (typeLHS == STRING_CHAR && typeRHS == STRING_CHAR) {
			return pkb.isCalls(pkb.getProcIndex(nameLHS), pkb.getProcIndex(nameRHS), isTrans);
		} else if (typeLHS == STRING_CHAR) {
			vector<int> stmt = pkb.getProcsCalledBy(pkb.getProcIndex(nameLHS), isTrans);
			RHS.setValues(stmt);
			return ValuesHandler::addAndProcessIntermediateSynonym(RHS);
		} else if (typeRHS == STRING_CHAR) {
			vector<int> stmt = pkb.getProcsCalling(pkb.getProcIndex(nameRHS), isTrans);
			LHS.setValues(stmt);
			return ValuesHandler::addAndProcessIntermediateSynonym(LHS);
		} else if (typeLHS == UNDEFINED && typeRHS == UNDEFINED) {
			//TODO: This clause might be redundant
			LHS.setValues(pkb.getCallsLhs());
			return ValuesHandler::addAndProcessIntermediateSynonym(LHS);
		} else if (typeLHS == UNDEFINED) {
			RHS.setValues(pkb.getCallsRhs());
			return ValuesHandler::addAndProcessIntermediateSynonym(RHS);
		} else if (typeRHS == UNDEFINED) {
			LHS.setValues(pkb.getCallsLhs());
			return ValuesHandler::addAndProcessIntermediateSynonym(LHS);
		} else {
			if (direction == LeftToRight) {
				pair<vector<int>, vector<int>> callsPair = evaluateCallsByLHS(LHS, RHS, isTrans);
				LHS.setValues(callsPair.first);
				RHS.setValues(callsPair.second);
			} else {
				pair<vector<int>, vector<int>> callsPair = evaluateCallsByRHS(LHS, RHS, isTrans);
				LHS.setValues(callsPair.first);
				RHS.setValues(callsPair.second);
			}
			return ValuesHandler::addAndProcessIntermediateSynonyms(LHS, RHS);
		}
	}

	/**
	* @param LHS
	* @param RHS
	* @param isTrans a flag to indicate the computation of Calls or Calls* relation
	* @return A pair of vectors denoting the pairs of values returned by the relation
	*/
	pair<vector<int>, vector<int>> evaluateCallsByLHS(Synonym LHS, Synonym RHS, bool isTrans)
	{
		vector<int> valuesLHS = ValuesHandler::getSynonym(LHS.getName()).getValues();
		vector<int> acceptedLHS;
		vector<int> acceptedRHS;

		for (unsigned int i = 0; i < valuesLHS.size(); i++) {
			vector<int> stmts = pkb.getProcsCalledBy(valuesLHS[i], isTrans);

			for (unsigned int j = 0; j < stmts.size(); j++) {
				acceptedLHS.push_back(valuesLHS[i]);
				acceptedRHS.push_back(stmts[j]);
			}
		}
		return make_pair(acceptedLHS, acceptedRHS);
	}

	/**
	* @param LHS
	* @param RHS
	* @param isTrans a flag to indicate the computation of Calls or Calls* relation
	* @return A pair of vectors denoting the pairs of values returned by the relation
	*/
	pair<vector<int>, vector<int>> evaluateCallsByRHS(Synonym LHS, Synonym RHS, bool isTrans)
	{
		vector<int> valuesRHS = ValuesHandler::getSynonym(RHS.getName()).getValues();
		vector<int> acceptedLHS;
		vector<int> acceptedRHS;

		for (unsigned int i = 0; i < valuesRHS.size(); i++) {
			vector<int> stmts = pkb.getProcsCalling(valuesRHS[i], isTrans);

			for (unsigned int j = 0; j < stmts.size(); j++) {
				acceptedLHS.push_back(stmts[j]);
				acceptedRHS.push_back(valuesRHS[i]);
			}
		}
		return make_pair(acceptedLHS, acceptedRHS);
	}

	/**
	* Processes the Next clause.
	* @param LHS
	* @param RHS
	* @param isTrans a flag to indicate the computation of Next or Next* relation
	* @return TRUE if the clause is valid. FALSE if the clause is not valid.
	*/
	bool processNextT(Synonym LHS, Synonym RHS, bool isTrans, DIRECTION direction)
	{
		SYNONYM_TYPE typeLHS = LHS.getType();
		SYNONYM_TYPE typeRHS = RHS.getType();
		string nameLHS = LHS.getName();
		string nameRHS = RHS.getName();

		if (typeLHS == STRING_INT && typeRHS == STRING_INT) {
			return pkb.isNext(stoi(nameLHS), stoi(nameRHS), isTrans);
		} else if (typeLHS == STRING_INT) {
			vector<int> stmt = pkb.getNextAfter(stoi(nameLHS), isTrans);
			RHS.setValues(stmt);
			return ValuesHandler::addAndProcessIntermediateSynonym(RHS);
		} else if (typeRHS == STRING_INT) {
			vector<int> stmt = pkb.getNextBefore(stoi(nameRHS), isTrans);
			LHS.setValues(stmt);
			return ValuesHandler::addAndProcessIntermediateSynonym(LHS);
		} else if (typeLHS == UNDEFINED && typeRHS == UNDEFINED) {
			//TODO: This clause might be redundant
			LHS.setValues(pkb.getNextLhs());
			return ValuesHandler::addAndProcessIntermediateSynonym(LHS);
		} else if (typeLHS == UNDEFINED) {
			RHS.setValues(pkb.getNextRhs());
			return ValuesHandler::addAndProcessIntermediateSynonym(RHS);
		} else if (typeRHS == UNDEFINED) {
			LHS.setValues(pkb.getNextLhs());
			return ValuesHandler::addAndProcessIntermediateSynonym(LHS);
		} else {
			if (direction == LeftToRight) {
				pair<vector<int>, vector<int>> callsPair = evaluateNextByLHS(LHS, RHS, isTrans);
				LHS.setValues(callsPair.first);
				RHS.setValues(callsPair.second);
			} else {
				pair<vector<int>, vector<int>> callsPair = evaluateNextByRHS(LHS, RHS, isTrans);
				LHS.setValues(callsPair.first);
				RHS.setValues(callsPair.second);
			}
			return ValuesHandler::addAndProcessIntermediateSynonyms(LHS, RHS);
		}
	}

	/**
	* @param LHS
	* @param RHS
	* @param isTrans a flag to indicate the computation of Next or Next* relation
	* @return A pair of vectors denoting the pairs of values returned by the relation
	*/
	pair<vector<int>, vector<int>> evaluateNextByLHS(Synonym LHS, Synonym RHS, bool isTrans)
	{
		vector<int> valuesLHS = ValuesHandler::getSynonym(LHS.getName()).getValues();
		vector<int> acceptedLHS;
		vector<int> acceptedRHS;

		for (unsigned int i = 0; i < valuesLHS.size(); i++) {
			vector<int> stmts = pkb.getNextAfter(valuesLHS[i], isTrans);

			for (unsigned int j = 0; j < stmts.size(); j++) {
				acceptedLHS.push_back(valuesLHS[i]);
				acceptedRHS.push_back(stmts[j]);
			}
		}
		return make_pair(acceptedLHS, acceptedRHS);
	}

	/**
	* @param LHS
	* @param RHS
	* @param isTrans a flag to indicate the computation of Next or Next* relation
	* @return A pair of vectors denoting the pairs of values returned by the relation
	*/
	pair<vector<int>, vector<int>> evaluateNextByRHS(Synonym LHS, Synonym RHS, bool isTrans)
	{
		vector<int> valuesRHS = ValuesHandler::getSynonym(RHS.getName()).getValues();
		vector<int> acceptedLHS;
		vector<int> acceptedRHS;

		for (unsigned int i = 0; i < valuesRHS.size(); i++) {
			vector<int> stmts = pkb.getNextBefore(valuesRHS[i], isTrans);

			for (unsigned int j = 0; j < stmts.size(); j++) {
				acceptedLHS.push_back(stmts[j]);
				acceptedRHS.push_back(valuesRHS[i]);
			}
		}
		return make_pair(acceptedLHS, acceptedRHS);
	}

	/**
	* Processes the Affects clause.
	* @param LHS
	* @param RHS
	* @param isTrans a flag to indicate the computation of Affects or Affects* relation
	* @return TRUE if the clause is valid. FALSE if the clause is not valid.
	*/
	bool processAffectsT(Synonym LHS, Synonym RHS, bool isTrans, DIRECTION direction)
	{
		SYNONYM_TYPE typeLHS = LHS.getType();
		SYNONYM_TYPE typeRHS = RHS.getType();

		if (typeLHS == STRING_INT && typeRHS == STRING_INT) {
			return pkb.isAffects(stoi(LHS.getName()), stoi(RHS.getName()), isTrans);
		} else if (typeLHS == STRING_INT) {
			vector<int> stmt = pkb.getAffectedBy(stoi(LHS.getName()), isTrans);
			RHS.setValues(stmt);
			return ValuesHandler::addAndProcessIntermediateSynonym(RHS);
		} else if (typeRHS == STRING_INT) {
			vector<int> stmt = pkb.getAffecting(stoi(RHS.getName()), isTrans);
			LHS.setValues(stmt);
			return ValuesHandler::addAndProcessIntermediateSynonym(LHS);
		} else if (typeLHS == UNDEFINED && typeRHS == UNDEFINED) {
			LHS.setValues(pkb.getAffectsLhs());
			return ValuesHandler::addAndProcessIntermediateSynonym(LHS);
		} else if (typeLHS == UNDEFINED) {
			RHS.setValues(pkb.getAffectsRhs());
			return ValuesHandler::addAndProcessIntermediateSynonym(RHS);
		} else if (typeRHS == UNDEFINED) {
			LHS.setValues(pkb.getAffectsLhs());
			return ValuesHandler::addAndProcessIntermediateSynonym(LHS);
		} else {
			if (direction == LeftToRight) {
				pair<vector<int>, vector<int>> affectsPair = evaluateAffectsByLHS(LHS, RHS, isTrans);
				LHS.setValues(affectsPair.first);
				RHS.setValues(affectsPair.second);
			} else {
				pair<vector<int>, vector<int>> affectsPair = evaluateAffectsByRHS(LHS, RHS, isTrans);
				LHS.setValues(affectsPair.first);
				RHS.setValues(affectsPair.second);
			}
			return ValuesHandler::addAndProcessIntermediateSynonyms(LHS, RHS);
		}
	}

	/**
	* @param LHS
	* @param RHS
	* @param isTrans a flag to indicate the computation of Affects or Affects* relation
	* @return A pair of vectors denoting the pairs of values returned by the relation
	*/
	pair<vector<int>, vector<int>> evaluateAffectsByLHS(Synonym LHS, Synonym RHS, bool isTrans)
	{
		vector<int> valuesLHS = ValuesHandler::getSynonym(LHS.getName()).getValues();
		vector<int> acceptedLHS;
		vector<int> acceptedRHS;

		for (unsigned int i = 0; i < valuesLHS.size(); i++) {
			vector<int> stmts = pkb.getAffectedBy(valuesLHS[i], isTrans);

			for (unsigned int j = 0; j < stmts.size(); j++) {
				acceptedLHS.push_back(valuesLHS[i]);
				acceptedRHS.push_back(stmts[j]);
			}
		}
		return make_pair(acceptedLHS, acceptedRHS);
	}

	/**
	* @param LHS
	* @param RHS
	* @param isTrans a flag to indicate the computation of Affects or Affects* relation
	* @return A pair of vectors denoting the pairs of values returned by the relation
	*/
	pair<vector<int>, vector<int>> evaluateAffectsByRHS(Synonym LHS, Synonym RHS, bool isTrans)
	{
		vector<int> valuesRHS = ValuesHandler::getSynonym(RHS.getName()).getValues();
		vector<int> acceptedLHS;
		vector<int> acceptedRHS;

		for (unsigned int i = 0; i < valuesRHS.size(); i++) {
			vector<int> stmts = pkb.getAffecting(valuesRHS[i], isTrans);

			for (unsigned int j = 0; j < stmts.size(); j++) {
				acceptedLHS.push_back(stmts[j]);
				acceptedRHS.push_back(valuesRHS[i]);
			}
		}
		return make_pair(acceptedLHS, acceptedRHS);
	}

	/**
	* Processes individual pattern clauses.
	* @param patternClause
	* @return TRUE if the clause is valid. FALSE if the clause is not valid.
	*/
	inline bool processPatternClause(QNode* patternClause) 
	{
		Synonym arg0 = patternClause->getArg0();
		Synonym LHS = patternClause->getArg1();
		Synonym RHS = patternClause->getArg2();
		SYNONYM_TYPE patternType = arg0.getType();

		switch (patternType) {
		case ASSIGN:
			return processAssignPattern(arg0, LHS, RHS);
		case IF:
			return processIfPattern(arg0, LHS, RHS);
		case WHILE:
			return processWhilePattern(arg0, LHS, RHS);
		default:
			return false;
		}
	}

	/**
	* Processes assign patterns.
	* @param arg0
	* @param LHS
	* @param RHS
	* @return TRUE if the clause is valid. FALSE if the clause is not valid.
	*/
	bool processAssignPattern(Synonym arg0, Synonym LHS, Synonym RHS) 
	{
		vector<int> isMatchStmts = pkb.patternMatchAssign(RHS.getName());

		if (isMatchStmts.size() == 0) {
			return false;
		}

		SYNONYM_TYPE typeLHS = LHS.getType();
		if (typeLHS == VARIABLE) {
			//If LHS is a variable synonym, use the return statements to probe the ModifiesTable
			vector<int> vars;

			for (vector<int>::iterator itr = isMatchStmts.begin(); itr != isMatchStmts.end(); ++itr) {
				vector<int> var = pkb.getModVarForStmt(*itr);  //Get the LHS of the assign statement
				vars.push_back(var[0]);  //Variable for assignment stmts must be in index 0
			}

			arg0.setValues(isMatchStmts);
			LHS.setValues(vars);
			ValuesHandler::addAndProcessIntermediateSynonyms(arg0, LHS);
			return true;
		} else if (typeLHS == UNDEFINED) {
			arg0.setValues(isMatchStmts);
			ValuesHandler::addAndProcessIntermediateSynonym(arg0);
			return true;
		} else {
			//LHS is a constant
			string nameLHS = LHS.getName();
			vector<int> matchingStmts;

			//Check that the assignment statements modifies the LHS
			for (vector<int>::iterator itr = isMatchStmts.begin(); itr != isMatchStmts.end(); ++itr) {
				int stmt = *itr;
				vector<int> vars = pkb.getModVarForStmt(stmt);
				string var = pkb.getVarName(vars[0]);  //Variable for assignment stmts must be in index 0
				if (var == nameLHS) {
					matchingStmts.push_back(stmt);
				}
			}
			arg0.setValues(matchingStmts);
			return ValuesHandler::addAndProcessIntermediateSynonym(arg0);
		}
	}

	/**
	* Processes if patterns.
	* @param arg0
	* @param LHS
	* @param RHS
	* @return TRUE if the clause is valid. FALSE if the clause is not valid.
	*/
	bool processIfPattern(Synonym arg0, Synonym LHS, Synonym RHS) 
	{
		SYNONYM_TYPE typeLHS = LHS.getType();

		//Find all if statements that uses LHS
		if (LHS.getType() == STRING_CHAR) {
			vector<int> stmts = pkb.patternMatchIf(LHS.getName());
			arg0.setValues(stmts);
			return ValuesHandler::addAndProcessIntermediateSynonym(arg0);
		} else if (typeLHS == UNDEFINED) {
			vector<int> ifStmts = ValuesHandler::getSynonym(arg0.getName()).getValues();
			return (ifStmts.size() != 0);  //Do nothing because pattern i(_, _, _) is always true if there are if statements
		} else {
			//LHS is a variable synonym
			vector<int> arg0Values = ValuesHandler::getSynonym(arg0.getName()).getValues();
			if (arg0Values.size() == 0) {
				return false;  //This check is necessary to prevent pairs with only one side filled to enter the values handler
			}

			vector<int> vars;
			for (unsigned int i = 0; i < arg0Values.size(); i++) {
				int var = pkb.getControlVariable(arg0Values[i]);
				vars.push_back(var);
			}
			arg0.setValues(arg0Values);
			LHS.setValues(vars);
			return ValuesHandler::addAndProcessIntermediateSynonyms(arg0, LHS);
		}
	}

	/**
	* Processes while patterns.
	* @param arg0
	* @param LHS
	* @param RHS
	* @return TRUE if the clause is valid. FALSE if the clause is not valid.
	*/
	bool processWhilePattern(Synonym arg0, Synonym LHS, Synonym RHS) 
	{
		SYNONYM_TYPE typeLHS = LHS.getType();

		//Find all while statements that uses LHS
		if (LHS.getType() == STRING_CHAR) {
			vector<int> stmts = pkb.patternMatchWhile(LHS.getName());
			arg0.setValues(stmts);
			return ValuesHandler::addAndProcessIntermediateSynonym(arg0);
		} else if (typeLHS == UNDEFINED) {
			vector<int> whileStmts = ValuesHandler::getSynonym(arg0.getName()).getValues();
			return (whileStmts.size() != 0);  //Do nothing because pattern w(_, _) is always true if there are while statements
		} else {
			//LHS is a variable synonym
			vector<int> arg0Values = ValuesHandler::getSynonym(arg0.getName()).getValues();
			if (arg0Values.size() == 0) {
				return false;  //This check is necessary to prevent pairs with only one side filled to enter the values handler
			}

			vector<int> vars;
			for (unsigned int i = 0; i < arg0Values.size(); i++) {
				int var = pkb.getControlVariable(arg0Values[i]);
				vars.push_back(var);
			}
			arg0.setValues(arg0Values);
			LHS.setValues(vars);
			return ValuesHandler::addAndProcessIntermediateSynonyms(arg0, LHS);
		}
	}

	/**
	* Processes with clause.
	* @param withClause
	* @return TRUE if the clause is valid. FALSE if the clause is not valid.
	*/
	bool processWithClause(QNode* withClause)
	{
		//Assume that var and int cannot be compared (should be checked by the query validator)
		Synonym LHS = withClause->getArg1();
		Synonym RHS = withClause->getArg2();
		SYNONYM_TYPE typeLHS = LHS.getType();
		SYNONYM_TYPE typeRHS = RHS.getType();

		if (typeLHS == STRING_CHAR && typeRHS == STRING_INT) {
			return false;
		} else if (typeLHS == STRING_INT && typeRHS == STRING_CHAR) {
			return false;
		} else if (typeLHS == STRING_CHAR && typeRHS == STRING_CHAR) {
			string arg1Value = LHS.getName();
			string arg2Value = RHS.getName();
			return arg1Value == arg2Value;
		} else if (typeLHS == STRING_INT && typeRHS == STRING_INT) {
			string arg1Value = LHS.getName();
			string arg2Value = RHS.getName();
			return arg1Value == arg2Value;
		} else if (typeLHS == STRING_CHAR || typeLHS == STRING_INT) {
			string arg1Value = LHS.getName();
			return ValuesHandler::filterEqualValue(RHS, arg1Value);
		} else if (typeRHS == STRING_CHAR || typeRHS == STRING_INT) {
			string arg2Value = RHS.getName();
			return ValuesHandler::filterEqualValue(LHS, arg2Value);
		} else {
			return ValuesHandler::filterEqualPair(LHS, RHS);
		}
	}
}
