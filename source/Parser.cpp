#pragma once

#include<stdio.h>
#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <sstream>
#include <regex>

#include "PKB.h"
#include "AST.h"
#include "VarTable.h"
#include "StmtTable.h"
#include "FollowsTable.h"
#include "ConstantTable.h"
#include "TNode.h"
#include "SourceParser.h"
#include "ExpressionParser.h"

using std::string;
using std::vector;

namespace Parser {
	ifstream inputFile;

	vector<string> buffer;
	vector<string>::iterator bufferIter;
	string currentParsedLine;
	
	vector<int> stmtListParent;
	vector<TNode*> ASTParent;

	int stmtNum;
	int progLineNum;

	bool isEof() {
		return inputFile.eof();
	}

	/**
	 * Initialise the parser with a filename
	 */
	bool initParser(string filename) {
		inputFile.open(filename);
		stmtNum = 0;
		progLineNum = 0;

		stmtListParent = vector<int>();
		
		return inputFile.is_open(); 
	}

	/**
	 * Returns the next line of input as a vector of tokens.
	 */
	bool parseLine() {
		if (!inputFile.is_open()) {
			throw new exception("Parse failure: Parser not initialised before parsing functions were called");
		}
	
		buffer.clear();

		string line;
		getline(inputFile, line);

		// drop everything after 2 backslash
		int pos;
		if ((pos = line.find("\\")) != string::npos) {
			line = line.substr(0, pos);
		}

		if (line.empty()) {
			if (inputFile.eof()) {
				inputFile.close();
				return false;
			}
			return parseLine();
		}
		progLineNum += 1;
		currentParsedLine = line;

		// remove blocks of multiple whitespace
		string multipleSpaces = "[\\s]+";
		std::tr1::regex separatorRegex(multipleSpaces);
		std::tr1::sregex_token_iterator reg_end;

		std::tr1::sregex_token_iterator rs(line.begin(), line.end(), separatorRegex, -1);

		// tokenise words and operators
		string operators = "([\\w\\d]+|[+=;{}])";
		std::regex operRegex(operators);

		for (; rs != reg_end; ++rs) {
			std::smatch match;
			string res(rs->str());
			while (std::regex_search(res, match, operRegex)) {
				if (match.empty()) {
					break;
				}
				buffer.push_back(match[0]);
				res = match.suffix().str();
			} 
		}
	
		return buffer.size() > 0;
	}

	/**
	 * Returns the next token
	 */
	string parseToken() {
		if (buffer.size() && bufferIter != buffer.end()) {
			return *(bufferIter ++);
		} else {
			bool res = parseLine();
			if (!res) {
				return parseToken();
			} else {
				bufferIter = buffer.begin();
				return *(bufferIter ++);
			}
		}
	}

	TNode* currentASTParent() {
		return ASTParent[ASTParent.size() - 1];
	}

	bool callPkb(string designEntity, string LHS, string RHS) {
		PKB pkb = PKB::getInstance();
		if (designEntity.compare("VarTable") == 0) {
			pkb.insertVar(RHS, atoi(LHS.c_str()));
	
			return true;
		} else if (designEntity.compare("StmtTable") == 0) {
			pkb.insertStmt(atoi(LHS.c_str()), RHS);
	
			return true;
		} else if (designEntity.compare("Modifies") == 0) {
			int modifiedVarIndex = pkb.getVarIndex(RHS);
			int statement = atoi(LHS.c_str());
			pkb.setModifies(statement, modifiedVarIndex);


			while (pkb.getParent(statement).size()) {
				statement = pkb.getParent(statement).at(0);
				if (statement > 0) {
					pkb.setModifies(statement, modifiedVarIndex);	
				}
			}
			
			return true;
		} else if (designEntity.compare("Uses") == 0) {
			int statement = atoi(LHS.c_str());
			int usedVarIndex = pkb.getVarIndex(RHS);
			
			pkb.setUses(statement, usedVarIndex);
			
			while (pkb.getParent(statement).size()) {
				statement = pkb.getParent(statement).at(0);
				if (statement > 0) {
					pkb.setUses(statement, usedVarIndex);	
				}
			}
			
			return true;
		} else if (designEntity.compare("ConstantTable") == 0) {
			pkb.insertConstant(atoi(RHS.c_str()), atoi(LHS.c_str()));
			
			return true;
		}

		return false;
	}

	/**
	 * Parses the next token and check if it is equal to the given target
	 */
	bool parse(string target) {
		string nextToken = parseToken();
		return nextToken.compare(target) == 0;
	}

	bool match(string token, string target) {
		return token.compare(target) == 0;
	}

	/**
	 * Matches if the given token follows the naming convention for NAME, as per the given grammar
	 */
	string matchName(string token) {
		std::regex nameRegex("[A-Za-z][\\w]*");
		return std::regex_match(token, nameRegex) ? token : "";
	}

	/**
	 * Matches if the given token follows the naming convention for INTEGER, as per the given grammar
	 */
	string matchInteger(string token) {
		std::regex intRegex("\\d+");
		return (std::regex_match(token,intRegex)) ? token : "";
	}

