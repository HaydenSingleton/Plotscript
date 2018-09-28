#include "catch.hpp"

#include <string>
#include <sstream>
#include <fstream>
#include <iostream>

#include "semantic_error.hpp"
#include "interpreter.hpp"
#include "expression.hpp"

Expression run(const std::string & program){

  Interpreter interp;
  std::istringstream iss(program);

  bool ok = interp.parseStream(iss);
  if(!ok){
    std::cerr << "Failed to parse: " << program << std::endl;
  }
  REQUIRE(ok == true);

  Expression result;
  REQUIRE_NOTHROW(result = interp.evaluate());

  return result;
}

bool run_and_expect_error(const std::string & program){

  std::istringstream iss(program);
  Interpreter interp;

  bool ok = interp.parseStream(iss);
  if(!ok){
    std::cerr << "Failed to parse: " << program << std::endl;
  }
  REQUIRE(ok == true);

  REQUIRE_THROWS_AS(interp.evaluate(), SemanticError);

  return ok;
}

TEST_CASE( "Test Interpreter parser with expected input", "[interpreter]" ) {

  std::string program = "(begin (define r 10) (* pi (* r r)))";

  std::istringstream iss(program);

  Interpreter interp;

  bool ok = interp.parseStream(iss);

  REQUIRE(ok == true);
}

TEST_CASE( "Test Interpreter parser with numerical literals", "[interpreter]" ) {

  std::vector<std::string> programs = {"(1)", "(+1)", "(+1e+0)", "(1e-0)"};

  for(auto program : programs){
    std::istringstream iss(program);

    Interpreter interp;

    bool ok = interp.parseStream(iss);

    REQUIRE(ok == true);
  }

  {
    std::istringstream iss("(define x 1abc)");

    Interpreter interp;

    bool ok = interp.parseStream(iss);

    REQUIRE(ok == false);
  }
}

TEST_CASE( "Test Interpreter parser with truncated input", "[interpreter]" ) {

  {
    std::string program = "(f";
    std::istringstream iss(program);

    Interpreter interp;
    bool ok = interp.parseStream(iss);
    REQUIRE(ok == false);
  }

  {
    std::string program = "(begin (define r 10) (* pi (* r r";
    std::istringstream iss(program);

    Interpreter interp;
    bool ok = interp.parseStream(iss);
    REQUIRE(ok == false);
  }
}

TEST_CASE( "Test Interpreter parser with extra input", "[interpreter]" ) {

  std::string program = "(begin (define r 10) (* pi (* r r))) )";
  std::istringstream iss(program);

  Interpreter interp;

  bool ok = interp.parseStream(iss);

  REQUIRE(ok == false);
}

TEST_CASE( "Test Interpreter parser with single non-keyword", "[interpreter]" ) {

  std::string program = "hello";
  std::istringstream iss(program);

  Interpreter interp;

  bool ok = interp.parseStream(iss);

  REQUIRE(ok == false);
}

TEST_CASE( "Test Interpreter parser with empty input", "[interpreter]" ) {

  std::string program;
  std::istringstream iss(program);

  Interpreter interp;

  bool ok = interp.parseStream(iss);

  REQUIRE(ok == false);
}

TEST_CASE( "Test Interpreter parser with empty expression", "[interpreter]" ) {

  std::string program = "( )";
  std::istringstream iss(program);

  Interpreter interp;

  bool ok = interp.parseStream(iss);

  REQUIRE(ok == false);
}

TEST_CASE( "Test Interpreter parser with bad number string", "[interpreter]" ) {

  std::string program = "(1abc)";
  std::istringstream iss(program);

  Interpreter interp;

  bool ok = interp.parseStream(iss);

  REQUIRE(ok == false);
}

TEST_CASE( "Test Interpreter parser with incorrect input. Regression Test", "[interpreter]" ) {

  std::string program = "(+ 1 2) (+ 3 4)";
  std::istringstream iss(program);

  Interpreter interp;

  bool ok = interp.parseStream(iss);

  REQUIRE(ok == false);
}

