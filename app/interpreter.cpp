#include "interpreter.h"

Interpreter::Interpreter()
{
	env = Environment();

	if (parseStream(std::ifstream(STARTUP_FILE)))
		evaluate();
	else
		std::cerr << "Invalid starting file." << std::endl;
}

bool Interpreter::parseStream(std::istream& text) noexcept {
	TokenSequence tokens = tokenize(text);
	ast = parse(tokens);
	return (ast != Expression());
}

Expression Interpreter::evaluate() {
	return ast.eval(env);
}