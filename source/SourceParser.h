#include <vector>
#include <assert.h>
using std::string;

namespace Parser {
	bool initParser(string);
	bool parseProgram();
	string matchOperator(string token);
	string matchName(string token);
	string matchInteger(string token);
}