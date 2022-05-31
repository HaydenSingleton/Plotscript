#include "doctest.h"
#include <interpreter.h>


TEST_CASE("Math functions") {

	Interpreter calc;

	SUBCASE("Addition") {
		auto stream = std::istringstream(std::string("(+ 2 3)"));
		calc.parseStream(stream);
		CHECK_EQ(calc.evaluate().toString(), "(5)");
		
		stream = std::istringstream(std::string("(+ 2 I)"));
		calc.parseStream(stream);
		CHECK_EQ(calc.evaluate().toString(), "(2, 1)");
		
		stream = std::istringstream("(+ 1 2 3 4 5)");
		calc.parseStream(stream);
		CHECK_EQ(calc.evaluate().toString(), "(15)");
	}

	SUBCASE("Subtraction") {
		std::istringstream text("(- 4 2)");
		calc.parseStream(text);
		CHECK_EQ(calc.evaluate().toString(), "(2)");
	}
}