 /** 
  *  This file holds all the enums and multiple typedefs used in our SPA system.
  */
#pragma once


typedef short PROC;


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
	AttrCompare,

	// Select and Pattern QNODE_TYPEs
	Selection, Pattern, With
};

enum SYNONYM_TYPE
{
	PROCEDURE, STMT, ASSIGN, CALL, WHILE, IF, VARIABLE, CONSTANT, PROG_LINE, 
	STRING_CHAR, STRING_INT, STRING_PATTERNS, //Used to represent IDENT, INTEGER and expression-spec respectively
	BOOLEAN,   //Used for select boolean clauses
	UNDEFINED  //Used to denote "_"
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
	Procedure, Assign, Plus, Variable, StmtLst, While, If, Constant, Minus, Times, Program, Call
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
		entRef, stmtRef, lineRef, varRef, ref
	};
}