#include "catch.hpp"

#include "environment.hpp"
#include "semantic_error.hpp"

#include <cmath>

TEST_CASE( "Test default constructor", "[environment]" ) {

  Environment env;

  REQUIRE(env.is_known(Atom("pi")));
  REQUIRE(env.is_exp(Atom("pi")));

  REQUIRE(!env.is_known(Atom("hi")));
  REQUIRE(!env.is_exp(Atom("hi")));

  REQUIRE(env.is_proc(Atom("+")));
  REQUIRE(env.is_proc(Atom("-")));
  REQUIRE(env.is_proc(Atom("*")));
  REQUIRE(env.is_proc(Atom("/")));
  REQUIRE(!env.is_proc(Atom("op")));
}

TEST_CASE( "Test get expression", "[environment]" ) {
  Environment env;

  REQUIRE(env.get_exp(Atom("pi")) == Expression(std::atan2(0, -1)));
  REQUIRE(env.get_exp(Atom("hi")) == Expression());
}

TEST_CASE( "Test add expression", "[environment]" ) {
  Environment env;

  Expression a(Atom(1.0));
  env.add_exp(Atom("one"), a);
  REQUIRE(env.is_known(Atom("one")));
  REQUIRE(env.is_exp(Atom("one")));
  REQUIRE(env.get_exp(Atom("one")) == a);

  Expression b(Atom("hello"));
  env.add_exp(Atom("hi"), b);
  REQUIRE(env.is_known(Atom("hi")));
  REQUIRE(env.is_exp(Atom("hi")));
  REQUIRE(env.get_exp(Atom("hi")) == b);

  REQUIRE_THROWS_AS(env.add_exp(Atom(1.0), b), SemanticError);
}

TEST_CASE( "Test get built-in procedure", "[environment]" ) {
	Environment env;

	INFO("default procedure");
	Procedure p1 = env.get_proc(Atom("doesnotexist"));
	Procedure p2 = env.get_proc(Atom("alsodoesnotexist"));
	REQUIRE(p1 == p2);
	std::vector<Expression> args;
	REQUIRE(p1(args) == Expression());
	REQUIRE(p2(args) == Expression());

	INFO("trying add procedure");
	Procedure padd = env.get_proc(Atom("+"));
	args.emplace_back(1.0);
	args.emplace_back(2.0);
	REQUIRE(padd(args) == Expression(3.0));

	INFO("add with complex args");
	std::complex<double> a(0, 1), b(1, 0), c(1, 1);
	args.clear();
	args.emplace_back(a);
	args.emplace_back(b);
	Expression result = padd(args);
	REQUIRE(result.isHeadComplex());
	REQUIRE(Expression(c).isHeadComplex());
	REQUIRE(result == Expression(Atom(c)));

	INFO("add semantic error")
	REQUIRE_THROWS_AS(padd(std::vector<Expression>{ Expression(std::string("not_valid_input")) }), SemanticError);
}

TEST_CASE( "Test reset", "[environment]" ) {
  Environment env;

  Expression a(Atom(1.0));
  env.add_exp(Atom("one"), a);
  Expression b(Atom("hello"));
  env.add_exp(Atom("hi"), b);

  env.reset();
  REQUIRE(!env.is_known(Atom("one")));
  REQUIRE(!env.is_exp(Atom("one")));
  REQUIRE(env.get_exp(Atom("one")) == Expression());
  REQUIRE(!env.is_known(Atom("hi")));
  REQUIRE(!env.is_exp(Atom("hi")));
  REQUIRE(env.get_exp(Atom("hi")) == Expression());
}

TEST_CASE( "Test semeantic errors", "[environment]" ) {

  Environment env;

  {
    Expression exp(Atom("begin"));

    REQUIRE_THROWS_AS(exp.eval(env), SemanticError);
  }
}

