#include "doctest.h"
#include <interpreter.h>

TEST_CASE("Math functions") {

	Interpreter calc;

	SUBCASE("Addition") {
		std::string expr("(+ 2 3)");
		CHECK(calc.parseString(expr));
		std::string result = calc.evaluate().toString();
		CHECK_EQ(result, "(5)");
		
		auto stream = std::istringstream(std::string("(+ 2 I)"));
		INFO(stream);
		CHECK(calc.parseStream(stream));
		CHECK_EQ(calc.evaluate().toString(), "(2, 1)");
		
		stream = std::istringstream("(+ 1 2 3 4 5)");
		CHECK(calc.parseStream(stream));
		CHECK_EQ(calc.evaluate().toString(), "(15)");
	}

	SUBCASE("Subtraction") {
		std::istringstream text("(- 4 2)");
		CHECK(calc.parseStream(text));
		CHECK_EQ(calc.evaluate().toString(), "(2)");
	}
}