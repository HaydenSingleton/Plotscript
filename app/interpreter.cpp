#include "interpreter.h"
#include "../build/startup_config.h"


Interpreter::Interpreter()
{
	env = Environment();

	auto stream = std::ifstream(STARTUP_FILE);
	if (parseStream(stream))
		evaluate();
	else
		std::cerr << "Invalid starting file." << std::endl;
}

bool Interpreter::parseStream(std::istream& text) {
	TokenSequence tokens = tokenize(text);
	ast = parse(tokens);
	return (ast != Expression());
}

Expression Interpreter::evaluate() {
	return ast.eval(env);
}