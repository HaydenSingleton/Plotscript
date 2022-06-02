#include "doctest.h"
#include <interpreter.h>

TEST_CASE("Interpreter tests") {

    Interpreter in;
	
	INFO("Interpreter parseStream tests");
	
	SUBCASE("string input") {
		std::string valid = "(+ 1 2)";
		CHECK(in.parseStream(valid));
	}

	
}