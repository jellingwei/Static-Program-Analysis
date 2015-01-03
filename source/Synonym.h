#pragma once

#include <string>
#include <vector>
#include <set>
using std::set;
using std::vector;
using std::string;
using std::to_string;

class Synonym
{
	//This class is used to denote synonyms used in the query tree
public:
	Synonym();
	Synonym(string type, string name);
	Synonym(string type, int name);
	Synonym(string type, string name, vector<string> values);
	Synonym(string type, string name, vector<int> values);
	Synonym(string type, string name, set<int> values);
	void setValues(set<int> values);
	string getType();
	string getName();
	set<int> getValues();

private:
	string _type;  //Type of synonym (Can use "string" or "boolean")
	string _name;  //Name of synonym (Name of constant if type is "string" e.g. "v") (also store "true" or "false")
	set<int> _values;  //Values or index that this synonym takes (empty if type is "string" or "boolean")
};
