#include "catch.hpp"

#include <string>
#include <sstream>
#include <fstream>
#include <iostream>

#include "semantic_error.hpp"
#include "interpreter.hpp"
#include "expression.hpp"
#include "startup_config.hpp"

Expression run(const std::string & program){

  Interpreter interp;

  std::ifstream startip_str(STARTUP_FILE);
  REQUIRE(interp.parseStream(startip_str));
  REQUIRE_NOTHROW(interp.evaluate());

  std::istringstream iss(program);
  INFO(program);

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

  std::ifstream startip_str(STARTUP_FILE);
  REQUIRE(interp.parseStream(startip_str));
  REQUIRE_NOTHROW(interp.evaluate());

  bool ok = interp.parseStream(iss);
  if(!ok){
    std::cerr << "Failed to parse: " << program << std::endl;
  }
  REQUIRE(ok == true);
  INFO(program);
  REQUIRE_THROWS_AS(interp.evaluate(), SemanticError);

  return true;
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

  {
    std::string program = "(begin)";
    INFO(program);
    REQUIRE(run_and_expect_error(program));
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

  std::vector<std::string> programs = { "(@ none)", // so such procedure
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

  std::string program = "(+ 1 2)";
  Expression result = run(program);
  REQUIRE(result == Expression(3.));

  std::string programF = "(~ 1 2)";
  REQUIRE(run_and_expect_error(programF));

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
  result = run(program);
  REQUIRE(result.isLambda());

  program = "(begin (define x 100) (lambda (x y) (+ x y 1)))";
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

  program = "(apply + (list 1 2 3 4))";
  result = run(program);
  REQUIRE(result == Expression(Atom(10)));

  program = "(apply (+ z I) (list 0))";
  REQUIRE(run_and_expect_error(program));

  program = "(apply 35 (list 0))";
  REQUIRE(run_and_expect_error(program));

  program = "(apply + 35)";
  REQUIRE(run_and_expect_error(program));

  program = "(apply + (list x I) (+ x I))";
  REQUIRE(run_and_expect_error(program));

  program = "(begin (define linear (lambda (a b x) (+ (* a x) b))) (apply linear (list 3 4 5)))";
  result = run(program);
  REQUIRE(result == Expression(Atom(19)));
}

TEST_CASE("Test handle_map", "[expression]"){

  std::string program = "(begin (define f (lambda (x) (* 2 x))) (map f (list 5)))";
  Expression result = run(program);
  REQUIRE(result.isList());

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

  program = "(map 3 (list 1 2 3 4))";
  REQUIRE(run_and_expect_error(program));

  program = "(apply + (+ x I))";
  REQUIRE(run_and_expect_error(program));
}

TEST_CASE("Test handle get/set property", "[expression]"){

  std::string program = "(begin (define f (set-property \"type\" \"number_list\" (list 0 1 2 3))))";
  Expression result = run(program);
  std::string program2 = "(list 0 1 2 3)";
  Expression expected_result = run(program2);
  REQUIRE(result == expected_result);

  std::string overwrite_property = "(define myList (set-property \"type\" \"HAHAHA GOTTEM\" (set-property \"type\" \"number_list\" (list 0 1 2 3))))";
  REQUIRE(run(overwrite_property)== expected_result);

  std::string first_arg_error = "(define myList (set-property not_a_string \"WILL FAIL\" (list 0 1 2 3)))";
  REQUIRE(run_and_expect_error(first_arg_error));

  std::string sp_too_many_args = "(define myList (set-property \"type\" \"WILL FAIL\" \"Because 4 args\" (list 1)))";
  REQUIRE(run_and_expect_error(sp_too_many_args));

  std::string getprop_program = "(get-property \"type\" (set-property \"type\" \"number_list\" (list 0 1 2 3)))";
  REQUIRE(run(getprop_program)== Expression(Atom("\"number_list\"")));

  std::string getprop_returns_none = "(get-property \"yeeet\" (set-property \"type\" \"number_list\" (list 0 1 2 3)))";
  REQUIRE(run(getprop_returns_none)== Expression());

  std::string first_arg_not_string = "(get-property 10 (set-property \"type\" \"number_list\" (list 0 1 2 3)))";
  REQUIRE(run_and_expect_error(first_arg_not_string));

  std::string secon_arg_proc = "(get-property \"size\" +)";
  REQUIRE(run_and_expect_error(secon_arg_proc));

  std::string gp_too_many_args = "(get-property \"yote\" (set-property \"type\" \"number_list\" (list 0 1 2 3)) \"my dude\")";
  REQUIRE(run_and_expect_error(gp_too_many_args));
}

TEST_CASE("Test output widget helper functions", "[expression]") {

  std::string program = "(define a (make-point 2 2))";

  Expression e = run(program);
  REQUIRE(e.isPoint());
  REQUIRE(!e.isLine());
  REQUIRE(!e.isText());
  REQUIRE(e.toString() == "((2) (2))");
  REQUIRE(e.getNumericalProperty("\"size\"") == 0);
  e.setPointSize(5.0);
  REQUIRE(e.getNumericalProperty("\"size\"") == 5.0);

  program = "(set-property \"thickness\" 4 (make-line (make-point 0 0) (make-point 3 3)))";
  e = run(program);
  REQUIRE(e.isLine());
  REQUIRE(!e.isPoint());
  REQUIRE(!e.isText());
  REQUIRE(e.toString() == "(((0) (0)) ((3) (3)))");
  REQUIRE(e.getNumericalProperty("\"thickness\"") == 4.0);
  e.setLineThickness(2.0);
  REQUIRE(e.getNumericalProperty("\"thickness\"") == 2.0);

  program = "(set-property \"text-scale\" 2 (set-property \"text-rotation\" 10 (set-property \"position\" (make-point 2 -2) (make-text \"General Kenobi\"))))";
  e = run(program);
  REQUIRE(e.isText());
  REQUIRE(!e.isPoint());
  REQUIRE(!e.isLine());
  std::tuple<double, double, double, double, bool> target2 = {2, -2, 2, 10, true};
  REQUIRE(e.getTextProperties() == target2);
}

TEST_CASE("Test handle discrete-plot", "[expression]") {
  std::string program;
  program = R"( (discrete-plot (list (list -1 -1) (list 1 1)) (list (list "title" "The Title") (list "abscissa-label" "X Label") (list "ordinate-label" "Y Label"))) )";
  Expression e = run(program);

  REQUIRE(e.isDP());
  REQUIRE(e.tailLength() == 17);

  program = "(discrete-plot (+ 2 3) (list (list 1)))";
  REQUIRE(run_and_expect_error(program));
  program = "(discrete-plot (list (list -1 -1) (list 1 1)) (list (list 1)) (list 2) (list (list 3)))";
  REQUIRE(run_and_expect_error(program));
}

TEST_CASE("Test handle continuous-plot", "[expression]") {
  std::string program;
  program = "(begin (define f (lambda (x) (+ (* 2 x) 1))) (continuous-plot f (list -2 2)))";
  Expression e = run(program);

  REQUIRE(e.isCP());
  REQUIRE(e.tailLength() == 60);
  REQUIRE(e.tailConstBegin()->isLine());

  program = R"(define f (lambda (x) (+ (* 2 x) 1))) (continuous-plot f (list -2 2) (list (list "title" "The Title") (list "abscissa-label" "X Label") (list "ordinate-label" "Y Label")) )";
  Expression r = run(program);

  program = "(begin (continuous-plot (* 1) (list -2 2)))";
  REQUIRE(run_and_expect_error(program));

  program = "(begin (define f (lambda (x) (+ (* 2 x) 1))) (continuous-plot f 5) )";
  REQUIRE(run_and_expect_error(program));

  program = "(begin (define f (lambda (x) (+ (* 2 x) 1))) (continuous-plot f (list 1 1 1) (list 1 1 1) (list 0)) )";
  REQUIRE(run_and_expect_error(program));

  program = "(begin (define f (lambda (x) (+ (* 2 x) 1))) (continuous-plot f (list 1 1 -1 -1) \"not a list\"))";
  REQUIRE(run_and_expect_error(program));
}