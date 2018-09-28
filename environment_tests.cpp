#include "catch.hpp"

#include "environment.hpp"
#include "semantic_error.hpp"

#include <cmath>

TEST_CASE( "Test default constructor", "[environment]" ) {

  Environment env;

  REQUIRE(env.is_known(Atom("pi")));
  REQUIRE(env.is_exp(Atom("pi")));

  REQUIRE(!env.is_known(Atom("hi")));
  REQUIRE(!env.is_exp(Atom("hi")));

  REQUIRE(env.is_proc(Atom("+")));
  REQUIRE(env.is_proc(Atom("-")));
  REQUIRE(env.is_proc(Atom("*")));
  REQUIRE(env.is_proc(Atom("/")));
  REQUIRE(!env.is_proc(Atom("op")));
}

TEST_CASE( "Test assignment constructor", "[environment]" ) {

  Environment env;

  REQUIRE(env.is_known(Atom("pi")));
  REQUIRE(env.is_exp(Atom("pi")));

  REQUIRE(!env.is_known(Atom("hi")));
  REQUIRE(!env.is_exp(Atom("hi")));

  env.add_exp(Atom("yosh"), Expression(5));
  REQUIRE(env.is_known(Atom("yosh")));

  Environment copy_of_env = env;

  REQUIRE(copy_of_env.is_known(Atom("yosh")));

  copy_of_env.add_exp(Atom("only_in_Copy"), Expression(5));
  REQUIRE(copy_of_env.is_known(Atom("only_in_Copy")));
  REQUIRE(!env.is_known(Atom("only_in_Copy")));

  REQUIRE(copy_of_env.is_known(Atom("pi")));
  REQUIRE(copy_of_env.is_exp(Atom("pi")));

  REQUIRE(!copy_of_env.is_known(Atom("hi")));
  REQUIRE(!copy_of_env.is_exp(Atom("hi")));
}

TEST_CASE( "Test get expression", "[environment]" ) {
  Environment env;

  REQUIRE(env.get_exp(Atom("pi")) == Expression(std::atan2(0, -1)));
  REQUIRE(env.get_exp(Atom("hi")) == Expression());
}

TEST_CASE( "Test add expression", "[environment]" ) {
  Environment env;

  Expression a(Atom(1.0));
  env.add_exp(Atom("one"), a);
  REQUIRE(env.is_known(Atom("one")));
  REQUIRE(env.is_exp(Atom("one")));
  REQUIRE(env.get_exp(Atom("one")) == a);

  Expression b(Atom("hello"));
  env.add_exp(Atom("hi"), b);
  REQUIRE(env.is_known(Atom("hi")));
  REQUIRE(env.is_exp(Atom("hi")));
  REQUIRE(env.get_exp(Atom("hi")) == b);

  REQUIRE_THROWS_AS(env.add_exp(Atom(1.0), b), SemanticError);
}

TEST_CASE( "Test get built-in procedure", "[environment]" ) {
	Environment env;

	INFO("default procedure");
	Procedure p1 = env.get_proc(Atom("doesnotexist"));
	Procedure p2 = env.get_proc(Atom("alsodoesnotexist"));
	REQUIRE(p1 == p2);
	std::vector<Expression> args;
	REQUIRE(p1(args) == Expression());
	REQUIRE(p2(args) == Expression());

	INFO("trying add procedure");
	Procedure padd = env.get_proc(Atom("+"));
	args.emplace_back(1.0);
	args.emplace_back(2.0);
	REQUIRE(padd(args) == Expression(3.0));

	INFO("add with complex args");
	std::complex<double> a(0, 1), b(1, 0), c(1, 1);
	args.clear();
	args.emplace_back(a);
	args.emplace_back(b);
	Expression result = padd(args);
	REQUIRE(result.isHeadComplex());
	REQUIRE(Expression(c).isHeadComplex());
	REQUIRE(result == Expression(Atom(c)));

	INFO("add semantic error")
	REQUIRE_THROWS_AS(padd(std::vector<Expression>{ Expression(std::string("not_valid_input")) }), SemanticError);
}

