#pragma once

#include<stdio.h>
#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <sstream>
#include <regex>
#include <exception>

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
namespace Parser 
{
	using std::string;
	using std::vector;
	using std::regex;

	namespace var // contains state of the parser
	{
		ifstream inputFile;

		vector<string> buffer;
		vector<string>::iterator bufferIter;
		string currentParsedLine;
	
		vector<int> stmtListParent;
		vector<TNode*> ASTParent;

		int stmtNum;
		int progLineNum;
		int currentProcIndex;
	}

	/**
	 * Initialises and prepares the parser for parsing. 
	 * The file specified by filename is opened for parsing. 
	 * Return TRUE if the file can be successfully opened. Otherwise, return FALSE.
	 */
	bool initParser(string filename) 
	{
		using namespace Parser::var;
		inputFile.open(filename);
		stmtNum = 0;
		progLineNum = 0;

		stmtListParent = vector<int>();
		
		return inputFile.is_open(); 
	}

	namespace util 
	{
		using namespace Parser::var;
		
		bool parseStmtList();

		/**
		 * Returns the next line of input as a vector of tokens.
		 */
		bool parseLine() 
		{
			if (!inputFile.is_open()) 
			{
				throw new logic_error("Parse failure: Parser not initialised before parsing functions were called");
			}
	
			buffer.clear();

			string line;
			getline(inputFile, line);

			// drop everything after 2 backslash
			int pos;
			if ((pos = line.find("\\")) != string::npos) 
			{
				line = line.substr(0, pos);
			}

			if (line.empty()) 
			{
				if (inputFile.eof()) 
				{
					inputFile.close();
					return false;
				}
				return parseLine();
			}
			progLineNum += 1;
			currentParsedLine = line;

			// remove blocks of multiple whitespace
			string multipleSpaces = "[\\s]+";
			regex separatorRegex(multipleSpaces);
			sregex_token_iterator reg_end;

			sregex_token_iterator rs(line.begin(), line.end(), separatorRegex, -1);

			// tokenise words and operators
			string operators = "([\\w\\d]+|[+=;{}\\(\\)])";
			regex operRegex(operators);

			for (; rs != reg_end; ++rs) 
			{
				std::smatch match;
				string res(rs->str());
				while (std::regex_search(res, match, operRegex)) 
				{
					if (match.empty()) 
					{
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
		string parseToken() 
		{
			if (buffer.size() && bufferIter != buffer.end()) 
			{
				return *(bufferIter ++);
			} else {
				bool res = parseLine();
				if (!res) 
				{
					return inputFile.eof()? "" : parseToken();
				} else {
					bufferIter = buffer.begin();
					return *(bufferIter ++);
				}
			}
		}

		string peekToken() 
		{
			if (buffer.size() && bufferIter != buffer.end()) 
			{
				return *(bufferIter);
			} else {
				bool res = parseLine();
				if (!res) 
				{
					return inputFile.eof()? "" : parseToken();
				} else {
					bufferIter = buffer.begin();
					return *(bufferIter);
				}
			}

		}

		bool isEof() 
		{
			if (peekToken().empty()) {
				return inputFile.eof();
			} else {
				return false;
			}
		}


		TNode* currentASTParent() 
		{
			return ASTParent[ASTParent.size() - 1];
		}

		bool callPkb(string designEntity, string LHS, string RHS) 
		{
			PKB pkb = PKB::getInstance();
			if (designEntity.compare("VarTable") == 0) 
			{
				pkb.insertVar(RHS, atoi(LHS.c_str()));
	
				return true;
			} else if (designEntity.compare("StmtTable") == 0) 
			{
				pkb.insertStmt(atoi(LHS.c_str()), RHS);
	
				return true;
			} else if (designEntity.compare("Modifies") == 0) 
			{
				int modifiedVarIndex = pkb.getVarIndex(RHS);
				int statement = atoi(LHS.c_str());
				pkb.setModifies(statement, modifiedVarIndex);


				while (pkb.getParent(statement).size()) 
				{
					statement = pkb.getParent(statement).at(0);
					if (statement > 0) 
					{
						pkb.setModifies(statement, modifiedVarIndex);	
					}
				}

				pkb.setModifiesProc(currentProcIndex, modifiedVarIndex);
			
				return true;
			} else if (designEntity.compare("Uses") == 0) 
			{
				int statement = atoi(LHS.c_str());
				int usedVarIndex = pkb.getVarIndex(RHS);
			
				pkb.setUses(statement, usedVarIndex);
			
				while (pkb.getParent(statement).size()) 
				{
					statement = pkb.getParent(statement).at(0);
					if (statement > 0) 
					{
						pkb.setUses(statement, usedVarIndex);	
					}
				}

				pkb.setUsesProc(currentProcIndex, usedVarIndex);
			
				return true;
			} else if (designEntity.compare("ConstantTable") == 0) 
			{
				pkb.insertConstant(atoi(RHS.c_str()), atoi(LHS.c_str()));
			
				return true;
			} else if (designEntity.compare("ProcTable") == 0) {
				pkb.insertProc(LHS);

				return true;
			}

			return false;
		}

		/**
		 * Parses the next token and check if it is equal to the given target
		 */
		bool parse(string target) 
		{
			string nextToken = parseToken();
			return nextToken.compare(target) == 0;
		}

		bool match(string token, string target) 
		{
			return token.compare(target) == 0;
		}



		/**
		 * Parses an expression. 
		 * @param exprRoot the root of the expression, this should be the assignNode itself.
		 * Assumptions: LHS = RHS => [LHS] and [=] took up 2 tokens in the buffer, and has already been parsed
		 *              The expression only consists of factors and '+'
		 */
		bool parseExpr(TNode* exprRoot) 
		{
			bool res = true;
			const int alreadyParsed = 2;
			
			vector<string>::const_iterator first = buffer.begin() + alreadyParsed;
			vector<string>::const_iterator last = buffer.end();
			vector<string> slicedBuffer(first, last);

			ExpressionParser exprParser;
			exprParser.updateStmtNum(stmtNum);
			TNode* top = exprParser.parseExpressionForAST(slicedBuffer);

			PKB::getInstance().createLink(Child, exprRoot, top);

			while ((bufferIter++)->compare(";") != 0) 
			{}

			return res;
		}

		string parseName() 
		{
			string token = parseToken();
			return matchName(token);
		}

		bool parseIf(string token) 
		{
			// match if
			bool res = match(token, "if"); 
			if (!res) return false;

			// match variable
			string varName = parseName();

			if (varName.size() == 0) return false;
			callPkb("VarTable", std::to_string(static_cast<long long>(stmtNum)), varName);

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

			pair<int, TNode*> stmtNumToNodePair(stmtNum, node);
			PKB::getInstance().nodeTable.insert(stmtNumToNodePair);

			// Follows from prev stmt in the stmt list
			TNode* prevStmtInStmtList = currentASTParent()->hasChild() ? currentASTParent()->getChildren()->back() : NULL;
			if (prevStmtInStmtList != NULL) 
			{
				pkb.setFollows(prevStmtInStmtList, node);
			}

			// Set Parent
			pkb.setParent(currentASTParent(), node);

			int ifStmtNum = stmtNum;
			ASTParent.push_back(stmtlistNode);
			// parse inner stmt list
			parseStmtList();

			// remove the stmtList node from ASTParent
			ASTParent.erase(ASTParent.end() - 1); 

			// parse optional "else"
			string nextToken = peekToken();
			bool optionalElse = match(nextToken, "else");
			if (optionalElse) 
			{
				res = parse("else");
				
				TNode* stmtlist2Node = pkb.createTNode(StmtLst, ifStmtNum, -1);
				// 2nd stmtList node as third child
				pkb.createLink(Child, node, stmtlist2Node); 
				ASTParent.push_back(stmtlist2Node);

				res = parse("{");
				if (!res) return false;
				
				parseStmtList();

				// remove the stmtList node from ASTParent
				ASTParent.erase(ASTParent.end() - 1);
			}

			callPkb("Uses", std::to_string(static_cast<long long>(ifStmtNum)), varName);
		
			return res;
		}

		bool parseWhile(string token) 
		{
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

			// Follows from prev stmt in the stmt list
			TNode* prevStmtInStmtList = currentASTParent()->hasChild() ? currentASTParent()->getChildren()->back() : NULL;
			if (prevStmtInStmtList != NULL) 
			{
				pkb.setFollows(prevStmtInStmtList, node);
			}

			// Set Parent
			pkb.setParent(currentASTParent(), node);

			int whileStmtNum = stmtNum;
			ASTParent.push_back(stmtlistNode);
			// parse inner stmt list
			parseStmtList();
		
			// remove the while node from ASTParent
			ASTParent.erase(ASTParent.end() - 1); 
		
			callPkb("Uses", std::to_string(static_cast<long long>(whileStmtNum)), varName);
		
			return res;
		}

		bool parseAssign(string firstToken) 
		{
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
			if (prevStmtInStmtList) 
			{
				PKB::getInstance().setFollows(prevStmtInStmtList, node);
			}
			PKB::getInstance().setParent(currentASTParent(), node);
		
			res = parseExpr(RHSNode);
		
			callPkb("Modifies", std::to_string(static_cast<long long>(stmtNum)), varName);
		
			return res;
		}

		bool parseCall(string firstToken) {
			string procName = parseName();

			if (procName.size() == 0) return false;

			bool res = parse("=");
			if (!res) return false;
		

			// AST interactions
			AST* ast = PKB::getInstance().ast;
			PKB pkb = PKB::getInstance();
			TNode* node = pkb.createTNode(Call, stmtNum, -1);
			
			TNode* RHSNode = node;  // pass the assignNode directly to parseExpr, which will attach the variable/constant to it

		
			pair<int, TNode*> stmtNumToNodePair(stmtNum, node);
			PKB::getInstance().nodeTable.insert(stmtNumToNodePair);


			TNode* prevStmtInStmtList = currentASTParent()->hasChild() ? currentASTParent()->getChildren()->back() : NULL;
			if (prevStmtInStmtList) 
			{
				PKB::getInstance().setFollows(prevStmtInStmtList, node);
			}
			PKB::getInstance().setParent(currentASTParent(), node);
		
			res = parseExpr(RHSNode);
		
			callPkb("CallTable", std::to_string(static_cast<long long>(currentProcIndex)), procName);
		
			return res;
		}


		/**
		 * Matches a statement.
		 */
		bool matchStmt(string firstToken) 
		{
			bool res = true;
		
			if (currentParsedLine.find("=") != string::npos) 
			{
				stmtNum += 1;
			
				callPkb("StmtTable", std::to_string(static_cast<long long>(stmtNum)), "assign");
				res = parseAssign(firstToken);
				if (stmtListParent.size() > 0) 
				{
					int parent = stmtListParent[stmtListParent.size() - 1];
					callPkb("Parent", std::to_string(static_cast<long long>(parent)), std::to_string(static_cast<long long>(stmtNum)));
				}
			} else if (firstToken == "while") 
			{
				stmtNum += 1;

				callPkb("StmtTable", std::to_string(static_cast<long long>(stmtNum)), "while");
				res = parseWhile(firstToken);

				if (stmtListParent.size() > 0) 
				{
					int parent = stmtListParent[stmtListParent.size() - 1];
					callPkb("Parent", std::to_string(static_cast<long long>(parent)), std::to_string(static_cast<long long>(stmtNum)));
				}
			} else if (firstToken == "if") 
			{
				stmtNum += 1;

				callPkb("StmtTable", std::to_string(static_cast<long long>(stmtNum)), "if");
				res = parseIf(firstToken);

				if (stmtListParent.size() > 0) 
				{
					int parent = stmtListParent[stmtListParent.size() - 1];
					callPkb("Parent", std::to_string(static_cast<long long>(parent)), std::to_string(static_cast<long long>(stmtNum)));
				}
			} else if (firstToken == "call") 
			{
				stmtNum += 1;

				callPkb("StmtTable", std::to_string(static_cast<long long>(stmtNum)), "call");
				res = parseCall(firstToken);
			}

			return res;
		}

		bool parseStmtList() 
		{
			string nextToken = parseToken();
			bool res;

			bool isEndOfStmtList = nextToken.compare("}") == 0;
			while (!isEndOfStmtList) 
			{
				res = matchStmt(nextToken);
				if (!res) {
					return false; 
				}
			
				nextToken = parseToken();			
			
				isEndOfStmtList = (nextToken.compare("}") == 0) ;
			}
			return true;
		}

		bool parseProcedure() 
		{
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
		
			TNode* root = pkb.getRoot();
			int procIndex = pkb.getProcIndex(procName);
			TNode* procNode = pkb.createTNode(Procedure, 0, procIndex);
			pkb.createLink(Child, root, procNode);
			TNode* stmtListNode = pkb.createTNode(StmtLst, 0, 0);
			pkb.createLink(Child, procNode, stmtListNode);
		
			ASTParent.push_back(stmtListNode);

			res = parseStmtList();

			// remove the stmtlist node from ASTParent
			ASTParent.erase(ASTParent.end() - 1); 

			return res;
		}

		void traverseAndPrintTree(TNode* root) 
		{
			vector<TNode*> nodeQueue;

			nodeQueue.push_back(root);
			while (!nodeQueue.empty()) 
			{
			
			
				bool isConstant = (nodeQueue.front()->getNodeType() == Constant);
				bool isVariable = (nodeQueue.front()->getNodeType() == Variable);
				bool isPlus = (nodeQueue.front()->getNodeType() == Plus);
				bool isAssign = (nodeQueue.front()->getNodeType() == Assign);
				bool isStmtLst = (nodeQueue.front()->getNodeType() == StmtLst);

				cout << *(nodeQueue.front()) << endl;
				cout << " isConstant: " << isConstant << " isVariable:" << isVariable << " isPlus: " << isPlus << " isAssign: " << isAssign << " isStmtLst" << isStmtLst << endl;
				
				if (nodeQueue.front()->getParent() != NULL)
				{
					cout << "  parent: " << nodeQueue.front()->getParent()->getNodeValueIdx() << " from stmt " << nodeQueue.front()->getParent()->getStmtNumber() << endl;
				}
			
				vector<TNode*>* children = nodeQueue.front()->getChildren();
				if (nodeQueue.front()->hasChild()) 
				{
					cout << " has child" << endl;
					cout << "  has " << (int)children->size() << " children" << endl;
					for (int i =0; i < (int)children->size(); ++i) 
					{
						TNode* node = children->at(i);
						if (node->getNodeType() != Plus) 
						{
							nodeQueue.push_back(node);
						}
					}
				}

				if (isAssign) 
				{
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
	string matchName(string token) 
	{
		regex nameRegex("[A-Za-z][\\w]*");
		return regex_match(token, nameRegex) ? token : "";
	}

	/**
	 * Matches integers
	 *  @return true if the given token follows the naming convention for INTEGER, as per SIMPLE grammar
	 */
	string matchInteger(string token) 
	{
		regex intRegex("\\d+");
		return (regex_match(token,intRegex)) ? token : "";
	}

	/**
	 * Matches operators
	 * @return true if the given token is an operator
	 */
	string matchOperator(string token) 
	{
		regex operRegex("[;\\+-*=]");
		return (regex_match(token, operRegex)) ? token: "";
	}

	/**
	 * Parses the program and populate data structures in the PKB. 
	 * Return TRUE if the program can successfully be parsed according to the SIMPLE grammar. Otherwise, return FALSE. 
	 */
	bool parseProgram() 
	{
		using util::isEof;

		bool isParseSuccessful = true;
		while (!isEof()) {
			isParseSuccessful &= util::parseProcedure();
		}

		assert(isParseSuccessful); // do not evaluate queries if parser has failed
		return isParseSuccessful;
	}
	
}

