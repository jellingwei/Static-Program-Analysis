//@TODO: What if no while loops and query asks for while
//@TODO: if patterns
//@TODO: Use one side to probe instead of finding all pairs
//@TODO: For pairs, have to consider having "_" as the parameters
//@TODO: Special handling for relation(_, _) if this does not exist
//@TODO: With clauses for v.varName = p.procName

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
	bool processSuchThatNode(QNode* suchThatNode);
	bool processPatternNode(QNode* patternNode);
	bool processWithNode(QNode* withNode);

	//Functions to process clauses
	inline bool processSuchThatClause(QNode* clauseNode);
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

	//Functions to process pattern clauses
	bool processPatternClause(QNode* patternClause);
	bool processAssignPattern(Synonym arg0, Synonym arg1, Synonym arg2);
	bool processWhilePattern(Synonym arg0, Synonym arg1, Synonym arg2);

	//Functions to process with clauses
	bool processWithClause(QNode* withClause);

	pair<vector<int>, vector<int>> filterPairWithSynonymType(pair<vector<int>, vector<int>> allPairs,
		SYNONYM_TYPE arg1Type, SYNONYM_TYPE arg2Type);

	PKB pkb = PKB::getInstance();  //Get the instance of the PKB singleton

	/**
	* Processes the query tree given a query tree node.
	* Return an empty vector if the Such That or Pattern clauses are invalid.
	*/
	vector<Synonym> processQueryTree(QueryTree* qTreeRoot) 
	{
		IntermediateValuesHandler::initialize(qTreeRoot->getSynonymsMap());
		vector<Synonym> synonymResult;

		bool isValid = processSuchThatNode(qTreeRoot->getSuchThatNode());

		if (isValid) {
			isValid = processPatternNode(qTreeRoot->getPatternNode());
		}

		if (isValid) {
			isValid = processWithNode(qTreeRoot->getWithNode());
		}

		//All clauses are valid
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
	bool processSuchThatNode(QNode* suchThatNode) 
	{
		int numberOfClauses = suchThatNode->getNumberOfChildren();
		QNode* clauseNode = suchThatNode->getChild();

		for (int i = 0; i < numberOfClauses; i++) {
			bool isValid = processSuchThatClause(clauseNode);

			if (!isValid) {
				return false;
			} else {
				clauseNode = suchThatNode->getNextChild();
			}
		}
		return true;
	}

	/**
	* Method to processes each such that clause from the such that node
	* Returns true if a clause is valid, false otherwise
	*/
	inline bool processSuchThatClause(QNode* clauseNode) 
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
		case NextS:
		case Affects:
		case AffectsS:
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
			Synonym synonym(arg2Type, arg2.getName(), stmts);
			IntermediateValuesHandler::addAndProcessIntermediateSynonym(synonym);
		} else if (arg2Type == STRING) {
			//arg2 is the variable that is modified, find the statements
			vector<int> stmts = pkb.getModStmtNum(pkb.getVarIndex(arg2.getName()));
			if (stmts.size() == 0) {
				return false;
			}
			Synonym synonym(arg1Type, arg1.getName(), stmts);
			IntermediateValuesHandler::addAndProcessIntermediateSynonym(synonym);
		} else if (arg1Type == UNDEFINED && arg2Type == UNDEFINED) {
			return true;
		} else if (arg1Type == UNDEFINED) {
			Synonym RHS(arg2Type, arg2.getName(), pkb.getModifiesRhs());
			IntermediateValuesHandler::addAndProcessIntermediateSynonym(RHS);
			return true;
		} else if (arg2Type == UNDEFINED) {
			Synonym LHS(arg1Type, arg1.getName(), pkb.getModifiesLhs());
			IntermediateValuesHandler::addAndProcessIntermediateSynonym(LHS);
			return true;
		} else {
			//Use LHS temporarily
			pair<vector<int>, vector<int>> modifiesPair = evaluateModifiesByLHS(arg1, arg2);

			if (modifiesPair.first.size() == 0 || modifiesPair.second.size() == 0) {
				return false;
			}

			Synonym LHS(arg1Type, arg1.getName(), modifiesPair.first);
			Synonym RHS(arg2Type, arg2.getName(), modifiesPair.second);
			IntermediateValuesHandler::addAndProcessIntermediateSynonyms(LHS, RHS);
		}
		return true;
	}
	
	pair<vector<int>, vector<int>> evaluateModifiesByLHS(Synonym LHS, Synonym RHS)
	{
		vector<int> valuesLHS = IntermediateValuesHandler::getSynonymWithName(LHS.getName()).getValues();
		set<int> valuesRHS = IntermediateValuesHandler::getSynonymWithName(RHS.getName()).getValuesSet();
		vector<int> acceptedLHS;
		vector<int> acceptedRHS;

		for (unsigned int i = 0; i < valuesLHS.size(); i++) {
			vector<int> stmts = pkb.getModVarForStmt(valuesLHS[i]);

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
			Synonym synonym(arg2Type, arg2.getName(), vars);
			IntermediateValuesHandler::addAndProcessIntermediateSynonym(synonym);
		} else if (arg2Type == STRING) {
			//arg2 is the variable that is used, find the statements that uses it
			vector<int> stmts = pkb.getUsesStmtNum(pkb.getVarIndex(arg2.getName()));
			if (stmts.size() == 0) {
				return false;
			}
			Synonym synonym(arg1Type, arg1.getName(), stmts);
			IntermediateValuesHandler::addAndProcessIntermediateSynonym(synonym);
		} else if (arg1Type == UNDEFINED && arg2Type != UNDEFINED) {
			//@todo Try to find variables that have been modified
			return true;
		} else if (arg1Type == UNDEFINED && arg2Type == UNDEFINED) {
			return true;
		} else if (arg1Type == UNDEFINED) {
			Synonym RHS(arg2Type, arg2.getName(), pkb.getUsesRhs());
			IntermediateValuesHandler::addAndProcessIntermediateSynonym(RHS);
			return true;
		} else if (arg2Type == UNDEFINED) {
			Synonym LHS(arg1Type, arg1.getName(), pkb.getUsesLhs());
			IntermediateValuesHandler::addAndProcessIntermediateSynonym(LHS);
			return true;
		} else {
			//Use LHS temporarily
			pair<vector<int>, vector<int>> usesPair = evaluateUsesByLHS(arg1, arg2);

			if (usesPair.first.size() == 0 || usesPair.second.size() == 0) {
				return false;
			}

			Synonym LHS(arg1Type, arg1.getName(), usesPair.first);
			Synonym RHS(arg2Type, arg2.getName(), usesPair.second);
			IntermediateValuesHandler::addAndProcessIntermediateSynonyms(LHS, RHS);
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
			Synonym synonym(arg2Type, arg2.getName(), stmts);
			IntermediateValuesHandler::addAndProcessIntermediateSynonym(synonym);
		} else if (arg2Type == STRING) {
			vector<int> stmts = pkb.getParent(stoi(arg2.getName()), isTrans);
			if (stmts.size() == 0) {
				return false;
			}
			Synonym synonym(arg1Type, arg1.getName(), stmts);
			IntermediateValuesHandler::addAndProcessIntermediateSynonym(synonym);
		} else if (arg1Type == UNDEFINED && arg2Type == UNDEFINED) {
			return true;
		} else if (arg1Type == UNDEFINED) {
			Synonym RHS(arg2Type, arg2.getName(), pkb.getParentRhs());
			IntermediateValuesHandler::addAndProcessIntermediateSynonym(RHS);
			return true;
		} else if (arg2Type == UNDEFINED) {
			Synonym LHS(arg1Type, arg1.getName(), pkb.getParentLhs());
			IntermediateValuesHandler::addAndProcessIntermediateSynonym(LHS);
			return true;
		} else {
			pair<vector<int>, vector<int>> parentsPair = evaluateParentByLHS(arg1, arg2, isTrans);

			if (parentsPair.first.size() == 0 || parentsPair.second.size() == 0) {
				return false;
			}

			Synonym LHS(arg1Type, arg1.getName(), parentsPair.first);
			Synonym RHS(arg2Type, arg2.getName(), parentsPair.second);
			IntermediateValuesHandler::addAndProcessIntermediateSynonyms(LHS, RHS);
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
			Synonym synonym(arg2Type, arg2.getName(), stmt);
			IntermediateValuesHandler::addAndProcessIntermediateSynonym(synonym);
		} else if (arg2Type == STRING) {
			// Given stmtNum2, get stmtNum1 such that Follows(stmt1, stmt2) is satisfied
			vector<int> stmt = pkb.getStmtFollowedTo(stoi(arg2.getName()), isTrans);
			if (stmt.size() == 0) {
				return false;
			}
			Synonym synonym(arg1Type, arg1.getName(), stmt);
			IntermediateValuesHandler::addAndProcessIntermediateSynonym(synonym);
		} else if (arg1Type == UNDEFINED && arg2Type == UNDEFINED) {
			return true;
		} else if (arg1Type == UNDEFINED) {
			Synonym RHS(arg2Type, arg2.getName(), pkb.getFollowsRhs());
			IntermediateValuesHandler::addAndProcessIntermediateSynonym(RHS);
			return true;
		} else if (arg2Type == UNDEFINED) {
			Synonym LHS(arg1Type, arg1.getName(), pkb.getFollowsLhs());
			IntermediateValuesHandler::addAndProcessIntermediateSynonym(LHS);
			return true;
		} else {
			pair<vector<int>, vector<int>> followsPair = evaluateFollowsByLHS(arg1, arg2, isTrans);

			if (followsPair.first.size() == 0 || followsPair.second.size() == 0) {
				return false;
			}

			Synonym LHS(arg1Type, arg1.getName(), followsPair.first);
			Synonym RHS(arg2Type, arg2.getName(), followsPair.second);
			IntermediateValuesHandler::addAndProcessIntermediateSynonyms(LHS, RHS);
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
			pair<vector<int>, vector<int>> callsPair = evaluateCallsByLHS(arg1, arg2, isTrans);

			if (callsPair.first.size() == 0 || callsPair.second.size() == 0) {
				return false;
			}

			Synonym LHS(arg1Type, arg1.getName(), callsPair.first);
			Synonym RHS(arg2Type, arg2.getName(), callsPair.second);
			IntermediateValuesHandler::addAndProcessIntermediateSynonyms(LHS, RHS);
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

	/**
	* Method to process the pattern node in the query tree
	* Returns true if all the patterns are valid or there are no patterns to match
	* Returns false if there is one pattern that is invalid
	*/
	bool processPatternNode(QNode* patternNode) 
	{
		int numberOfPatterns = patternNode->getNumberOfChildren();
		QNode* patternClause = patternNode->getChild();

		for (int i = 0; i < numberOfPatterns; i++) {
			bool isValid = processPatternClause(patternClause);

			if (!isValid) {
				return false;
			} else {
				patternClause = patternNode->getNextChild();
			}
		}
		//There are no patterns to match or patterns are valid
		return true;
	}

	/**
	* Method to process individual pattern clauses
	* Returns true if the pattern clause is valid, false otherwise
	*/
	bool processPatternClause(QNode* patternClause) 
	{
		Synonym arg0 = patternClause->getArg0();
		Synonym arg1 = patternClause->getArg1();
		Synonym arg2 = patternClause->getArg2();
		SYNONYM_TYPE patternType = arg0.getType();

		switch (patternType) {
		case ASSIGN:
			return processAssignPattern(arg0, arg1, arg2);
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

			Synonym LHS(arg0.getType(), arg0.getName(), isMatchStmts);
			Synonym RHS(arg1Type, arg1.getName(), vars);
			IntermediateValuesHandler::addAndProcessIntermediateSynonyms(LHS, RHS);
			return true;
		} else if (arg1Type == UNDEFINED) {
			Synonym synonym(arg0.getType(), arg0.getName(), isMatchStmts);
			IntermediateValuesHandler::addAndProcessIntermediateSynonym(synonym);
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
			Synonym synonym(arg0.getType(), arg0.getName(), matchingStmts);
			IntermediateValuesHandler::addAndProcessIntermediateSynonym(synonym);
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
			Synonym synonym(arg0.getType(), arg0.getName(), stmts);
			if (stmts.size() == 0) {
				return false;
			}
			IntermediateValuesHandler::addAndProcessIntermediateSynonym(synonym);
			return true;
		} else if (arg1Type == UNDEFINED) {
			return true;  //Do nothing because pattern w(_, _) is always true if there are while statements
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
			Synonym LHS(arg0.getType(), arg0.getName(), arg0Values);
			Synonym RHS(arg1.getType(), arg1.getName(), vars);
			IntermediateValuesHandler::addAndProcessIntermediateSynonyms(LHS, RHS);
			return true;
		}
	}

	bool processWithNode(QNode* withNode) 
	{
		int numberOfWith = withNode->getNumberOfChildren();
		QNode* withClause = withNode->getChild();

		for (int i = 0; i < numberOfWith; i++) {
			bool isValid = processWithClause(withClause);

			if (!isValid) {
				return false;
			} else {
				withClause = withNode->getNextChild();
			}
		}
		//There are no with nodes or they are valid
		return true;
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