TEST_CASE("Test multiply procedure", "[environment]") {

	Environment env;
	Procedure pmul = env.get_proc(Atom("*"));

	INFO("mul real args");
	std::vector<Expression> one_two = { Expression(1.0), Expression(2.0) };
	REQUIRE(pmul(one_two) == Expression(2.0));

	INFO("mul complex args");
	std::vector<Expression> complex_args = { Expression(std::complex<double>(1, 1)), Expression(std::complex<double>(2, 2)) };
	REQUIRE(pmul(complex_args) == Expression(std::complex<double>(0, 4)));

	INFO("mul error message");
	std::vector<Expression> not_a_number = { Expression(std::string("F")), Expression(std::string("F")) };
	REQUIRE_THROWS_AS(pmul(not_a_number), SemanticError);
}

TEST_CASE("Test subneg procedure", "[environment]") {

	Environment env;
	Procedure psubneg = env.get_proc(Atom("-"));

	INFO("subneg real arguments");
	std::vector<Expression> one_two = { Expression(1.0), Expression(2.0) };
	REQUIRE(psubneg(one_two) == Expression(-1.0));
	std::vector<Expression> four = { Expression(4.0) };
	REQUIRE(psubneg(four) == Expression(-4.0));

	INFO("subneg complex arguments");
	std::vector<Expression> complex_one = { Expression(std::complex<double>(0, 1)) };
	REQUIRE(psubneg(complex_one) == Expression(std::complex<double>(0, -1)));
	std::vector<Expression> complex_args = { Expression(std::complex<double>(1, 1)), Expression(std::complex<double>(2, 2)) };
	REQUIRE(psubneg(complex_args) == Expression(std::complex<double>(-1, -1)));

	INFO("subneg error messages");
	std::vector<Expression> not_a_number = { Expression(std::string("F")) };
	REQUIRE_THROWS_AS(psubneg(not_a_number), SemanticError);
	std::vector<Expression> two_not_numbers = { Expression(std::string("pay")), Expression(std::string("respect")) };
	REQUIRE_THROWS_AS(psubneg(two_not_numbers), SemanticError);
	std::vector<Expression> too_many_args = { Expression(1.1), Expression(2.2), Expression(3.3) };
	REQUIRE_THROWS_AS(psubneg(too_many_args), SemanticError);
}

TEST_CASE("Test division procedure", "[environment]") {

	Environment env;
	Procedure pdiv = env.get_proc(Atom("/"));

	INFO("Div with real args");
	std::vector<Expression> one_two = { Expression(1.0), Expression(2.0) };
	REQUIRE(pdiv(one_two) == Expression(0.5));

	INFO("Div with complex args");
	std::vector<Expression> complex_args = { Expression(std::complex<double>(1, 1)), Expression(std::complex<double>(2, 2)) };
	REQUIRE(pdiv(complex_args) == Expression(std::complex<double>(0.5, 0)));

	INFO("Div error messages");
	std::vector<Expression> not_a_number = { Expression(std::string("F")), Expression(std::string("one")) };
	REQUIRE_THROWS_AS(pdiv(not_a_number), SemanticError);
	std::vector<Expression> too_many_args = { Expression(1.0), Expression(2.0), Expression(3.0) };
	REQUIRE_THROWS_AS(pdiv(too_many_args), SemanticError);
}

TEST_CASE("Test square root procedure", "[environment]") {

	Environment env;
	Procedure psqrt = env.get_proc(Atom("sqrt"));

	INFO("Square root with real args");
	std::vector<Expression> four = { Expression(4.0) };
	REQUIRE(psqrt(four) == Expression(2.0));

	INFO("Square root with complex or negative args");
	std::vector<Expression> negative_four = { Expression(-4.0) };
	REQUIRE(psqrt(negative_four) == Expression(std::complex<double>(0, 2)));
	std::vector<Expression> complex_two_I = { Expression(std::complex<double>(0,2)) };
	REQUIRE(psqrt(complex_two_I) == Expression(std::complex<double>(1,1)));

	INFO("Square root error message");
	std::vector<Expression> not_a_number = { Expression(std::string("F")) };
	REQUIRE_THROWS_AS(psqrt(not_a_number), SemanticError);
	std::vector<Expression> too_many_args = { Expression(45.0), Expression(420) };
	REQUIRE_THROWS_AS(psqrt(too_many_args), SemanticError);
}

