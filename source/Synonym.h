#pragma once

#include <string>
#include <vector>
#include <set>
using std::set;
using std::vector;
using std::string;
using std::to_string;

enum SYNONYM_TYPE
{
	PROCEDURE, STMT, ASSIGN, CALL, WHILE, IF, VARIABLE, CONSTANT, PROG_LINE, STRING
};

enum SYNONYM_ATTRIBUTE
{
	procName, varName, value, stmtNo
};

class Synonym
{
	//This class is used to denote synonyms used in the query tree
public:
	Synonym();
	~Synonym();
	Synonym(SYNONYM_TYPE type, string name);
	Synonym(SYNONYM_TYPE type, int name);
	Synonym(SYNONYM_TYPE type, string name, SYNONYM_ATTRIBUTE attribute);
	Synonym(SYNONYM_TYPE type, string name, vector<string> values);
	Synonym(SYNONYM_TYPE type, string name, vector<int> values);
	Synonym(SYNONYM_TYPE type, string name, set<int> values);
	void setValues(vector<int> values);
	void setValues(set<int> values);
	SYNONYM_TYPE getType();
	SYNONYM_ATTRIBUTE getAttribute();
	string getName();
	vector<int> getValues();
	set<int> getValuesSet();

private:
	SYNONYM_TYPE _type;  //Type of synonym (Can use "string" or "boolean")
	string _name;  //Name of synonym (Name of constant if type is "string" e.g. "v") (also store "true" or "false")
	vector<int> _values;  //Values or index that this synonym takes (empty if type is "string" or "boolean")
	SYNONYM_ATTRIBUTE _attribute;  //Used for "with" clauses
};
