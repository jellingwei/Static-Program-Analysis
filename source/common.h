/** 
*  This file holds all the enums and multiple typedefs used in our SPA system.
*/
#pragma once

#include <set>
#include <stack>
#include <string>
#include <vector>
#include <list>
using std::set;
using std::vector;
using std::list;
using std::string;
using std::to_string;

#define MAX_SYNONYMS_ASSUMED 100

//different from SYNONYM_TYPE
typedef int CONSTANT_;
typedef int PROG_LINE_;
typedef bool BOOLEAN_;

typedef int VALUE;
typedef int INTEGER;
typedef int STATEMENT;
typedef int CONTAINER_STATEMENT;
typedef int VAR_INDEX;
typedef int PROC_INDEX;
typedef int SYNONYM_INDEX;
typedef bool STATUS;
typedef bool TRANS_CLOSURE;

typedef string FILENAME;
typedef string VARNAME;
typedef string PROCNAME;
typedef string EXPRESSION;
typedef string SYNONYM_NAME;
typedef string STATEMENT_TYPE;

typedef set<int> VALUE_SET;
typedef vector<int> VALUE_LIST;
typedef vector<int> CONSTANT_LIST;
typedef vector<int> STATEMENT_LIST;
typedef vector<int> PROGLINE_LIST;
typedef vector<int> ASSIGNMENT_LIST;

typedef vector<VAR_INDEX> VARINDEX_LIST;
typedef vector<PROC_INDEX> PROCINDEX_LIST;


enum CNODE_TYPE
{
	Assign_C, While_C, If_C, Call_C, Proc_C, EndProc_C, EndIf_C
};

enum CLINK_TYPE
{
	Before, After, Inside, Inside2
};

enum QNODE_TYPE
{
	// Default QueryTree Placeholder QNODE_TYPEs
	ROOT, RESULT, CLAUSES,

	// Query QNODE_TYPEs
	ModifiesP, ModifiesS,
	UsesP, UsesS,
	Calls, CallsT,
	Parent, ParentT,
	Follows, FollowsT,
	Next, NextT,
	Affects, AffectsT,
	Contains,ContainsT,
	Sibling,
	NextBip, NextBipT, 
	AffectsBip, AffectsBipT, 
	AttrCompare,
	PatternAssign, PatternIf, PatternWhile,

	// Select and Pattern QNODE_TYPEs
	Selection, Pattern, With
};

enum SYNONYM_TYPE
{
	PROCEDURE, STMT, ASSIGN, CALL, WHILE, IF, VARIABLE, CONSTANT, PROG_LINE, 
	STRING_CHAR, STRING_INT, STRING_PATTERNS, //Used to represent IDENT, INTEGER and expression-spec respectively
	BOOLEAN,   //Used for select boolean clauses
	UNDEFINED,  //Used to denote "_"
	PROGRAM, STMTLST, PLUS, MINUS, TIMES  //Used for Contains and Siblings
};

enum SYNONYM_ATTRIBUTE
{
	empty,     //if SYNONYM_ATTRIBUTE() 
	procName,  //Used for procedure and call
	varName,   //Used for var
	value,     //Used for constant
	stmtNo     //Used for all others including call

};

enum TNODE_TYPE
{
	Procedure, Assign, Plus, Variable, StmtLst, While, If, Constant, Minus, Times, Program, Call, Stmt, Nil
};

enum LINK_TYPE
{
	Par, Right_Sibling, Left_Sibling, Child
};

enum DIRECTION
{
	LeftToRight, RightToLeft
};

namespace QueryParser
{
	enum REF_TYPE
	{
		entRef, stmtRef, lineRef, varRef, stmtLstRef, nodeRef, ref
	};
}

struct IntegerPair
{
	int value1;
	int value2;

	IntegerPair(int val1, int val2): value1(val1), value2(val2) {}

	bool operator==(const IntegerPair &other) const
	{ 
		return (value1 == other.value1 && value2 == other.value2);
	}
};

struct Pair_Hasher
{
public:
	std::size_t operator() ( const IntegerPair &p ) const
	{
		using std::size_t;
		return p.value1 * MAX_SYNONYMS_ASSUMED + p.value2;
	}

	bool operator() ( const IntegerPair &a, const IntegerPair &b) const
	{
		return (a.value1 * MAX_SYNONYMS_ASSUMED + a.value2) == (b.value1 * MAX_SYNONYMS_ASSUMED + b.value2);
	}
};
