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
#include "common.h"
#include "common_list.h"

/**
@brief Namespace containing functions for the evaluating of queries.

*/
namespace QueryEvaluator 
{
	//Private functions to evaluate the query tree
	SYNONYM_LIST processResultNode(QNode* resultNode, BOOLEAN_ isValid);
	inline BOOLEAN_ processClause(QNode* clauseNode);

	//Functions to process clauses
	BOOLEAN_ processModifies(Synonym LHS, Synonym RHS, DIRECTION direction);
	pair<vector<int>, vector<int>> evaluateModifiesByPair(Synonym LHS, Synonym RHS);
	pair<vector<int>, vector<int>> evaluateModifiesByLHS(Synonym LHS, Synonym RHS);
	pair<vector<int>, vector<int>> evaluateModifiesByRHS(Synonym LHS, Synonym RHS);

	BOOLEAN_ processUses(Synonym LHS, Synonym RHS, DIRECTION direction);
	pair<vector<int>, vector<int>> evaluateUsesByPair(Synonym LHS, Synonym RHS);
	pair<vector<int>, vector<int>> evaluateUsesByLHS(Synonym LHS, Synonym RHS);
	pair<vector<int>, vector<int>> evaluateUsesByRHS(Synonym LHS, Synonym RHS);

	BOOLEAN_ processParentT(Synonym LHS, Synonym RHS, TRANS_CLOSURE isTrans, DIRECTION direction);
	pair<vector<int>, vector<int>> evaluateParentByPair(Synonym LHS, Synonym RHS, TRANS_CLOSURE isTrans);
	pair<vector<int>, vector<int>> evaluateParentByLHS(Synonym LHS, Synonym RHS, TRANS_CLOSURE isTrans);
	pair<vector<int>, vector<int>> evaluateParentByRHS(Synonym LHS, Synonym RHS, TRANS_CLOSURE isTrans);

	BOOLEAN_ processFollowsT(Synonym LHS, Synonym RHS, TRANS_CLOSURE isTrans, DIRECTION direction);
	pair<vector<int>, vector<int>> evaluateFollowsByPair(Synonym LHS, Synonym RHS, TRANS_CLOSURE isTrans);
	pair<vector<int>, vector<int>> evaluateFollowsByLHS(Synonym LHS, Synonym RHS, TRANS_CLOSURE isTrans);
	pair<vector<int>, vector<int>> evaluateFollowsByRHS(Synonym LHS, Synonym RHS, TRANS_CLOSURE isTrans);

	BOOLEAN_ processCallsT(Synonym LHS, Synonym RHS, TRANS_CLOSURE isTrans, DIRECTION direction);
	pair<vector<int>, vector<int>> evaluateCallsByPair(Synonym LHS, Synonym RHS, TRANS_CLOSURE isTrans);
	pair<vector<int>, vector<int>> evaluateCallsByLHS(Synonym LHS, Synonym RHS, TRANS_CLOSURE isTrans);
	pair<vector<int>, vector<int>> evaluateCallsByRHS(Synonym LHS, Synonym RHS, TRANS_CLOSURE isTrans);

	BOOLEAN_ processNextT(Synonym LHS, Synonym RHS, TRANS_CLOSURE isTrans, DIRECTION direction);
	pair<vector<int>, vector<int>> evaluateNextByPair(Synonym LHS, Synonym RHS, TRANS_CLOSURE isTrans);
	pair<vector<int>, vector<int>> evaluateNextByLHS(Synonym LHS, Synonym RHS, TRANS_CLOSURE isTrans);
	pair<vector<int>, vector<int>> evaluateNextByRHS(Synonym LHS, Synonym RHS, TRANS_CLOSURE isTrans);

	BOOLEAN_ processAffectsT(Synonym LHS, Synonym RHS, TRANS_CLOSURE isTrans, DIRECTION direction);
	pair<vector<int>, vector<int>> evaluateAffectsByPair(Synonym LHS, Synonym RHS, TRANS_CLOSURE isTrans);
	pair<vector<int>, vector<int>> evaluateAffectsByLHS(Synonym LHS, Synonym RHS, TRANS_CLOSURE isTrans);
	pair<vector<int>, vector<int>> evaluateAffectsByRHS(Synonym LHS, Synonym RHS, TRANS_CLOSURE isTrans);
	
	BOOLEAN_ processNextBipT(Synonym LHS, Synonym RHS, TRANS_CLOSURE isTrans, DIRECTION direction);
	pair<vector<int>, vector<int>> evaluateNextBipByPair(Synonym LHS, Synonym RHS, TRANS_CLOSURE isTrans);
	pair<vector<int>, vector<int>> evaluateNextBipByLHS(Synonym LHS, Synonym RHS, TRANS_CLOSURE isTrans);
	pair<vector<int>, vector<int>> evaluateNextBipByRHS(Synonym LHS, Synonym RHS, TRANS_CLOSURE isTrans);
	
