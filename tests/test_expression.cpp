#include "doctest.h"
#include <expression.h>
#include <environment.h>
#include <semantic_error.h>

TEST_CASE("Expression constructors") {

	Environment env = Environment();

	SUBCASE("Default constructor") {
		Expression exp;
		CHECK(exp == Expression());
		CHECK_THROWS_AS(exp.eval(env), SemanticError);
	}

	SUBCASE("Value constructor") {
		Expression e(1.01);
		Expression s(Token("*"));

		CHECK(e.head().asNumber() == 1.01);
		CHECK(!e.head().isComplex());

		CHECK(e != s);
		CHECK(s.head().toString() == "*");
		CHECK(s.tail() == nullptr);

		Atom a("+");
		Expression plus(a);
		CHECK(plus.head() == Atom("+"));
	}
}