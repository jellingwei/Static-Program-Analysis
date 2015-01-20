#include <algorithm>
using std::sort;

#include "Synonym.h"

Synonym::Synonym()
{
	_type = "";
	_name = "";
}

Synonym::Synonym(string type, string name)
{
	_type = type;
	_name = name;
}

Synonym::Synonym(string type, int name)
{
	_type = type;
	_name = to_string(static_cast<long long>(name));
}

Synonym::Synonym(string type, string name, vector<string> values)
{
	_type = type;
	_name = name;
	
	for (unsigned int i = 0; i < values.size(); i++)
	{
		_values.push_back(std::stoi(values[i]));
	}
}

Synonym::Synonym(string type, string name, vector<int> values)
{
	_type = type;
	_name = name;
	_values = values;
}

Synonym::Synonym(string type, string name, set<int> values)
{
	_type = type;
	_name = name;
	
	for (set<int>::iterator itr = values.begin(); itr != values.end(); ++itr)
	{
		_values.push_back(*itr);
	}
}

string Synonym::getType()
{
	return _type;
}

string Synonym::getName()
{
	return _name;
}

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
