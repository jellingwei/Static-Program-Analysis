//TODO: What if no while loops and query asks for while
//TODO: With clauses
//TODO: if patterns
//TODO: Use one side to probe instead of finding all pairs
//TODO: Calls table for calls relation

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

#include "QueryEvaluator.h"
#include "PKB.h"
#include "IntermediateValuesHandler.h"
#include "AbstractWrapper.h"

namespace QueryEvaluator 
{
	//Private functions to evaluate the query tree
	vector<Synonym> processResultNode(QNode* resultNode);
	bool processSuchThatNode(QNode* suchThatNode);
	bool processPatternNode(QNode* patternNode);
	bool processWithNode(QNode* withNode);

	//Functions to process clauses
	bool processSuchThatClause(QNode* clauseNode);
	bool processModifies(Synonym arg1, Synonym arg2);
	bool processUses(Synonym arg1, Synonym arg2);
	bool processParent(Synonym arg1, Synonym arg2);
	bool processParentS(Synonym arg1, Synonym arg2);
	bool processFollows(Synonym arg1, Synonym arg2);
	bool processFollowsS(Synonym arg1, Synonym arg2);
	bool processCalls(Synonym arg1, Synonym arg2);
	bool processCallsS(Synonym arg1, Synonym arg2);

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
	* @todo A more efficient way of organizing this function
	*/
	vector<Synonym> processQueryTree(QueryTree* qTreeRoot) 
	{
		IntermediateValuesHandler::initialize(qTreeRoot->getSynonymsMap());
		vector<Synonym> synonymResult;

		bool isValid = processSuchThatNode(qTreeRoot->getSuchThatNode());
		if (!isValid) {
			return synonymResult;  //Return empty vector
		}

		isValid = processPatternNode(qTreeRoot->getPatternNode());
		if (!isValid) {
			return synonymResult;  //Return empty vector
		}

		isValid = true;  //processWithNode(qTreeRoot->getWithNode());
		if (!isValid) {
			return synonymResult;  //Return empty vector
		}

		//All clauses are valid
		synonymResult = processResultNode(qTreeRoot->getResultNode());
		return synonymResult;
	}

