//TODO: Select boolean
//TODO: While pattern for w(v,_)
//TODO: What if no while loops and query asks for while
//TODO: With clauses
//TODO: * Queries
//TODO: Pattern queries
//TODO: Use one side to probe instead of finding all pairs
//TODO: Remove query validation from query evaluator

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

	//Functions to process clauses
	bool processModifies(Synonym arg1, Synonym arg2);
	bool processUses(Synonym arg1, Synonym arg2);
	bool processParent(Synonym arg1, Synonym arg2);
	bool processParentS(Synonym arg1, Synonym arg2);
	bool evaluateParentS(Synonym arg1, Synonym arg2);
	bool processFollows(Synonym arg1, Synonym arg2);
	bool processFollowsS(Synonym arg1, Synonym arg2);
	bool evaluateFollowsS(Synonym arg1, Synonym arg2);

	//Functions to process pattern clauses
	bool processPatternClause(QNode* patternClause);
	bool processAssignPattern(Synonym arg0, Synonym arg1, Synonym arg2);
	bool processWhilePattern(Synonym arg0, Synonym arg1, Synonym arg2);

	bool validateModifiesOrUsesArgs(Synonym arg1, Synonym arg2);  //Validates the inputs for modifies and uses
	bool validateParentArgs(Synonym arg1, Synonym arg2);  //Validates the inputs for parent
	bool validateFollowsArgs(Synonym arg1, Synonym arg2);  //Validates the inputs for follows
	bool validateAssignPatternArgs(Synonym arg0, Synonym arg1, Synonym arg2);  //Validates the input for assign pattern
	bool validateWhilePatternArgs(Synonym arg0, Synonym arg1, Synonym arg2);  //Validates the input for while patterns

	Synonym findSynonymWithName(string wantedSynonymName);
	pair<vector<int>, vector<int>> filterPairWithSynonymType(pair<vector<int>, vector<int>> allPairs, string arg1Type, string arg2Type);

	//New structures for the new query evaluator
	vector<vector<int>> allIntermediateValues;
	vector<string> allIntermediateNames;
	unordered_map<string, string> synonymMap;

	void addAndProcessIntermediateSynonym(Synonym synonym);
	void addAndProcessIntermediateSynonyms(Synonym LHS, Synonym RHS);
	void joinWithExistingValues(Synonym synonym);
	void joinWithExistingValues(Synonym LHS, Synonym RHS);
	void intersectWithExistingValues(int synonymIndex, vector<int> probeValues);
	void intersectWithExistingValues(int indexLHS, vector<int> LHSValues, int indexRHS, vector<int> RHSValues);
	void intersectAndJoinWithExistingValues(int existingIndex, Synonym probe, Synonym newValues);
	set<int> getIntermediateValues(int synonymIndex);

	PKB pkb = PKB::getInstance();  //Get the instance of the PKB singleton

	/**
	* Processes the query tree given a query tree node
	* Returns an empty vector if the Such That or Pattern clauses are invalid
	*/
	vector<Synonym> processQueryTree(QueryTree* qTreeRoot) {
		allIntermediateValues.clear();
		allIntermediateNames.clear();
		synonymMap = qTreeRoot->getSynonymsMap();

		//initializeSynonymVectorUsingMap(qTreeRoot->getSynonymsMap());
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
		if (arg1Type == "String" && arg2Type == "String") {
			return pkb.isModifies(stoi(arg1.getName()), pkb.getVarIndex(arg2.getName()));
		} else if (arg1Type == "String") {
			//arg1 is the line number, find the variables that are modified
			vector<int> stmts = pkb.getModVarForStmt(stoi(arg1.getName()));
			Synonym synonym(arg2Type, arg2.getName(), stmts);
			addAndProcessIntermediateSynonym(synonym);
		} else if (arg2Type == "String") {
			//arg2 is the variable that is modified, find the statements
			vector<int> stmts = pkb.getModStmtNum(pkb.getVarIndex(arg2.getName()));
			Synonym synonym(arg1Type, arg1.getName(), stmts);
			addAndProcessIntermediateSynonym(synonym);
		} else {
			pair<vector<int>, vector<int>> allModifiesPair = pkb.getAllModPair();
			pair<vector<int>, vector<int>> filteredModifiesPair = 
				filterPairWithSynonymType(allModifiesPair, arg1Type, arg2Type);

			Synonym LHS(arg1Type, arg1.getName(), filteredModifiesPair.first);
			Synonym RHS(arg1Type, arg1.getName(), filteredModifiesPair.second);
			addAndProcessIntermediateSynonyms(LHS, RHS);

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
		if (arg1Type == "String" && arg2Type == "String") {
			return pkb.isUses(stoi(arg1.getName()), pkb.getVarIndex(arg2.getName()));
		} else if (arg1Type == "String") {
			//arg1 is the line number, find the variable that is used
			vector<int> vars = pkb.getUsesVarForStmt(stoi(arg1.getName()));
			Synonym synonym(arg2Type, arg2.getName(), vars);
			addAndProcessIntermediateSynonym(synonym);
		} else if (arg2Type == "String") {
			//arg2 is the variable that is used, find the statements that uses it
			vector<int> stmts = pkb.getUsesStmtNum(pkb.getVarIndex(arg2.getName()));
			Synonym synonym(arg1Type, arg1.getName(), stmts);
			addAndProcessIntermediateSynonym(synonym);
		} else {
			pair<vector<int>, vector<int>> allUsesPair = pkb.getAllUsesPair();
			pair<vector<int>, vector<int>> filteredUsesPair = 
				filterPairWithSynonymType(allUsesPair, arg1Type, arg2Type);

			Synonym LHS(arg1Type, arg1.getName(), filteredUsesPair.first);
			Synonym RHS(arg2Type, arg2.getName(), filteredUsesPair.second);
			addAndProcessIntermediateSynonyms(LHS, RHS);

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

		if (arg1Type == "String" && arg2Type == "String") {
			return pkb.isParent(stoi(arg1.getName()), stoi(arg2.getName()));
		} else if (arg1Type == "String") {
			vector<int> stmts = pkb.getChild(stoi(arg1.getName()));
			Synonym synonym(arg2Type, arg2.getName(), stmts);
			addAndProcessIntermediateSynonym(synonym);
		} else if (arg2Type == "String") {
			vector<int> stmts = pkb.getParent(stoi(arg2.getName()));
			Synonym synonym(arg1Type, arg1.getName(), stmts);
			addAndProcessIntermediateSynonym(synonym);
		} else {
			pair<vector<int>, vector<int>> allParentsPair = pkb.getAllParentPairs(false);  //false for non transitive closure
			pair<vector<int>, vector<int>> filteredParentsPair = 
				filterPairWithSynonymType(allParentsPair, arg1Type, arg2Type);

			Synonym LHS(arg1Type, arg1.getName(), filteredParentsPair.first);
			Synonym RHS(arg2Type, arg2.getName(), filteredParentsPair.second);
			addAndProcessIntermediateSynonyms(LHS, RHS);

			if (filteredParentsPair.first.size() == 0 || filteredParentsPair.second.size() == 0) {
				return false;
			}
		}
		return true;
	}

	/**
	* Method to processes the parent clause
	* Returns true if this clause is valid, false otherwise
	*/
	bool processParentS(Synonym arg1, Synonym arg2) {
		bool isValid = validateParentArgs(arg1, arg2);

		if (!isValid || AbstractWrapper::GlobalStop) {
			return false;
		}

		string arg1Type = arg1.getType();
		string arg2Type = arg2.getType();

		if (arg1Type == "String" && arg2Type == "String") {
			return pkb.isParent(stoi(arg1.getName()), stoi(arg2.getName()));
		} else if (arg1Type == "String") {
			vector<int> stmts = pkb.getChild(stoi(arg1.getName()));
			Synonym synonym(arg2Type, arg2.getName(), stmts);
			addAndProcessIntermediateSynonym(synonym);
		} else if (arg2Type == "String") {
			vector<int> stmts = pkb.getParent(stoi(arg2.getName()));
			Synonym synonym(arg1Type, arg1.getName(), stmts);
			addAndProcessIntermediateSynonym(synonym);
		} else {
			pair<vector<int>, vector<int>> allParentsPair = pkb.getAllParentPairs(false);  //false for non transitive closure
			pair<vector<int>, vector<int>> filteredParentsPair = 
				filterPairWithSynonymType(allParentsPair, arg1Type, arg2Type);

			Synonym LHS(arg1Type, arg1.getName(), filteredParentsPair.first);
			Synonym RHS(arg2Type, arg2.getName(), filteredParentsPair.second);
			addAndProcessIntermediateSynonyms(LHS, RHS);

			if (filteredParentsPair.first.size() == 0 || filteredParentsPair.second.size() == 0) {
				return false;
			}
		}
		return true;
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

		if (arg1Type == "String" && arg2Type == "String") {
			return pkb.isFollows(stoi(arg1.getName()), stoi(arg2.getName()));
		} else if (arg1Type == "String") {
			// Given stmtNum1, get stmtNum2 such that Follows(stmt1, stmt2) is satisfied
			vector<int> stmt = pkb.getStmtFollowedFrom(stoi(arg1.getName()));
			Synonym synonym(arg2Type, arg2.getName(), stmt);
			addAndProcessIntermediateSynonym(synonym);
		} else if (arg2Type == "String") {
			// Given stmtNum2, get stmtNum1 such that Follows(stmt1, stmt2) is satisfied
			vector<int> stmt = pkb.getStmtFollowedTo(stoi(arg2.getName()));
			Synonym synonym(arg1Type, arg1.getName(), stmt);
			addAndProcessIntermediateSynonym(synonym);
		} else {
			pair<vector<int>, vector<int>> allFollowsPair = pkb.getAllFollowsPairs(false);  //false for non transitive closure
			pair<vector<int>, vector<int>> filteredFollowsPair = 
				filterPairWithSynonymType(allFollowsPair, arg1Type, arg2Type);

			Synonym LHS(arg1Type, arg1.getName(), filteredFollowsPair.first);
			Synonym RHS(arg2Type, arg2.getName(), filteredFollowsPair.second);
			addAndProcessIntermediateSynonyms(LHS, RHS);

			if (filteredFollowsPair.first.size() == 0 || filteredFollowsPair.second.size() == 0) {
				return false;
			}
			return true;
		}
		return true;
	}

	bool processFollowsS(Synonym arg1, Synonym arg2) {
		bool isValid = validateFollowsArgs(arg1, arg2);

		if (!isValid || AbstractWrapper::GlobalStop) {
			return false;
		}

		string arg1Type = arg1.getType();
		string arg2Type = arg2.getType();

		if (arg1Type == "String" && arg2Type == "String") {
			return pkb.isFollows(stoi(arg1.getName()), stoi(arg2.getName()));
		} else if (arg1Type == "String") {
			// Given stmtNum1, get stmtNum2 such that Follows(stmt1, stmt2) is satisfied
			vector<int> stmt = pkb.getStmtFollowedFrom(stoi(arg1.getName()));
			Synonym synonym(arg2Type, arg2.getName(), stmt);
			addAndProcessIntermediateSynonym(synonym);
		} else if (arg2Type == "String") {
			// Given stmtNum2, get stmtNum1 such that Follows(stmt1, stmt2) is satisfied
			vector<int> stmt = pkb.getStmtFollowedTo(stoi(arg2.getName()));
			Synonym synonym(arg1Type, arg1.getName(), stmt);
			addAndProcessIntermediateSynonym(synonym);
		} else {
			pair<vector<int>, vector<int>> allFollowsPair = pkb.getAllFollowsPairs(false);  //false for non transitive closure
			pair<vector<int>, vector<int>> filteredFollowsPair = 
				filterPairWithSynonymType(allFollowsPair, arg1Type, arg2Type);

			Synonym LHS(arg1Type, arg1.getName(), filteredFollowsPair.first);
			Synonym RHS(arg2Type, arg2.getName(), filteredFollowsPair.second);
			addAndProcessIntermediateSynonyms(LHS, RHS);

			if (filteredFollowsPair.first.size() == 0 || filteredFollowsPair.second.size() == 0) {
				return false;
			}
			return true;
		}
		return true;
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
			arg1Type != "while" && arg1Type != "prog_line") {
				return false;
		}
		if (arg2Type != "String" && arg2Type != "variable" && arg2Type != "_") {
			return false;
		}

		if (arg1Type != "String" && arg2Type != "String" && arg1.getName() == arg2.getName()) {
			return false;  //arg1 and arg2 cannot have the same names if they are synoyms
		}

		//If it reaches here, the types are valid. Check for the values of arg1 if it is a constant string
		if (arg1Type == "String") {
			char arg1Value = arg1.getName()[0];  //Get the value of arg1
			if (!isdigit(arg1Value)) {
				return false;  //arg1 must be a digit
			}
		} else {
			//Check that this synonym exists in the synonymVector
			/* if (findSynonymWithName(arg1.getName()).getName() == "-1") {
			return false;
			} */
		}

		//If it reaches here, arg1 is valid. Check for the values of arg2 if it is a constant string
		if (arg2Type == "String") {
			int varIndex = pkb.getVarIndex(arg2.getName());
			if (varIndex == -1) {
				//This variable is not found in the var table
				return false;
			}

			char arg2Value = arg2.getName()[0];  //Get the value of arg2
			if (!isalpha(arg2Value)) {
				return false;  //arg2 must be a character
			}
		} else if (arg2Type != "_") {
			//Check that this synonym exists in the synonymVector
			/* if (findSynonymWithName(arg2.getName()).getName() == "-1") {
			return false;
			} */
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
			arg1Type != "while" && arg1Type != "prog_line") {
				return false;
		}
		if (arg2Type != "String" && arg2Type != "stmt" && arg2Type != "assign" && arg2Type != "_" && 
			arg2Type != "while" && arg2Type != "prog_line") {
				return false;
		}

		//If it reaches here, the types are valid
		if (arg1Type == "String") {
			char arg1Value = arg1.getName()[0];  //Get the value of arg1
			if (!isdigit(arg1Value)) {
				return false;  //Check for that arg1 is a number if it is a constant string
			}
		} else if (arg1Type != "_") {
			//Check that this synonym exists in the synonymVector
			/* if (findSynonymWithName(arg1.getName()).getName() == "-1") {
			return false;
			} */
		}

		//If it reaches here, arg1 is valid
		if (arg2Type == "String") {
			char arg2Value = arg2.getName()[0];  //Get the value of arg2
			if (!isdigit(arg2Value)) {
				return false;  //Check that arg2 is a number if it is a constant string
			}
		} else if (arg2Type != "_") {
			//Check that this synonym exists in the synonymVector
			/* if (findSynonymWithName(arg2.getName()).getName() == "-1") {
			return false;
			} */
		}

		if (arg1Type == "String" && arg2Type == "String") {
			//Since the two are constant strings, they must be digits by the checks above
			if (stoi(arg1.getName()) >= stoi(arg2.getName())) {
				return false;  //arg1 must be smaller than arg2 or else it is false
			}
		}

		if (arg1Type != "String" && arg2Type != "String" && 
			arg1Type != "_" && arg2Type != "_" && arg1.getName() == arg2.getName()) {
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
			arg1Type != "while" && arg1Type != "prog_line") {
				return false;
		}
		if (arg2Type != "String" && arg2Type != "stmt" && arg2Type != "assign" && arg2Type != "_" && 
			arg2Type != "while" && arg2Type != "prog_line") {
				return false;
		}

		//If it reaches here, the types are valid
		if (arg1Type == "String") {
			char arg1Value = arg1.getName()[0];  //Get the value of arg1
			if (!isdigit(arg1Value)) {
				return false;  //Check for that arg1 is a number if it is a constant string
			}
		} else if (arg1Type != "_") {
			//Check that this synonym exists in the synonymVector
			/* if (findSynonymWithName(arg1.getName()).getName() == "-1") {
			return false;
			} */
		}

		//If it reaches here, arg1 is valid
		if (arg2Type == "String") {
			char arg2Value = arg2.getName()[0];  //Get the value of arg2
			if (!isdigit(arg2Value)) {
				return false;  //Check that arg2 is a number if it is a constant string
			}
		} else if (arg2Type != "_") {
			//Check that this synonym exists in the synonymVector
			/* if (findSynonymWithName(arg2.getName()).getName() == "-1") {
			return false;
			} */
		}

		if (arg1Type == "String" && arg2Type == "String") {
			//Since the two are constant strings, they must be digits by the checks above
			if (stoi(arg1.getName()) >= stoi(arg2.getName())) {
				return false;  //arg1 must be smaller than arg2 or else it is false
			}
		}

		if (arg1Type != "String" && arg2Type != "String" && 
			arg1Type != "_" && arg2Type != "_" && arg1.getName() == arg2.getName()) {
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

		if (arg1Type == "variable") {
			//If LHS is a variable synonym, use the return statements to probe the ModifiesTable
			vector<int> LHSVariables;

			for (vector<int>::iterator itr = isMatchStmts.begin(); itr != isMatchStmts.end(); ++itr) {
				vector<int> vars = pkb.getModVarForStmt(*itr);  //Get the LHS of the assign statement
				LHSVariables.push_back(vars[0]);  //Variable for assignment stmts must be in index 0
			}
			Synonym LHS(arg0.getType(), arg0.getName(), isMatchStmts);
			Synonym RHS(arg1Type, arg1.getName(), LHSVariables);
			addAndProcessIntermediateSynonyms(LHS, RHS);

			if (isMatchStmts.size() == 0) {
				return false;
			}
			return true;
		} else if (arg1Type == "_") {
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
			addAndProcessIntermediateSynonym(synonym);
			return true;
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
		if (arg1.getType() == "String") {
			vector<int> stmts = pkb.patternMatchWhile(arg1.getName());
			Synonym synonym(arg0.getType(), arg0.getName(), stmts);
			addAndProcessIntermediateSynonym(synonym);
			return true;
		} else if (arg1Type == "_") {
			return true;  //Do nothing because pattern w(_, _) is always true if there are while statements
		} else {
			//LHS is a variable synonym
			//TODO
			return true;
		}
	}

	/**
	* Method to validate assign patterns
	* Returns true if the arguments of the assign pattern are valid, false otherwise
	*/
	bool validateAssignPatternArgs(Synonym arg0, Synonym arg1, Synonym arg2) {
		string arg1Type = arg1.getType();

		/* if (findSynonymWithName(arg0.getName()).getName() == "-1") {
		//Check if this assign synonym has been declared
		return false;
		} */

		if (arg1Type != "String" && arg1Type != "variable" && arg1Type != "_") {
			return false;
		}

		/* if (arg1Type == "variable" && findSynonymWithName(arg1.getName()).getName() == "-1") {
		//Check if this variable synonym has been declared
		return false;
		} */

		return true;
	}

	/**
	* Method to validate while patterns
	* Returns true if the arguments of the assign pattern are valid, false otherwise
	*/
	bool validateWhilePatternArgs(Synonym arg0, Synonym arg1, Synonym arg2) {
		string arg1Type = arg1.getType();

		/* if (findSynonymWithName(arg0.getName()).getName() == "-1") {
		//Check if this while synonym has been declared
		return false;
		} */

		if (arg1Type != "String" && arg1Type != "variable" && arg1Type != "_") {
			return false;  //arg1 can only be a constant string or a variable synonym or "_"
		}

		/* if (arg1Type == "variable" && findSynonymWithName(arg1.getName()).getName() == "-1") {
		//Check if this variable synonym has been declared
		return false;
		} */

		if (arg2.getName() != "_") {
			return false;  //arg2 must be "_"
		}

		return true;
	}

	int findIntermediateSynonymIndex(string synonymName) {
		for (unsigned int i = 0; i < allIntermediateNames.size(); i++) {
			if (synonymName == allIntermediateNames[i]) {
				return i;
			}
		}
		return -1;
	}

	void addAndProcessIntermediateSynonym(Synonym synonym) {
		if (synonym.getType() == "_") {
			return;
		}

		string name = synonym.getName();
		int synonymIndex = findIntermediateSynonymIndex(name);

		if (synonymIndex == -1) {
			//If this synonym is not yet in the table,
			//get all the default values, intersect and join

			string type = synonym.getType();
			vector<int> allValues;
			set<int> intermediateValues = synonym.getValuesSet();
			set<int> finalValues;

			//Get the default values
			if (type == "variable") {
				allValues = pkb.getAllVarIndex();
			} else if (type == "constant") {
				allValues = pkb.getAllConstant();
			} else {
				allValues = pkb.getStmtNumForType(type);
			}

			//Intersection
			for (unsigned int i = 0; i < allValues.size(); i++) {
				if (isValueExist(intermediateValues, allValues[i])) {
					finalValues.insert(allValues[i]);
				}
			}

			//Join
			Synonym newSynonym(type, name, finalValues);
			joinWithExistingValues(newSynonym);
		} else {
			//This synonym is already in the table
			//Just do intersection with the existing intermediate values
			intersectWithExistingValues(synonymIndex, synonym.getValues());
		}
	}

	void addAndProcessIntermediateSynonyms(Synonym LHS, Synonym RHS) {
		if (LHS.getType() == "_" && RHS.getType() == "_") {
			return;
		} else if (LHS.getType() == "_") {
			addAndProcessIntermediateSynonym(RHS);
		} else if (RHS.getType() == "_") {
			addAndProcessIntermediateSynonym(LHS);
		}

		int indexLHS = findIntermediateSynonymIndex(LHS.getName());
		int indexRHS = findIntermediateSynonymIndex(RHS.getName());

		if (indexLHS == -1 && indexRHS == -1) {
			//Both LHS and RHS are not in the table
			//Cartesian product the two values with the current table
			joinWithExistingValues(LHS, RHS);
		} else if (indexLHS == -1) {
			//LHS is not in the table while RHS is
			//Use RHS to intersect with the table values then join with the LHS
			intersectAndJoinWithExistingValues(indexRHS, RHS, LHS);
		} else if (indexRHS == -1) {
			//RHS is not in the table while LHS is
			//Use LHS to intersect with the table values then join with the RHS
			intersectAndJoinWithExistingValues(indexLHS, LHS, RHS);
		} else {
			//Both LHS and RHS are in the table
			//Use both the values to intersect with the table
			intersectWithExistingValues(indexLHS, LHS.getValues(), indexRHS, RHS.getValues());
		}
	}


	/**
	* Helper method to do a cartesian product of the intermediate synonym values with other values
	*/
	void joinWithExistingValues(Synonym synonym) {
		vector<int> synonymValues = synonym.getValues();
		vector<vector<int>> acceptedValues;

		allIntermediateNames.push_back(synonym.getName());

		if (allIntermediateValues.size() == 0) {
			for (vector<int>::iterator itr = synonymValues.begin(); itr != synonymValues.end(); ++itr) {
				vector<int> newRow;
				newRow.push_back(*itr);
				acceptedValues.push_back(newRow);
			}
		} else {
			for (vector<int>::iterator itr = synonymValues.begin(); itr != synonymValues.end(); ++itr) {
				for (unsigned int i = 0; i < allIntermediateValues.size(); i++) {
					vector<int> newRow(allIntermediateValues[i]);
					newRow.push_back(*itr);
					acceptedValues.push_back(newRow);
				}
			}
		}

		swap(allIntermediateValues, acceptedValues);
	}

	/**
	* Helper method to do a cartesian product of the intermediate synonym values with other values
	* Overloaded method which takes in two arguments instead of one
	*/
	void joinWithExistingValues(Synonym LHS, Synonym RHS) {
		vector<int> valuesLHS = LHS.getValues();
		vector<int> valuesRHS = RHS.getValues();
		vector<vector<int>> acceptedValues;

		allIntermediateNames.push_back(LHS.getName());
		allIntermediateNames.push_back(RHS.getName());

		if (allIntermediateValues.size() == 0) {
			for (unsigned int i = 0; i < valuesLHS.size(); i++) {
				vector<int> newRow;
				newRow.push_back(valuesLHS[i]);
				newRow.push_back(valuesRHS[i]);
				acceptedValues.push_back(newRow);
			}
		} else {
			for (unsigned int i = 0; i < valuesLHS.size(); i++) {
				for (unsigned int j = 0; j < allIntermediateValues.size(); j++) {
					vector<int> newRow(allIntermediateValues[j]);
					newRow.push_back(valuesLHS[i]);
					newRow.push_back(valuesRHS[i]);
					acceptedValues.push_back(newRow);
				}
			}
		}
		swap(allIntermediateValues, acceptedValues);
	}

	/**
	* Helper method to do a set intersection of the intermediate values in the table
	*/
	void intersectWithExistingValues(int synonymIndex, vector<int> probeValues) {
		vector<vector<int>> acceptedValues;
		set<int> probeValuesSet;

		for (unsigned int i = 0; i < probeValues.size(); i++) {
			probeValuesSet.insert(probeValues[i]);
		}

		for (unsigned int i = 0; i < allIntermediateValues.size(); i++) {
			if (isValueExist(probeValuesSet, (allIntermediateValues[i])[synonymIndex])) {
				acceptedValues.push_back(allIntermediateValues[i]);
			}
		}
		swap(allIntermediateValues, acceptedValues);
	}

	/**
	* Helper method to do a set intersection of the intermediate values in the table
	* Overloaded method which takes in two pairs of arguments instead of one pair
	*/
	void intersectWithExistingValues(int indexLHS, vector<int> LHSValues, int indexRHS, vector<int> RHSValues) {
		vector<vector<int>> acceptedValues;

		for (unsigned int i = 0; i < allIntermediateValues.size(); i++) {
			for (unsigned int j = 0; j < LHSValues.size(); j++) {
				if (allIntermediateValues[i][indexLHS] == LHSValues[j] && allIntermediateValues[i][indexRHS] == RHSValues[j]) {
					acceptedValues.push_back(allIntermediateValues[i]);
				}
			}
		}
		swap(allIntermediateValues, acceptedValues);
	}

	void intersectAndJoinWithExistingValues(int existingIndex, Synonym probe, Synonym newValues) {
		vector<vector<int>> acceptedValues;
		vector<int> probeValues = probe.getValues();

		allIntermediateNames.push_back(newValues.getName());

		for (unsigned int i = 0; i < allIntermediateValues.size(); i++) {
			for (unsigned int j = 0; j < probeValues.size(); j++) {
				if (probeValues[j] == allIntermediateValues[i][existingIndex]) {
					vector<int> newRow(allIntermediateValues[i]);
					newRow.push_back(probeValues[j]);
					acceptedValues.push_back(newRow);
				} else if (probeValues[j] > allIntermediateValues[i][existingIndex]) {
					break;  //Stop looping the inner probe values if it exceeds the outer value
				}
			}
		}

		swap(allIntermediateValues, acceptedValues);
	}

	/**
	* Helper method to get the synonym with its final values
	* Gets the default values if this synonym is not in the intermediate values
	* Otherwise, performs set intersection with the intermediate values
	* Returns a new synonym with the final values
	*/
	Synonym findSynonymWithName(string wantedName) {
		unordered_map<string, string>::iterator itr = synonymMap.find(wantedName);

		string name = itr->first;
		string type = itr->second;		
		int synonymIndex = findIntermediateSynonymIndex(name);

		if (synonymIndex == -1) {
			vector<int> synonymValues;
			if (type == "variable") {
				synonymValues = pkb.getAllVarIndex();
			} else if (type == "constant") {
				synonymValues = pkb.getAllConstant();
			} else {
				synonymValues = pkb.getStmtNumForType(type);
			}

			Synonym synonym(type, name, synonymValues);
			return synonym;
		} else {
			set<int> intermediateValues = getIntermediateValues(synonymIndex);
			Synonym synonym(type, name, intermediateValues);
			return synonym;
		}
	}

	set<int> getIntermediateValues(int synonymIndex) {
		set<int> intermediateValues;
		for (unsigned int i = 0; i < allIntermediateValues.size(); i++) {
			intermediateValues.insert(allIntermediateValues[i][synonymIndex]);
		}
		return intermediateValues;
	}

	/**
	* Helper method to check if a particular value is contained in the set
	*/
	inline bool isValueExist(set<int> setToSearch, int value) {
		int count = setToSearch.count(value);

		if (count >= 1) {
			return true;
		}
		return false;
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
		for (unsigned int i = 0; i < allPairs.first.size(); i++) {
			int firstElement = allPairs.first[i];
			int secondElement = allPairs.second[i];

			if (arg1Type == "stmt" || arg1Type == "prog_line" || arg1Type == "_") {
				if (arg2Type == "stmt" || arg2Type == "prog_line" || arg2Type == "variable" || arg2Type == "_") {
					return allPairs;
				} else if (arg2Type == "constant" && pkb.isConstant(secondElement)) {
					//The constant table has been probed and arg2 constant value exists
					filteredFirstElements.push_back(firstElement);
					filteredSecondElements.push_back(secondElement);
				} else if (arg2Type == pkb.getType(secondElement)) {
					filteredFirstElements.push_back(firstElement);
					filteredSecondElements.push_back(secondElement);
				}
			} else if (arg1Type == "constant" && pkb.isConstant(firstElement)) {
				//The constant table has been probed and arg1 constant value exists
				if (arg2Type == "stmt" || arg2Type == "prog_line" || arg2Type == "variable" || arg2Type == "_") {
					filteredFirstElements.push_back(firstElement);
					filteredSecondElements.push_back(secondElement);
				} else if (arg2Type == "constant" && pkb.isConstant(secondElement)) {
					//The constant table has been probed and arg2 constant value exists
					filteredFirstElements.push_back(firstElement);
					filteredSecondElements.push_back(secondElement);
				} else if (arg2Type == pkb.getType(secondElement)) {
					filteredFirstElements.push_back(firstElement);
					filteredSecondElements.push_back(secondElement);
				}
			} else if (arg1Type == pkb.getType(firstElement)) {
				if (arg2Type == "stmt" || arg2Type == "prog_line" || arg2Type == "variable" || arg2Type == "_") {
					filteredFirstElements.push_back(firstElement);
					filteredSecondElements.push_back(secondElement);
				} else if (arg2Type == "constant" && pkb.isConstant(secondElement)) {
					//The constant table has been probed and arg2 constant value exists
					filteredFirstElements.push_back(firstElement);
					filteredSecondElements.push_back(secondElement);
				} else if (arg2Type == pkb.getType(secondElement)) {
					filteredFirstElements.push_back(firstElement);
					filteredSecondElements.push_back(secondElement);
				}
			}
		}

		pair<vector<int>, vector<int>> filteredPairs(filteredFirstElements, filteredSecondElements);
		return filteredPairs;
	}
}