	BOOLEAN_ processAffectsBipT(Synonym LHS, Synonym RHS, TRANS_CLOSURE isTrans, DIRECTION direction);
	pair<vector<int>, vector<int>> evaluateAffectsBipByPair(Synonym LHS, Synonym RHS, TRANS_CLOSURE isTrans);
	pair<vector<int>, vector<int>> evaluateAffectsBipByLHS(Synonym LHS, Synonym RHS, TRANS_CLOSURE isTrans);
	pair<vector<int>, vector<int>> evaluateAffectsBipByRHS(Synonym LHS, Synonym RHS, TRANS_CLOSURE isTrans);

	//Functions to process pattern clauses
	inline BOOLEAN_ processPatternClause(QNode* patternClause);
	BOOLEAN_ processAssignPattern(Synonym arg0, Synonym LHS, Synonym RHS);
	BOOLEAN_ processIfPattern(Synonym arg0, Synonym LHS, Synonym RHS);
	BOOLEAN_ processWhilePattern(Synonym arg0, Synonym LHS, Synonym RHS);

	//Functions to process with clauses
	BOOLEAN_ processWithClause(QNode* withClause);

	PKB pkb = PKB::getInstance();  //Get the instance of the PKB singleton

	/**
	* Proxy to reset the ValuesHandler from the query optimiser
	* @param name_to_type_map Maps from the synonym name to its type
	*/
	void resetValues(unordered_map<string, SYNONYM_TYPE> name_to_type_map)
	{
		ValuesHandler::initialize(name_to_type_map);
	}

	/**
	* Processes the query tree given a query tree node.
	* @param qTreeRoot the root node of the query tree
	* @return an empty vector if the Such That or Pattern clauses are invalid.
	*/
	SYNONYM_LIST processQueryTree(QueryTree* qTreeRoot) 
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
	SYNONYM_LIST processResultNode(QNode* resultNode, BOOLEAN_ isValid) 
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

