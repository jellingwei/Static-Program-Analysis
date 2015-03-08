//@TODO: if patterns
//@TODO: Special handling for relation(_, _) if this does not exist

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
#include "IntermediateValuesHandler.h"
#include "AbstractWrapper.h"

namespace QueryEvaluator 
{
	//Private functions to evaluate the query tree
	vector<Synonym> processResultNode(QNode* resultNode, bool isValid);
	inline bool processClausesNode(QNode* clausesNode);
	inline bool processClause(QNode* clauseNode);

	//Functions to process clauses
	bool processModifies(Synonym LHS, Synonym RHS);
	pair<vector<int>, vector<int>> evaluateModifiesByLHS(Synonym LHS, Synonym RHS);
	pair<vector<int>, vector<int>> evaluateModifiesByRHS(Synonym LHS, Synonym RHS);

	bool processUses(Synonym LHS, Synonym RHS);
	pair<vector<int>, vector<int>> evaluateUsesByLHS(Synonym LHS, Synonym RHS);
	pair<vector<int>, vector<int>> evaluateUsesByRHS(Synonym LHS, Synonym RHS);

	bool processParentT(Synonym LHS, Synonym RHS, bool isTrans);
	pair<vector<int>, vector<int>> evaluateParentByLHS(Synonym LHS, Synonym RHS, bool isTrans);
	pair<vector<int>, vector<int>> evaluateParentByRHS(Synonym LHS, Synonym RHS, bool isTrans);

	bool processFollowsT(Synonym LHS, Synonym RHS, bool isTrans);
	pair<vector<int>, vector<int>> evaluateFollowsByLHS(Synonym LHS, Synonym RHS, bool isTrans);
	pair<vector<int>, vector<int>> evaluateFollowsByRHS(Synonym LHS, Synonym RHS, bool isTrans);

	bool processCallsT(Synonym LHS, Synonym RHS, bool isTrans);
	pair<vector<int>, vector<int>> evaluateCallsByLHS(Synonym LHS, Synonym RHS, bool isTrans);
	pair<vector<int>, vector<int>> evaluateCallsByRHS(Synonym LHS, Synonym RHS, bool isTrans);

	bool processNextT(Synonym LHS, Synonym RHS, bool isTrans);
	pair<vector<int>, vector<int>> evaluateNextByLHS(Synonym LHS, Synonym RHS, bool isTrans);
	pair<vector<int>, vector<int>> evaluateNextByRHS(Synonym LHS, Synonym RHS, bool isTrans);

	bool processAffectsT(Synonym LHS, Synonym RHS, bool isTrans);
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
	* Return an empty vector if the Such That or Pattern clauses are invalid.
	*/
	vector<Synonym> processQueryTree(QueryTree* qTreeRoot) 
	{
		IntermediateValuesHandler::initialize(qTreeRoot->getSynonymsMap());
		vector<Synonym> synonymResult;

		bool isValid = processClausesNode(qTreeRoot->getClausesNode());

		synonymResult = processResultNode(qTreeRoot->getResultNode(), isValid);
		return synonymResult;
	}

	/**
	* Processes the result node in the query tree
	* Returns the wanted synonym in a vector
	*/
	vector<Synonym> processResultNode(QNode* resultNode, bool isValid) 
	{
		vector<Synonym> result;

		QNode* resultChildNode = resultNode->getChild();
		int numberOfSynonyms = resultNode->getNumberOfChildren();

		for (int i = 0; i < numberOfSynonyms; i++) {
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
			Synonym s = IntermediateValuesHandler::getSynonymWithName(wantedSynonymName);
			result.push_back(s);
			resultChildNode = resultNode->getNextChild();
		}
		return result;
	}

	/**
	* Processes the such that node in the query tree
	* Returns true if the clauses are valid, false otherwise
	*/
	inline bool processClausesNode(QNode* clausesNode) 
	{
		int numberOfClauses = clausesNode->getNumberOfChildren();
		QNode* clauseNode = clausesNode->getChild();

		for (int i = 0; i < numberOfClauses; i++) {
			bool isValid = processClause(clauseNode);

			if (!isValid) {
				return false;
			} else {
				clauseNode = clausesNode->getNextChild();
			}
		}
		return true;
	}

	/**
	* Method to processes each such that clause from the such that node
	* Returns true if a clause is valid, false otherwise
	*/
	inline bool processClause(QNode* clauseNode) 
	{
		QNODE_TYPE qnode_type = clauseNode->getNodeType();
		Synonym LHS = clauseNode->getArg1();
		Synonym RHS = clauseNode->getArg2();

		switch (qnode_type) {
		case ModifiesP:
		case ModifiesS:
			return processModifies(LHS, RHS);
		case UsesP:
		case UsesS:
			return processUses(LHS, RHS);
		case Parent:
			return processParentT(LHS, RHS, false);
		case ParentT:
			return processParentT(LHS, RHS, true);
		case Follows:
			return processFollowsT(LHS, RHS, false);
		case FollowsT:
			return processFollowsT(LHS, RHS, true);
		case Calls:
			return processCallsT(LHS, RHS, false);
		case CallsT:
			return processCallsT(LHS, RHS, true);
		case Next:
			return processNextT(LHS, RHS, false);
		case NextT:
			return processNextT(LHS, RHS, true);
		case Affects:
			return processAffectsT(LHS, RHS, false);
		case AffectsT:
			return processAffectsT(LHS, RHS, true);
		case Pattern:
			return processPatternClause(clauseNode);
		case With:
			return processWithClause(clauseNode);
		default:
			return false;
		}
	}