TEST_CASE( "Test reset", "[environment]" ) {
  Environment env;

  Expression a(Atom(1.0));
  env.add_exp(Atom("one"), a);
  Expression b(Atom("hello"));
  env.add_exp(Atom("hi"), b);

  env.reset();
  REQUIRE(!env.is_known(Atom("one")));
  REQUIRE(!env.is_exp(Atom("one")));
  REQUIRE(env.get_exp(Atom("one")) == Expression());
  REQUIRE(!env.is_known(Atom("hi")));
  REQUIRE(!env.is_exp(Atom("hi")));
  REQUIRE(env.get_exp(Atom("hi")) == Expression());
}

TEST_CASE( "Test semeantic errors", "[environment]" ) {

  Environment env;

  {
    Expression exp(Atom("begin"));

    REQUIRE_THROWS_AS(exp.eval(env), SemanticError);
  }
}

TEST_CASE("Test multiply procedure", "[environment]") {

	Environment env;
	Procedure pmul = env.get_proc(Atom("*"));

	std::vector<Expression> one_two = { Expression(1.0), Expression(2.0) };
	REQUIRE(pmul(one_two) == Expression(2.0));

	std::vector<Expression> complex_args = { Expression(std::complex<double>(1, 1)), Expression(std::complex<double>(2, 2)) };
	REQUIRE(pmul(complex_args) == Expression(std::complex<double>(0, 4)));

	std::vector<Expression> not_a_number = { Expression(std::string("F")), Expression(std::string("F")) };
	REQUIRE_THROWS_AS(pmul(not_a_number), SemanticError);
}

TEST_CASE("Test subneg procedure", "[environment]") {

	Environment env;
	Procedure psubneg = env.get_proc(Atom("-"));

	std::vector<Expression> one_two = { Expression(1.0), Expression(2.0) };
	REQUIRE(psubneg(one_two) == Expression(-1.0));

	std::vector<Expression> four = { Expression(4.0) };
	REQUIRE(psubneg(four) == Expression(-4.0));

	std::vector<Expression> complex_one = { Expression(std::complex<double>(0, 1)) };
	REQUIRE(psubneg(complex_one) == Expression(std::complex<double>(0, -1)));
	std::vector<Expression> complex_args = { Expression(std::complex<double>(1, 1)), Expression(std::complex<double>(2, 2)) };
	REQUIRE(psubneg(complex_args) == Expression(std::complex<double>(-1, -1)));

	std::vector<Expression> not_a_number = { Expression(std::string("F")) };
	REQUIRE_THROWS_AS(psubneg(not_a_number), SemanticError);
	std::vector<Expression> two_not_numbers = { Expression(std::string("pay")), Expression(std::string("respect")) };
	REQUIRE_THROWS_AS(psubneg(two_not_numbers), SemanticError);
	std::vector<Expression> too_many_args = { Expression(1.1), Expression(2.2), Expression(3.3) };
	REQUIRE_THROWS_AS(psubneg(too_many_args), SemanticError);
}

