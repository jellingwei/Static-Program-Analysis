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
#include "AbstractWrapper.h"

namespace QueryEvaluator {

	//Private functions to evaluate the query tree
	vector<Synonym> processResultNode(QNode* resultNode);
	bool processSuchThatNode(QNode* suchThatNode);
	bool processPatternNode(QNode* patternNode);
	bool processSuchThatClause(QNode* clauseNode);

	bool processModifies(Synonym arg1, Synonym arg2);
	bool processUses(Synonym arg1, Synonym arg2);
	bool processParent(Synonym arg1, Synonym arg2);
	bool processParentS(Synonym arg1, Synonym arg2);
	bool evaluateParentS(Synonym arg1, Synonym arg2);
	bool processFollows(Synonym arg1, Synonym arg2);
	bool processFollowsS(Synonym arg1, Synonym arg2);
	bool evaluateFollowsS(Synonym arg1, Synonym arg2);

	bool processPatternClause(QNode* patternClause);
	bool processAssignPattern(Synonym arg0, Synonym arg1, Synonym arg2);
	bool processWhilePattern(Synonym arg0, Synonym arg1, Synonym arg2);

	bool validateModifiesOrUsesArgs(Synonym arg1, Synonym arg2);  //Validates the inputs for modifies and uses
	bool validateParentArgs(Synonym arg1, Synonym arg2);  //Validates the inputs for parent
	bool validateFollowsArgs(Synonym arg1, Synonym arg2);  //Validates the inputs for follows
	bool validateAssignPatternArgs(Synonym arg0, Synonym arg1, Synonym arg2);  //Validates the input for assign pattern
	bool validateWhilePatternArgs(Synonym arg0, Synonym arg1, Synonym arg2);  //Validates the input for while patterns

	//Private helper functions
	void initializeSynonymVectorUsingMap(unordered_map<string,string> synonymMap);  //Initializes all the synonyms declared
	bool filterResults(Synonym s, vector<int> newValues);  //Do an intersection of the current synonym with the new values
	void filterDependentValues(Synonym synonym);  //A function to filter dependent values
	void filterTwinDependentValues();  //A function to filter dependent values that are twinned
	void swapOldSynonymWithNewSynonym(Synonym newSynonym);  //Helper function for filterResults
	void addResultsToTemp(vector<int> results);  //Used to add the results of * relations into tempVector above
	pair<vector<int>, vector<int>> filterPairWithSynonymType(pair<vector<int>, vector<int>> allPairs, string arg1Type, string arg2Type);
	Synonym findSynonymWithName(string name);  //Return the synonym from synonym vector that has this name

	//Private variables that can be used in any function in the namespace
	vector<Synonym> synonymVector;  //Synonym vector that is used to store the synonyms with their values
	vector<int> tempVector;  //A temp vector to store the intermediate values of * queries
	vector<pair<vector<int>, vector<int>>> synonymValuesPair;  //A vector to store paired synonym values
	vector<pair<string, string>> synonymNamesPair;  //A vector to store the names paired synonyms in queries

	PKB pkb = PKB::getInstance();

	/**
	* Processes the query tree given a query tree node
	* Returns an empty vector if the Such That or Pattern clauses are invalid
	*/
	vector<Synonym> processQueryTree(QueryTree* qTreeRoot) {
		synonymVector.clear();
		tempVector.clear();
		synonymValuesPair.clear();
		synonymNamesPair.clear();

		initializeSynonymVectorUsingMap(qTreeRoot->getSynonymsMap());
		vector<Synonym> synonymResult;

		bool isValidSuchThat = processSuchThatNode(qTreeRoot->getSuchThatNode());
		if (!isValidSuchThat) {
			return synonymResult;  //Return empty vector
		}

		bool isValidPattern = processPatternNode(qTreeRoot->getPatternNode());
		if (!isValidPattern) {
			return synonymResult;  //Return empty vector
		}

		//Both such that and pattern clauses are valid
		synonymResult = processResultNode(qTreeRoot->getResultNode());
		return synonymResult;
	}

	/**
	* The query parser uses a synonym map to assign synonym names and types
	* This function takes that map and initializes the synonym vector
	* The synonyms take on all possible values by probing the various tables
	*/
	void initializeSynonymVectorUsingMap(unordered_map<string,string> synonymMap) {
		synonymVector.clear();

		for (unordered_map<string, string>::iterator itr = synonymMap.begin(); itr != synonymMap.end(); ++itr) {
			vector<int> synonymValues;
			string name = itr->first;
			string type = itr->second;

			if (type == "variable") {
				synonymValues = pkb.getAllVarIndex();
			} else if (type == "constant") {
				synonymValues = pkb.getAllConstant();
			} else {
				synonymValues = pkb.getStmtNumForType(type);
			}
			Synonym synonym(type, name, synonymValues);
			synonymVector.push_back(synonym);
		}
	}

