#include <algorithm>
using std::sort;

#include "Synonym.h"

Synonym::Synonym()
{
	_isEmpty = true;
}

Synonym::~Synonym()
{
}

/**
 * Set the synonym type and name to the given type and name respectively.
 */
Synonym::Synonym(SYNONYM_TYPE type, string name)
{
	_isEmpty = false;
	_type = type;
	_name = name;
}

/**
 * Set the synonym type and name to the given type and name respectively.
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
 */
Synonym::Synonym(SYNONYM_TYPE type, string name, set<int> values)
{
	_isEmpty = false;
	_type = type;
	_name = name;
	_valuesSet = values;
}

bool Synonym::isEmpty()
{
	return _isEmpty;
}

void Synonym::setValues(vector<int> values)
{
	_values = values;
	_valuesSet.clear();
}

void Synonym::setValues(set<int> values)
{
	_valuesSet = values;
	_values.clear();
}

/**
 * Return the type of the synonym.
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
 * Return the name of the synonym.
 */
string Synonym::getName()
{
	return _name;
}

/**
 * Return a list of values of the synonym. 
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

set<int> Synonym::getValuesSet()
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
	case STRING:
		return "string";
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
	} else if (synonymType == "string") {
		return STRING;
	} else if (synonymType == "boolean") {
		return BOOLEAN;
	} else {
		return UNDEFINED;
	}
}