TEST_CASE("Test division procedure", "[environment]") {

	Environment env;
	Procedure pdiv = env.get_proc(Atom("/"));

	std::vector<Expression> one_two = { Expression(1.0), Expression(2.0) };
	REQUIRE(pdiv(one_two) == Expression(0.5));
	std::vector<Expression> recip = { Expression(4.0) };
	REQUIRE(pdiv(recip) == Expression(0.25));

	std::vector<Expression> complex_args = { Expression(std::complex<double>(1, 1)), Expression(std::complex<double>(2, 2)) };
	REQUIRE(pdiv(complex_args) == Expression(std::complex<double>(0.5, 0)));
  std::vector<Expression> complex_and_real = { Expression(std::complex<double>(1, 1)), Expression(2) };
  REQUIRE(pdiv(complex_and_real) == Expression(std::complex<double>(0.5, 0.5)));
  std::vector<Expression> one_complex_arg = { Expression(std::complex<double>(2,2)) };
  REQUIRE(pdiv(one_complex_arg) == Expression(std::complex<double>(0.25,-0.25)));

	std::vector<Expression> not_a_number = { Expression(std::string("F")), Expression(std::string("one")) };
	REQUIRE_THROWS_AS(pdiv(not_a_number), SemanticError);
  std::vector<Expression> too_many_args = { Expression(1.1), Expression(2.2), Expression(3.3) };
	REQUIRE_THROWS_AS(pdiv(too_many_args), SemanticError);
}

TEST_CASE("Test square root procedure", "[environment]") {

	Environment env;
	Procedure psqrt = env.get_proc(Atom("sqrt"));

	std::vector<Expression> four = { Expression(4.0) };
	REQUIRE(psqrt(four) == Expression(2.0));

	std::vector<Expression> negative_four = { Expression(-4.0) };
	REQUIRE(psqrt(negative_four) == Expression(std::complex<double>(0, 2)));
	std::vector<Expression> complex_two_I = { Expression(std::complex<double>(0,2)) };
	REQUIRE(psqrt(complex_two_I) == Expression(std::complex<double>(1,1)));

	std::vector<Expression> not_a_number = { Expression(std::string("F")) };
	REQUIRE_THROWS_AS(psqrt(not_a_number), SemanticError);
	std::vector<Expression> too_many_args = { Expression(45.0), Expression(420) };
	REQUIRE_THROWS_AS(psqrt(too_many_args), SemanticError);
}

TEST_CASE("Test power procedure", "[environment]") {

	Environment env;
	Procedure ppow = env.get_proc(Atom("^"));

	std::vector<Expression> real_args = { Expression(1), Expression(1) };
	REQUIRE(ppow(real_args) == Expression(1));

	std::vector<Expression> complex_args = { Expression(std::complex<double>(0, 1)), Expression(5.0) };
	REQUIRE(ppow(complex_args) == Expression(std::complex<double>(0,1)));

	std::vector<Expression> not_a_number = { Expression(std::string("F")), Expression(5.0) };
	REQUIRE_THROWS_AS(ppow(not_a_number), SemanticError);
	std::vector<Expression> too_many_args = { Expression(4.0), Expression(1), Expression(std::complex<double>(3,3)) };
	REQUIRE_THROWS_AS(ppow(too_many_args), SemanticError);
}

TEST_CASE("Test natural log procedure", "[environment]") {

	Environment env;
	Procedure pln = env.get_proc(Atom("ln"));

    std::vector<Expression> one = { Expression(1.0) };
    REQUIRE(pln(one) == Expression(0));
    std::vector<Expression> e = { Expression(env.get_exp(Atom("e"))) };
    REQUIRE(pln(e) == Expression(1.0));

    std::vector<Expression> complex_arg = { Expression(std::complex<double>(0, 1)) };
    REQUIRE_THROWS_AS(pln(complex_arg), SemanticError);
    std::vector<Expression> not_a_number = { Expression(std::string("F")) };
    REQUIRE_THROWS_AS(pln(not_a_number), SemanticError);
    std::vector<Expression> too_many_args = { Expression(4.0), Expression(1), Expression(std::complex<double>(3,3)) };
    REQUIRE_THROWS_AS(pln(too_many_args), SemanticError);
}

TEST_CASE("Test sine procedure", "[environment]") {

	Environment env;
    Procedure psin = env.get_proc(Atom("sin"));

	std::vector<Expression> pi = { env.get_exp(Atom("pi")) };
	REQUIRE(psin(pi) == Expression(0.0));

    std::vector<Expression> too_many_args = { Expression(4.0), Expression(1), Expression(std::complex<double>(3,3)) };
    REQUIRE_THROWS_AS(psin(too_many_args), SemanticError);

    std::vector<Expression> not_a_number = { Expression(std::string("F")) };
	REQUIRE_THROWS_AS(psin(not_a_number), SemanticError);
}

