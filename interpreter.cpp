#include "interpreter.hpp"

bool Interpreter::parseStream(std::istream & expression) noexcept{

  TokenSequenceType tokens = tokenize(expression);

  ast = parse(tokens);

  return (ast != Expression());
};

Expression Interpreter::evaluate(){

  return ast.eval(env);
}