	string matchOperator(string token) {
		std::regex operRegex("[;\\+-*=]");
		return (std::regex_match(token, operRegex)) ? token: "";
	}

	string parseFactorConstantAndOperator(vector<TNode*>* listOfNodes) {
		string nextToken = parseToken();
		string res = matchName(nextToken);
	
		bool isName = !res.empty();
		if (isName) {
			callPkb("VarTable", std::to_string(static_cast<long long>(stmtNum)), res);
			callPkb("Uses", std::to_string(static_cast<long long>(stmtNum)), res);

			listOfNodes->push_back(PKB::getInstance().createTNode(Variable, 
								   stmtNum, PKB::getInstance().getVarIndex(res)));
		} else { 
			res = matchInteger(nextToken);

			bool isConstant = !res.empty();
			if (isConstant) {
				callPkb("ConstantTable", std::to_string(static_cast<long long>(stmtNum)), res);

				listOfNodes->push_back(PKB::getInstance().createTNode(Constant, 
									   stmtNum, atoi(res.c_str())));
			} else {  
				// not a number either, therefore is an operator
				res = matchOperator(nextToken);
			}
		}

		return res;
	}

	/**
	 * Parses an expression. 
	 * @param exprRoot the root of the expression, if the expr consists of only a variable, this should be the assignNode itself,
	 *                 otherwise, the first plusNode
	 * Assumptions: LHS = RHS => [LHS] and [=] took up 2 tokens in the buffer, and has already been parsed
	 *              The expression only consists of factors and '+'
	 */
	bool parseExpr(TNode* exprRoot) {
		bool res = true;
		const int alreadyParsed = 2;
		int bufferSize = buffer.size() - alreadyParsed;

		ExpressionParser exprParser;
		exprParser.updateBuffer(buffer); 
		exprParser.updateStmtNum(stmtNum);

		for (int i = 0; i < buffer.size() ; i++) {
			cout << buffer[i] << "  ";
		}
		cout << endl;

		TNode* top = exprParser.parse();


		PKB::getInstance().createLink(Child, exprRoot, top);

		/*vector<TNode*>* listOfNodes = new vector<TNode*>();
		for (int i = 0; i < bufferSize; i++) {  
			string res = parseFactorConstantAndOperator(listOfNodes);

			if (res.empty()) {
				return false;
			} else if (match(res, ";")) {
				break;
			}
		}

		// add the expression to the AST
		PKB pkb = PKB::getInstance();

		TNode* parentNode = exprRoot;
		
		if (!listOfNodes->empty()) {
			for (int i = listOfNodes->size() - 1; i >= 2; --i) {
				TNode* nextParentNode = pkb.createTNode(Plus, stmtNum, -2);
				
				pkb.createLink(Child, parentNode, nextParentNode);
				pkb.createLink(Child, parentNode, listOfNodes->at(i)); 
					
				parentNode = nextParentNode;
			}

			pkb.createLink(Child, parentNode, listOfNodes->at(0));
			if (listOfNodes->size() > 1) {
				pkb.createLink(Child, parentNode, listOfNodes->at(1));
			} 
		}
		*/

		cout << "drop curline" << endl;
		//parseLine(); // drop the current line, which is the assignment statement
		while ((bufferIter++)->compare(";") != 0) {
		}

		return res;
	}

	string parseName() {
		string token = parseToken();
		return matchName(token);
	}

	bool parseStmtList();

	bool parseWhile(string token) {
		bool res = match(token, "while"); 
		if (!res) return false;

		stmtListParent.push_back(stmtNum);

		string varName = parseName();
		if (varName.empty()) return false;

		callPkb("VarTable", std::to_string(static_cast<long long>(stmtNum)), varName);

		res = parse("{");
		if (!res) return false;

		// AST interactions
		AST* ast = PKB::getInstance().ast;
		PKB pkb = PKB::getInstance();

		TNode* node = pkb.createTNode(While, stmtNum, -1);
		TNode* LHSNode = pkb.createTNode(Variable, stmtNum, 
			pkb.getVarIndex(varName));
		pkb.createLink(Child, node, LHSNode);
		TNode* stmtlistNode = pkb.createTNode(StmtLst, stmtNum, -1);
		pkb.createLink(Child, node, stmtlistNode);

		pair<int, TNode*> stmtNumToNodePair(stmtNum, node);
		PKB::getInstance().nodeTable.insert(stmtNumToNodePair);

		TNode* prevStmtInStmtList = currentASTParent()->hasChild() ? currentASTParent()->getChildren()->back() : NULL;
		if (prevStmtInStmtList != NULL) {
			pkb.setFollows(prevStmtInStmtList, node);
		}

		
		pkb.setParent(currentASTParent(), node);

		int whileStmtNum = stmtNum;
		ASTParent.push_back(stmtlistNode);
		parseStmtList();
		
		ASTParent.erase(ASTParent.end() - 1); // remove the while node from ASTParent
		
		callPkb("Uses", std::to_string(static_cast<long long>(whileStmtNum)), varName);
		
		return res;
	}

