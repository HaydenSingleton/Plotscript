#include "interpreter.h"
#include "startup_config.h"

Interpreter::Interpreter()
{
	env = Environment();

	auto stream = std::ifstream(STARTUP_FILE);
	if (parseStream(stream))
		evaluate();
	else
		std::cerr << "Invalid starting file: " << STARTUP_FILE << std::endl;
}

bool Interpreter::parseStream(std::istream& text) {
	if (text.bad()) return false;

	TokenSequence tokens = tokenize(text);

	ast = parse(tokens);
	return (ast != Expression());
}

bool Interpreter::interpret(std::string& text) {

	auto stream = std::istringstream(text);
	return parseStream(stream);
}

Expression Interpreter::evaluate() {
	return ast.eval(env);
}