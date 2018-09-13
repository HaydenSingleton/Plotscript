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

  INFO("default procedure")
  Procedure p1 = env.get_proc(Atom("doesnotexist"));
  Procedure p2 = env.get_proc(Atom("alsodoesnotexist"));
  REQUIRE(p1 == p2);
  std::vector<Expression> args;
  REQUIRE(p1(args) == Expression());
  REQUIRE(p2(args) == Expression());

  INFO("trying add procedure")
  Procedure padd = env.get_proc(Atom("+"));
  args.emplace_back(1.0);
  args.emplace_back(2.0);
  REQUIRE(padd(args) == Expression(3.0));
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

TEST_CASE( "Test arithmetic expressions", "[environment]" ) {

  Environment env;

  // Variables to test with

  std::vector<Expression> one_two;
  one_two.emplace_back(1.0);
  one_two.emplace_back(2.0);

  std::vector<Expression> four;
  four.emplace_back(4.0);

  std::vector<Expression> negative_four;
  negative_four.emplace_back(-4.0);

  std::vector<Expression> one;
  one.emplace_back(1.0);

  std::vector<Expression> pi;
  pi.emplace_back(env.get_exp(Atom("pi")));

  std::vector<Expression> complex_args;
  std::complex<double> c_one(1,1);
  complex_args.emplace_back(c_one);

  std::complex<double> c_two(2,2);
  complex_args.emplace_back(c_two);

  std::vector<Expression> complex_one;
  std::complex<double> c_zero_one(0,1);
  complex_one.emplace_back(c_zero_one);

  std::vector<Expression> not_a_number;
  not_a_number.emplace_back("F");


  /* Procedures to test */

  // multiply
  Procedure pmul = env.get_proc(Atom("*"));
  REQUIRE(pmul(one_two) == Expression(2.0));
  std::complex<double> four_I(0,4);
  REQUIRE(pmul(complex_args) == Expression(four_I));
  REQUIRE_THROWS_AS(pmul(not_a_number), SemanticError);

  // subtraction and negation
  Procedure psubneg = env.get_proc(Atom("-"));
  REQUIRE(psubneg(one_two) == Expression(-1.0));
  REQUIRE(psubneg(four) == Expression(-4.0));
  std::complex<double> zero_neg_one_I(0,-1);
  REQUIRE(psubneg(complex_one) == Expression(zero_neg_one_I));
  REQUIRE_THROWS_AS(psubneg(not_a_number), SemanticError);

  // division
  Procedure pdiv = env.get_proc(Atom("/"));
  REQUIRE(pdiv(one_two) == Expression(0.5));
  std::complex<double> onehalf_zero_I(0.5,0);
  REQUIRE(pdiv(complex_args) == Expression(onehalf_zero_I));
  REQUIRE_THROWS_AS(pdiv(not_a_number), SemanticError);

  // square root
  Procedure psqrt = env.get_proc(Atom("sqrt"));
  REQUIRE(psqrt(four) == Expression(2.0));
  std::complex<double> two_I(0,2);
  REQUIRE(psqrt(negative_four) == Expression(two_I));
  REQUIRE_THROWS_AS(psqrt(not_a_number), SemanticError);

  // power
  Procedure ppow = env.get_proc(Atom("^"));
  REQUIRE(ppow(one_two) == Expression(1.0));
  REQUIRE_THROWS_AS(ppow(not_a_number), SemanticError);

  // natural log
  Procedure pln = env.get_proc(Atom("ln"));
  REQUIRE(pln(one) == Expression(0));
  REQUIRE_THROWS_AS(pln(not_a_number), SemanticError);

  // sine
  Procedure psin = env.get_proc(Atom("sin"));
  REQUIRE(psin(pi) == Expression(0.0));
  REQUIRE_THROWS_AS(psin(not_a_number), SemanticError);

  // cosine
  Procedure pcos = env.get_proc(Atom("cos"));
  REQUIRE(pcos(pi) == Expression(-1.0));
  REQUIRE_THROWS_AS(pcos(not_a_number), SemanticError);

  // tangent
  Procedure ptan = env.get_proc(Atom("tan"));
  REQUIRE(ptan(pi) == Expression(0.0));
  REQUIRE_THROWS_AS(ptan(not_a_number), SemanticError);

}
