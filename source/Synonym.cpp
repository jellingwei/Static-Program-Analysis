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
		_values.insert(std::stoi(values[i]));
	}
}

Synonym::Synonym(string type, string name, vector<int> values)
{
	_type = type;
	_name = name;
	
	for (unsigned int i = 0; i < values.size(); i++)
	{
		_values.insert(values[i]);
	}
}

Synonym::Synonym(string type, string name, set<int> values)
{
	_type = type;
	_name = name;
	_values = values;
}

void Synonym::setValues(set<int> values)
{
	_values = values;
}

string Synonym::getType()
{
	return _type;
}

set<int> Synonym::getValues()
{
	return _values;
}

string Synonym::getName()
{
	return _name;
}
