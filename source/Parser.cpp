#pragma once

#include<stdio.h>
#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <sstream>
#include <regex>
#include <exception>
#include <algorithm>

#include "PKB.h"
#include "AST.h"
#include "VarTable.h"
#include "StmtTable.h"
#include "FollowsTable.h"
#include "ConstantTable.h"
#include "TNode.h"
#include "SourceParser.h"
#include "ExpressionParser.h"

/**
	@brief Namespace containing functions for parsing the source SIMPLE file as well as utility functions to parse and match strings from
			the SIMPLE grammar

 */
namespace Parser {
	using std::string;
	using std::vector;
	using std::regex;

	namespace var { // contains state of the parser
		ifstream inputFile;

		vector<string> buffer;
		vector<string>::iterator bufferIter;
		string currentParsedLine;
	
		vector<int> stmtListParent;
		vector<TNode*> astParent;

		int stmtNum;
		int progLineNum;
		int currentProcIndex;
	}

	/**
	 * Initialises and prepares the parser for parsing. 
	 * The file specified by filename is opened for parsing. 
	 * Return TRUE if the file can be successfully opened. Otherwise, return FALSE.
	 */
	bool initParser(string filename) {
		using namespace Parser::var;
		inputFile.open(filename);
		stmtNum = 0;
		progLineNum = 0;

		stmtListParent = vector<int>();
		
		return inputFile.is_open(); 
	}

	namespace util {
		using namespace Parser::var;
		
		bool parseStmtList();