TEST_CASE( "Test Interpreter result with literal expressions", "[interpreter]" ) {

  { // Number
    std::string program = "(4)";
    Expression result = run(program);
    REQUIRE(result == Expression(4.));
  }

  { // Symbol
    std::string program = "(pi)";
    Expression result = run(program);
    REQUIRE(result == Expression(atan2(0, -1)));
  }

}

TEST_CASE( "Test Interpreter result with simple procedures (add)", "[interpreter]" ) {

  { // add, binary case
    std::string program = "(+ 1 2)";
    INFO(program);
    Expression result = run(program);
    REQUIRE(result == Expression(3.));
  }

  { // add, 3-ary case
    std::string program = "(+ 1 2 3)";
    INFO(program);
    Expression result = run(program);
    REQUIRE(result == Expression(6.));
  }

  { // add, 6-ary case
    std::string program = "(+ 1 2 3 4 5 6)";
    INFO(program);
    Expression result = run(program);
    REQUIRE(result == Expression(21.));
  }
}

TEST_CASE( "Test Interpreter special forms: begin and define", "[interpreter]" ) {

  {
    std::string program = "(define answer 42)";
    INFO(program);
    Expression result = run(program);
    REQUIRE(result == Expression(42.));
  }

  {
    std::string program = "(begin (define answer 42)\n(answer))";
    INFO(program);
    Expression result = run(program);
    REQUIRE(result == Expression(42.));
  }

  {
    std::string program = "(begin (define answer (+ 9 11)) (answer))";
    INFO(program);
    Expression result = run(program);
    REQUIRE(result == Expression(20.));
  }

  {
    std::string program = "(begin (define a 1) (define b 1) (+ a b))";
    INFO(program);
    Expression result = run(program);
    REQUIRE(result == Expression(2.));
  }
}

TEST_CASE( "Test a medium-sized expression", "[interpreter]" ) {

  {
    std::string program = "(+ (+ 10 1) (+ 30 (+ 1 1)))";
    Expression result = run(program);
    REQUIRE(result == Expression(43.));
  }
}

TEST_CASE( "Test arithmetic procedures", "[interpreter]" ) {

  {
    std::vector<std::string> programs = {"(+ 1 -2)",
					 "(+ -3 1 1)",
					 "(- 1)",
					 "(- 1 2)",
					 "(* 1 -1)",
					 "(* 1 1 -1)",
					 "(/ -1 1)",
					 "(/ 1 -1)"};

    for(auto s : programs){
      Expression result = run(s);
      REQUIRE(result == Expression(-1.));
    }
  }
}


TEST_CASE( "Test some semantically invalid expresions", "[interpreter]" ) {

  std::vector<std::string> programs = {"(@ none)", // so such procedure
				       "(- 1 1 2)", // too many arguments
				       "(define begin 1)", // redefine special form
				       "(define pi 3.14)"}; // redefine builtin symbol
    for(auto s : programs){
      Interpreter interp;

      std::istringstream iss(s);

      bool ok = interp.parseStream(iss);
      REQUIRE(ok == true);

      REQUIRE_THROWS_AS(interp.evaluate(), SemanticError);
    }
}

TEST_CASE( "Test for exceptions from semantically incorrect input", "[interpreter]" ) {

  std::string input = R"((+ 1 a))";

  Interpreter interp;

  std::istringstream iss(input);

  bool ok = interp.parseStream(iss);
  REQUIRE(ok == true);

  REQUIRE_THROWS_AS(interp.evaluate(), SemanticError);
}

TEST_CASE( "Test malformed define", "[interpreter]" ) {

    std::string input = R"((define a 1 2))";

  Interpreter interp;

  std::istringstream iss(input);

  bool ok = interp.parseStream(iss);
  REQUIRE(ok == true);

  REQUIRE_THROWS_AS(interp.evaluate(), SemanticError);
}

TEST_CASE( "Test using number as procedure", "[interpreter]" ) {
  std::string input = R"((1 2 3))";

  Interpreter interp;

  std::istringstream iss(input);

  bool ok = interp.parseStream(iss);
  REQUIRE(ok == true);

  REQUIRE_THROWS_AS(interp.evaluate(), SemanticError);
}

