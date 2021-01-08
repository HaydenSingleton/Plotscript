#include "catch.hpp"
#include "expression.hpp"

TEST_CASE( "Test default expression", "[expression]" ) {

  Expression exp;

  REQUIRE(exp.isNone());

  REQUIRE(!exp.head().isNumber());
  REQUIRE(!exp.head().isSymbol());
}

TEST_CASE( "Test double expression", "[expression]" ) {

  Expression exp(6.023);
  Expression exp2(Atom(6.023));


  REQUIRE(exp.head().isNumber());
  REQUIRE(!exp.head().isSymbol());

  REQUIRE(exp2.head().isNumber());
  REQUIRE(!exp2.head().isSymbol());
}


TEST_CASE( "Test symbol expression", "[expression]" ) {

  Expression exp(Atom("asymbol"));

  REQUIRE(!exp.head().isNumber());
  REQUIRE(exp.head().isSymbol());
}

TEST_CASE( "Test string expression", "[expression]") {

  Expression exp(Atom("\"yeet! Dab on em.\""));

  REQUIRE(exp.head().isString());
  REQUIRE(!exp.head().isSymbol());
}

TEST_CASE( "Test constructor default", "[expression]") {
  Expression a = Expression();
  REQUIRE(a.isNone());
}

TEST_CASE( "Test constructor basic", "[expression]") {
  Expression a = Expression(1);
  Expression b = Expression(Atom(1));

  REQUIRE(a == b);
}

TEST_CASE( "Test constructor lists", "[expression]") {
  std::vector<Expression> items = {Expression(1), Expression(2)};
  Expression li = Expression(items);
  Expression li2 = Expression(std::vector<Expression>{});

  REQUIRE(li.isList());
  REQUIRE(li2.isList());
}

TEST_CASE( "Test constructor lambda", "[expression]") {
  std::vector<Expression> args = {Expression(1), Expression(2)};
  Expression fun;
  Expression la = Expression(args, fun);

  REQUIRE(la.isLambda());

}

TEST_CASE( "Test constructor graphics", "[expression]") {

}


TEST_CASE( "Test constructor plots", "[expression]") {

}

