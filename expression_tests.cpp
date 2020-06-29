#include "catch.hpp"
#include "expression.hpp"

TEST_CASE( "Test default expression", "[expression]" ) {

  Expression exp;

  REQUIRE(!exp.head().isNumber());
  REQUIRE(!exp.head().isSymbol());
}

TEST_CASE( "Test double expression", "[expression]" ) {

  Expression exp(6.023);

  REQUIRE(exp.head().isNumber());
  REQUIRE(!exp.head().isSymbol());
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
