#include <algorithm>
using std::sort;

#include "Synonym.h"

Synonym::~Synonym()
{
}

/**
 * Set the synonym type and name to be an empty string.
 */
Synonym::Synonym()
{
	_type = "";
	_name = "";
}

/**
 * Set the synonym type and name to the given type and name respectively.
 */
Synonym::Synonym(string type, string name)
{
	_type = type;
	_name = name;
}

/**
 * Set the synonym type and name to the given type and name respectively.
 */
Synonym::Synonym(string type, int name)
{
	_type = type;
	_name = to_string(static_cast<long long>(name));
}

/**
 * Set the synonym type and name to the given type and name respectively.
 * Also, set the synonym to a list of values given.
 */
Synonym::Synonym(string type, string name, vector<string> values)
{
	_type = type;
	_name = name;
	
	for (unsigned int i = 0; i < values.size(); i++)
	{
		_values.push_back(std::stoi(values[i]));
	}
}

/**
 * Set the synonym type and name to the given type and name respectively.
 */
Synonym::Synonym(string type, string name, vector<int> values)
{
	_type = type;
	_name = name;
	_values = values;
}

/**
 * Set the synonym type and name to the given type and name respectively.
 * Also, set the synonym to a list of values given.
 */
Synonym::Synonym(string type, string name, set<int> values)
{
	_type = type;
	_name = name;
	
	for (set<int>::iterator itr = values.begin(); itr != values.end(); ++itr)
	{
		_values.push_back(*itr);
	}
}

/**
 * Return the type of the synonym.
 */
string Synonym::getType()
{
	return _type;
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
	return _values;
}

set<int> Synonym::getValuesSet()
{
	set<int> returnValues;
	
	for (unsigned int i = 0; i < _values.size(); i++)
	{
		returnValues.insert(_values[i]);
	}
	return returnValues;
}