TEST_CASE("Test cosine procedure", "[environment]") {

	Environment env;
	Procedure pcos = env.get_proc(Atom("cos"));

    std::vector<Expression> pi = { env.get_exp(Atom("pi")) };
    REQUIRE(pcos(pi) == Expression(-1.0));

    std::vector<Expression> too_many_args = { Expression(4.0), Expression(1), Expression(std::complex<double>(3,3)) };
    REQUIRE_THROWS_AS(pcos(too_many_args), SemanticError);

    std::vector<Expression> not_a_number = { Expression(std::string("F")) };
    REQUIRE_THROWS_AS(pcos(not_a_number), SemanticError);
}

TEST_CASE("Test tangent procedure", "[environment]") {

	Environment env;
    Procedure ptan = env.get_proc(Atom("tan"));

    std::vector<Expression> pi = { env.get_exp(Atom("pi")) };
    REQUIRE(ptan(pi) == Expression(0.0));

    std::vector<Expression> too_many_args = { Expression(4.0), Expression(1), Expression(std::complex<double>(3,3)) };
    REQUIRE_THROWS_AS(ptan(too_many_args), SemanticError);

    std::vector<Expression> not_a_number = { Expression(std::string("F")) };
    REQUIRE_THROWS_AS(ptan(not_a_number), SemanticError);
}

TEST_CASE("Testing real procedure", "[environment]") {

    Environment env;
    Procedure preal = env.get_proc(Atom("real"));

    std::vector<Expression> complex_arg = { Expression(std::complex<double>(100,10)) };
    REQUIRE(preal(complex_arg) == Expression(100));

    std::vector<Expression> real_arg = { Expression(5.0) };
    REQUIRE_THROWS_AS(preal(real_arg), SemanticError);

    std::vector<Expression> too_many_args = { Expression(std::complex<double>(1,1)), Expression(1.0) };
    REQUIRE_THROWS_AS(preal(too_many_args), SemanticError);
}

TEST_CASE("Testing imag procedure", "[environment]") {

    Environment env;
    Procedure pimag = env.get_proc(Atom("imag"));

    std::vector<Expression> complex_arg = { Expression(std::complex<double>(10,100)) };
    REQUIRE(pimag(complex_arg) == Expression(100));

    std::vector<Expression> real_arg = { Expression(5.0) };
    REQUIRE_THROWS_AS(pimag(real_arg), SemanticError);

    std::vector<Expression> too_many_args = { Expression(std::complex<double>(1,1)), Expression(1.0) };
    REQUIRE_THROWS_AS(pimag(too_many_args), SemanticError);
}

TEST_CASE("Testing mag procedure", "[environment]") {

    Environment env;
    Procedure pmag = env.get_proc(Atom("mag"));

    std::vector<Expression> complex_arg = { Expression(std::complex<double>(0,1)) };
    REQUIRE(pmag(complex_arg) == Expression(1));

    std::vector<Expression> real_arg = { Expression(5.0) };
    REQUIRE_THROWS_AS(pmag(real_arg), SemanticError);

    std::vector<Expression> too_many_args = { Expression(std::complex<double>(1,1)), Expression(1.0) };
    REQUIRE_THROWS_AS(pmag(too_many_args), SemanticError);
}

TEST_CASE("Testing arg procedure", "[environment]") {

    Environment env;
    Procedure parg = env.get_proc(Atom("arg"));

    std::vector<Expression> complex_arg = { Expression(std::complex<double>(0,0)) };
    REQUIRE(parg(complex_arg) == Expression(0));

    std::vector<Expression> real_arg = { Expression(5.0) };
    REQUIRE_THROWS_AS(parg(real_arg), SemanticError);

    std::vector<Expression> too_many_args = { Expression(std::complex<double>(1,1)), Expression(1.0) };
    REQUIRE_THROWS_AS(parg(too_many_args), SemanticError);
}

