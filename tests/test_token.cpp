#include "doctest.h"
#include <token.h>

TEST_CASE("Test Token creation") {

	Token tko(Token::OPEN);

	CHECK(tko.type() == Token::OPEN);
	CHECK(tko.toString() == "(");

	Token tkc(Token::CLOSE);

	CHECK(tkc.type() == Token::CLOSE);
	CHECK(tkc.toString() == ")");

	Token tks("thevalue");

	CHECK(tks.type() == Token::STRING);
	CHECK(tks.toString() == "thevalue");

	Token tkq("\"a quote\"");

	CHECK(tkq.type() == Token::STRING);
	CHECK(tkq.toString() == "\"a quote\"");
}

TEST_CASE("Test tokenize") {
	std::string input(R"(( A a aa )aal (aalii)) 3 ;asdfd)");
	std::istringstream iss(input);

	TokenSequence tokens = tokenize(iss);

	CHECK(tokens.front().type() == Token::OPEN);
	tokens.pop_front();

	CHECK(tokens.front().type() == Token::STRING);
	CHECK(tokens.front().toString() == "A");
	tokens.pop_front();

	CHECK(tokens.front().type() == Token::STRING);
	CHECK(tokens.front().toString() == "a");
	tokens.pop_front();

	CHECK(tokens.front().type() == Token::STRING);
	CHECK(tokens.front().toString() == "aa");
	tokens.pop_front();

	CHECK(tokens.front().type() == Token::CLOSE);
	tokens.pop_front();

	CHECK(tokens.front().type() == Token::STRING);
	CHECK(tokens.front().toString() == "aal");
	tokens.pop_front();

	CHECK(tokens.front().type() == Token::OPEN);
	tokens.pop_front();

	CHECK(tokens.front().type() == Token::STRING);
	CHECK(tokens.front().toString() == "aalii");
	tokens.pop_front();

	CHECK(tokens.front().type() == Token::CLOSE);
	tokens.pop_front();

	CHECK(tokens.front().type() == Token::CLOSE);
	tokens.pop_front();

	CHECK(tokens.front().type() == Token::STRING);
	CHECK(tokens.front().toString() == "3");
	tokens.pop_front();

	CHECK(tokens.empty());
}