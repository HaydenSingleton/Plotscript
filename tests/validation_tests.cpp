#include "doctest.h"
#include <interpreter.h>


TEST_CASE("Math functions") {

	Interpreter calc;

	SUBCASE("Addition") {
		calc.parseStream(std::istringstream("(+ 2 3)"));
		CHECK_EQ(calc.evaluate().toString(), "(5)");
		
		calc.parseStream(std::istringstream("(+ 2 I)"));
		CHECK_EQ(calc.evaluate().toString(), "(2, 1)");
		
		calc.parseStream(std::istringstream("(+ 1 2 3 4 5)"));
		CHECK_EQ(calc.evaluate().toString(), "(15)");
	}

	SUBCASE("Subtraction") {
		std::istringstream text("(- 4 2)");
		calc.parseStream(text);
		CHECK_EQ(calc.evaluate().toString(), "(2)");
	}
}