	/**
	* Processes the result node in the query tree
	* Returns the wanted synonym in a vector
	*/
	vector<Synonym> processResultNode(QNode* resultNode) {
		vector<Synonym> result;

		QNode* resultChildNode = resultNode->getChild();
		int numberOfSynonyms = resultNode->getNumberOfChildren();

		for (int i = 0; i < numberOfSynonyms; i++) {
			Synonym wantedSynonym = resultChildNode->getArg1();
			string wantedSynonymName = wantedSynonym.getName();
			Synonym s = findSynonymWithName(wantedSynonymName);
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
	bool processSuchThatNode(QNode* suchThatNode) {
		int numberOfClauses = suchThatNode->getNumberOfChildren();
		QNode* clauseNode = suchThatNode->getChild();

		for (int i = 0; i < numberOfClauses; i++) {
			bool isValid = processSuchThatClause(clauseNode);

			if (!isValid) {
				return false;
			}
			else {
				clauseNode = suchThatNode->getNextChild();
			}
		}
		return true;
	}

	/**
	* Method to processes each such that clause from the such that node
	* Returns true if a clause is valid, false otherwise
	*/
	bool processSuchThatClause(QNode* clauseNode) {
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
		default:
			return false;
		}
	}

	/**
	* Method to processes the modifies clause
	* Returns true if this clause is valid, false otherwise
	*/
	bool processModifies(Synonym arg1, Synonym arg2) {
		bool isValid = validateModifiesOrUsesArgs(arg1, arg2);

		if (!isValid || AbstractWrapper::GlobalStop) {
			return false;
		}

		string arg1Type = arg1.getType();
		string arg2Type = arg2.getType();

		//The arguments are valid, begin processing the arguments
		if (arg1Type == "String" && arg2Type == "String")
		{
			return pkb.isModifies(stoi(arg1.getName()), pkb.getVarIndex(arg2.getName()));
		}
		else if (arg1Type == "String")
		{
			//arg1 is the line number, find the variables that are modified
			vector<int> stmts = pkb.getModVarForStmt(stoi(arg1.getName()));
			return filterResults(arg2, stmts);

		}
		else if (arg2Type == "String")
		{
			//arg2 is the variable that is modified, find the statements
			vector<int> stmts = pkb.getModStmtNum(pkb.getVarIndex(arg2.getName()));
			return filterResults(arg1, stmts);
		}
		else
		{
			pair<vector<int>, vector<int>> allModifiesPair = pkb.getAllModPair();
			pair<vector<int>, vector<int>> filteredModifiesPair = 
				filterPairWithSynonymType(allModifiesPair, arg1Type, arg2Type);

			filterResults(arg1, filteredModifiesPair.first);
			filterResults(arg2, filteredModifiesPair.second);
			synonymValuesPair.push_back(filteredModifiesPair);
			synonymNamesPair.push_back(pair<string, string>(arg1.getName(), arg2.getName()));

			if (filteredModifiesPair.first.size() == 0 || filteredModifiesPair.second.size() == 0) {
				return false;
			}
		}
		return true;
	}

	/**
	* Method to processes the uses clause
	* Returns true if this clause is valid, false otherwise
	*/
	bool processUses(Synonym arg1, Synonym arg2) {
		bool isValid = validateModifiesOrUsesArgs(arg1, arg2);

		if (!isValid || AbstractWrapper::GlobalStop) {
			return false;
		}

		string arg1Type = arg1.getType();
		string arg2Type = arg2.getType();

		//The arguments are valid, begin processing the arguments
		if (arg1Type == "String" && arg2Type == "String")
		{
			return pkb.isUses(stoi(arg1.getName()), pkb.getVarIndex(arg2.getName()));
		}
		else if (arg1Type == "String")
		{
			//arg1 is the line number, find the variable that is used
			vector<int> vars = pkb.getUsesVarForStmt(stoi(arg1.getName()));
			return filterResults(arg2, vars);
		}
		else if (arg2Type == "String")
		{
			//arg2 is the variable that is used, find the statements that uses it
			vector<int> stmts = pkb.getUsesStmtNum(pkb.getVarIndex(arg2.getName()));
			return filterResults(arg1, stmts);
		}
		else
		{
			pair<vector<int>, vector<int>> allUsesPair = pkb.getAllUsesPair();
			pair<vector<int>, vector<int>> filteredUsesPair = 
				filterPairWithSynonymType(allUsesPair, arg1Type, arg2Type);

			filterResults(arg1, filteredUsesPair.first);
			filterResults(arg2, filteredUsesPair.second);
			synonymValuesPair.push_back(filteredUsesPair);
			synonymNamesPair.push_back(pair<string, string>(arg1.getName(), arg2.getName()));

			if (filteredUsesPair.first.size() == 0 || filteredUsesPair.second.size() == 0) {
				return false;
			}
		}
		return true;
	}

	/**
	* Method to processes the parent clause
	* Returns true if this clause is valid, false otherwise
	*/
	bool processParent(Synonym arg1, Synonym arg2) {
		bool isValid = validateParentArgs(arg1, arg2);

		if (!isValid || AbstractWrapper::GlobalStop) {
			return false;
		}

		string arg1Type = arg1.getType();
		string arg2Type = arg2.getType();

		if (arg1Type == "String" && arg2Type == "String")
		{
			return pkb.isParent(stoi(arg1.getName()), stoi(arg2.getName()));
		}
		else if (arg1Type == "String")
		{
			vector<int> stmts = pkb.getChild(stoi(arg1.getName()));
			return filterResults(arg2, stmts);
		}
		else if (arg2Type == "String")
		{
			vector<int> stmts = pkb.getParent(stoi(arg2.getName()));
			return filterResults(arg1, stmts);
		}
		else
		{
			pair<vector<int>, vector<int>> allParentsPair = pkb.getAllParentPairs(false);  //false for non transitive closure
			pair<vector<int>, vector<int>> filteredParentsPair = 
				filterPairWithSynonymType(allParentsPair, arg1Type, arg2Type);

			filterResults(arg1, filteredParentsPair.first);
			filterResults(arg2, filteredParentsPair.second);
			synonymValuesPair.push_back(filteredParentsPair);
			synonymNamesPair.push_back(pair<string, string>(arg1.getName(), arg2.getName()));

			if (filteredParentsPair.first.size() == 0 || filteredParentsPair.second.size() == 0) {
				return false;
			}
		}
		return true;
	}

	/**
	* Method to processes the parent* clause
	* Returns true if this clause is valid, false otherwise
	*/
	bool processParentS(Synonym arg1, Synonym arg2) {
		bool isEvaluated = evaluateParentS(arg1, arg2);
		string arg1Type = arg1.getType();
		string arg2Type = arg2.getType();

		if (arg1Type == "String" && arg2Type == "String")
		{
			return isEvaluated;
		}
		else if (arg1Type == "String")
		{
			return filterResults(arg2, tempVector);
		}
		else if (arg2Type == "String")
		{
			return filterResults(arg1, tempVector);
		}
		else
		{
			return isEvaluated;
		}
	}

	/**
	* Recursive method to evaluate the parent* clause
	* Returns true if this clause is valid, false otherwise
	*/
	bool evaluateParentS(Synonym arg1, Synonym arg2) {
		bool isValid = validateParentArgs(arg1, arg2);

		if (!isValid || AbstractWrapper::GlobalStop) {
			return false;
		}

		string arg1Type = arg1.getType();
		string arg2Type = arg2.getType();

		if (arg1Type == "String" && arg2Type == "String")
		{
			bool isParent = pkb.isParent(stoi(arg1.getName()), stoi(arg2.getName()));

			if (isParent)
			{
				return true;
			}
			else
			{
				//Use the getParent so that don't need to iterate through all the children
				vector<int> stmts = pkb.getParent(stoi(arg2.getName()));

				if (stmts.size() == 0)
				{
					return false;
				}

				Synonym s(arg2.getType(), stmts[0]);  //Parent statement must be definitely be in index 0
				return evaluateParentS(arg1, s);  //Check for the parent of new arg2
			}
		}
		else if (arg1Type == "String")
		{
			//Get childrenS
			vector<int> stmts = pkb.getChild(stoi(arg1.getName()));
			addResultsToTemp(stmts);

			for (unsigned int i = 0; i < stmts.size() && stmts.size() != 0; i++)
			{
				//Only need to get childrenS for container statements
				if (pkb.getType(stmts[i]) == "while")
				{
					Synonym s(arg1.getType(), stmts[i]);
					evaluateParentS(s, arg2);
				}
			}
			return true;
		}
		else if (arg2Type == "String")
		{
			//get parentS
			vector<int> stmts = pkb.getParent(stoi(arg2.getName()));
			addResultsToTemp(stmts);

			if (stmts.size() == 0)
			{
				return true;
			}

			Synonym s(arg2.getType(), stmts[0]);  //Parent statement must be definitely be in index 0
			return evaluateParentS(arg1, s);
		}
		else
		{
			pair<vector<int>, vector<int>> allParentsPair = pkb.getAllParentPairs(true);  //true for transitive closure
			pair<vector<int>, vector<int>> filteredParentsPair = 
				filterPairWithSynonymType(allParentsPair, arg1.getType(), arg2.getType());

			filterResults(arg1, filteredParentsPair.first);
			filterResults(arg2, filteredParentsPair.second);
			synonymValuesPair.push_back(filteredParentsPair);
			synonymNamesPair.push_back(pair<string, string>(arg1.getName(), arg2.getName()));

			if (filteredParentsPair.first.size() == 0 || filteredParentsPair.second.size() == 0) {
				return false;
			}
			return true;
		}
	}

	/**
	* Method to processes the follows clause
	* Returns true if this clause is valid, false otherwise
	*/
	bool processFollows(Synonym arg1, Synonym arg2) {
		bool isValid = validateFollowsArgs(arg1, arg2);

		if (!isValid || AbstractWrapper::GlobalStop) {
			return false;
		}

		string arg1Type = arg1.getType();
		string arg2Type = arg2.getType();

		if (arg1Type == "String" && arg2Type == "String")
		{
			return pkb.isFollows(stoi(arg1.getName()), stoi(arg2.getName()));
		}
		else if (arg1Type == "String")
		{
			// Given stmtNum1, get stmtNum2 such that Follows(stmt1, stmt2) is satisfied
			vector<int> stmt = pkb.getStmtFollowedFrom(stoi(arg1.getName()));
			return filterResults(arg2, stmt);
		}
		else if (arg2Type == "String")
		{
			// Given stmtNum2, get stmtNum1 such that Follows(stmt1, stmt2) is satisfied
			vector<int> stmt = pkb.getStmtFollowedTo(stoi(arg2.getName()));
			return filterResults(arg1, stmt);
		}
		else
		{
			pair<vector<int>, vector<int>> allFollowsPair = pkb.getAllFollowsPairs(false);  //false for non transitive closure
			pair<vector<int>, vector<int>> filteredFollowsPair = 
				filterPairWithSynonymType(allFollowsPair, arg1Type, arg2Type);

			filterResults(arg1, filteredFollowsPair.first);
			filterResults(arg2, filteredFollowsPair.second);
			synonymValuesPair.push_back(filteredFollowsPair);
			synonymNamesPair.push_back(pair<string, string>(arg1.getName(), arg2.getName()));

			if (filteredFollowsPair.first.size() == 0 || filteredFollowsPair.second.size() == 0) {
				return false;
			}
			return true;
		}
	}

	/**
	* Method to processes the follows* clause
	* Returns true if this clause is valid, false otherwise
	*/
	bool processFollowsS(Synonym arg1, Synonym arg2) {
		bool isEvaluated = evaluateFollowsS(arg1, arg2);
		string arg1Type = arg1.getType();
		string arg2Type = arg2.getType();

		if (arg1Type == "String" && arg2Type == "String")
		{
			return isEvaluated;
		}
		else if (arg1Type == "String")
		{
			return filterResults(arg2, tempVector);
		}
		else if (arg2Type == "String")
		{
			return filterResults(arg1, tempVector);
		}
		else
		{
			return isEvaluated;
		}
	}

	/**
	* Recursive method to processes the follows* clause
	* Returns true if this clause is valid, false otherwise
	*/
	bool evaluateFollowsS(Synonym arg1, Synonym arg2) {
		bool isValid = validateFollowsArgs(arg1, arg2);

		if (!isValid || AbstractWrapper::GlobalStop) {
			return false;
		}

		string arg1Type = arg1.getType();
		string arg2Type = arg2.getType();

		if (arg1Type == "String" && arg2Type == "String")
		{
			//Does not matter (yet) whether to use followsBefore or followsAfter
			bool isFollows = pkb.isFollows(stoi(arg1.getName()), stoi(arg2.getName()));

			if (isFollows)
			{
				return true;
			}
			else
			{
				//Use follows after
				vector<int> stmts = pkb.getStmtFollowedFrom(stoi(arg1.getName()));
				if (stmts.size() == 0)
				{
					return false;
				}

				Synonym s(arg1.getType(), stmts[0]);  //The follows statement must be in index 0
				return evaluateFollowsS(s, arg2);  //Get follows after using new arg1
			}
		}
		else if (arg1Type == "String")
		{
			//Get follows after
			vector<int> stmt = pkb.getStmtFollowedFrom(stoi(arg1.getName()));
			addResultsToTemp(stmt);

			if (stmt.size() == 0)
			{
				return true;
			}

			Synonym s(arg1.getType(), stmt[0]);  //The follows statement must be in index 0
			return evaluateFollowsS(s, arg2);  //Get follows after using new arg1
		}
		else if (arg2Type == "String")
		{
			//Get follows before
			vector<int> stmt = pkb.getStmtFollowedTo(stoi(arg2.getName()));
			addResultsToTemp(stmt);

			if (stmt.size() == 0)
			{
				return true;
			}

			Synonym s(arg2.getType(), stmt[0]);  //The follows statement must be in index 0
			return evaluateFollowsS(arg1, s);
		}
		else
		{
			pair<vector<int>, vector<int>> allFollowsPair = pkb.getAllFollowsPairs(true);  //true for transitive closure
			pair<vector<int>, vector<int>> filteredFollowsPair = 
				filterPairWithSynonymType(allFollowsPair, arg1.getType(), arg2.getType());

			filterResults(arg1, filteredFollowsPair.first);
			filterResults(arg2, filteredFollowsPair.second);
			synonymValuesPair.push_back(filteredFollowsPair);
			synonymNamesPair.push_back(pair<string, string>(arg1.getName(), arg2.getName()));

			if (filteredFollowsPair.first.size() == 0 || filteredFollowsPair.second.size() == 0) {
				return false;
			}
			return true;
		}
	}

	/**
	* Method to validate the arguments of modifies or uses clause
	* Returns true if the arguments are valid, false otherwise
	*/
	bool validateModifiesOrUsesArgs(Synonym arg1, Synonym arg2) {
		//arg1 must be a statement(but not constant) and cannot have "_"
		//arg2 must be a character, variable or "_"
		string arg1Type = arg1.getType();
		string arg2Type = arg2.getType();
		if (arg1Type != "String" && arg1Type != "stmt" && arg1Type != "assign" && 
			arg1Type != "while" && arg1Type != "prog_line")
		{
			return false;
		}
		if (arg2Type != "String" && arg2Type != "variable" && arg2Type != "_")
		{
			return false;
		}

		if (arg1Type != "String" && arg2Type != "String" && arg1.getName() == arg2.getName())
		{
			return false;  //arg1 and arg2 cannot have the same names if they are synoyms
		}

		//If it reaches here, the types are valid. Check for the values of arg1 if it is a constant string
		if (arg1Type == "String")
		{
			char arg1Value = arg1.getName()[0];  //Get the value of arg1
			if (!isdigit(arg1Value))
			{
				return false;  //arg1 must be a digit
			}
		}
		else
		{
			//Check that this synonym exists in the synonymVector
			if (findSynonymWithName(arg1.getName()).getName() == "-1")
			{
				return false;
			}
		}

		//If it reaches here, arg1 is valid. Check for the values of arg2 if it is a constant string
		if (arg2Type == "String")
		{
			int varIndex = pkb.getVarIndex(arg2.getName());
			if (varIndex == -1)
			{
				//This variable is not found in the var table
				return false;
			}

			char arg2Value = arg2.getName()[0];  //Get the value of arg2
			if (!isalpha(arg2Value))
			{
				return false;  //arg2 must be a character
			}
		}
		else if (arg2Type != "_")
		{
			//Check that this synonym exists in the synonymVector
			if (findSynonymWithName(arg2.getName()).getName() == "-1")
			{
				return false;
			}
		}
		return true;  //Everything is valid
	}

	/**
	* Method to validate the arguments of parent or follows clause
	* Returns true if the arguments are valid, false otherwise
	*/
	bool validateParentArgs(Synonym arg1, Synonym arg2) {
		//arg1 and arg2 must be numbers(but not assign, constants), number synonyms or "_"
		string arg1Type = arg1.getType();
		string arg2Type = arg2.getType();
		if (arg1Type != "String" && arg1Type != "stmt" && arg1Type != "_" && 
			arg1Type != "while" && arg1Type != "prog_line")
		{
			return false;
		}
		if (arg2Type != "String" && arg2Type != "stmt" && arg2Type != "assign" && arg2Type != "_" && 
			arg2Type != "while" && arg2Type != "prog_line")
		{
			return false;
		}

		//If it reaches here, the types are valid
		if (arg1Type == "String")
		{
			char arg1Value = arg1.getName()[0];  //Get the value of arg1
			if (!isdigit(arg1Value))
			{
				return false;  //Check for that arg1 is a number if it is a constant string
			}
		}
		else if (arg1Type != "_")
		{
			//Check that this synonym exists in the synonymVector
			if (findSynonymWithName(arg1.getName()).getName() == "-1")
			{
				return false;
			}
		}

		//If it reaches here, arg1 is valid
		if (arg2Type == "String")
		{
			char arg2Value = arg2.getName()[0];  //Get the value of arg2
			if (!isdigit(arg2Value))
			{
				return false;  //Check that arg2 is a number if it is a constant string
			}
		}
		else if (arg2Type != "_")
		{
			//Check that this synonym exists in the synonymVector
			if (findSynonymWithName(arg2.getName()).getName() == "-1")
			{
				return false;
			}
		}

		if (arg1Type == "String" && arg2Type == "String")
		{
			//Since the two are constant strings, they must be digits by the checks above
			if (stoi(arg1.getName()) >= stoi(arg2.getName()))
			{
				return false;  //arg1 must be smaller than arg2 or else it is false
			}
		}

		if (arg1Type != "String" && arg2Type != "String" && 
			arg1Type != "_" && arg2Type != "_" && arg1.getName() == arg2.getName())
		{
			return false;  //arg1 and arg2 cannot have the same synonym name if they are synonyms
		}

		return true;  //Everything is valid
	}

	/**
	* Method to validate the arguments of parent or follows clause
	* Returns true if the arguments are valid, false otherwise
	*/
	bool validateFollowsArgs(Synonym arg1, Synonym arg2) {
		//arg1 and arg2 must be numbers(but not constants), number synonyms or "_"
		string arg1Type = arg1.getType();
		string arg2Type = arg2.getType();
		if (arg1Type != "String" && arg1Type != "stmt" && arg1Type != "assign" && arg1Type != "_" && 
			arg1Type != "while" && arg1Type != "prog_line")
		{
			return false;
		}
		if (arg2Type != "String" && arg2Type != "stmt" && arg2Type != "assign" && arg2Type != "_" && 
			arg2Type != "while" && arg2Type != "prog_line")
		{
			return false;
		}

		//If it reaches here, the types are valid
		if (arg1Type == "String")
		{
			char arg1Value = arg1.getName()[0];  //Get the value of arg1
			if (!isdigit(arg1Value))
			{
				return false;  //Check for that arg1 is a number if it is a constant string
			}
		}
		else if (arg1Type != "_")
		{
			//Check that this synonym exists in the synonymVector
			if (findSynonymWithName(arg1.getName()).getName() == "-1")
			{
				return false;
			}
		}

		//If it reaches here, arg1 is valid
		if (arg2Type == "String")
		{
			char arg2Value = arg2.getName()[0];  //Get the value of arg2
			if (!isdigit(arg2Value))
			{
				return false;  //Check that arg2 is a number if it is a constant string
			}
		}
		else if (arg2Type != "_")
		{
			//Check that this synonym exists in the synonymVector
			if (findSynonymWithName(arg2.getName()).getName() == "-1")
			{
				return false;
			}
		}

		if (arg1Type == "String" && arg2Type == "String")
		{
			//Since the two are constant strings, they must be digits by the checks above
			if (stoi(arg1.getName()) >= stoi(arg2.getName()))
			{
				return false;  //arg1 must be smaller than arg2 or else it is false
			}
		}

		if (arg1Type != "String" && arg2Type != "String" && 
			arg1Type != "_" && arg2Type != "_" && arg1.getName() == arg2.getName())
		{
			return false;  //arg1 and arg2 cannot have the same synonym name if they are synonyms
		}

		return true;  //Everything is valid
	}

	/**
	* Method to process the pattern node in the query tree
	* Returns true if all the patterns are valid or there are no patterns to match
	* Returns false if there is one pattern that is invalid
	*/
	bool processPatternNode(QNode* patternNode) {
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
	bool processPatternClause(QNode* patternClause) {
		Synonym arg0 = patternClause->getArg0();
		Synonym arg1 = patternClause->getArg1();
		Synonym arg2 = patternClause->getArg2();
		string patternType = arg0.getType();

		if (patternType == "assign") {
			return processAssignPattern(arg0, arg1, arg2);
		} else if (patternType == "while") {
			return processWhilePattern(arg0, arg1, arg2);
		} else {
			return false;
		}
	}

	/**
	* Method to process assign patterns
	* Returns true if the pattern clause is valid, false otherwise
	*/
	bool processAssignPattern(Synonym arg0, Synonym arg1, Synonym arg2) {
		bool isValid = validateAssignPatternArgs(arg0, arg1, arg2);

		if (!isValid || AbstractWrapper::GlobalStop) {
			return false;
		}

		string arg1Type = arg1.getType();
		vector<int> isMatchStmts = pkb.patternMatchAssign(arg2.getName());
		filterResults(arg0, isMatchStmts);  //Do intersection between the current values and returned values
		set<int> assignValues = findSynonymWithName(arg0.getName()).getValues();  //Get the filtered values

		if (arg1Type == "variable")
		{
			//If LHS is a variable synonym, use the return statements to probe the ModifiesTable
			set<int> LHSValues = findSynonymWithName(arg1.getName()).getValues();
			vector<int> matchingStmts;
			tempVector.clear();

			for (set<int>::iterator itr = assignValues.begin(); itr != assignValues.end(); ++itr)
			{
				vector<int> vars = pkb.getModVarForStmt(*itr);  //Get the LHS of the assign statement
				if (isValueExist(LHSValues, vars[0]))
				{
					//Only use the assignment statements that match the variable synonym values
					matchingStmts.push_back(*itr);
					addResultsToTemp(vars);
				}
			}
			filterResults(arg0, matchingStmts);
			filterResults(arg1, tempVector);
			synonymValuesPair.push_back(pair<vector<int>, vector<int>>(matchingStmts, tempVector));
			synonymNamesPair.push_back(pair<string, string>(arg0.getName(), arg1.getName()));
			filterTwinDependentValues();

			if (matchingStmts.size() == 0 || tempVector.size() == 0)
			{
				return false;
			}
			return true;
		}
		else if (arg1Type == "_")
		{
			return true;
		}
		else
		{
			//LHS is not a synonym and not "_"
			string LHS = arg1.getName();
			vector<int> matchingStmts;

			//Check that the assignment statements modifies the LHS
			for (set<int>::iterator itr = assignValues.begin(); itr != assignValues.end(); ++itr)
			{
				int stmt = *itr;
				vector<int> vars = pkb.getModVarForStmt(stmt);
				string var = pkb.getVarName(vars[0]);  //Variable for assignment stmts must be in index 0
				if (var == LHS)
				{
					matchingStmts.push_back(stmt);
				}
			}
			return filterResults(arg0, matchingStmts);
		}
	}

	/**
	* Method to process while patterns
	* Returns true if the pattern clause is valid, false otherwise
	*/
	bool processWhilePattern(Synonym arg0, Synonym arg1, Synonym arg2) {
		//Validate while pattern
		bool isValid = validateWhilePatternArgs(arg0, arg1, arg2);

		if (!isValid || AbstractWrapper::GlobalStop) {
			return false;
		}

		string arg1Type = arg1.getType();

		//Find all while statements that uses LHS
		if (arg1.getType() == "String")
		{
			vector<int> stmts = pkb.patternMatchWhile(arg1.getName());
			return filterResults(arg0, stmts);
		}
		else if (arg1Type == "_")
		{
			return true;  //Do nothing because pattern w(_, _) is always true if there are while statements
		}
		else
		{
			//LHS is a variable synonym
			set<int> whileStmts = findSynonymWithName(arg0.getName()).getValues();
			set<int> LHSValues = findSynonymWithName(arg1.getName()).getValues();
			vector<int> vars;  //A vector to store the values of the while control variables
			vector<int> validWhileStmts;
			tempVector.clear();

			for (set<int>::iterator itr = whileStmts.begin(); itr != whileStmts.end(); ++itr)
			{
				int var = pkb.getControlVariable(*itr);
				if (isValueExist(LHSValues, var))
				{
					//Only use the assignment statements that match the variable synonym values
					validWhileStmts.push_back(*itr);
					vars.push_back(var);
				}
			}
			filterResults(arg0, validWhileStmts);
			filterResults(arg1, vars);
			synonymValuesPair.push_back(pair<vector<int>, vector<int>>(validWhileStmts, vars));
			synonymNamesPair.push_back(pair<string, string>(arg0.getName(), arg1.getName()));
			filterTwinDependentValues();

			if (validWhileStmts.size() == 0 || vars.size() == 0)
			{
				return false;
			}
			return true;
		}
	}

	/**
	* Method to validate assign patterns
	* Returns true if the arguments of the assign pattern are valid, false otherwise
	*/
	bool validateAssignPatternArgs(Synonym arg0, Synonym arg1, Synonym arg2) {
		string arg1Type = arg1.getType();

		if (findSynonymWithName(arg0.getName()).getName() == "-1") {
			//Check if this assign synonym has been declared
			return false;
		}

		if (arg1Type != "String" && arg1Type != "variable" && arg1Type != "_") {
			return false;
		}

		if (arg1Type == "variable" && findSynonymWithName(arg1.getName()).getName() == "-1") {
			//Check if this variable synonym has been declared
			return false;
		}

		return true;
	}

	/**
	* Method to validate while patterns
	* Returns true if the arguments of the assign pattern are valid, false otherwise
	*/
	bool validateWhilePatternArgs(Synonym arg0, Synonym arg1, Synonym arg2) {
		string arg1Type = arg1.getType();

		if (findSynonymWithName(arg0.getName()).getName() == "-1") {
			//Check if this while synonym has been declared
			return false;
		}

		if (arg1Type != "String" && arg1Type != "variable" && arg1Type != "_") {
			return false;  //arg1 can only be a constant string or a variable synonym or "_"
		}

		if (arg1Type == "variable" && findSynonymWithName(arg1.getName()).getName() == "-1") {
			//Check if this variable synonym has been declared
			return false;
		}

		if (arg2.getName() != "_") {
			return false;  //arg2 must be "_"
		}

		return true;
	}

	/**
	* Helper method to filter the results by doing set intersection for synonyms
	* This way, only the relevant values are left behind
	* Paired dependent values are handled in another function
	* Returns true if there are (given) values to intersect, false otherwise
	*/
	bool filterResults(Synonym synonym, vector<int> results) {

		if (synonym.getType() == "_" && results.size() != 0) {
			return true;
		}

		synonym = findSynonymWithName(synonym.getName());
		if (synonym.getName() == "-1") {
			//To handle "_" or undeclared synonyms
			return false;
		}

		set<int> filteredValues = intersectWithCurrentValues(results, synonym.getValues());
		Synonym newSynonym(synonym.getType(), synonym.getName(), filteredValues);
		swapOldSynonymWithNewSynonym(newSynonym);
		filterDependentValues(newSynonym);

		if (results.size() == 0) {
			return false;
		}

		return true;
	}

	/**
	* Helper method to swap the old synonym in the synonym vector to the new one
	* The new synonym has the updated values from set intersection performed
	*/
	void swapOldSynonymWithNewSynonym(Synonym newSynonym) {
		for (unsigned int i = 0; i < synonymVector.size(); i++) {
			if (synonymVector[i].getName() == newSynonym.getName()) {
				swap(synonymVector[i], newSynonym);
				break;
			}
		}
	}

	/**
	* Helper method to perform set intersection
	* Returns the set of values that are included in both lists
	*/
	set<int> intersectWithCurrentValues(vector<int> resultValues, set<int> currentValues) {
		set<int> filteredValues;

		for (unsigned int i = 0; i < resultValues.size(); i++) {
			if (isValueExist(currentValues, resultValues[i])) {
				filteredValues.insert(resultValues[i]);
			}
		}
		return filteredValues;
	}

	/**
	* Helper method to filter the paired dependent values
	*/
	void filterDependentValues(Synonym synonym) {
		vector<int> probeValues;  //Values that are used in probing
		vector<int> otherValues;  //The other value that is not used in the probing
		vector<int> preFilteredValues;  //Values that are stored to be filtered
		set<int> synonymValuesSet = synonym.getValues();
		string dependentSynonymName;
		string synonymName = synonym.getName();

		//Go through the whole synonymValuesPair
		for (unsigned int i = 0; i < synonymValuesPair.size(); i++) 
		{
			pair<vector<int>, vector<int>> valuesPair = synonymValuesPair[i];
			pair<string, string> namesPair = synonymNamesPair[i];

			if (namesPair.first == synonymName)
			{
				probeValues = valuesPair.first;
				otherValues = valuesPair.second;
				dependentSynonymName = namesPair.second;
			}
			else if (namesPair.second == synonymName)
			{
				probeValues = valuesPair.second;
				otherValues = valuesPair.first;
				dependentSynonymName = namesPair.first;
			}
			else
			{
				continue;  //There is no synonym value dependent on this synonym
			}

			if (dependentSynonymName == "_" || synonymName == "_")
			{
				continue;  //Do nothing if "_" are involved
			}

			for (unsigned int j = 0; j < probeValues.size(); j++)
			{
				if (isValueExist(synonymValuesSet, probeValues[j]))
				{
					preFilteredValues.push_back(otherValues[j]);
				}
			}

			Synonym dependentSynonym = findSynonymWithName(dependentSynonymName);
			set<int> dependentSynonymValues = dependentSynonym.getValues();
			set<int> filteredDependentValues = intersectWithCurrentValues(preFilteredValues, dependentSynonymValues);
			Synonym newSynonym(dependentSynonym.getType(), dependentSynonym.getName(), filteredDependentValues);
			swapOldSynonymWithNewSynonym(newSynonym);
		}
	}

	/**
	* Helper method to filter twinned (two synonyms) paired values
	*/
	void filterTwinDependentValues() {
		unsigned int pairsSize = synonymValuesPair.size();
		unsigned int newestIndex = pairsSize - 1;

		if (pairsSize < 2) {
			return;
		}

		string newestPairName1 = synonymNamesPair[newestIndex].first;
		string newestPairName2 = synonymNamesPair[newestIndex].second;
		vector<int> newestPairValues1 = synonymValuesPair[newestIndex].first;
		vector<int> newestPairValues2 = synonymValuesPair[newestIndex].second;

		for (unsigned int i = 0; i < newestIndex; i++) {
			string comparingPairName1 = synonymNamesPair[i].first;
			string comparingPairName2 = synonymNamesPair[1].second;

			if (newestPairName1 != comparingPairName1 && newestPairName1 != comparingPairName2) {
				continue;
			}
			if (newestPairName2 != comparingPairName1 && newestPairName2 != comparingPairName2) {
				continue;
			}

			//If it reaches here, it means that there are twinned paired values
			vector<int> comparingPairValues1;
			vector<int> comparingPairValues2;
			vector<int> acceptedValues1;
			vector<int> acceptedValues2;

			if (newestPairName1 == comparingPairName1) {
				comparingPairValues1 = synonymValuesPair[i].first;
				comparingPairValues2 = synonymValuesPair[i].second;
			} else {
				comparingPairValues1 = synonymValuesPair[i].second;
				comparingPairValues2 = synonymValuesPair[i].first;
			}

			//Compare the newest values with the existing values
			for (unsigned int j = 0; j < newestPairValues1.size(); j++)
			{
				for (unsigned int k = 0; k < comparingPairValues1.size(); k++)
				{
					if (newestPairValues1[j] == comparingPairValues1[k] && newestPairValues2[j] == comparingPairValues2[k])
					{
						acceptedValues1.push_back(newestPairValues1[j]);
						acceptedValues2.push_back(newestPairValues2[j]);
					}
				}
			}

			Synonym synonym1 = findSynonymWithName(newestPairName1);
			set<int> synonym1Values = synonym1.getValues();
			set<int> filteredSynonym1Values = intersectWithCurrentValues(acceptedValues1, synonym1Values);
			Synonym newSynonym1(synonym1.getType(), synonym1.getName(), filteredSynonym1Values);
			swapOldSynonymWithNewSynonym(newSynonym1);

			Synonym synonym2 = findSynonymWithName(newestPairName2);
			set<int> synonym2Values = synonym2.getValues();
			set<int> filteredSynonym2Values = intersectWithCurrentValues(acceptedValues2, synonym2Values);
			Synonym newSynonym2(synonym2.getType(), synonym2.getName(), filteredSynonym2Values);
			swapOldSynonymWithNewSynonym(newSynonym2);
		}
	}

	/**
	* Helper method to get the synonym from the synonym vector given the name
	* Returns the synonym if it exists
	* Returns a synonym with -1 as name and type if it does not
	*/
	Synonym findSynonymWithName(string name) {
		for (unsigned int i = 0; i < synonymVector.size(); i++) {
			if (synonymVector[i].getName() == name) {
				return synonymVector[i];
			}
		}
		Synonym s("-1", "-1");
		return s;
	}

	/**
	* Helper method to check if a particular value is contained in the set
	* This is to enhance code readability but could have additional function call overheads
	*/
	bool isValueExist(set<int> setToSearch, int value) {
		int count = setToSearch.count(value);

		if (count >= 1) {
			return true;
		}
		return false;
	}

	/**
	* Helper method to add intermediate values to the temp vector
	*/
	void addResultsToTemp(vector<int> results) {
		for (unsigned int i = 0; i < results.size(); i++) {
			tempVector.push_back(results[i]);
		}
	}

	/**
	* Helper method to filter the paired synonym values
	* Because the getAllPairs function from the various tables does not distinguish between the types,
	* this function filters only the requested types from all the pairs
	* Returns the filtered pair of values
	*/
	pair<vector<int>, vector<int>> filterPairWithSynonymType(pair<vector<int>, vector<int>> allPairs, string arg1Type, string arg2Type)
	{
		vector<int> filteredFirstElements;
		vector<int> filteredSecondElements;

		//Just have to loop for one of the pairs since the number of pairs must be the same
		for (unsigned int i = 0; i < allPairs.first.size(); i++)
		{
			int firstElement = allPairs.first[i];
			int secondElement = allPairs.second[i];

			if (arg1Type == "stmt" || arg1Type == "prog_line" || arg1Type == "_")
			{
				if (arg2Type == "stmt" || arg2Type == "prog_line" || arg2Type == "variable" || arg2Type == "_")
				{
					return allPairs;
				}
				else if (arg2Type == "constant" && pkb.isConstant(secondElement))
				{
					//The constant table has been probed and arg2 constant value exists
					filteredFirstElements.push_back(firstElement);
					filteredSecondElements.push_back(secondElement);
				}
				else if (arg2Type == pkb.getType(secondElement))
				{
					filteredFirstElements.push_back(firstElement);
					filteredSecondElements.push_back(secondElement);
				}
			}
			else if (arg1Type == "constant" && pkb.isConstant(firstElement))
			{
				//The constant table has been probed and arg1 constant value exists
				if (arg2Type == "stmt" || arg2Type == "prog_line" || arg2Type == "variable" || arg2Type == "_")
				{
					filteredFirstElements.push_back(firstElement);
					filteredSecondElements.push_back(secondElement);
				}
				else if (arg2Type == "constant" && pkb.isConstant(secondElement))
				{
					//The constant table has been probed and arg2 constant value exists
					filteredFirstElements.push_back(firstElement);
					filteredSecondElements.push_back(secondElement);
				}
				else if (arg2Type == pkb.getType(secondElement))
				{
					filteredFirstElements.push_back(firstElement);
					filteredSecondElements.push_back(secondElement);
				}
			}
			else if (arg1Type == pkb.getType(firstElement))
			{
				if (arg2Type == "stmt" || arg2Type == "prog_line" || arg2Type == "variable" || arg2Type == "_")
				{
					filteredFirstElements.push_back(firstElement);
					filteredSecondElements.push_back(secondElement);
				}
				else if (arg2Type == "constant" && pkb.isConstant(secondElement))
				{
					//The constant table has been probed and arg2 constant value exists
					filteredFirstElements.push_back(firstElement);
					filteredSecondElements.push_back(secondElement);
				}
				else if (arg2Type == pkb.getType(secondElement))
				{
					filteredFirstElements.push_back(firstElement);
					filteredSecondElements.push_back(secondElement);
				}
			}
		}

		pair<vector<int>, vector<int>> filteredPairs(filteredFirstElements, filteredSecondElements);
		return filteredPairs;
	}
}
