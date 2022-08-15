#include "doctest.h"
#include <interpreter.h>

TEST_CASE("Interpreter tests") {

    Interpreter in;
	
	INFO("Interpreter parseStream tests");
	
	SUBCASE("string input") {
		std::string text = "(+ 1 2)";

		auto stream = std::istringstream(text);
		CHECK(in.parseStream(stream));
	}
		
}