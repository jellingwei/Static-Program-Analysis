#include <algorithm>
#include "Synonym.h"
using std::sort;

Synonym::Synonym()
{
	_isEmpty = true;
}

Synonym::~Synonym()
{
}

/**
 * Set the synonym type and name to the given type and name respectively.
 * @param type the SYNONYM_TYPE which can be PROCEDURE/STMT/ASSIGN/CALL/WHILE/IF/VARIABLE/CONSTANT/PROG_LINE/STRING_CHAR/
 *											 STRING_INT/STRING_PATTERNS/UNDEFINED
 * @param name the name of the synonym, e.g a for ASSIGN
 */
Synonym::Synonym(SYNONYM_TYPE type, string name)
{
	_isEmpty = false;
	_type = type;
	_name = name;
}

/**
 * Set the synonym type and name to the given type and name respectively.
 * @param type the SYNONYM_TYPE which can be PROCEDURE/STMT/ASSIGN/CALL/WHILE/IF/VARIABLE/CONSTANT/PROG_LINE/STRING_CHAR/
 *											 STRING_INT/STRING_PATTERNS/UNDEFINED 
 * @param name the name of the synonym, e.g a for ASSIGN
 * @param attribute the attribute of the synonym which can be procName/varName/value/stmtNum
 */
Synonym::Synonym(SYNONYM_TYPE type, string name, SYNONYM_ATTRIBUTE attribute)
{
	_isEmpty = false;
	_type = type;
	_name = name;
	_attribute = attribute;
}

/**
 * Set the synonym type and name to the given type and name respectively.
 * @param type the SYNONYM_TYPE which can be PROCEDURE/STMT/ASSIGN/CALL/WHILE/IF/VARIABLE/CONSTANT/PROG_LINE/STRING_CHAR/
 *											 STRING_INT/STRING_PATTERNS/UNDEFINED
 * @param name the name of the synonym, e.g a for ASSIGN
 */
Synonym::Synonym(SYNONYM_TYPE type, int name)
{
	_isEmpty = false;
	_type = type;
	_name = to_string(static_cast<long long>(name));
}

/**
 * Set the synonym type and name to the given type and name respectively.
 * Also, set the synonym to a list of values given.
 */
/*Synonym::Synonym(SYNONYM_TYPE type, string name, vector<string> values)
{
	_isEmpty = false;
	_type = type;
	_name = name;
	
	for (unsigned int i = 0; i < values.size(); i++)
	{
		_values.push_back(std::stoi(values[i]));
	}
}*/

/**
 * Set the synonym type and name to the given type and name respectively.
 * @param type the SYNONYM_TYPE which can be PROCEDURE/STMT/ASSIGN/CALL/WHILE/IF/VARIABLE/CONSTANT/PROG_LINE/STRING_CHAR/
 *											 STRING_INT/STRING_PATTERNS/UNDEFINED
 * @param name the name of the synonym, e.g a for ASSIGN
 * @param values a list of the possible kinds of values associated with the synonym which can be statement numbers, indexes.
 */
Synonym::Synonym(SYNONYM_TYPE type, string name, vector<int> values)
{
	_isEmpty = false;
	_type = type;
	_name = name;
	_values = values;
}

/**
 * Set the synonym type and name to the given type and name respectively.
 * Also, set the synonym to a list of values given.
 * @param type the SYNONYM_TYPE which can be PROCEDURE/STMT/ASSIGN/CALL/WHILE/IF/VARIABLE/CONSTANT/PROG_LINE/STRING_CHAR/
 *											 STRING_INT/STRING_PATTERNS/UNDEFINED
 * @param name the name of the synonym, e.g a for ASSIGN
 * @param values a list of the possible kinds of values associated with the synonym which can be statement numbers, indexes.
 */
Synonym::Synonym(SYNONYM_TYPE type, string name, VALUE_SET values)
{
	_isEmpty = false;
	_type = type;
	_name = name;
	_valuesSet = values;
}

BOOLEAN_ Synonym::isEmpty()
{
	return _isEmpty;
}

BOOLEAN_ Synonym::isSynonym()
{
	return _type == PROCEDURE || _type == STMT || _type == ASSIGN || 
		_type == CALL || _type == WHILE || _type == IF || _type == VARIABLE ||
		_type == CONSTANT || _type == PROG_LINE || _type == STMTLST || 
		_type == PLUS || _type == MINUS || _type == TIMES || _type == PROGRAM;
}