		/**
		 * Returns the next line of input as a vector of tokens.
		 */
		bool parseLine() 
		{
			if (!inputFile.is_open()) {
				throw new logic_error("Parse failure: Parser not initialised before parsing functions were called");
			}
	
			buffer.clear();

			string line;
			getline(inputFile, line);
			progLineNum += 1;

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
			
			currentParsedLine = line;

			// remove blocks of multiple whitespace
			string multipleSpaces = "[\\s]+";
			regex separatorRegex(multipleSpaces);
			sregex_token_iterator reg_end;

			sregex_token_iterator rs(line.begin(), line.end(), separatorRegex, -1);

			// tokenise words and operators
			string operators = "([\\w\\d]+|[*\\-+=;{}\\(\\)])";
			regex operRegex(operators);

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
					return inputFile.eof()? "" : parseToken();
				} else {
					bufferIter = buffer.begin();
					return *(bufferIter ++);
				}
			}
		}

		string peekToken() {
			if (buffer.size() && bufferIter != buffer.end()) {
				return *(bufferIter);
			} else {
				bool res = parseLine();
				if (!res) {
					return inputFile.eof()? "" : peekToken();
				} else {
					bufferIter = buffer.begin();
					return *(bufferIter);
				}
			}

		}

		bool isEof() {
			if (peekToken().empty()) {
				return inputFile.eof();
			} else {
				return false;
			}
		}


		TNode* currentASTParent() {
			return astParent[astParent.size() - 1];
		}

		bool callPkb(string designEntity, string LHS, string RHS) {
			PKB pkb = PKB::getInstance();
			if (designEntity.compare("VarTable") == 0) {
				pkb.insertVar(RHS, atoi(LHS.c_str()));
	
				return true;
			} else if (designEntity.compare("StmtTable") == 0) {
//				pkb.insertStmt(atoi(LHS.c_str()), RHS);
	
				return true;
			} else if (designEntity.compare("Modifies") == 0) {
				int modifiedVarIndex = pkb.getVarIndex(RHS);
				int statement = atoi(LHS.c_str());
				return true;
			} else if (designEntity.compare("Uses") == 0) {
				int statement = atoi(LHS.c_str());
				int usedVarIndex = pkb.getVarIndex(RHS);
				return true;
			} else if (designEntity.compare("ConstantTable") == 0) {
				pkb.insertConstant(atoi(RHS.c_str()), atoi(LHS.c_str()));
			
				return true;
			} else if (designEntity.compare("ProcTable") == 0) {
				pkb.insertProc(LHS);

				return true;
			} else if (designEntity.compare("CallsTable") == 0) {
				int leftProc = atoi(LHS.c_str());
				int rightProc = atoi(RHS.c_str());
				pkb.setCalls(leftProc, rightProc);

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


		bool createVariablesForExpr(vector<string> tokenizedBuffer) {
			bool status = true;
			for (int i = 0; i < tokenizedBuffer.size(); i++) {
				
				bool isVariable = !matchName(tokenizedBuffer[i]).empty();
				if (!isVariable) {
					continue;
				}

				string varName = tokenizedBuffer[i];
				
				int varIndex = PKB::getInstance().insertVar(varName, stmtNum);
				status &= (varIndex != -1 );
			}
			return status;
		}

		/**
		 * Parses an expression. 
		 * @param exprRoot the root of the expression, this should be the assignNode itself.
		 * Assumptions: LHS = RHS => [LHS] and [=] took up 2 tokens in the buffer, and has already been parsed
		 *              The expression only consists of factors and '+'
		 */
		bool parseExpr(TNode* exprRoot) {
			bool res = true;
			const int alreadyParsed = 2;
			
			vector<string>::const_iterator first = buffer.begin() + alreadyParsed;
			vector<string>::const_iterator last = buffer.end();
			vector<string> slicedBuffer(first, last);

			ExpressionParser exprParser;
			createVariablesForExpr(slicedBuffer);

			exprParser.updateStmtNum(stmtNum);
			exprParser.updateProcIndex(currentProcIndex);
			TNode* top = exprParser.parseExpressionForAST(slicedBuffer);

			PKB::getInstance().createLink(Child, exprRoot, top);

			while ((bufferIter++)->compare(";") != 0) 
			{}

			return res;
		}

		string parseName() {
			string token = parseToken();
			return matchName(token);
		}

		bool parseIf(string token) {
			// match if
			bool res = match(token, "if"); 
			if (!res) return false;

			// match variable
			string varName = parseName();

			if (varName.size() == 0) return false;
			callPkb("VarTable", std::to_string(static_cast<long long>(stmtNum)), varName);

			res = parse("then");
			res = parse("{");
			if (!res) return false;


			// AST interactions
			AST* ast = PKB::getInstance().ast;
			PKB pkb = PKB::getInstance();

			TNode* node = pkb.createTNode(If, stmtNum, -1);
			TNode* LHSNode = pkb.createTNode(Variable, stmtNum, 
				pkb.getVarIndex(varName));
			// control variable as first child
			pkb.createLink(Child, node, LHSNode); 
			TNode* stmtlistNode = pkb.createTNode(StmtLst, stmtNum, -1);
			// stmtList node as second child
			pkb.createLink(Child, node, stmtlistNode); 

			//pair<int, TNode*> stmtNumToNodePair(stmtNum, node);
			//PKB::getInstance().nodeTable.insert(stmtNumToNodePair);
			PKB::getInstance().insertStmt(stmtNum, "if", node, currentProcIndex);

			// Follows from prev stmt in the stmt list
			TNode* prevStmtInStmtList = currentASTParent()->hasChild() ? currentASTParent()->getChildren()->back() : NULL;
			if (prevStmtInStmtList != NULL) {
				pkb.setFollows(prevStmtInStmtList, node);
			}

			// Set Parent
			pkb.setParent(currentASTParent(), node);

			int ifStmtNum = stmtNum;
			astParent.push_back(stmtlistNode);
			// parse inner stmt list
			parseStmtList();
			// remove the stmtList node from ASTParent
			astParent.erase(astParent.end() - 1); 

			// parse optional "else"
			string nextToken = peekToken();
			bool optionalElse = match(nextToken, "else");
			if (optionalElse) {
				res = parse("else");
				
				TNode* stmtlist2Node = pkb.createTNode(StmtLst, ifStmtNum, -1);
				// 2nd stmtList node as third child
				pkb.createLink(Child, node, stmtlist2Node); 
				astParent.push_back(stmtlist2Node);

				res = parse("{");
				if (!res) return false;
				
				parseStmtList();
				// remove the stmtList node from ASTParent
				astParent.erase(astParent.end() - 1);
			} 

			//callPkb("Uses", std::to_string(static_cast<long long>(ifStmtNum)), varName);
		
			return res;
		}

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

			//pair<int, TNode*> stmtNumToNodePair(stmtNum, node);
			//PKB::getInstance().nodeTable.insert(stmtNumToNodePair);
			PKB::getInstance().insertStmt(stmtNum, "while", node, currentProcIndex);

			// Follows from prev stmt in the stmt list
			TNode* prevStmtInStmtList = currentASTParent()->hasChild() ? currentASTParent()->getChildren()->back() : NULL;
			if (prevStmtInStmtList != NULL) {
				pkb.setFollows(prevStmtInStmtList, node);
			}

			// Set Parent
			pkb.setParent(currentASTParent(), node);

			int whileStmtNum = stmtNum;
			astParent.push_back(stmtlistNode);
			// parse inner stmt list
			parseStmtList();
		
			// remove the while node from ASTParent
			astParent.erase(astParent.end() - 1); 
		
			//callPkb("Uses", std::to_string(static_cast<long long>(whileStmtNum)), varName);
		
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

		
			//pair<int, TNode*> stmtNumToNodePair(stmtNum, node);
			//PKB::getInstance().nodeTable.insert(stmtNumToNodePair);
			PKB::getInstance().insertStmt(stmtNum, "assign", node, currentProcIndex);


			TNode* prevStmtInStmtList = currentASTParent()->hasChild() ? currentASTParent()->getChildren()->back() : NULL;
			if (prevStmtInStmtList) {
				PKB::getInstance().setFollows(prevStmtInStmtList, node);
			}
			PKB::getInstance().setParent(currentASTParent(), node);
		
			res = parseExpr(RHSNode);
		
			//callPkb("Modifies", std::to_string(static_cast<long long>(stmtNum)), varName);
		
			return res;
		}

		bool parseCall(string firstToken) {
			string procName = parseName();
			if (procName.size() == 0) return false;

			PKB pkb = PKB::getInstance();
			int index = pkb.insertProc(procName);
		
			// AST interactions
			AST* ast = PKB::getInstance().ast;
			TNode* node = pkb.createTNode(Call, stmtNum, index);
		
			//pair<int, TNode*> stmtNumToNodePair(stmtNum, node);
			//PKB::getInstance().nodeTable.insert(stmtNumToNodePair);
			PKB::getInstance().insertStmt(stmtNum, "call", node, currentProcIndex);

			TNode* prevStmtInStmtList = currentASTParent()->hasChild() ? currentASTParent()->getChildren()->back() : NULL;
			if (prevStmtInStmtList) 
			{
				PKB::getInstance().setFollows(prevStmtInStmtList, node);
			}
			PKB::getInstance().setParent(currentASTParent(), node);
		
			int calledProcIndex = pkb.getProcIndex(procName);
			callPkb("CallsTable", std::to_string(static_cast<long long>(currentProcIndex)), std::to_string(static_cast<long long>(calledProcIndex)));
	
			string nextToken = parseToken();
			return match(nextToken, ";");
		}


		/**
		 * Matches a statement.
		 */
		bool matchStmt(string firstToken) {
			bool res = true;
			if (currentParsedLine.find("=") == string::npos && firstToken != "while" && firstToken != "if" && firstToken != "call") {
				return true;
			}

			stmtNum += 1;
			PKB::getInstance().stmtToProcMap.insert(make_pair<int, int>(stmtNum, currentProcIndex));
			PKB::getInstance().stmtNumToProcLineMap.insert(make_pair<int, int>(stmtNum, progLineNum));

			if (currentParsedLine.find("=") != string::npos) {
				res = parseAssign(firstToken);
			} else if (firstToken == "while") {
				res = parseWhile(firstToken);
			} else if (firstToken == "if") {
				res = parseIf(firstToken);	
			} else if (firstToken == "call") {
				res = parseCall(firstToken);
			}

			if (stmtListParent.size() > 0)  {
				int parent = stmtListParent[stmtListParent.size() - 1];
				callPkb("Parent", std::to_string(static_cast<long long>(parent)), std::to_string(static_cast<long long>(stmtNum)));
			}

			return res;
		}

		bool parseStmtList() {
			string nextToken = parseToken();
			bool res;

			bool isEndOfStmtList = nextToken.compare("}") == 0;
			while (!isEndOfStmtList) {
				res = matchStmt(nextToken);
				if (!res) {
					return false; 
				}
			
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
		
			// add procName to the procTable
			callPkb("ProcTable", procName, procName);
			PKB pkb = PKB::getInstance();
			currentProcIndex = pkb.getProcIndex(procName);

			res = parse("{");
			if (!res) return false;
		
			// attach procedure to root, make stmtlist node
			TNode* root = pkb.getRoot();
			int procIndex = pkb.getProcIndex(procName);
			TNode* procNode = pkb.createTNode(Procedure, 0, procIndex);
			pkb.createLink(Child, root, procNode);
			TNode* stmtListNode = pkb.createTNode(StmtLst, 0, 0);
			pkb.createLink(Child, procNode, stmtListNode);
		
			astParent.push_back(stmtListNode);

			res = parseStmtList();  

			// remove the stmtlist node from ASTParent
			astParent.erase(astParent.end() - 1); 

			return res;
		}

		void traverseAndPrintTree(TNode* root) {
			vector<TNode*> nodeQueue;

			nodeQueue.push_back(root);
			while (!nodeQueue.empty()) {
				bool isConstant = (nodeQueue.front()->getNodeType() == Constant);
				bool isVariable = (nodeQueue.front()->getNodeType() == Variable);
				bool isPlus = (nodeQueue.front()->getNodeType() == Plus);
				bool isAssign = (nodeQueue.front()->getNodeType() == Assign);
				bool isStmtLst = (nodeQueue.front()->getNodeType() == StmtLst);

				cout << *(nodeQueue.front()) << endl;
				cout << " isConstant: " << isConstant << " isVariable:" << isVariable << " isPlus: " << isPlus << " isAssign: " << isAssign << " isStmtLst" << isStmtLst << endl;
				
				if (nodeQueue.front()->getParent() != NULL){
					cout << "  parent: " << nodeQueue.front()->getParent()->getNodeValueIdx() << " from stmt " << nodeQueue.front()->getParent()->getStmtNumber() << endl;
				}
			
				vector<TNode*>* children = nodeQueue.front()->getChildren();
				if (nodeQueue.front()->hasChild()) {
					cout << " has child" << endl;
					cout << "  has " << (int)children->size() << " children" << endl;
					for (int i =0; i < (int)children->size(); ++i) {
						TNode* node = children->at(i);
						if (node->getNodeType() != Plus && node->getNodeType() != Times && node->getNodeType() != Minus) {
							nodeQueue.push_back(node);
						}
					}
				}

				if (isAssign) {
					// print out the expression
					cout << "assignment expression is " << *(children->at(1)) << endl;
				}

				nodeQueue.erase(nodeQueue.begin());
			}

		}
	}

	
	/**
	 * Matches names
	 * @return true if the given token follows the naming convention for NAME, as per the given grammar
	 */
	string matchName(string token) {
		regex nameRegex("[A-Za-z][\\w]*");
		return regex_match(token, nameRegex) ? token : "";
	}

	/**
	 * Matches integers
	 *  @return true if the given token follows the naming convention for INTEGER, as per SIMPLE grammar
	 */
	string matchInteger(string token) {
		regex intRegex("\\d+");
		return (regex_match(token,intRegex)) ? token : "";
	}

	/**
	 * Matches operators
	 * @return true if the given token is an operator
	 */
	string matchOperator(string token) {
		regex operRegex("[;\\+-\\*=]");
		return (regex_match(token, operRegex)) ? token: "";
	}

	/**
	 * Parses the program and populate data structures in the PKB. 
	 * Return TRUE if the program can successfully be parsed according to the SIMPLE grammar. Otherwise, return FALSE. 
	 */
	bool parseProgram() {
		using util::isEof;

		bool isParseSuccessful = true;
		while (!isEof() && isParseSuccessful) {
			isParseSuccessful &= util::parseProcedure();
		}

		assert(isParseSuccessful); // do not evaluate queries if parser has failed
		return isParseSuccessful;
	}
	
}