TEST_CASE("Test power procedure", "[environment]") {

	Environment env;
	Procedure ppow = env.get_proc(Atom("^"));

	std::complex<double> c_zero_one(0, 1);
	std::vector<Expression> complex_one = { Expression(c_zero_one) };


	INFO("Pow with real argument");
	std::vector<Expression> real_args = { Expression(2.0), Expression(3.0) };
	REQUIRE(ppow(real_args) == Expression(8.0));

	INFO("Pow with complex arguments");
	std::vector<Expression> complex_args = { Expression(std::complex<double>(0, 1)), Expression(5.0) };
	REQUIRE(ppow(complex_args) == Expression(std::complex<double>(0,1)));

	INFO("Pow error messages");
	std::vector<Expression> not_a_number = { Expression(std::string("F")), Expression(5.0) };
	REQUIRE_THROWS_AS(ppow(not_a_number), SemanticError);
	std::vector<Expression> too_many_args = { Expression(4.0), Expression(1), Expression(std::complex<double>(3,3)) };
	REQUIRE_THROWS_AS(ppow(too_many_args), SemanticError);
}

TEST_CASE("Test natural log procedure", "[environment]") {

	Environment env;
	Procedure pln = env.get_proc(Atom("ln"));

	INFO("Natural log with real argument");
    std::vector<Expression> one = { Expression(1.0) };
    REQUIRE(pln(one) == Expression(0));
    std::vector<Expression> e = { Expression(env.get_exp(Atom("e"))) };
    REQUIRE(pln(e) == Expression(1.0));

	INFO("Natural log error messages");
    std::vector<Expression> complex_arg = { Expression(std::complex<double>(0, 1)) };
    REQUIRE_THROWS_AS(pln(complex_arg), SemanticError);
    std::vector<Expression> not_a_number = { Expression(std::string("F")) };
    REQUIRE_THROWS_AS(pln(not_a_number), SemanticError);
    std::vector<Expression> too_many_args = { Expression(4.0), Expression(1), Expression(std::complex<double>(3,3)) };
    REQUIRE_THROWS_AS(pln(too_many_args), SemanticError);
}

TEST_CASE("Test sine procedure", "[environment]") {

	Environment env;
	std::vector<Expression> pi = { env.get_exp(Atom("pi")) };
	std::vector<Expression> four = { Expression(4.0) };
	std::complex<double> c_zero_one(0, 1);
	std::vector<Expression> complex_one = { Expression(c_zero_one) };
	std::vector<Expression> not_a_number = { Expression(std::string("F")) };

	Procedure psin = env.get_proc(Atom("sin"));
	REQUIRE(psin(pi) == Expression(0.0));
	REQUIRE_THROWS_AS(psin(not_a_number), SemanticError);
}

TEST_CASE("Test cosine procedure", "[environment]") {

	Environment env;
	std::vector<Expression> pi = { env.get_exp(Atom("pi")) };
	std::vector<Expression> four = { Expression(4.0) };
	std::complex<double> c_zero_one(0, 1);
	std::vector<Expression> complex_one = { Expression(c_zero_one) };
	std::vector<Expression> not_a_number = { Expression(std::string("F")) };

	Procedure pcos = env.get_proc(Atom("cos"));
	REQUIRE(pcos(pi) == Expression(-1.0));
	REQUIRE_THROWS_AS(pcos(not_a_number), SemanticError);
}

TEST_CASE("Test tangent procedure", "[environment]") {

	Environment env;
	std::vector<Expression> pi = { env.get_exp(Atom("pi")) };
	std::vector<Expression> four = { Expression(4.0) };
	std::complex<double> c_zero_one(0, 1);
	std::vector<Expression> complex_one = { Expression(c_zero_one) };
	std::vector<Expression> not_a_number = { Expression(std::string("F")) };

	Procedure ptan = env.get_proc(Atom("tan"));
	REQUIRE(ptan(pi) == Expression(0.0));
	REQUIRE_THROWS_AS(ptan(not_a_number), SemanticError);
}