TEST_CASE("Test handle_define errors", "[interpreter]"){
    {
      std::string input = R"((define 5 10))";
      Interpreter interp;
      std::istringstream iss(input);
      bool ok = interp.parseStream(iss);
      REQUIRE(ok == true);
      REQUIRE_THROWS_AS(interp.evaluate(), SemanticError);
  }
  {
    std::string input = R"((define + 20))";
    Interpreter interp;
    std::istringstream iss(input);
    bool ok = interp.parseStream(iss);
    REQUIRE(ok == true);
    REQUIRE_THROWS_AS(interp.evaluate(), SemanticError);
  }
  {
    std::string input = R"((define q 20 40 *))";
    Interpreter interp;
    std::istringstream iss(input);
    bool ok = interp.parseStream(iss);
    REQUIRE(ok == true);
    REQUIRE_THROWS_AS(interp.evaluate(), SemanticError);
  }

}

TEST_CASE("Test apply", "[expression]"){

  Atom op("+");
  std::vector<Expression> args = {Expression(1), Expression(2)};
  Environment env;

  Expression exp(op, args);

  REQUIRE(exp.eval(env) == Expression(3));

}

TEST_CASE("Test eval", "[expression]"){

  std::string input = R"((first list))";
  Interpreter interp;
  std::istringstream iss(input);
  REQUIRE(interp.parseStream(iss));
  REQUIRE_THROWS_AS(interp.evaluate(), SemanticError);
}

TEST_CASE("Test handle_begin, handle_lookup", "[expression]"){

  Interpreter interp;
  {
    std::string program = "R((begin ))";
    std::istringstream iss(program);
    REQUIRE_FALSE(interp.parseStream(iss));
    REQUIRE_THROWS_AS(interp.evaluate(), SemanticError);
  }
  {
    std::string program = "R((qwerty))";
    std::istringstream iss(program);
    REQUIRE_FALSE(interp.parseStream(iss));
    REQUIRE_THROWS_AS(interp.evaluate(), SemanticError);
  }

}

TEST_CASE("Test handle_lambda", "[expression]"){

  std::string program = "(begin (define f (lambda (x) (* 2 x))) (f 5))";
  Expression result = run(program);
  REQUIRE(result == Expression(10.));

  program = "(lambda (x y) (+ x y 1))";
  run(program);

  program = "(begin (define f (lambda (x y) (# x y))) (f 5))";
  REQUIRE(run_and_expect_error(program));

  program = "(begin (define f (lambda (+ x I) (+ x I))) (f 5))";
  REQUIRE(run_and_expect_error(program));

}

TEST_CASE("Test handle_apply", "[expression]"){

  std::string program = "(begin (define f (lambda (x) (* 2 x))) (apply f (list 5)))";
  Expression result = run(program);
  REQUIRE(result == Expression(10.));

  program = "(apply / (list 1 4))";
  run(program);

  program = "(apply + (list x I) (+ x I))";
  REQUIRE(run_and_expect_error(program));

  program = "(apply + (+ x I))";
  REQUIRE(run_and_expect_error(program));

  program = "(apply lambda (list 0 1))";
  REQUIRE(run_and_expect_error(program));

  program = "(apply (+ x I) (list 0 1))";
  REQUIRE(run_and_expect_error(program));
}

TEST_CASE("Test handle_map", "[expression]"){

  std::string program = "(begin (define f (lambda (x) (* 2 x))) (map f (list 5)))";
  Expression result = run(program);
  std::string program2 = "(list 10)";
  Expression expected_result = run(program2);
  REQUIRE(result == expected_result);

  program = "(map / (list 1 4 8))";
  result = run(program);
  program2 = "(list 1 0.25 0.125)";
  expected_result = run(program2);
  REQUIRE(result == expected_result);

  program = "(map + (list x I) (+ x I))";
  REQUIRE(run_and_expect_error(program));

  program = "(apply + (+ x I))";
  REQUIRE(run_and_expect_error(program));
}