	bool parseAssign(string firstToken) {
		string varName = matchName(firstToken);

		if (varName.size() == 0) return false;
		callPkb("VarTable", std::to_string(static_cast<long long>(stmtNum)), varName);

		bool res = parse("=");
		if (!res) return false;
		

		// AST interactions
		AST* ast = PKB::getInstance().ast;
		PKB pkb = PKB::getInstance();
		TNode* node = pkb.createTNode(Assign, stmtNum, -1);
		TNode* LHSNode = pkb.createTNode(Variable, stmtNum, 
			pkb.getVarIndex(varName));

		pkb.createLink(Child, node, LHSNode);

		TNode* RHSNode = node;  // pass the assignNode directly to parseExpr, which will attach the variable/constant to it

		
		pair<int, TNode*> stmtNumToNodePair(stmtNum, node);
		PKB::getInstance().nodeTable.insert(stmtNumToNodePair);


		TNode* prevStmtInStmtList = currentASTParent()->hasChild() ? currentASTParent()->getChildren()->back() : NULL;
		if (prevStmtInStmtList) {
			PKB::getInstance().setFollows(prevStmtInStmtList, node);
		}

		PKB::getInstance().setParent(currentASTParent(), node);
		
		res = parseExpr(RHSNode);
		
		callPkb("Modifies", std::to_string(static_cast<long long>(stmtNum)), varName);
		
		return res;
	}


	/**
	 * Matches a statement.
	 * Assumptions: stmt can only be assignment or while
	 * @TODO rewrite in cs3202
	 */
	bool matchStmt(string firstToken) {
		bool res = true;
		
		if (currentParsedLine.find("=") != string::npos) {
			stmtNum += 1;
			
			callPkb("StmtTable", std::to_string(static_cast<long long>(stmtNum)), "assign");
			res = parseAssign(firstToken);
			if (stmtListParent.size() > 0) {
				int parent = stmtListParent[stmtListParent.size() - 1];
				callPkb("Parent", std::to_string(static_cast<long long>(parent)), std::to_string(static_cast<long long>(stmtNum)));
			}
		} else {
			stmtNum += 1;

			callPkb("StmtTable", std::to_string(static_cast<long long>(stmtNum)), "while");
			res = parseWhile(firstToken);

			if (stmtListParent.size() > 0) {
				int parent = stmtListParent[stmtListParent.size() - 1];
				callPkb("Parent", std::to_string(static_cast<long long>(parent)), std::to_string(static_cast<long long>(stmtNum)));
			}
		}

		return res;
	}

	bool parseStmtList() {
		string nextToken = parseToken();
		bool res;

		bool isEndOfStmtList = nextToken.compare("}") == 0;
		while (!isEndOfStmtList) {
			res = matchStmt(nextToken);
			if (!res) {return false; }
			
			nextToken = parseToken();			
			
			isEndOfStmtList = (nextToken.compare("}") == 0) ;
		}
		return true;
	}

	bool parseProcedure() {
		bool res;

		res = parse("procedure");
		if (!res) return false;
		
		string procName = parseName();
		if (procName.size() == 0) return false;
		
		res = parse("{");
		if (!res) return false;
		
		
		ASTParent.push_back(PKB::getInstance().getRoot());

		res = parseStmtList();

		if (!res) return false;

		return true;
	}

	void traverseAndPrintTree(TNode* root) {
		vector<TNode*> nodeQueue;

		nodeQueue.push_back(root);
		while (!nodeQueue.empty()) {
			
			cout << nodeQueue.front()->getNodeValueIdx() << " from stmt " << nodeQueue.front()->getStmtNumber() << endl;
			cout << " is a variable node: " << (nodeQueue.front()->getNodeType() == Variable) << endl;
			bool isConstant = (nodeQueue.front()->getNodeType() == Constant);
			bool isPlus = (nodeQueue.front()->getNodeType() == Plus);
			bool isAssign = (nodeQueue.front()->getNodeType() == Assign);

			cout << " isConstant: " << isConstant << " isPlus: " << isPlus << " isAssign: " << isAssign <<  endl;
			cout << "  and parent of it is " << nodeQueue.front()->getParent()->getNodeValueIdx() << " from stmt " << nodeQueue.front()->getParent()->getStmtNumber() << endl;
			vector<TNode*>* children = nodeQueue.front()->getChildren();
			if (nodeQueue.front()->hasChild()) {
				cout << "has child" << endl;
				cout << " has " << (int)children->size() << " children" << endl;
				for (int i =0; i < (int)children->size(); ++i) {
					TNode* node = children->at(i);
					nodeQueue.push_back(node);
				}
			}
			nodeQueue.erase(nodeQueue.begin());
		}

	}

	bool parseProgram() {
		// Assignment 4 only involves program with one procedure, so only one
		// matchProcedure call is neccessary
		bool res = parseProcedure();
		assert(res); // do not evaluate queries if parser has failed
		//cout << "parser has completed the parse" << endl;
		//traverseAndPrintTree(PKB::getInstance().getRoot());
		return res;
	}
	
}

