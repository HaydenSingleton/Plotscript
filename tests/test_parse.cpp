#include "doctest.h"
#include <parse.h>

TEST_CASE("Parser") {

	SUBCASE("Test parser with expected input") {

		std::string program = "(begin (define r 10) (* pi (* r r)))";
		std::istringstream iss(program);

		TokenSequence tokens = tokenize(iss);
		CHECK_NE(parse(tokens), Expression());
	}

	SUBCASE("Test unbalanced parens") {

		std::string program = "((begin (+ 1))))))";
		std::istringstream iss(program);

		TokenSequence tokens = tokenize(iss);
		Expression out = parse(tokens);
		CHECK_EQ(out, Expression());
	}

	SUBCASE("Test bad Number literal") {

		std::string program = "(define a 1.2abc)";
		std::istringstream iss(program);

		TokenSequence tokens = tokenize(iss);
		Expression out = parse(tokens);
		CHECK_EQ(out, Expression());
	}

	SUBCASE("Test missing parens") {

		std::string program = "+ 1 2";
		std::istringstream iss(program);

		TokenSequence tokens = tokenize(iss);
		Expression out = parse(tokens);
		CHECK_EQ(out, Expression());
	}

	SUBCASE("Test empty parens") {

		std::string program = "()";
		std::istringstream iss(program);

		TokenSequence tokens = tokenize(iss);
		Expression out = parse(tokens);
		CHECK_EQ(out, Expression());
	}

	SUBCASE("Test open quotes") {

		std::string program = "(\"this string is unbalanced)";
		std::istringstream iss(program);
		REQUIRE(iss.good());

		TokenSequence tokens = tokenize(iss);
		CHECK_FALSE(tokens.empty());

		Expression out = parse(tokens);
		INFO(out.toString());
		CHECK_EQ(out.toString(), "NONE");
	}

	SUBCASE("Test closed quotes") {

		std::string program = "(\"Hello World!\")";
		std::istringstream iss(program);

		TokenSequence tokens = tokenize(iss);
		Expression out = parse(tokens);
		CHECK_EQ(out, Expression(Atom("\"Hello World!\"")));
	}
}