			Synonym s = ValuesHandler::getSynonym(wantedSynonym);
			result.push_back(s);
		} else if (numberOfSynonyms > 1 && isValid) {
			vector<Synonym> wantedSynonyms;

			for (int i = 0; i < numberOfSynonyms; i++) {
				Synonym wantedSynonym = resultChildNode->getArg1();
				wantedSynonyms.push_back(wantedSynonym);
				resultChildNode = resultNode->getNextChild();
			}
			result = ValuesHandler::getSynonymTuples(wantedSynonyms);
		} 
		return result;
	}

	/**
	* Processes the such that node in the query tree.
	* @param clausesNode
	* @eturn TRUE if the clauses are valid. FALSE if the clauses are not valid.
	*/
	inline BOOLEAN_ processClausesNode(QNode* clausesNode) 
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
	inline BOOLEAN_ processClause(QNode* clauseNode) 
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
		case NextBip:
			return processNextBipT(LHS, RHS, false, direction);
		case NextBipT:
			return processNextBipT(LHS, RHS, true, direction);
		case AffectsBip:
			return processAffectsBipT(LHS, RHS, false, direction);
		case AffectsBipT:
			return processAffectsBipT(LHS, RHS, true, direction);
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
	BOOLEAN_ processModifies(Synonym LHS, Synonym RHS, DIRECTION direction) 
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
			pair<vector<int>, vector<int>> modifiesPair;

			if (ValuesHandler::isExistInMainTable(nameLHS) && ValuesHandler::isExistInMainTable(nameRHS)) {
				modifiesPair = evaluateModifiesByPair(LHS, RHS);
			} else if (direction == LeftToRight) {
				modifiesPair = evaluateModifiesByLHS(LHS, RHS);
			} else {
				modifiesPair = evaluateModifiesByRHS(LHS, RHS);
			}
			LHS.setValues(modifiesPair.first);
			RHS.setValues(modifiesPair.second);
			return ValuesHandler::addAndProcessIntermediateSynonyms(LHS, RHS);
		}
	}

	pair<vector<int>, vector<int>> evaluateModifiesByPair(Synonym LHS, Synonym RHS)
	{
		pair<vector<int>, vector<int>> valuesPair = ValuesHandler::getIntermediateValuesPair(LHS.getName(), RHS.getName());
		vector<int> valuesLHS = valuesPair.first;
		vector<int> valuesRHS = valuesPair.second;
		vector<int> acceptedLHS;
		vector<int> acceptedRHS;

		if (LHS.getType() == PROCEDURE) {
			for (unsigned int i = 0; i < valuesLHS.size(); i++) {
				int valueLHS = valuesLHS[i];
				int valueRHS = valuesRHS[i];
				if (pkb.isModifiesProc(valueLHS, valueRHS)) {
					acceptedLHS.push_back(valueLHS);
					acceptedRHS.push_back(valueRHS);
				}
			}
		} else {
			for (unsigned int i = 0; i < valuesLHS.size(); i++) {
				int valueLHS = valuesLHS[i];
				int valueRHS = valuesRHS[i];
				if (pkb.isModifies(valueLHS, valueRHS)) {
					acceptedLHS.push_back(valueLHS);
					acceptedRHS.push_back(valueRHS);
				}
			}
		}
		return make_pair(acceptedLHS, acceptedRHS);
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
	BOOLEAN_ processUses(Synonym LHS, Synonym RHS, DIRECTION direction) 
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
			pair<vector<int>, vector<int>> usesPair;

			if (ValuesHandler::isExistInMainTable(nameLHS) && ValuesHandler::isExistInMainTable(nameRHS)) {
				usesPair = evaluateUsesByPair(LHS, RHS);
			} else if (direction == LeftToRight) {
				usesPair = evaluateUsesByLHS(LHS, RHS);
			} else {
				usesPair = evaluateUsesByRHS(LHS, RHS);
			}
			LHS.setValues(usesPair.first);
			RHS.setValues(usesPair.second);
			return ValuesHandler::addAndProcessIntermediateSynonyms(LHS, RHS);
		}
	}

	pair<vector<int>, vector<int>> evaluateUsesByPair(Synonym LHS, Synonym RHS)
	{
		pair<vector<int>, vector<int>> valuesPair = ValuesHandler::getIntermediateValuesPair(LHS.getName(), RHS.getName());
		vector<int> valuesLHS = valuesPair.first;
		vector<int> valuesRHS = valuesPair.second;
		vector<int> acceptedLHS;
		vector<int> acceptedRHS;

		if (LHS.getType() == PROCEDURE) {
			for (unsigned int i = 0; i < valuesLHS.size(); i++) {
				int valueLHS = valuesLHS[i];
				int valueRHS = valuesRHS[i];
				if (pkb.isUsesProc(valueLHS, valueRHS)) {
					acceptedLHS.push_back(valueLHS);
					acceptedRHS.push_back(valueRHS);
				}
			}
		} else {
			for (unsigned int i = 0; i < valuesLHS.size(); i++) {
				int valueLHS = valuesLHS[i];
				int valueRHS = valuesRHS[i];
				if (pkb.isUses(valueLHS, valueRHS)) {
					acceptedLHS.push_back(valueLHS);
					acceptedRHS.push_back(valueRHS);
				}
			}
		}
		return make_pair(acceptedLHS, acceptedRHS);
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
	BOOLEAN_ processParentT(Synonym LHS, Synonym RHS, TRANS_CLOSURE isTrans, DIRECTION direction) 
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
			return pkb.isParentValid();
		} else if (typeLHS == UNDEFINED) {
			RHS.setValues(pkb.getParentRhs());
			return ValuesHandler::addAndProcessIntermediateSynonym(RHS);
		} else if (typeRHS == UNDEFINED) {
			LHS.setValues(pkb.getParentLhs());
			return ValuesHandler::addAndProcessIntermediateSynonym(LHS);
		} else {
			pair<vector<int>, vector<int>> parentsPair;

			if (ValuesHandler::isExistInMainTable(nameLHS) && ValuesHandler::isExistInMainTable(nameRHS)) {
				parentsPair = evaluateParentByPair(LHS, RHS, isTrans);
			} else if (direction == LeftToRight) {
				parentsPair = evaluateParentByLHS(LHS, RHS, isTrans);
			} else {
				parentsPair = evaluateParentByRHS(LHS, RHS, isTrans);
			}
			LHS.setValues(parentsPair.first);
			RHS.setValues(parentsPair.second);
			return ValuesHandler::addAndProcessIntermediateSynonyms(LHS, RHS);
		}
	}

	pair<vector<int>, vector<int>> evaluateParentByPair(Synonym LHS, Synonym RHS, TRANS_CLOSURE isTrans)
	{
		pair<vector<int>, vector<int>> valuesPair = ValuesHandler::getIntermediateValuesPair(LHS.getName(), RHS.getName());
		vector<int> valuesLHS = valuesPair.first;
		vector<int> valuesRHS = valuesPair.second;
		vector<int> acceptedLHS;
		vector<int> acceptedRHS;

		for (unsigned int i = 0; i < valuesLHS.size(); i++) {
			int valueLHS = valuesLHS[i];
			int valueRHS = valuesRHS[i];
			if (pkb.isParent(valueLHS, valueRHS, isTrans)) {
				acceptedLHS.push_back(valueLHS);
				acceptedRHS.push_back(valueRHS);
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
	pair<vector<int>, vector<int>> evaluateParentByLHS(Synonym LHS, Synonym RHS, TRANS_CLOSURE isTrans)
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
	pair<vector<int>, vector<int>> evaluateParentByRHS(Synonym LHS, Synonym RHS, TRANS_CLOSURE isTrans)
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
	BOOLEAN_ processFollowsT(Synonym LHS, Synonym RHS, TRANS_CLOSURE isTrans, DIRECTION direction) 
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
			return pkb.isFollowsValid();
		} else if (typeLHS == UNDEFINED) {
			RHS.setValues(pkb.getFollowsRhs());
			return ValuesHandler::addAndProcessIntermediateSynonym(RHS);
		} else if (typeRHS == UNDEFINED) {
			LHS.setValues(pkb.getFollowsLhs());
			return ValuesHandler::addAndProcessIntermediateSynonym(LHS);
		} else {
			pair<vector<int>, vector<int>> followsPair;

			if (ValuesHandler::isExistInMainTable(nameLHS) && ValuesHandler::isExistInMainTable(nameRHS)) {
				followsPair = evaluateFollowsByPair(LHS, RHS, isTrans);
			} else if (direction == LeftToRight) {
				followsPair = evaluateFollowsByLHS(LHS, RHS, isTrans);
			} else {
				followsPair = evaluateFollowsByRHS(LHS, RHS, isTrans);
			}
			LHS.setValues(followsPair.first);
			RHS.setValues(followsPair.second);
			return ValuesHandler::addAndProcessIntermediateSynonyms(LHS, RHS);
		}
	}

	pair<vector<int>, vector<int>> evaluateFollowsByPair(Synonym LHS, Synonym RHS, TRANS_CLOSURE isTrans)
	{
		pair<vector<int>, vector<int>> valuesPair = ValuesHandler::getIntermediateValuesPair(LHS.getName(), RHS.getName());
		vector<int> valuesLHS = valuesPair.first;
		vector<int> valuesRHS = valuesPair.second;
		vector<int> acceptedLHS;
		vector<int> acceptedRHS;

		for (unsigned int i = 0; i < valuesLHS.size(); i++) {
			int valueLHS = valuesLHS[i];
			int valueRHS = valuesRHS[i];
			if (pkb.isFollows(valueLHS, valueRHS, isTrans)) {
				acceptedLHS.push_back(valueLHS);
				acceptedRHS.push_back(valueRHS);
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
	pair<vector<int>, vector<int>> evaluateFollowsByLHS(Synonym LHS, Synonym RHS, TRANS_CLOSURE isTrans)
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
	pair<vector<int>, vector<int>> evaluateFollowsByRHS(Synonym LHS, Synonym RHS, TRANS_CLOSURE isTrans)
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
	BOOLEAN_ processCallsT(Synonym LHS, Synonym RHS, TRANS_CLOSURE isTrans, DIRECTION direction)
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
			return pkb.isCallsValid();
		} else if (typeLHS == UNDEFINED) {
			RHS.setValues(pkb.getCallsRhs());
			return ValuesHandler::addAndProcessIntermediateSynonym(RHS);
		} else if (typeRHS == UNDEFINED) {
			LHS.setValues(pkb.getCallsLhs());
			return ValuesHandler::addAndProcessIntermediateSynonym(LHS);
		} else {
			pair<vector<int>, vector<int>> callsPair;

			if (ValuesHandler::isExistInMainTable(nameLHS) && ValuesHandler::isExistInMainTable(nameRHS)) {
				callsPair = evaluateCallsByPair(LHS, RHS, isTrans);
			} else if (direction == LeftToRight) {
				callsPair = evaluateCallsByLHS(LHS, RHS, isTrans);
			} else {
				callsPair = evaluateCallsByRHS(LHS, RHS, isTrans);
			}
			LHS.setValues(callsPair.first);
			RHS.setValues(callsPair.second);
			return ValuesHandler::addAndProcessIntermediateSynonyms(LHS, RHS);
		}
	}

	pair<vector<int>, vector<int>> evaluateCallsByPair(Synonym LHS, Synonym RHS, TRANS_CLOSURE isTrans)
	{
		pair<vector<int>, vector<int>> valuesPair = ValuesHandler::getIntermediateValuesPair(LHS.getName(), RHS.getName());
		vector<int> valuesLHS = valuesPair.first;
		vector<int> valuesRHS = valuesPair.second;
		vector<int> acceptedLHS;
		vector<int> acceptedRHS;

		for (unsigned int i = 0; i < valuesLHS.size(); i++) {
			int valueLHS = valuesLHS[i];
			int valueRHS = valuesRHS[i];
			if (pkb.isCalls(valueLHS, valueRHS, isTrans)) {
				acceptedLHS.push_back(valueLHS);
				acceptedRHS.push_back(valueRHS);
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
	pair<vector<int>, vector<int>> evaluateCallsByLHS(Synonym LHS, Synonym RHS, TRANS_CLOSURE isTrans)
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
	pair<vector<int>, vector<int>> evaluateCallsByRHS(Synonym LHS, Synonym RHS, TRANS_CLOSURE isTrans)
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
	BOOLEAN_ processNextT(Synonym LHS, Synonym RHS, TRANS_CLOSURE isTrans, DIRECTION direction)
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
			return pkb.isNextValid();
		} else if (typeLHS == UNDEFINED) {
			vector<int> existingRHSValues = ValuesHandler::getSynonym(nameRHS).getValues();
			vector<int> acceptedValues;
			for (unsigned int i = 0; i < existingRHSValues.size(); i++) {
				int value = existingRHSValues[i];
				if (pkb.isNextRhsValid(value)) {
					acceptedValues.push_back(value);
				}
			}
			RHS.setValues(acceptedValues);
			return ValuesHandler::addAndProcessIntermediateSynonym(RHS);
		} else if (typeRHS == UNDEFINED) {
			vector<int> existingLHSValues = ValuesHandler::getSynonym(nameLHS).getValues();
			vector<int> acceptedValues;
			for (unsigned int i = 0; i < existingLHSValues.size(); i++) {
				int value = existingLHSValues[i];
				if (pkb.isNextLhsValid(value)) {
					acceptedValues.push_back(value);
				}
			}
			LHS.setValues(acceptedValues);
			return ValuesHandler::addAndProcessIntermediateSynonym(LHS);
		} else {
			pair<vector<int>, vector<int>> nextPair;

			if (ValuesHandler::isExistInMainTable(nameLHS) && ValuesHandler::isExistInMainTable(nameRHS)) {
				nextPair = evaluateNextByPair(LHS, RHS, isTrans);
			} else if (direction == LeftToRight) {
				nextPair = evaluateNextByLHS(LHS, RHS, isTrans);
			} else {
				nextPair = evaluateNextByRHS(LHS, RHS, isTrans);
			}
			LHS.setValues(nextPair.first);
			RHS.setValues(nextPair.second);
			return ValuesHandler::addAndProcessIntermediateSynonyms(LHS, RHS);
		}
	}

	pair<vector<int>, vector<int>> evaluateNextByPair(Synonym LHS, Synonym RHS, TRANS_CLOSURE isTrans)
	{
		pair<vector<int>, vector<int>> valuesPair = ValuesHandler::getIntermediateValuesPair(LHS.getName(), RHS.getName());
		vector<int> valuesLHS = valuesPair.first;
		vector<int> valuesRHS = valuesPair.second;
		vector<int> acceptedLHS;
		vector<int> acceptedRHS;

		for (unsigned int i = 0; i < valuesLHS.size(); i++) {
			int valueLHS = valuesLHS[i];
			int valueRHS = valuesRHS[i];
			if (pkb.isNext(valueLHS, valueRHS, isTrans)) {
				acceptedLHS.push_back(valueLHS);
				acceptedRHS.push_back(valueRHS);
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
	pair<vector<int>, vector<int>> evaluateNextByLHS(Synonym LHS, Synonym RHS, TRANS_CLOSURE isTrans)
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
	pair<vector<int>, vector<int>> evaluateNextByRHS(Synonym LHS, Synonym RHS, TRANS_CLOSURE isTrans)
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
	BOOLEAN_ processAffectsT(Synonym LHS, Synonym RHS, TRANS_CLOSURE isTrans, DIRECTION direction)
	{
		SYNONYM_TYPE typeLHS = LHS.getType();
		SYNONYM_TYPE typeRHS = RHS.getType();
		string nameLHS = LHS.getName();
		string nameRHS = RHS.getName();

		if (typeLHS == STRING_INT && typeRHS == STRING_INT) {
			return pkb.isAffects(stoi(nameLHS), stoi(nameRHS), isTrans);
		} else if (typeLHS == STRING_INT) {
			vector<int> stmt = pkb.getAffectedBy(stoi(nameLHS), isTrans);
			RHS.setValues(stmt);
			return ValuesHandler::addAndProcessIntermediateSynonym(RHS);
		} else if (typeRHS == STRING_INT) {
			vector<int> stmt = pkb.getAffecting(stoi(nameRHS), isTrans);
			LHS.setValues(stmt);
			return ValuesHandler::addAndProcessIntermediateSynonym(LHS);
		} else if (typeLHS == UNDEFINED && typeRHS == UNDEFINED) {
			return pkb.isAffectsValid();
		} else if (typeLHS == UNDEFINED) {
			vector<int> existingRHSValues = ValuesHandler::getSynonym(nameRHS).getValues();
			vector<int> acceptedValues;
			for (unsigned int i = 0; i < existingRHSValues.size(); i++) {
				int value = existingRHSValues[i];
				if (pkb.isAffectsRhsValid(value)) {
					acceptedValues.push_back(value);
				}
			}
			RHS.setValues(acceptedValues);
			return ValuesHandler::addAndProcessIntermediateSynonym(RHS);
		} else if (typeRHS == UNDEFINED) {
			vector<int> existingLHSValues = ValuesHandler::getSynonym(nameLHS).getValues();
			vector<int> acceptedValues;
			for (unsigned int i = 0; i < existingLHSValues.size(); i++) {
				int value = existingLHSValues[i];
				if (pkb.isAffectsLhsValid(value)) {
					acceptedValues.push_back(value);
				}
			}
			LHS.setValues(acceptedValues);
			return ValuesHandler::addAndProcessIntermediateSynonym(LHS);
		} else {
			pair<vector<int>, vector<int>> affectsPair;

			if (ValuesHandler::isExistInMainTable(nameLHS) && ValuesHandler::isExistInMainTable(nameRHS)) {
				affectsPair = evaluateAffectsByPair(LHS, RHS, isTrans);
			} else if (direction == LeftToRight) {
				affectsPair = evaluateAffectsByLHS(LHS, RHS, isTrans);
			} else {
				affectsPair = evaluateAffectsByRHS(LHS, RHS, isTrans);
			}
			LHS.setValues(affectsPair.first);
			RHS.setValues(affectsPair.second);
			return ValuesHandler::addAndProcessIntermediateSynonyms(LHS, RHS);
		}
	}

	pair<vector<int>, vector<int>> evaluateAffectsByPair(Synonym LHS, Synonym RHS, TRANS_CLOSURE isTrans)
	{
		pair<vector<int>, vector<int>> valuesPair = ValuesHandler::getIntermediateValuesPair(LHS.getName(), RHS.getName());
		vector<int> valuesLHS = valuesPair.first;
		vector<int> valuesRHS = valuesPair.second;
		vector<int> acceptedLHS;
		vector<int> acceptedRHS;

		for (unsigned int i = 0; i < valuesLHS.size(); i++) {
			int valueLHS = valuesLHS[i];
			int valueRHS = valuesRHS[i];
			if (pkb.isAffects(valueLHS, valueRHS, isTrans)) {
				acceptedLHS.push_back(valueLHS);
				acceptedRHS.push_back(valueRHS);
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
	pair<vector<int>, vector<int>> evaluateAffectsByLHS(Synonym LHS, Synonym RHS, TRANS_CLOSURE isTrans)
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
	pair<vector<int>, vector<int>> evaluateAffectsByRHS(Synonym LHS, Synonym RHS, TRANS_CLOSURE isTrans)
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
	* Processes the NextBip clause.
	* @param LHS
	* @param RHS
	* @param isTrans a flag to indicate the computation of NextBip or NextBip* relation
	* @return TRUE if the clause is valid. FALSE if the clause is not valid.
	*/
	BOOLEAN_ processNextBipT(Synonym LHS, Synonym RHS, TRANS_CLOSURE isTrans, DIRECTION direction)
	{
		SYNONYM_TYPE typeLHS = LHS.getType();
		SYNONYM_TYPE typeRHS = RHS.getType();
		string nameLHS = LHS.getName();
		string nameRHS = RHS.getName();

		if (typeLHS == STRING_INT && typeRHS == STRING_INT) {
			return pkb.isNextBip(stoi(nameLHS), stoi(nameRHS), isTrans);
		} else if (typeLHS == STRING_INT) {
			vector<int> stmt = pkb.getNextBipAfter(stoi(nameLHS), isTrans);
			RHS.setValues(stmt);
			return ValuesHandler::addAndProcessIntermediateSynonym(RHS);
		} else if (typeRHS == STRING_INT) {
			vector<int> stmt = pkb.getNextBipBefore(stoi(nameRHS), isTrans);
			LHS.setValues(stmt);
			return ValuesHandler::addAndProcessIntermediateSynonym(LHS);
		} else if (typeLHS == UNDEFINED && typeRHS == UNDEFINED) {
			return true;  //Waiting for isNextBipValid();
		} else if (typeLHS == UNDEFINED) {
			RHS.setValues(pkb.getNextBipRhs());
			return ValuesHandler::addAndProcessIntermediateSynonym(RHS);
		} else if (typeRHS == UNDEFINED) {
			LHS.setValues(pkb.getNextBipLhs());
			return ValuesHandler::addAndProcessIntermediateSynonym(LHS);
		} else {
			pair<vector<int>, vector<int>> nextBipPair;

			if (ValuesHandler::isExistInMainTable(nameLHS) && ValuesHandler::isExistInMainTable(nameRHS)) {
				nextBipPair = evaluateNextBipByPair(LHS, RHS, isTrans);
			} else if (direction == LeftToRight) {
				nextBipPair = evaluateNextBipByLHS(LHS, RHS, isTrans);
			} else {
				nextBipPair = evaluateNextBipByRHS(LHS, RHS, isTrans);
			}
			LHS.setValues(nextBipPair.first);
			RHS.setValues(nextBipPair.second);
			return ValuesHandler::addAndProcessIntermediateSynonyms(LHS, RHS);
		}
	}

	pair<vector<int>, vector<int>> evaluateNextBipByPair(Synonym LHS, Synonym RHS, TRANS_CLOSURE isTrans)
	{
		pair<vector<int>, vector<int>> valuesPair = ValuesHandler::getIntermediateValuesPair(LHS.getName(), RHS.getName());
		vector<int> valuesLHS = valuesPair.first;
		vector<int> valuesRHS = valuesPair.second;
		vector<int> acceptedLHS;
		vector<int> acceptedRHS;

		for (unsigned int i = 0; i < valuesLHS.size(); i++) {
			int valueLHS = valuesLHS[i];
			int valueRHS = valuesRHS[i];
			if (pkb.isNextBip(valueLHS, valueRHS, isTrans)) {
				acceptedLHS.push_back(valueLHS);
				acceptedRHS.push_back(valueRHS);
			}
		}
		return make_pair(acceptedLHS, acceptedRHS);
	}

	/**
	* @param LHS
	* @param RHS
	* @param isTrans a flag to indicate the computation of NextBip or NextBip* relation
	* @return A pair of vectors denoting the pairs of values returned by the relation
	*/
	pair<vector<int>, vector<int>> evaluateNextBipByLHS(Synonym LHS, Synonym RHS, TRANS_CLOSURE isTrans)
	{
		vector<int> valuesLHS = ValuesHandler::getSynonym(LHS.getName()).getValues();
		vector<int> acceptedLHS;
		vector<int> acceptedRHS;

		for (unsigned int i = 0; i < valuesLHS.size(); i++) {
			vector<int> stmts = pkb.getNextBipAfter(valuesLHS[i], isTrans);

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
	* @param isTrans a flag to indicate the computation of NextBip or NextBip* relation
	* @return A pair of vectors denoting the pairs of values returned by the relation
	*/
	pair<vector<int>, vector<int>> evaluateNextBipByRHS(Synonym LHS, Synonym RHS, TRANS_CLOSURE isTrans)
	{
		vector<int> valuesRHS = ValuesHandler::getSynonym(RHS.getName()).getValues();
		vector<int> acceptedLHS;
		vector<int> acceptedRHS;

		for (unsigned int i = 0; i < valuesRHS.size(); i++) {
			vector<int> stmts = pkb.getNextBipBefore(valuesRHS[i], isTrans);

			for (unsigned int j = 0; j < stmts.size(); j++) {
				acceptedLHS.push_back(stmts[j]);
				acceptedRHS.push_back(valuesRHS[i]);
			}
		}
		return make_pair(acceptedLHS, acceptedRHS);
	}
	
	/**
	* Processes the AffectsBip clause.
	* @param LHS
	* @param RHS
	* @param isTrans a flag to indicate the computation of AffectsBip or AffectsBip* relation
	* @return TRUE if the clause is valid. FALSE if the clause is not valid.
	*/
	BOOLEAN_ processAffectsBipT(Synonym LHS, Synonym RHS, TRANS_CLOSURE isTrans, DIRECTION direction)
	{
		SYNONYM_TYPE typeLHS = LHS.getType();
		SYNONYM_TYPE typeRHS = RHS.getType();
		string nameLHS = LHS.getName();
		string nameRHS = RHS.getName();

		if (typeLHS == STRING_INT && typeRHS == STRING_INT) {
			return pkb.isAffectsBip(stoi(nameLHS), stoi(nameRHS), isTrans);
		} else if (typeLHS == STRING_INT) {
			vector<int> stmt = pkb.getAffectsBipAfter(stoi(nameLHS), isTrans);
			RHS.setValues(stmt);
			return ValuesHandler::addAndProcessIntermediateSynonym(RHS);
		} else if (typeRHS == STRING_INT) {
			vector<int> stmt = pkb.getAffectsBipBefore(stoi(nameRHS), isTrans);
			LHS.setValues(stmt);
			return ValuesHandler::addAndProcessIntermediateSynonym(LHS);
		} else if (typeLHS == UNDEFINED && typeRHS == UNDEFINED) {
			return true;  //Waiting for isAffectsBipValid()
		} else if (typeLHS == UNDEFINED) {
			RHS.setValues(pkb.getAffectsBipRhs());
			return ValuesHandler::addAndProcessIntermediateSynonym(RHS);
		} else if (typeRHS == UNDEFINED) {
			LHS.setValues(pkb.getAffectsBipLhs());
			return ValuesHandler::addAndProcessIntermediateSynonym(LHS);
		} else {
			pair<vector<int>, vector<int>> affectsBipPair;

			if (ValuesHandler::isExistInMainTable(nameLHS) && ValuesHandler::isExistInMainTable(nameRHS)) {
				affectsBipPair = evaluateAffectsBipByPair(LHS, RHS, isTrans);
			} else if (direction == LeftToRight) {
				affectsBipPair = evaluateAffectsBipByLHS(LHS, RHS, isTrans);
			} else {
				affectsBipPair = evaluateAffectsBipByRHS(LHS, RHS, isTrans);
			}
			LHS.setValues(affectsBipPair.first);
			RHS.setValues(affectsBipPair.second);
			return ValuesHandler::addAndProcessIntermediateSynonyms(LHS, RHS);
		}
	}

	pair<vector<int>, vector<int>> evaluateAffectsBipByPair(Synonym LHS, Synonym RHS, TRANS_CLOSURE isTrans)
	{
		pair<vector<int>, vector<int>> valuesPair = ValuesHandler::getIntermediateValuesPair(LHS.getName(), RHS.getName());
		vector<int> valuesLHS = valuesPair.first;
		vector<int> valuesRHS = valuesPair.second;
		vector<int> acceptedLHS;
		vector<int> acceptedRHS;

		for (unsigned int i = 0; i < valuesLHS.size(); i++) {
			int valueLHS = valuesLHS[i];
			int valueRHS = valuesRHS[i];
			if (pkb.isAffectsBip(valueLHS, valueRHS, isTrans)) {
				acceptedLHS.push_back(valueLHS);
				acceptedRHS.push_back(valueRHS);
			}
		}
		return make_pair(acceptedLHS, acceptedRHS);
	}

	/**
	* @param LHS
	* @param RHS
	* @param isTrans a flag to indicate the computation of AffectsBip or AffectsBip* relation
	* @return A pair of vectors denoting the pairs of values returned by the relation
	*/
	pair<vector<int>, vector<int>> evaluateAffectsBipByLHS(Synonym LHS, Synonym RHS, TRANS_CLOSURE isTrans)
	{
		vector<int> valuesLHS = ValuesHandler::getSynonym(LHS.getName()).getValues();
		vector<int> acceptedLHS;
		vector<int> acceptedRHS;

		for (unsigned int i = 0; i < valuesLHS.size(); i++) {
			vector<int> stmts = pkb.getAffectsBipAfter(valuesLHS[i], isTrans);

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
	* @param isTrans a flag to indicate the computation of AffectsBip or AffectsBip* relation
	* @return A pair of vectors denoting the pairs of values returned by the relation
	*/
	pair<vector<int>, vector<int>> evaluateAffectsBipByRHS(Synonym LHS, Synonym RHS, TRANS_CLOSURE isTrans)
	{
		vector<int> valuesRHS = ValuesHandler::getSynonym(RHS.getName()).getValues();
		vector<int> acceptedLHS;
		vector<int> acceptedRHS;

		for (unsigned int i = 0; i < valuesRHS.size(); i++) {
			vector<int> stmts = pkb.getAffectsBipBefore(valuesRHS[i], isTrans);

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
	inline BOOLEAN_ processPatternClause(QNode* patternClause) 
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
	BOOLEAN_ processAssignPattern(Synonym arg0, Synonym LHS, Synonym RHS) 
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
			return ValuesHandler::addAndProcessIntermediateSynonyms(arg0, LHS);
		} else if (typeLHS == UNDEFINED) {
			arg0.setValues(isMatchStmts);
			return ValuesHandler::addAndProcessIntermediateSynonym(arg0);
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
	BOOLEAN_ processIfPattern(Synonym arg0, Synonym LHS, Synonym RHS) 
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
	BOOLEAN_ processWhilePattern(Synonym arg0, Synonym LHS, Synonym RHS) 
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
	BOOLEAN_ processWithClause(QNode* withClause)
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