TEST_CASE("Testing conj procedure", "[environment]") {

    Environment env;
    Procedure pconj = env.get_proc(Atom("conj"));

    std::vector<Expression> complex_arg = { Expression(std::complex<double>(1,1)) };
    REQUIRE(pconj(complex_arg) == Expression(std::complex<double>(1,-1)));

    std::vector<Expression> real_arg = { Expression(5.0) };
    REQUIRE_THROWS_AS(pconj(real_arg), SemanticError);

    std::vector<Expression> too_many_args = { Expression(std::complex<double>(1,1)), Expression(1.0) };
    REQUIRE_THROWS_AS(pconj(too_many_args), SemanticError);
}

TEST_CASE("Testing list procedure", "[environment]") {

    Environment env;
    Procedure plist = env.get_proc(Atom("list"));

    std::vector<Expression> l_010 = { Expression(0), Expression(1), Expression(0) };
    REQUIRE(plist(l_010).isList());
    std::vector<Expression> test_to_compare = { Expression(0), Expression(1), Expression(0) };
    REQUIRE(plist(l_010) == Expression(test_to_compare));

}

TEST_CASE("Testing first procedure", "[environment]") {

    Environment env;
    Procedure pfirst = env.get_proc(Atom("first"));
    Procedure plist = env.get_proc(Atom("list"));

    std::vector<Expression> l_010 = { Expression(0), Expression(1), Expression(0) };
    std::vector<Expression> testlist = { plist(l_010) };
    REQUIRE(pfirst(testlist) == Expression(0));

    std::vector<Expression> not_a_list = { Expression(std::complex<double>(1,0)) };
    REQUIRE_THROWS_AS(pfirst(not_a_list), SemanticError);

    std::vector<Expression> l = { };
    std::vector<Expression> empty_list = { plist(l) };
    REQUIRE_THROWS_AS(pfirst(empty_list), SemanticError);

    std::vector<Expression> too_many_args = { plist(l), plist(l_010) };
    REQUIRE_THROWS_AS(pfirst(too_many_args), SemanticError);
}

TEST_CASE("Testing rest procedure", "[environment]") {

    Environment env;
    Procedure prest = env.get_proc(Atom("rest"));
    Procedure plist = env.get_proc(Atom("list"));

    std::vector<Expression> l_010 = { Expression(0), Expression(1), Expression(0) };
    std::vector<Expression> testlist = { plist(l_010) };
    std::vector<Expression> resultList = { Expression(1), Expression(0) };
    REQUIRE(prest(testlist) == Expression(resultList));

    std::vector<Expression> not_a_list = { Expression(std::complex<double>(1,0)) };
    REQUIRE_THROWS_AS(prest(not_a_list), SemanticError);

    std::vector<Expression> l = { };
    std::vector<Expression> empty_list = { plist(l) };
    REQUIRE_THROWS_AS(prest(empty_list), SemanticError);

    std::vector<Expression> too_many_args = { plist(l), plist(l_010) };
    REQUIRE_THROWS_AS(prest(too_many_args), SemanticError);

}

TEST_CASE("Testing length procedure", "[environment]") {

    Environment env;
    Procedure plength = env.get_proc(Atom("length"));
    Procedure plist = env.get_proc(Atom("list"));

    std::vector<Expression> l_010 = { Expression(0), Expression(1), Expression(0) };
    std::vector<Expression> testlist = { plist(l_010) };
    REQUIRE(plength(testlist) == Expression(3));

    std::vector<Expression> not_a_list = { Expression(std::complex<double>(1,0)) };
    REQUIRE_THROWS_AS(plength(not_a_list), SemanticError);

    std::vector<Expression> l = { };
    std::vector<Expression> empty_list = { plist(l) };
    REQUIRE(plength(empty_list) == Expression(0));

    std::vector<Expression> too_many_args = { plist(l), plist(l_010) };
    REQUIRE_THROWS_AS(plength(too_many_args), SemanticError);
}