BOOLEAN_ Synonym::isConstant()
{
	return _type == STRING_CHAR || _type == STRING_INT || _type == STRING_PATTERNS;
}

BOOLEAN_ Synonym::isUndefined()
{
	return _type == UNDEFINED;
}

void Synonym::setValues(vector<int> values)
{
	_values = values;
	_valuesSet.clear();
}

void Synonym::setValues(VALUE_SET values)
{
	_valuesSet = values;
	_values.clear();
}

/**
 * @return the type of the synonym.
 */
SYNONYM_TYPE Synonym::getType()
{
	return _type;
}

SYNONYM_ATTRIBUTE Synonym::getAttribute()
{
	return _attribute;
}

/**
 * @return the name of the synonym.
 */
string Synonym::getName()
{
	return _name;
}

/**
 * @return a list of values of the synonym. 
 * If there are no values, return an empty list.
 */
vector<int> Synonym::getValues()
{
	if (_values.size() != 0) {
		return _values;
	} else if (_valuesSet.size() != 0) {
		for (auto itr = _valuesSet.begin(); itr != _valuesSet.end(); ++itr) {
			_values.push_back(*itr);
		}
	}
	return _values;
}

VALUE_SET Synonym::getValuesSet()
{
	if (_valuesSet.size() != 0) {
		return _valuesSet;
	} else if (_values.size() != 0) {
		for (unsigned int i = 0; i < _values.size(); i++) {
			_valuesSet.insert(_values[i]);
		}
	}
	return _valuesSet;
}

string Synonym::convertToString(SYNONYM_TYPE synonymType)
{
	switch (synonymType) {
	case PROCEDURE:
		return "procedure";
	case STMT:
		return "stmt";
	case ASSIGN:
		return "assign";
	case CALL:
		return "call";
	case WHILE:
		return "while";
	case IF:
		return "if";
	case VARIABLE:
		return "variable";
	case CONSTANT:
		return "constant";
	case PROG_LINE:
		return "prog_line";
	case STMTLST:
		return "stmtLst";
	case PLUS:
		return "plus";
	case MINUS:
		return "minus";
	case TIMES:
		return "times";
	case STRING_CHAR:
		return "string_char";
	case STRING_INT:
		return "string_int";
	case STRING_PATTERNS:
		return "string_patterns";
	case BOOLEAN:
		return "boolean";
	case UNDEFINED:
		return "_";
	default:
		return "";
	}
}

SYNONYM_TYPE Synonym::convertToEnum(string synonymType)
{
	if (synonymType == "procedure") {
		return PROCEDURE;
	} else if (synonymType == "stmt") {
		return STMT;
	} else if (synonymType == "assign") {
		return ASSIGN;
	} else if (synonymType == "call") {
		return CALL;
	} else if (synonymType == "while") {
		return WHILE;
	} else if (synonymType == "if") {
		return IF;
	} else if (synonymType == "variable") {
		return VARIABLE;
	} else if (synonymType == "constant") {
		return CONSTANT;
	} else if (synonymType == "prog_line") {
		return PROG_LINE;
	} else if (synonymType == "string_char") {
		return STRING_CHAR; 
	} else if (synonymType == "string_int") {
		return STRING_INT;
	} else if (synonymType == "string_patterns") {
		return STRING_PATTERNS;
	} else if (synonymType == "boolean") {
		return BOOLEAN;
	} else if (synonymType == "stmtLst") {
		return STMTLST;
	} else if (synonymType == "plus") {
		return PLUS;
	} else if (synonymType == "minus") {
		return MINUS;
	} else if (synonymType == "times") {
		return TIMES;
	} else {
		return UNDEFINED;
	}
}
string Synonym::convertSynonymAttrToString(SYNONYM_ATTRIBUTE synonymAttr)
{
	switch (synonymAttr) {
	case procName:
		return "procName";
	case varName:
		return "varName";
	case value:
		return "value";
	case stmtNo:
		return "stmtNo";
	case empty:
		return "";
	default:
		return "";
	}
}
SYNONYM_ATTRIBUTE Synonym::convertSynonymAttrToEnum(string synonymAttr)
{
	if (synonymAttr == "procName") {
		return procName;
	} else if (synonymAttr == "varName") {
		return varName;
	} else if (synonymAttr == "value") {
		return value;
	} else if (synonymAttr == "stmtNo"){
		return stmtNo;
	}else{
		return empty;
	}
}
