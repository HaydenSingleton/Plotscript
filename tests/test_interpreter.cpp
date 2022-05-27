#include "doctest.h"
#include <interpreter.h>

TEST_CASE("Interpreter tests") {

    Interpreter in;

	SUBCASE("input: (+ 1 2)") {
		std::istringstream valid("(+ 1 2)");
		CHECK(in.parseStream(valid));
	}
	SUBCASE("input: (+ 1 2") {
		std::istringstream invalid("(+ 1 2");
		CHECK(!in.parseStream(invalid));
		CHECK_THROWS(in.evaluate());
	}
}