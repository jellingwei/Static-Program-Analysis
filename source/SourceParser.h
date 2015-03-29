#pragma once

#include <vector>
#include <assert.h>

#include "common.h"

using std::string;

namespace Parser {
	STATUS initParser(string);
	STATUS parseProgram();
	string matchOperator(string token);
	string matchName(string token);
	string matchInteger(string token);
}