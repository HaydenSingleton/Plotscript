#pragma once

#include "environment.h"
#include "expression.h"
#include "parse.h"
#include "semantic_error.h"
#include "../build/startup_config.h"

#include <istream>
#include <sstream>
#include <fstream>

class Interpreter
{
public:
	Interpreter();

	bool parseStream(std::istream& text) noexcept;
	Expression evaluate();
private:
	Environment env;
	Expression ast;
};