	/**
	* Processes the result node in the query tree
	* Returns the wanted synonym in a vector
	*/
	vector<Synonym> processResultNode(QNode* resultNode) 
	{
		vector<Synonym> result;

		QNode* resultChildNode = resultNode->getChild();
		int numberOfSynonyms = resultNode->getNumberOfChildren();

		for (int i = 0; i < numberOfSynonyms; i++) {
			Synonym wantedSynonym = resultChildNode->getArg1();

			if (wantedSynonym.getType() == BOOLEAN) {
				Synonym s(BOOLEAN, "true");
				result.push_back(s);
				return result;
			}

			string wantedSynonymName = wantedSynonym.getName();
			Synonym s = IntermediateValuesHandler::getSynonymWithName(wantedSynonymName);
			if (s.getName() == "-1") {
				break;  //It should never come here
			}
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
	bool processSuchThatClause(QNode* clauseNode) 
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
			return processParent(arg1, arg2);
		case ParentS:
			return processParentS(arg1, arg2);
		case Follows:
			return processFollows(arg1, arg2);
		case FollowsS:
			return processFollowsS(arg1, arg2);
		case Calls:
			//return processCalls(arg1, arg2);
		case CallsS:
			//return processCallsS(arg1, arg2);
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
		} else {
			pair<vector<int>, vector<int>> allModifiesPair = pkb.getAllModPair();
			pair<vector<int>, vector<int>> filteredModifiesPair = 
				filterPairWithSynonymType(allModifiesPair, arg1Type, arg2Type);

			if (filteredModifiesPair.first.size() == 0 || filteredModifiesPair.second.size() == 0) {
				return false;
			}

			Synonym LHS(arg1Type, arg1.getName(), filteredModifiesPair.first);
			Synonym RHS(arg2Type, arg2.getName(), filteredModifiesPair.second);
			IntermediateValuesHandler::addAndProcessIntermediateSynonyms(LHS, RHS);
		}
		return true;
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
		} else {
			pair<vector<int>, vector<int>> allUsesPair = pkb.getAllUsesPair();
			pair<vector<int>, vector<int>> filteredUsesPair = 
				filterPairWithSynonymType(allUsesPair, arg1Type, arg2Type);

			if (filteredUsesPair.first.size() == 0 || filteredUsesPair.second.size() == 0) {
				return false;
			}

			Synonym LHS(arg1Type, arg1.getName(), filteredUsesPair.first);
			Synonym RHS(arg2Type, arg2.getName(), filteredUsesPair.second);
			IntermediateValuesHandler::addAndProcessIntermediateSynonyms(LHS, RHS);
		}
		return true;
	}

	/**
	* Method to processes the parent clause
	* Returns true if this clause is valid, false otherwise
	*/
	bool processParent(Synonym arg1, Synonym arg2) 
	{
		if (AbstractWrapper::GlobalStop) {
			return false;
		}

		SYNONYM_TYPE arg1Type = arg1.getType();
		SYNONYM_TYPE arg2Type = arg2.getType();

		if (arg1Type == STRING && arg2Type == STRING) {
			return pkb.isParent(stoi(arg1.getName()), stoi(arg2.getName()));
		} else if (arg1Type == STRING) {
			vector<int> stmts = pkb.getChild(stoi(arg1.getName()));
			if (stmts.size() == 0) {
				return false;
			}
			Synonym synonym(arg2Type, arg2.getName(), stmts);
			IntermediateValuesHandler::addAndProcessIntermediateSynonym(synonym);
		} else if (arg2Type == STRING) {
			vector<int> stmts = pkb.getParent(stoi(arg2.getName()));
			if (stmts.size() == 0) {
				return false;
			}
			Synonym synonym(arg1Type, arg1.getName(), stmts);
			IntermediateValuesHandler::addAndProcessIntermediateSynonym(synonym);
		} else {
			pair<vector<int>, vector<int>> allParentsPair = pkb.getAllParentPairs(false);  //false for non transitive closure
			pair<vector<int>, vector<int>> filteredParentsPair = 
				filterPairWithSynonymType(allParentsPair, arg1Type, arg2Type);

			if (filteredParentsPair.first.size() == 0 || filteredParentsPair.second.size() == 0) {
				return false;
			}

			Synonym LHS(arg1Type, arg1.getName(), filteredParentsPair.first);
			Synonym RHS(arg2Type, arg2.getName(), filteredParentsPair.second);
			IntermediateValuesHandler::addAndProcessIntermediateSynonyms(LHS, RHS);
		}
		return true;
	}

	/**
	* Method to processes the parent clause
	* Returns true if this clause is valid, false otherwise
	*/
	bool processParentS(Synonym arg1, Synonym arg2) 
	{
		if (AbstractWrapper::GlobalStop) {
			return false;
		}

		SYNONYM_TYPE arg1Type = arg1.getType();
		SYNONYM_TYPE arg2Type = arg2.getType();

		if (arg1Type == STRING && arg2Type == STRING) {
			return pkb.isParent(stoi(arg1.getName()), stoi(arg2.getName()), true);  //True for transitive closure
		} else if (arg1Type == STRING) {
			vector<int> stmts = pkb.getChild(stoi(arg1.getName()), true);  //True for transitive closure
			if (stmts.size() == 0) {
				return false;
			}
			Synonym synonym(arg2Type, arg2.getName(), stmts);
			IntermediateValuesHandler::addAndProcessIntermediateSynonym(synonym);
		} else if (arg2Type == STRING) {
			vector<int> stmts = pkb.getParent(stoi(arg2.getName()), true);  //True for transitive closure
			if (stmts.size() == 0) {
				return false;
			}
			Synonym synonym(arg1Type, arg1.getName(), stmts);
			IntermediateValuesHandler::addAndProcessIntermediateSynonym(synonym);
		} else {
			pair<vector<int>, vector<int>> allParentsPair = pkb.getAllParentPairs(true);  //True for transitive closure
			pair<vector<int>, vector<int>> filteredParentsPair = 
				filterPairWithSynonymType(allParentsPair, arg1Type, arg2Type);

			if (filteredParentsPair.first.size() == 0 || filteredParentsPair.second.size() == 0) {
				return false;
			}

			Synonym LHS(arg1Type, arg1.getName(), filteredParentsPair.first);
			Synonym RHS(arg2Type, arg2.getName(), filteredParentsPair.second);
			IntermediateValuesHandler::addAndProcessIntermediateSynonyms(LHS, RHS);
		}
		return true;
	}

	/**
	* Method to processes the follows clause
	* Returns true if this clause is valid, false otherwise
	*/
	bool processFollows(Synonym arg1, Synonym arg2) 
	{
		if (AbstractWrapper::GlobalStop) {
			return false;
		}

		SYNONYM_TYPE arg1Type = arg1.getType();
		SYNONYM_TYPE arg2Type = arg2.getType();

		if (arg1Type == STRING && arg2Type == STRING){
			return pkb.isFollows(stoi(arg1.getName()), stoi(arg2.getName()));
		} else if (arg1Type == STRING) {
			// Given stmtNum1, get stmtNum2 such that Follows(stmt1, stmt2) is satisfied
			vector<int> stmt = pkb.getStmtFollowedFrom(stoi(arg1.getName()));
			if (stmt.size() == 0) {
				return false;
			}
			Synonym synonym(arg2Type, arg2.getName(), stmt);
			IntermediateValuesHandler::addAndProcessIntermediateSynonym(synonym);
		} else if (arg2Type == STRING) {
			// Given stmtNum2, get stmtNum1 such that Follows(stmt1, stmt2) is satisfied
			vector<int> stmt = pkb.getStmtFollowedTo(stoi(arg2.getName()));
			if (stmt.size() == 0) {
				return false;
			}
			Synonym synonym(arg1Type, arg1.getName(), stmt);
			IntermediateValuesHandler::addAndProcessIntermediateSynonym(synonym);
		} else {
			pair<vector<int>, vector<int>> allFollowsPair = pkb.getAllFollowsPairs(false);  //false for non transitive closure
			pair<vector<int>, vector<int>> filteredFollowsPair = 
				filterPairWithSynonymType(allFollowsPair, arg1Type, arg2Type);

			if (filteredFollowsPair.first.size() == 0 || filteredFollowsPair.second.size() == 0) {
				return false;
			}

			Synonym LHS(arg1Type, arg1.getName(), filteredFollowsPair.first);
			Synonym RHS(arg2Type, arg2.getName(), filteredFollowsPair.second);
			IntermediateValuesHandler::addAndProcessIntermediateSynonyms(LHS, RHS);
		}
		return true;
	}

	bool processFollowsS(Synonym arg1, Synonym arg2) 
	{
		if (AbstractWrapper::GlobalStop) {
			return false;
		}

		SYNONYM_TYPE arg1Type = arg1.getType();
		SYNONYM_TYPE arg2Type = arg2.getType();

		if (arg1Type == STRING && arg2Type == STRING) {
			return pkb.isFollows(stoi(arg1.getName()), stoi(arg2.getName()), true);
		} else if (arg1Type == STRING) {
			// Given stmtNum1, get stmtNum2 such that Follows(stmt1, stmt2) is satisfied
			vector<int> stmt = pkb.getStmtFollowedFrom(stoi(arg1.getName()), true);
			if (stmt.size() == 0) {
				return false;
			}
			Synonym synonym(arg2Type, arg2.getName(), stmt);
			IntermediateValuesHandler::addAndProcessIntermediateSynonym(synonym);
		} else if (arg2Type == STRING) {
			// Given stmtNum2, get stmtNum1 such that Follows(stmt1, stmt2) is satisfied
			vector<int> stmt = pkb.getStmtFollowedTo(stoi(arg2.getName()), true);
			if (stmt.size() == 0) {
				return false;
			}
			Synonym synonym(arg1Type, arg1.getName(), stmt);
			IntermediateValuesHandler::addAndProcessIntermediateSynonym(synonym);
		} else {
			pair<vector<int>, vector<int>> allFollowsPair = pkb.getAllFollowsPairs(true);  //True for transitive closure
			pair<vector<int>, vector<int>> filteredFollowsPair = 
				filterPairWithSynonymType(allFollowsPair, arg1Type, arg2Type);

			if (filteredFollowsPair.first.size() == 0 || filteredFollowsPair.second.size() == 0) {
				return false;
			}

			Synonym LHS(arg1Type, arg1.getName(), filteredFollowsPair.first);
			Synonym RHS(arg2Type, arg2.getName(), filteredFollowsPair.second);
			IntermediateValuesHandler::addAndProcessIntermediateSynonyms(LHS, RHS);
		}
		return true;
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

	/**
	* Helper method to filter the paired synonym values
	* Because the getAllPairs function from the various tables does not distinguish between the types,
	* this function filters only the requested types from all the pairs
	* Returns the filtered pair of values
	*/
	pair<vector<int>, vector<int>> filterPairWithSynonymType(pair<vector<int>, vector<int>> allPairs, 
		SYNONYM_TYPE arg1Type, SYNONYM_TYPE arg2Type)
	{
		vector<int> filteredFirstElements;
		vector<int> filteredSecondElements;

		//Just have to loop for one of the pairs since the number of pairs must be the same
		for (unsigned int i = 0; i < allPairs.first.size(); i++) {
			int firstElement = allPairs.first[i];
			int secondElement = allPairs.second[i];

			if (arg1Type == STMT || arg1Type == PROG_LINE || arg1Type == UNDEFINED) {
				if (arg2Type == STMT || arg2Type == PROG_LINE || arg2Type == VARIABLE || arg2Type == UNDEFINED) {
					return allPairs;
				} else if (arg2Type == CONSTANT && pkb.isConstant(secondElement)) {
					//The constant table has been probed and arg2 constant value exists
					filteredFirstElements.push_back(firstElement);
					filteredSecondElements.push_back(secondElement);
				} else if (arg2Type == Synonym::convertToEnum(pkb.getType(secondElement))) {
					filteredFirstElements.push_back(firstElement);
					filteredSecondElements.push_back(secondElement);
				}
			} else if (arg1Type == CONSTANT && pkb.isConstant(firstElement)) {
				//The constant table has been probed and arg1 constant value exists
				if (arg2Type == STMT || arg2Type == PROG_LINE || arg2Type == VARIABLE || arg2Type == UNDEFINED) {
					filteredFirstElements.push_back(firstElement);
					filteredSecondElements.push_back(secondElement);
				} else if (arg2Type == CONSTANT && pkb.isConstant(secondElement)) {
					//The constant table has been probed and arg2 constant value exists
					filteredFirstElements.push_back(firstElement);
					filteredSecondElements.push_back(secondElement);
				} else if (arg2Type == Synonym::convertToEnum(pkb.getType(secondElement))) {
					filteredFirstElements.push_back(firstElement);
					filteredSecondElements.push_back(secondElement);
				}
			} else if (arg1Type == Synonym::convertToEnum(pkb.getType(firstElement))) {
				if (arg2Type == STMT || arg2Type == PROG_LINE || arg2Type == VARIABLE || arg2Type == UNDEFINED) {
					filteredFirstElements.push_back(firstElement);
					filteredSecondElements.push_back(secondElement);
				} else if (arg2Type == CONSTANT && pkb.isConstant(secondElement)) {
					//The constant table has been probed and arg2 constant value exists
					filteredFirstElements.push_back(firstElement);
					filteredSecondElements.push_back(secondElement);
				} else if (arg2Type == Synonym::convertToEnum(pkb.getType(secondElement))) {
					filteredFirstElements.push_back(firstElement);
					filteredSecondElements.push_back(secondElement);
				}
			}
		}

		pair<vector<int>, vector<int>> filteredPairs(filteredFirstElements, filteredSecondElements);
		return filteredPairs;
	}

	/*bool processWithNode(QNode* withNode) 
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
	}*/

	//@todo call.procName and call.stmtNo
	//@todo with clauses for constant strings
	/*bool processWithClause(QNode* withClause)
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
	IntermediateValuesHandler::filterEqualValue(arg2, arg1Value);
	} else if (arg2Type == STRING) {
	string arg2Value = arg2.getName();
	IntermediateValuesHandler::filterEqualValue(arg1, arg2Value);
	} else {
	IntermediateValuesHandler::filterEqualPair(arg1, arg2);
	}
	return true;
	}*/
}