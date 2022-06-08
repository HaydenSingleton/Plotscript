#pragma once

#include "environment.h"
#include "expression.h"
#include "parse.h"
#include "semantic_error.h"

#include <istream>
#include <sstream>
#include <fstream>

class Interpreter
{
public:
	Interpreter();

	bool parseStream(std::istream& text);
	bool parseString(std::string text);

	Expression evaluate();
private:
	Environment env;
	Expression ast;
};

