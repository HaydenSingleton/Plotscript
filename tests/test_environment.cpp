#include "doctest.h"
#include <environment.h>

TEST_CASE("Environment default state") {

	Environment env;

	Atom PI("pi");
	Atom PIE("pie");

	CHECK(env.is_known(PI));
	CHECK(env.is_exp(PI));
	CHECK(!env.is_proc(PI));

	CHECK(env.is_proc(Atom("+")));
	CHECK(env.is_proc(Atom("-")));
	CHECK(env.is_proc(Atom("*")));
	CHECK(env.is_proc(Atom("/")));
	CHECK(!env.is_proc(Atom("op")));

	SUBCASE("get helper function") {
		CHECK(env.get_exp(PI).head().asNumber() == std::atan2(0, -1));
		CHECK(env.get_exp(PI) == Expression(std::atan2(0, -1)));

		Expression result = env.get_exp(PIE);
		CHECK(result == Expression());
		CHECK(result.head().isNone());
	}

	SUBCASE("add helper function") {

		Expression exp(Atom(1.0));
		Atom sym("one");
		
		CHECK(!env.is_known(sym));
		env.add_exp(sym, exp);
		CHECK(env.is_known(sym));
	}
}
