#include "catch.hpp"

#include "atom.hpp"

TEST_CASE( "Test atom constructors", "[atom]" ) {

  {
    INFO("Default Constructor");
    Atom a;

    REQUIRE(a.isNone());
    REQUIRE(a.operator==(Atom()));
    REQUIRE(!a.isString());
    REQUIRE(!a.isSymbol());
  }

  {
    INFO("Number Constructor");
    Atom a(1.0);

    REQUIRE(!a.isNone());
    REQUIRE(a.isNumber());
    REQUIRE(!a.isSymbol());
  }

  {
    INFO("Symbol Constructor");
    Atom a("hi");

    REQUIRE(!a.isNone());
    REQUIRE(!a.isNumber());
    REQUIRE(a.isSymbol());
  }


  {
    INFO("Token Constructor");
    Token t("hi");
    Atom a(t);

    REQUIRE(!a.isNone());
    REQUIRE(!a.isNumber());
    REQUIRE(a.isSymbol());

    Token ts("\"hee hee\"");
    Atom b(ts);

    REQUIRE(!b.isNone());
    REQUIRE(!b.isNumber());
    REQUIRE(!b.isSymbol());
    REQUIRE(b.isString());
  }

  {
    INFO("Copy constructor");

    Atom a("hi");
    Atom c(a);
    REQUIRE(!a.isNone());
    REQUIRE(!c.isNumber());
    REQUIRE(c.isSymbol());

    Atom b(1.0);
    Atom d(b);
    REQUIRE(!a.isNone());
    REQUIRE(d.isNumber());
    REQUIRE(!d.isSymbol());

    Atom f("\"string my guy\"");
    Atom e(f);
    REQUIRE(!e.isNone());
    REQUIRE(!f.isNone());
    REQUIRE(!e.isNumber());
    REQUIRE(!f.isNumber());
    REQUIRE(!e.isSymbol()); 
    REQUIRE(!f.isSymbol());
    REQUIRE(e.isString());
    REQUIRE( f.isString());
  }
}

TEST_CASE( "Test atom assignments", "[atom]" ) {

  {
    INFO("default to default");
    Atom a;
    Atom b;
    b = a;
    REQUIRE(b.isNone());
    REQUIRE(!b.isNumber());
    REQUIRE(!b.isSymbol());
  }

  {
    INFO("default to number");
    Atom a;
    Atom b(1.0);
    b = a;
    REQUIRE(b.isNone());
    REQUIRE(!b.isNumber());
    REQUIRE(!b.isSymbol());
  }

  {
    INFO("default to symbol");
    Atom a;
    Atom b("hi");
    b = a;
    REQUIRE(b.isNone());
    REQUIRE(!b.isNumber());
    REQUIRE(!b.isSymbol());
  }

  {
    INFO("default to string");
    Atom a;
    Atom b("\"hi mom\"");
    b = a;
    REQUIRE(b.isNone());
    REQUIRE(!b.isNumber());
    REQUIRE(!b.isSymbol());
  }

  {
    INFO("number to default");
    Atom a(1.0);
    Atom b;
    b = a;
    REQUIRE(b.isNumber());
    REQUIRE(b.asNumber() == 1.0);
  }

  {
    INFO("number to number");
    Atom a(1.0);
    Atom b(2.0);
    b = a;
    REQUIRE(b.isNumber());
    REQUIRE(b.asNumber() == 1.0);
  }

  {
    INFO("number to symbol");
    Atom a("hi");
    Atom b(1.0);
    b = a;
    REQUIRE(b.isSymbol());
    REQUIRE(b.asSymbol() == "hi");
  }

  {
    INFO("number to string");
    Atom a("\"hi\"");
    Atom b;
    b = a;
    REQUIRE(b.isString());
    REQUIRE(b.asSymbol() == "\"hi\"");
    REQUIRE_FALSE(b.isNone());
  }

  {
    INFO("symbol to default");
    Atom a("hi");
    Atom b;
    b = a;
    REQUIRE(b.isSymbol());
    REQUIRE(b.asSymbol() == "hi");
  }

  {
    INFO("symbol to number");
    Atom a("hi");
    Atom b(1.0);
    b = a;
    REQUIRE(b.isSymbol());
    REQUIRE(b.asSymbol() == "hi");
  }

  {
    INFO("symbol to symbol");
    Atom a("hi");
    Atom b("bye");
    b = a;
    REQUIRE(b.isSymbol());
    REQUIRE(b.asSymbol() == "hi");
  }

  {
    INFO("default to string");
    Atom x;
    Atom y("\"str\"");
    REQUIRE(y.isString());
    REQUIRE(y.asString()=="\"str\"");
    x = y;
    REQUIRE(x.isString());
    REQUIRE(x.asString()=="\"str\"");
  }
}

TEST_CASE( "Test atom comparisons", "[atom]" ) {

  {
    INFO("compare default to default");
    Atom a;
    Atom b;
    REQUIRE(a == b);
  }

  {
    INFO("compare default to number");
    Atom a;
    Atom b(1.0);
    REQUIRE(a != b);
  }

  {
    INFO("compare default to symbol");
    Atom a;
    Atom b("hi");
    REQUIRE(a != b);
  }

  {
    INFO("compare number to default");
    Atom a(1.0);
    Atom b;
    REQUIRE(a != b);
  }

  {
    INFO("compare number to number");
    Atom a(1.0);
    Atom b(1.0);
    Atom c(2.0);
    REQUIRE(a == b);
    REQUIRE(a != c);
  }

  {
    INFO("compare number to symbol");
    Atom a(1.0);
    Atom b("hi");
    REQUIRE(a != b);
  }

  {
    INFO("compare symbol to default");
    Atom a("hi");
    Atom b;
    REQUIRE(a != b);
  }

  {
    INFO("compare symbol to number");
    Atom a("hi");
    Atom b(1.0);
    REQUIRE(a != b);
  }

  {
    INFO("compare symbol to symbol");
    Atom a("hi");
    Atom b("hi");
    Atom c("bye");
    REQUIRE(a == b);
    REQUIRE(a != c);
  }

  {
    INFO("compare string to string");
    Atom a("\"The Lord of the Rings\"");
    Atom b("\"The Lord of the Rings\"");
    Atom c("The Lord of the Rings");
    REQUIRE(a.isString());
    REQUIRE(c.isSymbol());
    REQUIRE(a!=c);
    REQUIRE(a==b);
  }

}



