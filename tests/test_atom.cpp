#include "doctest.h"
#include <atom.h>

TEST_CASE("Test atom constructors") {
	SUBCASE("Default constructor") {
		Atom a;
		CHECK(a.isNone());
		CHECK_EQ(a, Atom());
		CHECK(!a.isComplex());
		CHECK(!a.isSymbol());
		CHECK(!a.isNumber());
		REQUIRE(a.isNone());
	}
	SUBCASE("Number constructor") {
		Atom a(1.2);
		CHECK(a.isNumber());
		CHECK(!a.isSymbol());
		CHECK(!a.isComplex());
	}
	SUBCASE("Symbol constructor") {
		Atom a("+/-");
		CHECK(a.isSymbol());
		CHECK_EQ(a, Atom("+/-"));
		Atom b = a;
		CHECK_EQ(a, b);
		CHECK(b.isSymbol());
		CHECK(b.asNumber() == 0);
	}
}

TEST_CASE("Test atom comparisons") {

    SUBCASE("compare default to default")
    {
        Atom a;
        Atom b;
        CHECK(a == b);
    }

    SUBCASE("compare default to number")
    {
        Atom a;
        Atom b(1.0);
        CHECK(a != b);
    }

    SUBCASE("compare default to symbol")
    {
        Atom a;
        Atom b("hi");
        CHECK(a != b);
    }

    SUBCASE("compare number to default")
    {
        Atom a(1.0);
        Atom b;
        CHECK(a != b);
    }

    SUBCASE("compare number to number")
    {
        Atom a(1.0);
        Atom b(1.0);
        Atom c(2.0);
        CHECK(a == b);
        CHECK(a != c);
    }

    SUBCASE("compare number to symbol")
    {
        Atom a(1.0);
        Atom b("hi");
        CHECK(a != b);
    }

    SUBCASE("compare symbol to default")
    {
        Atom a("hi");
        Atom b;
        CHECK(a != b);
    }

    SUBCASE("compare symbol to number")
    {
        Atom a("hi");
        Atom b(1.0);
        CHECK(a != b);
    }

    SUBCASE("compare symbol to symbol")
    {
        Atom a("hi");
        Atom b("hi");
        Atom c("bye");
        CHECK(a == b);
        CHECK(a != c);
    }
}