TEST_CASE("Testing append procedure", "[environment]") {

    Environment env;
    Procedure pappend = env.get_proc(Atom("append"));
    Procedure plist = env.get_proc(Atom("list"));

    std::vector<Expression> l_010 = { Expression(0), Expression(1), Expression(0) };
    std::vector<Expression> l_989 = { Expression(9), Expression(8), Expression(9) };
    std::vector<Expression> test_arg = {plist(l_010), plist(l_989)};
    std::vector<Expression> resultList = { Expression(0), Expression(1), Expression(0), plist(l_989) };
    REQUIRE(pappend(test_arg) == Expression(resultList));

    std::vector<Expression> not_a_list = { Expression(std::complex<double>(1,0)), plist(l_010) };
    REQUIRE_THROWS_AS(pappend(not_a_list), SemanticError);

    std::vector<Expression> l = { };
    std::vector<Expression> empty_list = { plist(l) };
    REQUIRE_THROWS_AS(pappend(empty_list), SemanticError);

    std::vector<Expression> too_many_args = { plist(l), plist(l_010), plist(l_989) };
    REQUIRE_THROWS_AS(pappend(too_many_args), SemanticError);
}

TEST_CASE("Testing join procedure", "[environment]") {

    Environment env;
    Procedure pjoin = env.get_proc(Atom("join"));
    Procedure plist = env.get_proc(Atom("list"));

    std::vector<Expression> l_010 = { Expression(0), Expression(1), Expression(0) };
    std::vector<Expression> l_989 = { Expression(9), Expression(8), Expression(9) };
    std::vector<Expression> test_arg = {plist(l_010), plist(l_989)};
    std::vector<Expression> resultList = { Expression(0), Expression(1), Expression(0), Expression(9), Expression(8), Expression(9) };
    REQUIRE(pjoin(test_arg) == Expression(resultList));

    std::vector<Expression> not_a_list = { Expression(std::complex<double>(1,0)), Expression(10) };
    REQUIRE_THROWS_AS(pjoin(not_a_list), SemanticError);

    std::vector<Expression> l = { };
    std::vector<Expression> empty_list = { plist(l) };
    REQUIRE_THROWS_AS(pjoin(empty_list), SemanticError);

    std::vector<Expression> too_many_args = { plist(l), plist(l_010), plist(l) };
    REQUIRE_THROWS_AS(pjoin(too_many_args), SemanticError);
}

TEST_CASE("Testing range procedure", "[environment]") {

    Environment env;
    Procedure prange = env.get_proc(Atom("range"));
    Procedure plist = env.get_proc(Atom("list"));

    std::vector<Expression> test_args = { Expression(1), Expression(3), Expression(1) };
    std::vector<Expression> resultList = { Expression(1), Expression(2), Expression(3) };
    REQUIRE(prange(test_args) == Expression(resultList));

    std::vector<Expression> arg_nan = { Expression(1), Expression(std::string("f")), Expression(0) };
    REQUIRE_THROWS_AS(prange(arg_nan), SemanticError);

    std::vector<Expression> begin_larger_end = { Expression(10), Expression(5), Expression(1) };
    REQUIRE_THROWS_AS(prange(begin_larger_end), SemanticError);

    std::vector<Expression> end_zero = { Expression(10), Expression(20), Expression(-5) };
    REQUIRE_THROWS_AS(prange(end_zero), SemanticError);

    std::vector<Expression> not_3_args = { Expression(10) };
    REQUIRE_THROWS_AS(prange(not_3_args), SemanticError);
}