	/**
	* Method to processes the modifies clause
	* Returns true if this clause is valid, false otherwise
	*/
	bool processModifies(Synonym LHS, Synonym RHS) 
	{
		if (AbstractWrapper::GlobalStop) {
			return false;
		}

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
			if (stmts.size() == 0) {
				return false;
			}
			RHS.setValues(stmts);
			return IntermediateValuesHandler::addAndProcessIntermediateSynonym(RHS);
		} else if (typeLHS == STRING_CHAR) {
			//LHS is the proc name, find the variables that are modified
			vector<int> stmts = pkb.getModVarForProc(pkb.getProcIndex(nameLHS));
			if (stmts.size() == 0) {
				return false;
			}
			RHS.setValues(stmts);
			return IntermediateValuesHandler::addAndProcessIntermediateSynonym(RHS);
		} else if (typeRHS == STRING_CHAR) {
			//RHS is the variable that is modified, find the statements
			vector<int> stmts;
			if (typeLHS == PROCEDURE) {
				stmts = pkb.getModProcIndex(pkb.getVarIndex(nameRHS));
			} else {
				stmts = pkb.getModStmtNum(pkb.getVarIndex(nameRHS));
			}

			if (stmts.size() == 0) {
				return false;
			}
			LHS.setValues(stmts);
			return IntermediateValuesHandler::addAndProcessIntermediateSynonym(LHS);
		} else if (typeRHS == UNDEFINED) {
			//TODO: Does get modifies lhs get proc as well? 
			LHS.setValues(pkb.getModifiesLhs());
			return IntermediateValuesHandler::addAndProcessIntermediateSynonym(LHS);
		} else {
			//Use LHS temporarily
			pair<vector<int>, vector<int>> modifiesPair = evaluateModifiesByLHS(LHS, RHS);

			if (modifiesPair.first.size() == 0 || modifiesPair.second.size() == 0) {
				return false;
			}

			LHS.setValues(modifiesPair.first);
			RHS.setValues(modifiesPair.second);
			return IntermediateValuesHandler::addAndProcessIntermediateSynonyms(LHS, RHS);
		}
		return true;
	}

	pair<vector<int>, vector<int>> evaluateModifiesByLHS(Synonym LHS, Synonym RHS)
	{
		vector<int> valuesLHS = IntermediateValuesHandler::getSynonymWithName(LHS.getName()).getValues();
		set<int> valuesRHS = IntermediateValuesHandler::getSynonymWithName(RHS.getName()).getValuesSet();
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
				if (IntermediateValuesHandler::isValueExist(valuesRHS, stmts[j])) {
					acceptedLHS.push_back(valuesLHS[i]);
					acceptedRHS.push_back(stmts[j]);
				}
			}
		}
		return make_pair(acceptedLHS, acceptedRHS);
	}

	pair<vector<int>, vector<int>> evaluateModifiesByRHS(Synonym LHS, Synonym RHS)
	{
		set<int> valuesLHS = IntermediateValuesHandler::getSynonymWithName(LHS.getName()).getValuesSet();
		vector<int> valuesRHS = IntermediateValuesHandler::getSynonymWithName(RHS.getName()).getValues();
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
				if (IntermediateValuesHandler::isValueExist(valuesLHS, stmts[j])) {
					acceptedLHS.push_back(stmts[j]);
					acceptedRHS.push_back(valuesRHS[i]);
				}
			}
		}
		return make_pair(acceptedLHS, acceptedRHS);
	}

	/**
	* Method to processes the uses clause
	* Returns true if this clause is valid, false otherwise
	*/
	bool processUses(Synonym LHS, Synonym RHS) 
	{
		if (AbstractWrapper::GlobalStop) {
			return false;
		}

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
			if (vars.size() == 0) {
				return false;
			}
			RHS.setValues(vars);
			return IntermediateValuesHandler::addAndProcessIntermediateSynonym(RHS);
		} else if (typeLHS == STRING_CHAR) {
			vector<int> vars = pkb.getUsesVarForProc(pkb.getProcIndex(nameLHS));
			if (vars.size() == 0) {
				return false;
			}
			RHS.setValues(vars);
			return IntermediateValuesHandler::addAndProcessIntermediateSynonym(RHS);
		} else if (typeRHS == STRING_CHAR) {
			//RHS is the variable that is used, find the statements that uses it
			vector<int> stmts;
			if (typeLHS == PROCEDURE) {
				stmts = pkb.getUsesProcIndex(pkb.getVarIndex(nameRHS));
			} else {
				stmts = pkb.getUsesStmtNum(pkb.getVarIndex(nameRHS));
			}
			if (stmts.size() == 0) {
				return false;
			}
			LHS.setValues(stmts);
			return IntermediateValuesHandler::addAndProcessIntermediateSynonym(LHS);
		} else if (typeRHS == UNDEFINED) {
			//TODO: Does get uses lhs get proc as well? 
			LHS.setValues(pkb.getUsesLhs());
			return IntermediateValuesHandler::addAndProcessIntermediateSynonym(LHS);
		} else {
			//Use LHS temporarily
			pair<vector<int>, vector<int>> usesPair = evaluateUsesByLHS(LHS, RHS);

			if (usesPair.first.size() == 0 || usesPair.second.size() == 0) {
				return false;
			}

			LHS.setValues(usesPair.first);
			RHS.setValues(usesPair.second);
			return IntermediateValuesHandler::addAndProcessIntermediateSynonyms(LHS, RHS);
		}
		return true;
	}

	pair<vector<int>, vector<int>> evaluateUsesByLHS(Synonym LHS, Synonym RHS)
	{
		vector<int> valuesLHS = IntermediateValuesHandler::getSynonymWithName(LHS.getName()).getValues();
		set<int> valuesRHS = IntermediateValuesHandler::getSynonymWithName(RHS.getName()).getValuesSet();
		vector<int> acceptedLHS;
		vector<int> acceptedRHS;
		vector<int> stmts;

		for (unsigned int i = 0; i < valuesLHS.size(); i++) {
			if (LHS.getType() == PROCEDURE) {
				stmts = pkb.getUsesVarForProc(valuesLHS[i]);
			} else {
				stmts = pkb.getUsesVarForStmt(valuesLHS[i]);
			}

			for (unsigned int j = 0; j < stmts.size(); j++) {
				if (IntermediateValuesHandler::isValueExist(valuesRHS, stmts[j])) {
					acceptedLHS.push_back(valuesLHS[i]);
					acceptedRHS.push_back(stmts[j]);
				}
			}
		}
		return make_pair(acceptedLHS, acceptedRHS);
	}

	pair<vector<int>, vector<int>> evaluateUsesByRHS(Synonym LHS, Synonym RHS)
	{
		set<int> valuesLHS = IntermediateValuesHandler::getSynonymWithName(LHS.getName()).getValuesSet();
		vector<int> valuesRHS = IntermediateValuesHandler::getSynonymWithName(RHS.getName()).getValues();
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
				if (IntermediateValuesHandler::isValueExist(valuesLHS, stmts[j])) {
					acceptedLHS.push_back(stmts[j]);
					acceptedRHS.push_back(valuesRHS[i]);
				}
			}
		}
		return make_pair(acceptedLHS, acceptedRHS);
	}

	/**
	* Method to processes the parent clause
	* Returns true if this clause is valid, false otherwise
	*/
	bool processParentT(Synonym LHS, Synonym RHS, bool isTrans) 
	{
		if (AbstractWrapper::GlobalStop) {
			return false;
		}

		SYNONYM_TYPE typeLHS = LHS.getType();
		SYNONYM_TYPE typeRHS = RHS.getType();
		string nameLHS = LHS.getName();
		string nameRHS = RHS.getName();

		if (typeLHS == STRING_INT && typeRHS == STRING_INT) {
			return pkb.isParent(stoi(nameLHS), stoi(nameRHS), isTrans);
		} else if (typeLHS == STRING_INT) {
			vector<int> stmts = pkb.getChild(stoi(nameLHS), isTrans);
			if (stmts.size() == 0) {
				return false;
			}
			RHS.setValues(stmts);
			return IntermediateValuesHandler::addAndProcessIntermediateSynonym(RHS);
		} else if (typeRHS == STRING_INT) {
			vector<int> stmts = pkb.getParent(stoi(nameRHS), isTrans);
			if (stmts.size() == 0) {
				return false;
			}
			LHS.setValues(stmts);
			return IntermediateValuesHandler::addAndProcessIntermediateSynonym(LHS);
		} else if (typeLHS == UNDEFINED && typeRHS == UNDEFINED) {
			return true;
		} else if (typeLHS == UNDEFINED) {
			RHS.setValues(pkb.getParentRhs());
			return IntermediateValuesHandler::addAndProcessIntermediateSynonym(RHS);
		} else if (typeRHS == UNDEFINED) {
			LHS.setValues(pkb.getParentLhs());
			return IntermediateValuesHandler::addAndProcessIntermediateSynonym(LHS);
		} else {
			pair<vector<int>, vector<int>> parentsPair = evaluateParentByLHS(LHS, RHS, isTrans);

			if (parentsPair.first.size() == 0 || parentsPair.second.size() == 0) {
				return false;
			}

			LHS.setValues(parentsPair.first);
			RHS.setValues(parentsPair.second);
			return IntermediateValuesHandler::addAndProcessIntermediateSynonyms(LHS, RHS);
		}
		return true;
	}

	pair<vector<int>, vector<int>> evaluateParentByLHS(Synonym LHS, Synonym RHS, bool isTrans)
	{
		vector<int> valuesLHS = IntermediateValuesHandler::getSynonymWithName(LHS.getName()).getValues();
		set<int> valuesRHS = IntermediateValuesHandler::getSynonymWithName(RHS.getName()).getValuesSet();
		vector<int> acceptedLHS;
		vector<int> acceptedRHS;

		for (unsigned int i = 0; i < valuesLHS.size(); i++) {
			vector<int> stmts = pkb.getChild(valuesLHS[i], isTrans);

			for (unsigned int j = 0; j < stmts.size(); j++) {
				if (IntermediateValuesHandler::isValueExist(valuesRHS, stmts[j])) {
					acceptedLHS.push_back(valuesLHS[i]);
					acceptedRHS.push_back(stmts[j]);
				}
			}
		}
		return make_pair(acceptedLHS, acceptedRHS);
	}

	pair<vector<int>, vector<int>> evaluateParentByRHS(Synonym LHS, Synonym RHS, bool isTrans)
	{
		set<int> valuesLHS = IntermediateValuesHandler::getSynonymWithName(LHS.getName()).getValuesSet();
		vector<int> valuesRHS = IntermediateValuesHandler::getSynonymWithName(RHS.getName()).getValues();
		vector<int> acceptedLHS;
		vector<int> acceptedRHS;

		for (unsigned int i = 0; i < valuesRHS.size(); i++) {
			vector<int> stmts = pkb.getParent(valuesRHS[i], isTrans);

			for (unsigned int j = 0; j < stmts.size(); j++) {
				if (IntermediateValuesHandler::isValueExist(valuesLHS, stmts[j])) {
					acceptedLHS.push_back(stmts[j]);
					acceptedRHS.push_back(valuesRHS[i]);
				}
			}
		}
		return make_pair(acceptedLHS, acceptedRHS);
	}

	/**
	* Method to processes the follows clause
	* Returns true if this clause is valid, false otherwise
	*/
	bool processFollowsT(Synonym LHS, Synonym RHS, bool isTrans) 
	{
		if (AbstractWrapper::GlobalStop) {
			return false;
		}

		SYNONYM_TYPE typeLHS = LHS.getType();
		SYNONYM_TYPE typeRHS = RHS.getType();
		string nameLHS = LHS.getName();
		string nameRHS = RHS.getName();

		if (typeLHS == STRING_INT && typeRHS == STRING_INT) {
			return pkb.isFollows(stoi(nameLHS), stoi(nameRHS), isTrans);
		} else if (typeLHS == STRING_INT) {
			// Given stmtNum1, get stmtNum2 such that Follows(stmt1, stmt2) is satisfied
			vector<int> stmt = pkb.getStmtFollowedFrom(stoi(nameLHS), isTrans);
			if (stmt.size() == 0) {
				return false;
			}
			RHS.setValues(stmt);
			return IntermediateValuesHandler::addAndProcessIntermediateSynonym(RHS);
		} else if (typeRHS == STRING_INT) {
			// Given stmtNum2, get stmtNum1 such that Follows(stmt1, stmt2) is satisfied
			vector<int> stmt = pkb.getStmtFollowedTo(stoi(nameRHS), isTrans);
			if (stmt.size() == 0) {
				return false;
			}
			LHS.setValues(stmt);
			return IntermediateValuesHandler::addAndProcessIntermediateSynonym(LHS);
		} else if (typeLHS == UNDEFINED && typeRHS == UNDEFINED) {
			return true;
		} else if (typeLHS == UNDEFINED) {
			RHS.setValues(pkb.getFollowsRhs());
			return IntermediateValuesHandler::addAndProcessIntermediateSynonym(RHS);
		} else if (typeRHS == UNDEFINED) {
			LHS.setValues(pkb.getFollowsLhs());
			return IntermediateValuesHandler::addAndProcessIntermediateSynonym(LHS);
		} else {
			pair<vector<int>, vector<int>> followsPair = evaluateFollowsByLHS(LHS, RHS, isTrans);

			if (followsPair.first.size() == 0 || followsPair.second.size() == 0) {
				return false;
			}

			LHS.setValues(followsPair.first);
			RHS.setValues(followsPair.second);
			return IntermediateValuesHandler::addAndProcessIntermediateSynonyms(LHS, RHS);
		}
		return true;
	}

	pair<vector<int>, vector<int>> evaluateFollowsByLHS(Synonym LHS, Synonym RHS, bool isTrans)
	{
		vector<int> valuesLHS = IntermediateValuesHandler::getSynonymWithName(LHS.getName()).getValues();
		set<int> valuesRHS = IntermediateValuesHandler::getSynonymWithName(RHS.getName()).getValuesSet();
		vector<int> acceptedLHS;
		vector<int> acceptedRHS;

		for (unsigned int i = 0; i < valuesLHS.size(); i++) {
			vector<int> stmts = pkb.getStmtFollowedFrom(valuesLHS[i], isTrans);

			for (unsigned int j = 0; j < stmts.size(); j++) {
				if (IntermediateValuesHandler::isValueExist(valuesRHS, stmts[j])) {
					acceptedLHS.push_back(valuesLHS[i]);
					acceptedRHS.push_back(stmts[j]);
				}
			}
		}
		return make_pair(acceptedLHS, acceptedRHS);
	}

	pair<vector<int>, vector<int>> evaluateFollowsByRHS(Synonym LHS, Synonym RHS, bool isTrans)
	{
		set<int> valuesLHS = IntermediateValuesHandler::getSynonymWithName(LHS.getName()).getValuesSet();
		vector<int> valuesRHS = IntermediateValuesHandler::getSynonymWithName(RHS.getName()).getValues();
		vector<int> acceptedLHS;
		vector<int> acceptedRHS;

		for (unsigned int i = 0; i < valuesRHS.size(); i++) {
			vector<int> stmts = pkb.getStmtFollowedTo(valuesRHS[i], isTrans);

			for (unsigned int j = 0; j < stmts.size(); j++) {
				if (IntermediateValuesHandler::isValueExist(valuesLHS, stmts[j])) {
					acceptedLHS.push_back(stmts[j]);
					acceptedRHS.push_back(valuesRHS[i]);
				}
			}
		}
		return make_pair(acceptedLHS, acceptedRHS);
	}

	bool processCallsT(Synonym LHS, Synonym RHS, bool isTrans)
	{
		if (AbstractWrapper::GlobalStop) {
			return false;
		}

		SYNONYM_TYPE typeLHS = LHS.getType();
		SYNONYM_TYPE typeRHS = RHS.getType();
		string nameLHS = LHS.getName();
		string nameRHS = RHS.getName();

		if (typeLHS == STRING_CHAR && typeRHS == STRING_CHAR) {
			return pkb.isCalls(pkb.getProcIndex(nameLHS), pkb.getProcIndex(nameRHS), isTrans);
		} else if (typeLHS == STRING_CHAR) {
			vector<int> stmt = pkb.getProcsCalledBy(pkb.getProcIndex(nameLHS), isTrans);
			if (stmt.size() == 0) {
				return false;
			}
			RHS.setValues(stmt);
			return IntermediateValuesHandler::addAndProcessIntermediateSynonym(RHS);
		} else if (typeRHS == STRING_CHAR) {
			vector<int> stmt = pkb.getProcsCalling(pkb.getProcIndex(nameRHS), isTrans);
			if (stmt.size() == 0) {
				return false;
			}
			LHS.setValues(stmt);
			return IntermediateValuesHandler::addAndProcessIntermediateSynonym(LHS);
		} else if (typeLHS == UNDEFINED && typeRHS == UNDEFINED) {
			return true;
		} else if (typeLHS == UNDEFINED) {
			RHS.setValues(pkb.getCallsRhs());
			return IntermediateValuesHandler::addAndProcessIntermediateSynonym(RHS);
		} else if (typeRHS == UNDEFINED) {
			LHS.setValues(pkb.getCallsLhs());
			return IntermediateValuesHandler::addAndProcessIntermediateSynonym(LHS);
		} else {
			pair<vector<int>, vector<int>> callsPair = evaluateCallsByLHS(LHS, RHS, isTrans);

			if (callsPair.first.size() == 0 || callsPair.second.size() == 0) {
				return false;
			}

			LHS.setValues(callsPair.first);
			RHS.setValues(callsPair.second);
			return IntermediateValuesHandler::addAndProcessIntermediateSynonyms(LHS, RHS);
		}
		return true;
	}

	pair<vector<int>, vector<int>> evaluateCallsByLHS(Synonym LHS, Synonym RHS, bool isTrans)
	{
		vector<int> valuesLHS = IntermediateValuesHandler::getSynonymWithName(LHS.getName()).getValues();
		set<int> valuesRHS = IntermediateValuesHandler::getSynonymWithName(RHS.getName()).getValuesSet();
		vector<int> acceptedLHS;
		vector<int> acceptedRHS;

		for (unsigned int i = 0; i < valuesLHS.size(); i++) {
			vector<int> stmts = pkb.getProcsCalledBy(valuesLHS[i], isTrans);

			for (unsigned int j = 0; j < stmts.size(); j++) {
				if (IntermediateValuesHandler::isValueExist(valuesRHS, stmts[j])) {
					acceptedLHS.push_back(valuesLHS[i]);
					acceptedRHS.push_back(stmts[j]);
				}
			}
		}
		return make_pair(acceptedLHS, acceptedRHS);
	}

	pair<vector<int>, vector<int>> evaluateCallsByRHS(Synonym LHS, Synonym RHS, bool isTrans)
	{
		set<int> valuesLHS = IntermediateValuesHandler::getSynonymWithName(LHS.getName()).getValuesSet();
		vector<int> valuesRHS = IntermediateValuesHandler::getSynonymWithName(RHS.getName()).getValues();
		vector<int> acceptedLHS;
		vector<int> acceptedRHS;

		for (unsigned int i = 0; i < valuesRHS.size(); i++) {
			vector<int> stmts = pkb.getProcsCalling(valuesRHS[i], isTrans);

			for (unsigned int j = 0; j < stmts.size(); j++) {
				if (IntermediateValuesHandler::isValueExist(valuesLHS, stmts[j])) {
					acceptedLHS.push_back(stmts[j]);
					acceptedRHS.push_back(valuesRHS[i]);
				}
			}
		}
		return make_pair(acceptedLHS, acceptedRHS);
	}

	bool processNextT(Synonym LHS, Synonym RHS, bool isTrans)
	{
		if (AbstractWrapper::GlobalStop) {
			return false;
		}

		SYNONYM_TYPE typeLHS = LHS.getType();
		SYNONYM_TYPE typeRHS = RHS.getType();
		string nameLHS = LHS.getName();
		string nameRHS = RHS.getName();

		if (typeLHS == STRING_INT && typeRHS == STRING_INT) {
			return pkb.isNext(stoi(nameLHS), stoi(nameRHS), isTrans);
		} else if (typeLHS == STRING_INT) {
			vector<int> stmt = pkb.getNextAfter(stoi(nameLHS), isTrans);
			if (stmt.size() == 0) {
				return false;
			}
			RHS.setValues(stmt);
			return IntermediateValuesHandler::addAndProcessIntermediateSynonym(RHS);
		} else if (typeRHS == STRING_INT) {
			vector<int> stmt = pkb.getNextBefore(stoi(nameRHS), isTrans);
			if (stmt.size() == 0) {
				return false;
			}
			LHS.setValues(stmt);
			return IntermediateValuesHandler::addAndProcessIntermediateSynonym(LHS);
		} else if (typeLHS == UNDEFINED && typeRHS == UNDEFINED) {
			return true;
		} else if (typeLHS == UNDEFINED) {
			RHS.setValues(pkb.getNextRhs());
			return IntermediateValuesHandler::addAndProcessIntermediateSynonym(RHS);
		} else if (typeRHS == UNDEFINED) {
			LHS.setValues(pkb.getNextLhs());
			return IntermediateValuesHandler::addAndProcessIntermediateSynonym(LHS);
		} else {
			pair<vector<int>, vector<int>> callsPair = evaluateNextByLHS(LHS, RHS, isTrans);

			if (callsPair.first.size() == 0 || callsPair.second.size() == 0) {
				return false;
			}

			LHS.setValues(callsPair.first);
			RHS.setValues(callsPair.second);
			return IntermediateValuesHandler::addAndProcessIntermediateSynonyms(LHS, RHS);
		}
		return true;
	}

	pair<vector<int>, vector<int>> evaluateNextByLHS(Synonym LHS, Synonym RHS, bool isTrans)
	{
		vector<int> valuesLHS = IntermediateValuesHandler::getSynonymWithName(LHS.getName()).getValues();
		set<int> valuesRHS = IntermediateValuesHandler::getSynonymWithName(RHS.getName()).getValuesSet();
		vector<int> acceptedLHS;
		vector<int> acceptedRHS;

		for (unsigned int i = 0; i < valuesLHS.size(); i++) {
			vector<int> stmts = pkb.getNextAfter(valuesLHS[i], isTrans);

			for (unsigned int j = 0; j < stmts.size(); j++) {
				if (IntermediateValuesHandler::isValueExist(valuesRHS, stmts[j])) {
					acceptedLHS.push_back(valuesLHS[i]);
					acceptedRHS.push_back(stmts[j]);
				}
			}
		}
		return make_pair(acceptedLHS, acceptedRHS);
	}

	pair<vector<int>, vector<int>> evaluateNextByRHS(Synonym LHS, Synonym RHS, bool isTrans)
	{
		set<int> valuesLHS = IntermediateValuesHandler::getSynonymWithName(LHS.getName()).getValuesSet();
		vector<int> valuesRHS = IntermediateValuesHandler::getSynonymWithName(RHS.getName()).getValues();
		vector<int> acceptedLHS;
		vector<int> acceptedRHS;

		for (unsigned int i = 0; i < valuesRHS.size(); i++) {
			vector<int> stmts = pkb.getNextBefore(valuesRHS[i], isTrans);

			for (unsigned int j = 0; j < stmts.size(); j++) {
				if (IntermediateValuesHandler::isValueExist(valuesLHS, stmts[j])) {
					acceptedLHS.push_back(stmts[j]);
					acceptedRHS.push_back(valuesRHS[i]);
				}
			}
		}
		return make_pair(acceptedLHS, acceptedRHS);
	}

	bool processAffectsT(Synonym LHS, Synonym RHS, bool isTrans)
	{
		return false;
		/*if (AbstractWrapper::GlobalStop) {
		return false;
		}

		SYNONYM_TYPE typeLHS = LHS.getType();
		SYNONYM_TYPE typeRHS = RHS.getType();

		if (typeLHS == STRING_INT && typeRHS == STRING_INT) {
		return pkb.isCalls(pkb.getProcIndex(LHS.getName()), pkb.getProcIndex(RHS.getName()), isTrans);
		} else if (typeLHS == STRING_INT) {
		vector<int> stmt = pkb.getProcsCalledBy(pkb.getProcIndex(LHS.getName()), isTrans);
		if (stmt.size() == 0) {
		return false;
		}
		Synonym synonym(typeRHS, RHS.getName(), stmt);
		return IntermediateValuesHandler::addAndProcessIntermediateSynonym(synonym);
		} else if (typeRHS == STRING_INT) {
		vector<int> stmt = pkb.getProcsCalling(pkb.getProcIndex(RHS.getName()), isTrans);
		if (stmt.size() == 0) {
		return false;
		}
		Synonym synonym(typeLHS, LHS.getName(), stmt);
		return IntermediateValuesHandler::addAndProcessIntermediateSynonym(synonym);
		} else if (typeLHS == UNDEFINED && typeRHS == UNDEFINED) {
		return true;
		} else if (typeLHS == UNDEFINED) {
		Synonym RHS(typeRHS, RHS.getName(), pkb.getCallsRhs());
		return IntermediateValuesHandler::addAndProcessIntermediateSynonym(RHS);
		} else if (typeRHS == UNDEFINED) {
		Synonym LHS(typeLHS, LHS.getName(), pkb.getCallsLhs());
		return IntermediateValuesHandler::addAndProcessIntermediateSynonym(LHS);
		} else {
		pair<vector<int>, vector<int>> callsPair = evaluateAffectsByLHS(LHS, RHS, isTrans);

		if (callsPair.first.size() == 0 || callsPair.second.size() == 0) {
		return false;
		}

		Synonym LHS(typeLHS, LHS.getName(), callsPair.first);
		Synonym RHS(typeRHS, RHS.getName(), callsPair.second);
		return IntermediateValuesHandler::addAndProcessIntermediateSynonyms(LHS, RHS);
		}
		return true;*/
	}

	pair<vector<int>, vector<int>> evaluateAffectsByLHS(Synonym LHS, Synonym RHS, bool isTrans)
	{
		vector<int> valuesLHS = IntermediateValuesHandler::getSynonymWithName(LHS.getName()).getValues();
		set<int> valuesRHS = IntermediateValuesHandler::getSynonymWithName(RHS.getName()).getValuesSet();
		vector<int> acceptedLHS;
		vector<int> acceptedRHS;

		/*for (unsigned int i = 0; i < valuesLHS.size(); i++) {
		vector<int> stmts = pkb.getProcsCalledBy(valuesLHS[i], isTrans);

		for (unsigned int j = 0; j < stmts.size(); j++) {
		if (IntermediateValuesHandler::isValueExist(valuesRHS, stmts[j])) {
		acceptedLHS.push_back(valuesLHS[i]);
		acceptedRHS.push_back(stmts[j]);
		}
		}
		}*/
		return make_pair(acceptedLHS, acceptedRHS);
	}

	pair<vector<int>, vector<int>> evaluateAffectsByRHS(Synonym LHS, Synonym RHS, bool isTrans)
	{
		set<int> valuesLHS = IntermediateValuesHandler::getSynonymWithName(LHS.getName()).getValuesSet();
		vector<int> valuesRHS = IntermediateValuesHandler::getSynonymWithName(RHS.getName()).getValues();
		vector<int> acceptedLHS;
		vector<int> acceptedRHS;

		/*for (unsigned int i = 0; i < valuesRHS.size(); i++) {
		vector<int> stmts = pkb.getProcsCalling(valuesRHS[i], isTrans);

		for (unsigned int j = 0; j < stmts.size(); j++) {
		if (IntermediateValuesHandler::isValueExist(valuesLHS, stmts[j])) {
		acceptedLHS.push_back(stmts[j]);
		acceptedRHS.push_back(valuesRHS[i]);
		}
		}
		}*/
		return make_pair(acceptedLHS, acceptedRHS);
	}

	/**
	* Method to process individual pattern clauses
	* Returns true if the pattern clause is valid, false otherwise
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
	* Method to process assign patterns
	* Returns true if the pattern clause is valid, false otherwise
	*/
	bool processAssignPattern(Synonym arg0, Synonym LHS, Synonym RHS) 
	{
		if (AbstractWrapper::GlobalStop) {
			return false;
		}

		SYNONYM_TYPE typeLHS = LHS.getType();
		vector<int> isMatchStmts = pkb.patternMatchAssign(RHS.getName());

		if (isMatchStmts.size() == 0) {
			return false;
		}

		if (typeLHS == VARIABLE) {
			//If LHS is a variable synonym, use the return statements to probe the ModifiesTable
			vector<int> vars;

			for (vector<int>::iterator itr = isMatchStmts.begin(); itr != isMatchStmts.end(); ++itr) {
				vector<int> var = pkb.getModVarForStmt(*itr);  //Get the LHS of the assign statement
				vars.push_back(var[0]);  //Variable for assignment stmts must be in index 0
			}

			arg0.setValues(isMatchStmts);
			LHS.setValues(vars);
			IntermediateValuesHandler::addAndProcessIntermediateSynonyms(arg0, LHS);
			return true;
		} else if (typeLHS == UNDEFINED) {
			arg0.setValues(isMatchStmts);
			IntermediateValuesHandler::addAndProcessIntermediateSynonym(arg0);
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
			if (matchingStmts.size() == 0) {
				return false;
			}
			arg0.setValues(matchingStmts);
			IntermediateValuesHandler::addAndProcessIntermediateSynonym(arg0);
			return true;
		}
	}

	/**
	* Method to process if patterns
	* Returns true if the pattern clause is valid, false otherwise
	*/
	bool processIfPattern(Synonym arg0, Synonym LHS, Synonym RHS) 
	{
		if (AbstractWrapper::GlobalStop) {
			return false;
		}

		SYNONYM_TYPE typeLHS = LHS.getType();

		//Find all if statements that uses LHS
		if (LHS.getType() == STRING_CHAR) {
			vector<int> stmts = pkb.patternMatchIf(LHS.getName());
			if (stmts.size() == 0) {
				return false;
			}
			arg0.setValues(stmts);
			IntermediateValuesHandler::addAndProcessIntermediateSynonym(arg0);
			return true;
		} else if (typeLHS == UNDEFINED) {
			vector<int> ifStmts = IntermediateValuesHandler::getSynonymWithName(arg0.getName()).getValues();
			return (ifStmts.size() != 0);  //Do nothing because pattern i(_, _, _) is always true if there are if statements
		} else {
			//LHS is a variable synonym
			vector<int> arg0Values = IntermediateValuesHandler::getSynonymWithName(arg0.getName()).getValues();
			if (arg0Values.size() == 0) {
				return false;
			}

			vector<int> vars;
			for (unsigned int i = 0; i < arg0Values.size(); i++) {
				int var = pkb.getControlVariable(arg0Values[i]);
				vars.push_back(var);
			}
			arg0.setValues(arg0Values);
			LHS.setValues(vars);
			IntermediateValuesHandler::addAndProcessIntermediateSynonyms(arg0, LHS);
			return true;
		}
	}

	/**
	* Method to process while patterns
	* Returns true if the pattern clause is valid, false otherwise
	*/
	bool processWhilePattern(Synonym arg0, Synonym LHS, Synonym RHS) 
	{
		if (AbstractWrapper::GlobalStop) {
			return false;
		}

		SYNONYM_TYPE typeLHS = LHS.getType();

		//Find all while statements that uses LHS
		if (LHS.getType() == STRING_CHAR) {
			vector<int> stmts = pkb.patternMatchWhile(LHS.getName());
			if (stmts.size() == 0) {
				return false;
			}
			arg0.setValues(stmts);
			IntermediateValuesHandler::addAndProcessIntermediateSynonym(arg0);
			return true;
		} else if (typeLHS == UNDEFINED) {
			vector<int> whileStmts = IntermediateValuesHandler::getSynonymWithName(arg0.getName()).getValues();
			return (whileStmts.size() != 0);  //Do nothing because pattern w(_, _) is always true if there are while statements
		} else {
			//LHS is a variable synonym
			vector<int> arg0Values = IntermediateValuesHandler::getSynonymWithName(arg0.getName()).getValues();
			if (arg0Values.size() == 0) {
				return false;
			}

			vector<int> vars;
			for (unsigned int i = 0; i < arg0Values.size(); i++) {
				int var = pkb.getControlVariable(arg0Values[i]);
				vars.push_back(var);
			}
			arg0.setValues(arg0Values);
			LHS.setValues(vars);
			IntermediateValuesHandler::addAndProcessIntermediateSynonyms(arg0, LHS);
			return true;
		}
	}

	bool processWithClause(QNode* withClause)
	{
		//Assume that var and int cannot be compared (should be checked by the query validator)
		Synonym LHS = withClause->getArg1();
		Synonym RHS = withClause->getArg2();
		SYNONYM_TYPE typeLHS = LHS.getType();
		SYNONYM_TYPE typeRHS = RHS.getType();

		if (typeLHS == STRING_CHAR && typeRHS == STRING_CHAR) {
			string arg1Value = LHS.getName();
			string arg2Value = RHS.getName();
			return arg1Value == arg2Value;
		} else if (typeLHS == STRING_INT && typeRHS == STRING_INT) {
			string arg1Value = LHS.getName();
			string arg2Value = RHS.getName();
			return arg1Value == arg2Value;
		} else if (typeLHS == STRING_CHAR || typeLHS == STRING_INT) {
			string arg1Value = LHS.getName();
			return IntermediateValuesHandler::filterEqualValue(RHS, arg1Value);
		} else if (typeRHS == STRING_CHAR || typeRHS == STRING_INT) {
			string arg2Value = RHS.getName();
			return IntermediateValuesHandler::filterEqualValue(LHS, arg2Value);
		} else {
			return IntermediateValuesHandler::filterEqualPair(LHS, RHS);
		}
	}
}
