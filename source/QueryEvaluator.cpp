//@TODO: if patterns
//@TODO: For pairs, have to consider having "_" as the parameters
//@TODO: Special handling for relation(_, _) if this does not exist

#include <vector>
#include <string>
#include <set>
#include <cctype>  //For isdigit and isalpha
using std::set;
using std::vector;
using std::string;
using std::stoi;  //String to integer
using std::swap;
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
	bool processModifies(Synonym arg1, Synonym arg2);
	pair<vector<int>, vector<int>> evaluateModifiesByLHS(Synonym LHS, Synonym RHS);
	pair<vector<int>, vector<int>> evaluateModifiesByRHS(Synonym LHS, Synonym RHS);

	bool processUses(Synonym arg1, Synonym arg2);
	pair<vector<int>, vector<int>> evaluateUsesByLHS(Synonym LHS, Synonym RHS);
	pair<vector<int>, vector<int>> evaluateUsesByRHS(Synonym LHS, Synonym RHS);

	bool processParentT(Synonym arg1, Synonym arg2, bool isTrans);
	pair<vector<int>, vector<int>> evaluateParentByLHS(Synonym LHS, Synonym RHS, bool isTrans);
	pair<vector<int>, vector<int>> evaluateParentByRHS(Synonym LHS, Synonym RHS, bool isTrans);

	bool processFollowsT(Synonym arg1, Synonym arg2, bool isTrans);
	pair<vector<int>, vector<int>> evaluateFollowsByLHS(Synonym LHS, Synonym RHS, bool isTrans);
	pair<vector<int>, vector<int>> evaluateFollowsByRHS(Synonym LHS, Synonym RHS, bool isTrans);

	bool processCallsT(Synonym arg1, Synonym arg2, bool isTrans);
	pair<vector<int>, vector<int>> evaluateCallsByLHS(Synonym LHS, Synonym RHS, bool isTrans);
	pair<vector<int>, vector<int>> evaluateCallsByRHS(Synonym LHS, Synonym RHS, bool isTrans);

	bool processNextT(Synonym arg1, Synonym arg2, bool isTrans);
	pair<vector<int>, vector<int>> evaluateNextByLHS(Synonym LHS, Synonym RHS, bool isTrans);
	pair<vector<int>, vector<int>> evaluateNextByRHS(Synonym LHS, Synonym RHS, bool isTrans);

	bool processAffectsT(Synonym arg1, Synonym arg2, bool isTrans);
	pair<vector<int>, vector<int>> evaluateAffectsByLHS(Synonym LHS, Synonym RHS, bool isTrans);
	pair<vector<int>, vector<int>> evaluateAffectsByRHS(Synonym LHS, Synonym RHS, bool isTrans);

	//Functions to process pattern clauses
	inline bool processPatternClause(QNode* patternClause);
	bool processAssignPattern(Synonym arg0, Synonym arg1, Synonym arg2);
	bool processIfPattern(Synonym arg0, Synonym arg1, Synonym arg2);
	bool processWhilePattern(Synonym arg0, Synonym arg1, Synonym arg2);

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
		Synonym arg1 = clauseNode->getArg1();
		Synonym arg2 = clauseNode->getArg2();

		switch (qnode_type) {
		case Modifies:
			return processModifies(arg1, arg2);
		case Uses:
			return processUses(arg1, arg2);
		case Parent:
			return processParentT(arg1, arg2, false);
		case ParentS:
			return processParentT(arg1, arg2, true);
		case Follows:
			return processFollowsT(arg1, arg2, false);
		case FollowsS:
			return processFollowsT(arg1, arg2, true);
		case Calls:
			return processCallsT(arg1, arg2, false);
		case CallsS:
			return processCallsT(arg1, arg2, true);
		case Next:
			return processNextT(arg1, arg2, false);
		case NextS:
			return processNextT(arg1, arg2, true);
		case Affects:
			return processAffectsT(arg1, arg2, false);
		case AffectsS:
			return processAffectsT(arg1, arg2, true);
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
	bool processModifies(Synonym arg1, Synonym arg2) 
	{
		if (AbstractWrapper::GlobalStop) {
			return false;
		}

		SYNONYM_TYPE arg1Type = arg1.getType();
		SYNONYM_TYPE arg2Type = arg2.getType();

		//The arguments are valid, begin processing the arguments
		if (arg1Type == STRING && arg2Type == STRING) {
			return pkb.isModifies(stoi(arg1.getName()), pkb.getVarIndex(arg2.getName()));
		} else if (arg1Type == STRING) {
			//arg1 is the line number, find the variables that are modified
			vector<int> stmts = pkb.getModVarForStmt(stoi(arg1.getName()));
			if (stmts.size() == 0) {
				return false;
			}
			arg2.setValues(stmts);
			IntermediateValuesHandler::addAndProcessIntermediateSynonym(arg2);
		} else if (arg2Type == STRING) {
			//arg2 is the variable that is modified, find the statements
			vector<int> stmts = pkb.getModStmtNum(pkb.getVarIndex(arg2.getName()));
			if (stmts.size() == 0) {
				return false;
			}
			arg1.setValues(stmts);
			IntermediateValuesHandler::addAndProcessIntermediateSynonym(arg1);
		} else if (arg1Type == UNDEFINED && arg2Type == UNDEFINED) {
			return true;
		} else if (arg1Type == UNDEFINED) {
			arg2.setValues(pkb.getModifiesRhs());
			IntermediateValuesHandler::addAndProcessIntermediateSynonym(arg2);
			return true;
		} else if (arg2Type == UNDEFINED) {
			arg1.setValues(pkb.getModifiesLhs());
			IntermediateValuesHandler::addAndProcessIntermediateSynonym(arg1);
			return true;
		} else {
			//Use LHS temporarily
			pair<vector<int>, vector<int>> modifiesPair = evaluateModifiesByLHS(arg1, arg2);

			if (modifiesPair.first.size() == 0 || modifiesPair.second.size() == 0) {
				return false;
			}

			arg1.setValues(modifiesPair.first);
			arg2.setValues(modifiesPair.second);
			IntermediateValuesHandler::addAndProcessIntermediateSynonyms(arg1, arg2);
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
			vector<int> stmts = pkb.getModVarForStmt(valuesRHS[i]);

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
	bool processUses(Synonym arg1, Synonym arg2) 
	{
		if (AbstractWrapper::GlobalStop) {
			return false;
		}

		SYNONYM_TYPE arg1Type = arg1.getType();
		SYNONYM_TYPE arg2Type = arg2.getType();

		//The arguments are valid, begin processing the arguments
		if (arg1Type == STRING && arg2Type == STRING) {
			return pkb.isUses(stoi(arg1.getName()), pkb.getVarIndex(arg2.getName()));
		} else if (arg1Type == STRING) {
			//arg1 is the line number, find the variable that is used
			vector<int> vars = pkb.getUsesVarForStmt(stoi(arg1.getName()));
			if (vars.size() == 0) {
				return false;
			}
			arg2.setValues(vars);
			IntermediateValuesHandler::addAndProcessIntermediateSynonym(arg2);
		} else if (arg2Type == STRING) {
			//arg2 is the variable that is used, find the statements that uses it
			vector<int> stmts = pkb.getUsesStmtNum(pkb.getVarIndex(arg2.getName()));
			if (stmts.size() == 0) {
				return false;
			}
			arg1.setValues(stmts);
			IntermediateValuesHandler::addAndProcessIntermediateSynonym(arg1);
		} else if (arg1Type == UNDEFINED && arg2Type != UNDEFINED) {
			//@todo Uses should not have arg1 as "_"
			return true;
		} else if (arg1Type == UNDEFINED && arg2Type == UNDEFINED) {
			return true;
		} else if (arg1Type == UNDEFINED) {
			arg2.setValues(pkb.getUsesRhs());
			IntermediateValuesHandler::addAndProcessIntermediateSynonym(arg2);
			return true;
		} else if (arg2Type == UNDEFINED) {
			arg1.setValues(pkb.getUsesLhs());
			IntermediateValuesHandler::addAndProcessIntermediateSynonym(arg1);
			return true;
		} else {
			//Use LHS temporarily
			pair<vector<int>, vector<int>> usesPair = evaluateUsesByLHS(arg1, arg2);

			if (usesPair.first.size() == 0 || usesPair.second.size() == 0) {
				return false;
			}

			arg1.setValues(usesPair.first);
			arg2.setValues(usesPair.second);
			IntermediateValuesHandler::addAndProcessIntermediateSynonyms(arg1, arg2);
		}
		return true;
	}

	pair<vector<int>, vector<int>> evaluateUsesByLHS(Synonym LHS, Synonym RHS)
	{
		vector<int> valuesLHS = IntermediateValuesHandler::getSynonymWithName(LHS.getName()).getValues();
		set<int> valuesRHS = IntermediateValuesHandler::getSynonymWithName(RHS.getName()).getValuesSet();
		vector<int> acceptedLHS;
		vector<int> acceptedRHS;

		for (unsigned int i = 0; i < valuesLHS.size(); i++) {
			vector<int> stmts = pkb.getUsesVarForStmt(valuesLHS[i]);

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
			vector<int> stmts = pkb.getUsesStmtNum(valuesRHS[i]);

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
	bool processParentT(Synonym arg1, Synonym arg2, bool isTrans) 
	{
		if (AbstractWrapper::GlobalStop) {
			return false;
		}

		SYNONYM_TYPE arg1Type = arg1.getType();
		SYNONYM_TYPE arg2Type = arg2.getType();

		if (arg1Type == STRING && arg2Type == STRING) {
			return pkb.isParent(stoi(arg1.getName()), stoi(arg2.getName()), isTrans);
		} else if (arg1Type == STRING) {
			vector<int> stmts = pkb.getChild(stoi(arg1.getName()), isTrans);
			if (stmts.size() == 0) {
				return false;
			}
			arg2.setValues(stmts);
			IntermediateValuesHandler::addAndProcessIntermediateSynonym(arg2);
		} else if (arg2Type == STRING) {
			vector<int> stmts = pkb.getParent(stoi(arg2.getName()), isTrans);
			if (stmts.size() == 0) {
				return false;
			}
			arg1.setValues(stmts);
			IntermediateValuesHandler::addAndProcessIntermediateSynonym(arg1);
		} else if (arg1Type == UNDEFINED && arg2Type == UNDEFINED) {
			return true;
		} else if (arg1Type == UNDEFINED) {
			arg2.setValues(pkb.getParentRhs());
			IntermediateValuesHandler::addAndProcessIntermediateSynonym(arg2);
			return true;
		} else if (arg2Type == UNDEFINED) {
			arg1.setValues(pkb.getParentLhs());
			IntermediateValuesHandler::addAndProcessIntermediateSynonym(arg1);
			return true;
		} else {
			pair<vector<int>, vector<int>> parentsPair = evaluateParentByLHS(arg1, arg2, isTrans);

			if (parentsPair.first.size() == 0 || parentsPair.second.size() == 0) {
				return false;
			}

			arg1.setValues(parentsPair.first);
			arg2.setValues(parentsPair.second);
			IntermediateValuesHandler::addAndProcessIntermediateSynonyms(arg1, arg2);
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
	bool processFollowsT(Synonym arg1, Synonym arg2, bool isTrans) 
	{
		if (AbstractWrapper::GlobalStop) {
			return false;
		}

		SYNONYM_TYPE arg1Type = arg1.getType();
		SYNONYM_TYPE arg2Type = arg2.getType();

		if (arg1Type == STRING && arg2Type == STRING){
			return pkb.isFollows(stoi(arg1.getName()), stoi(arg2.getName()), isTrans);
		} else if (arg1Type == STRING) {
			// Given stmtNum1, get stmtNum2 such that Follows(stmt1, stmt2) is satisfied
			vector<int> stmt = pkb.getStmtFollowedFrom(stoi(arg1.getName()), isTrans);
			if (stmt.size() == 0) {
				return false;
			}
			arg2.setValues(stmt);
			IntermediateValuesHandler::addAndProcessIntermediateSynonym(arg2);
		} else if (arg2Type == STRING) {
			// Given stmtNum2, get stmtNum1 such that Follows(stmt1, stmt2) is satisfied
			vector<int> stmt = pkb.getStmtFollowedTo(stoi(arg2.getName()), isTrans);
			if (stmt.size() == 0) {
				return false;
			}
			arg1.setValues(stmt);
			IntermediateValuesHandler::addAndProcessIntermediateSynonym(arg1);
		} else if (arg1Type == UNDEFINED && arg2Type == UNDEFINED) {
			return true;
		} else if (arg1Type == UNDEFINED) {
			arg2.setValues(pkb.getFollowsRhs());
			IntermediateValuesHandler::addAndProcessIntermediateSynonym(arg2);
			return true;
		} else if (arg2Type == UNDEFINED) {
			arg1.setValues(pkb.getFollowsLhs());
			IntermediateValuesHandler::addAndProcessIntermediateSynonym(arg1);
			return true;
		} else {
			pair<vector<int>, vector<int>> followsPair = evaluateFollowsByLHS(arg1, arg2, isTrans);

			if (followsPair.first.size() == 0 || followsPair.second.size() == 0) {
				return false;
			}

			arg1.setValues(followsPair.first);
			arg2.setValues(followsPair.second);
			IntermediateValuesHandler::addAndProcessIntermediateSynonyms(arg1, arg2);
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

	bool processCallsT(Synonym arg1, Synonym arg2, bool isTrans)
	{
		if (AbstractWrapper::GlobalStop) {
			return false;
		}

		SYNONYM_TYPE arg1Type = arg1.getType();
		SYNONYM_TYPE arg2Type = arg2.getType();

		if (arg1Type == STRING && arg2Type == STRING) {
			return pkb.isCalls(pkb.getProcIndex(arg1.getName()), pkb.getProcIndex(arg2.getName()), isTrans);
		} else if (arg1Type == STRING) {
			vector<int> stmt = pkb.getProcsCalledBy(pkb.getProcIndex(arg1.getName()), isTrans);
			if (stmt.size() == 0) {
				return false;
			}
			arg2.setValues(stmt);
			IntermediateValuesHandler::addAndProcessIntermediateSynonym(arg2);
		} else if (arg2Type == STRING) {
			vector<int> stmt = pkb.getProcsCalling(pkb.getProcIndex(arg2.getName()), isTrans);
			if (stmt.size() == 0) {
				return false;
			}
			arg1.setValues(stmt);
			IntermediateValuesHandler::addAndProcessIntermediateSynonym(arg1);
		} else if (arg1Type == UNDEFINED && arg2Type == UNDEFINED) {
			return true;
		} else if (arg1Type == UNDEFINED) {
			arg2.setValues(pkb.getCallsRhs());
			IntermediateValuesHandler::addAndProcessIntermediateSynonym(arg2);
			return true;
		} else if (arg2Type == UNDEFINED) {
			arg1.setValues(pkb.getCallsLhs());
			IntermediateValuesHandler::addAndProcessIntermediateSynonym(arg1);
			return true;
		} else {
			pair<vector<int>, vector<int>> callsPair = evaluateCallsByLHS(arg1, arg2, isTrans);

			if (callsPair.first.size() == 0 || callsPair.second.size() == 0) {
				return false;
			}

			arg1.setValues(callsPair.first);
			arg2.setValues(callsPair.second);
			IntermediateValuesHandler::addAndProcessIntermediateSynonyms(arg1, arg2);
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

	bool processNextT(Synonym arg1, Synonym arg2, bool isTrans)
	{
		if (AbstractWrapper::GlobalStop) {
			return false;
		}

		SYNONYM_TYPE arg1Type = arg1.getType();
		SYNONYM_TYPE arg2Type = arg2.getType();

		if (arg1Type == STRING && arg2Type == STRING) {
			return pkb.isNext(stoi(arg1.getName()), stoi(arg2.getName()), isTrans);
		} else if (arg1Type == STRING) {
			vector<int> stmt = pkb.getNextAfter(stoi(arg1.getName()), isTrans);
			if (stmt.size() == 0) {
				return false;
			}
			arg2.setValues(stmt);
			IntermediateValuesHandler::addAndProcessIntermediateSynonym(arg2);
		} else if (arg2Type == STRING) {
			vector<int> stmt = pkb.getNextBefore(stoi(arg2.getName()), isTrans);
			if (stmt.size() == 0) {
				return false;
			}
			arg1.setValues(stmt);
			IntermediateValuesHandler::addAndProcessIntermediateSynonym(arg1);
		} else if (arg1Type == UNDEFINED && arg2Type == UNDEFINED) {
			return true;
		} else if (arg1Type == UNDEFINED) {
			arg2.setValues(pkb.getNextRhs());
			IntermediateValuesHandler::addAndProcessIntermediateSynonym(arg2);
			return true;
		} else if (arg2Type == UNDEFINED) {
			arg1.setValues(pkb.getNextLhs());
			IntermediateValuesHandler::addAndProcessIntermediateSynonym(arg1);
			return true;
		} else {
			pair<vector<int>, vector<int>> callsPair = evaluateNextByLHS(arg1, arg2, isTrans);

			if (callsPair.first.size() == 0 || callsPair.second.size() == 0) {
				return false;
			}

			arg1.setValues(callsPair.first);
			arg2.setValues(callsPair.second);
			IntermediateValuesHandler::addAndProcessIntermediateSynonyms(arg1, arg2);
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

	bool processAffectsT(Synonym arg1, Synonym arg2, bool isTrans)
	{
		return false;
		/*if (AbstractWrapper::GlobalStop) {
		return false;
		}

		SYNONYM_TYPE arg1Type = arg1.getType();
		SYNONYM_TYPE arg2Type = arg2.getType();

		if (arg1Type == STRING && arg2Type == STRING) {
		return pkb.isCalls(pkb.getProcIndex(arg1.getName()), pkb.getProcIndex(arg2.getName()), isTrans);
		} else if (arg1Type == STRING) {
		vector<int> stmt = pkb.getProcsCalledBy(pkb.getProcIndex(arg1.getName()), isTrans);
		if (stmt.size() == 0) {
		return false;
		}
		Synonym synonym(arg2Type, arg2.getName(), stmt);
		IntermediateValuesHandler::addAndProcessIntermediateSynonym(synonym);
		} else if (arg2Type == STRING) {
		vector<int> stmt = pkb.getProcsCalling(pkb.getProcIndex(arg2.getName()), isTrans);
		if (stmt.size() == 0) {
		return false;
		}
		Synonym synonym(arg1Type, arg1.getName(), stmt);
		IntermediateValuesHandler::addAndProcessIntermediateSynonym(synonym);
		} else if (arg1Type == UNDEFINED && arg2Type == UNDEFINED) {
		return true;
		} else if (arg1Type == UNDEFINED) {
		Synonym RHS(arg2Type, arg2.getName(), pkb.getCallsRhs());
		IntermediateValuesHandler::addAndProcessIntermediateSynonym(RHS);
		return true;
		} else if (arg2Type == UNDEFINED) {
		Synonym LHS(arg1Type, arg1.getName(), pkb.getCallsLhs());
		IntermediateValuesHandler::addAndProcessIntermediateSynonym(LHS);
		return true;
		} else {
		pair<vector<int>, vector<int>> callsPair = evaluateAffectsByLHS(arg1, arg2, isTrans);

		if (callsPair.first.size() == 0 || callsPair.second.size() == 0) {
		return false;
		}

		Synonym LHS(arg1Type, arg1.getName(), callsPair.first);
		Synonym RHS(arg2Type, arg2.getName(), callsPair.second);
		IntermediateValuesHandler::addAndProcessIntermediateSynonyms(LHS, RHS);
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
		Synonym arg1 = patternClause->getArg1();
		Synonym arg2 = patternClause->getArg2();
		SYNONYM_TYPE patternType = arg0.getType();

		switch (patternType) {
		case ASSIGN:
			return processAssignPattern(arg0, arg1, arg2);
		case IF:
			return processIfPattern(arg0, arg1, arg2);
		case WHILE:
			return processWhilePattern(arg0, arg1, arg2);
		default:
			return false;
		}
	}

	/**
	* Method to process assign patterns
	* Returns true if the pattern clause is valid, false otherwise
	*/
	bool processAssignPattern(Synonym arg0, Synonym arg1, Synonym arg2) 
	{
		if (AbstractWrapper::GlobalStop) {
			return false;
		}

		SYNONYM_TYPE arg1Type = arg1.getType();
		vector<int> isMatchStmts = pkb.patternMatchAssign(arg2.getName());

		if (isMatchStmts.size() == 0) {
			return false;
		}

		if (arg1Type == VARIABLE) {
			//If LHS is a variable synonym, use the return statements to probe the ModifiesTable
			vector<int> vars;

			for (vector<int>::iterator itr = isMatchStmts.begin(); itr != isMatchStmts.end(); ++itr) {
				vector<int> var = pkb.getModVarForStmt(*itr);  //Get the LHS of the assign statement
				vars.push_back(var[0]);  //Variable for assignment stmts must be in index 0
			}

			arg0.setValues(isMatchStmts);
			arg1.setValues(vars);
			IntermediateValuesHandler::addAndProcessIntermediateSynonyms(arg0, arg1);
			return true;
		} else if (arg1Type == UNDEFINED) {
			arg0.setValues(isMatchStmts);
			IntermediateValuesHandler::addAndProcessIntermediateSynonym(arg0);
			return true;
		} else {
			//LHS is a constant
			string LHS = arg1.getName();
			vector<int> matchingStmts;

			//Check that the assignment statements modifies the LHS
			for (vector<int>::iterator itr = isMatchStmts.begin(); itr != isMatchStmts.end(); ++itr) {
				int stmt = *itr;
				vector<int> vars = pkb.getModVarForStmt(stmt);
				string var = pkb.getVarName(vars[0]);  //Variable for assignment stmts must be in index 0
				if (var == LHS) {
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
	bool processIfPattern(Synonym arg0, Synonym arg1, Synonym arg2) 
	{
		if (AbstractWrapper::GlobalStop) {
			return false;
		}

		SYNONYM_TYPE arg1Type = arg1.getType();

		//Find all if statements that uses LHS
		if (arg1.getType() == STRING) {
			vector<int> stmts = pkb.patternMatchIf(arg1.getName());
			if (stmts.size() == 0) {
				return false;
			}
			arg0.setValues(stmts);
			IntermediateValuesHandler::addAndProcessIntermediateSynonym(arg0);
			return true;
		} else if (arg1Type == UNDEFINED) {
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
			arg1.setValues(vars);
			IntermediateValuesHandler::addAndProcessIntermediateSynonyms(arg0, arg1);
			return true;
		}
	}

	/**
	* Method to process while patterns
	* Returns true if the pattern clause is valid, false otherwise
	*/
	bool processWhilePattern(Synonym arg0, Synonym arg1, Synonym arg2) 
	{
		if (AbstractWrapper::GlobalStop) {
			return false;
		}

		SYNONYM_TYPE arg1Type = arg1.getType();

		//Find all while statements that uses LHS
		if (arg1.getType() == STRING) {
			vector<int> stmts = pkb.patternMatchWhile(arg1.getName());
			if (stmts.size() == 0) {
				return false;
			}
			arg0.setValues(stmts);
			IntermediateValuesHandler::addAndProcessIntermediateSynonym(arg0);
			return true;
		} else if (arg1Type == UNDEFINED) {
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
			arg1.setValues(vars);
			IntermediateValuesHandler::addAndProcessIntermediateSynonyms(arg0, arg1);
			return true;
		}
	}

	bool processWithClause(QNode* withClause)
	{
		//Assume that var and int cannot be compared (should be checked by the query validator)
		Synonym arg1 = withClause->getArg1();
		Synonym arg2 = withClause->getArg2();
		SYNONYM_TYPE arg1Type = arg1.getType();
		SYNONYM_TYPE arg2Type = arg2.getType();

		if (arg1Type == STRING && arg2Type == STRING) {
			string arg1Value = arg1.getName();
			string arg2Value = arg2.getName();
			return arg1Value == arg2Value;
		} else if (arg1Type == STRING) {
			string arg1Value = arg1.getName();
			return IntermediateValuesHandler::filterEqualValue(arg2, arg1Value);
		} else if (arg2Type == STRING) {
			string arg2Value = arg2.getName();
			return IntermediateValuesHandler::filterEqualValue(arg1, arg2Value);
		} else {
			return IntermediateValuesHandler::filterEqualPair(arg1, arg2);
		}
	}
}
