#pragma once

#include "common.h"

class Synonym
{
	//This class is used to denote synonyms used in the query tree
public:
	Synonym();
	~Synonym();
	Synonym(SYNONYM_TYPE type, string name);
	Synonym(SYNONYM_TYPE type, int name);
	Synonym(SYNONYM_TYPE type, string name, SYNONYM_ATTRIBUTE attribute);
	//Synonym(SYNONYM_TYPE type, string name, vector<string> values);
	Synonym(SYNONYM_TYPE type, string name, vector<int> values);
	Synonym(SYNONYM_TYPE type, string name, VALUE_SET values);
	
	void setValues(vector<int> values);
	void setValues(VALUE_SET values);
	
	SYNONYM_TYPE getType();
	SYNONYM_ATTRIBUTE getAttribute();
	string getName();
	vector<int> getValues();
	VALUE_SET getValuesSet();
	BOOLEAN_ isEmpty(); //Check if Synonym() is created. ie an empty Synonym
	
	static string convertToString(SYNONYM_TYPE synonymType);
	static SYNONYM_TYPE convertToEnum(string synonymType);
	static string convertSynonymAttrToString(SYNONYM_ATTRIBUTE synonymAttr);
	static SYNONYM_ATTRIBUTE convertSynonymAttrToEnum(string synonymAttr);

	bool operator== (const Synonym& synonym2) const
	{
		return (_type == synonym2._type && _name == synonym2._name);
	}

private:
	SYNONYM_TYPE _type;  //Type of synonym (Can use "string" or "boolean")
	string _name;  //Name of synonym (Name of constant if type is "string" e.g. "v") (also store "true" or "false")
	vector<int> _values;  //Values or index that this synonym takes (empty if type is "string" or "boolean")
	VALUE_SET _valuesSet;
	SYNONYM_ATTRIBUTE _attribute;  //Used for "with" clauses
	BOOLEAN_ _isEmpty;
};
