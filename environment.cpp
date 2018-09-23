#include "environment.hpp"

#include <cassert>
#include <cmath>

#include "environment.hpp"
#include "semantic_error.hpp"

/***********************************************************************
Helper Functions
**********************************************************************/

// predicate, the number of args is nargs
bool nargs_equal(const std::vector<Expression> & args, unsigned nargs){
  return args.size() == nargs;
}

// check if the expression holds a real or imaginary number
bool is_num_type(const Expression & a) {
  return a.isHeadComplex() || a.isHeadNumber();
}

/***********************************************************************
Each of the functions below have the signature that corresponds to the
typedef'd Procedure function pointer.
**********************************************************************/

// the default procedure always returns an expresison of type None
Expression default_proc(const std::vector<Expression> & args){
  args.size(); // make compiler happy we used this parameter
  return Expression();
};

Expression add(const std::vector<Expression> & args){

  std::complex<double> result;
  bool noComplexArgs = true;
  // check all aruments are numbers, while adding
  for( auto & a :args){
    if(a.isHeadNumber()){
      result += a.head().asNumber();
    }
    else if(a.isHeadComplex()){
      result += a.head().asComplex();
      noComplexArgs = false;
    }
    else{
      throw SemanticError("Error in call to add, argument not a number");
    }
  }

  if(noComplexArgs){
    return Expression(result.real());
  }
  else {
    return Expression(result);
  }

};

Expression mul(const std::vector<Expression> & args){

  // check all aruments are numbers, while multiplying
  std::complex<double> result = 1.0;
  bool noComplexArgs = true;

  for( auto & a :args){
    if(a.isHeadNumber()) {
      result *= a.head().asNumber();
    }
    else if(a.isHeadComplex()){
      noComplexArgs = false;
      result *= a.head().asComplex();
    }
    else{
      throw SemanticError("Error in call to mul, argument not a number");
    }
  }

  // Return a real number if all arguments are real, otherwise return a complex number
  if(noComplexArgs){
    return Expression(result.real());
  }
  else {
    return Expression(result);
  }
};

Expression subneg(const std::vector<Expression> & args){

  std::complex<double> result = 0;

  // preconditions
  if(nargs_equal(args,1)){
    if(args[0].isHeadNumber()){
      result = -args[0].head().asNumber();
      return Expression(result.real());
    }
    else if (args[0].isHeadComplex()) {
      result = -args[0].head().asComplex();
      return Expression(result);
    }
    else {
      throw SemanticError("Error in call to negate: invalid argument type.");
    }
  }
  else if(nargs_equal(args,2)){
    if(is_num_type(args[0]) && is_num_type(args[1])){
      result = args[0].head().asComplex() - args[1].head().asComplex();
    }
    else{
      throw SemanticError("Error in call to subtraction: invalid argument types.");
    }
  }
  else{
    throw SemanticError("Error in call to subtraction or negation: invalid number of arguments.");
  }

  if(args[0].isHeadComplex()||args[1].isHeadComplex()){
    return Expression(result);
  }
  else {
    return Expression(result.real());
  }
};

Expression div(const std::vector<Expression> & args) {

  std::complex<double> result = 0;

  if(nargs_equal(args,2)){
    if(is_num_type(args[0]) && is_num_type(args[1])) {
      result = args[0].head().asComplex() / args[1].head().asComplex();
    }
    else{
      throw SemanticError("Error in call to division: invalid argument.");
    }
  }
  else{
    throw SemanticError("Error in call to division: invalid number of arguments.");
  }

  // If either argument was a complex number leave the result, else return only the real part
  if(args[0].isHeadComplex()||args[1].isHeadComplex()){
    return Expression(result);
  }
  else {
    return Expression(result.real());
  }
};

Expression sqrt(const std::vector<Expression> & args) {
	double result = 0;
  std::complex<double> cresult;

	if (nargs_equal(args, 1)) {
		if (args[0].isHeadNumber() && args[0].head().asNumber() >= 0) {
			result = std::sqrt(args[0].head().asNumber());
      return Expression(result);
		}
		else if(args[0].isHeadComplex() || args[0].head().asNumber() < 0){
			cresult = std::sqrt(std::complex<double>(args[0].head().asComplex()));
      return Expression(cresult);
		}
    else {
      throw SemanticError("Error in call to division: invalid argument(s)");
    }
	}
	else {
		throw SemanticError("Error in call to division: invalid number of arguments.");
	}
};

Expression pow(const std::vector<Expression> & args) {

	std::complex<double> result(0,0);

	if (nargs_equal(args, 2)) {
		if(is_num_type(args[0]) && is_num_type(args[1])) {
			result = std::pow(args[0].head().asComplex(), args[1].head().asComplex());
		}
		else {
			throw SemanticError("Error in call to power function: invalid argument.");
		}
	}
	else {
		throw SemanticError("Error in call to power function: invalid number of arguments.");
	}

	if(args[0].isHeadComplex()||args[1].isHeadComplex()){
    return Expression(result);
  }
  else {
    return Expression(result.real());
  }
};

Expression ln(const std::vector<Expression> & args) {
    double result = 0;

    if (nargs_equal(args, 1)) {
        if (args[0].isHeadNumber() && args[0].head().asNumber() > 0) {
            result = std::log(args[0].head().asNumber());
        }
        else {
            throw SemanticError("Error in call to ln: invalid argument.");
        }
    }
    else {
        throw SemanticError("Error in call to ln: invalid number of arguments.");
    }
    return Expression(result);
};

Expression sin(const std::vector<Expression> & args) {
    double result = 0;

    if (nargs_equal(args, 1)) {
        if (args[0].isHeadNumber()) {
            result = std::sin(args[0].head().asNumber());
        }
        else {
            throw SemanticError("Error in call to sin: invalid argument.");
        }
    }
    else {
        throw SemanticError("Error in call to sin: invalid number of arguments.");
    }
    return Expression(result);
};

Expression cos(const std::vector<Expression> & args) {
    double result = 0;

    if (nargs_equal(args, 1)) {
        if (args[0].isHeadNumber()) {
            result = std::cos(args[0].head().asNumber());
        }
        else {
            throw SemanticError("Error in call to cos: invalid argument.");
        }
    }
    else {
        throw SemanticError("Error in call to cos: invalid number of arguments.");
    }
    return Expression(result);
};

Expression tan(const std::vector<Expression> & args) {
    double result = 0;

    if (nargs_equal(args, 1)) {
        if (args[0].isHeadNumber()) {
            result = std::tan(args[0].head().asNumber());
        }
        else {
            throw SemanticError("Error in call to tan: negative argument.");
        }
    }
    else {
        throw SemanticError("Error in call to tan: invalid number of arguments.");
    }
    return Expression(result);
};

Expression real(const std::vector<Expression> & args) {
  double result = 0;

  if(nargs_equal(args, 1)) {
    if (args[0].isHeadComplex()) {
      result = args[0].head().asComplex().real();
    }
    else {
      throw SemanticError("Error in call to real: not a complex argument.");
    }
  }
  else {
    throw SemanticError("Error in call to real: invalid number of arguments.");
  }

  return Expression(result);
};

Expression imag(const std::vector<Expression> & args) {
  double result;

  if(nargs_equal(args, 1)) {
    if (args[0].isHeadComplex()) {
      result = args[0].head().asComplex().imag();
    }
    else {
      throw SemanticError("Error in call to imag: not a complex argument.");
    }
  }
  else {
    throw SemanticError("Error in call to imag: invalid number of arguments.");
  }

  return Expression(result);
};

Expression mag(const std::vector<Expression> & args) {
  double result = 0;

  if(nargs_equal(args, 1)) {
    if (args[0].isHeadComplex()) {
      std::complex<double> n = args[0].head().asComplex();
      result = std::abs(n);
    }
    else {
      throw SemanticError("Error in call to mag: not a complex argument.");
    }
  }
  else {
    throw SemanticError("Error in call to mag: invalid number of arguments.");
  }

  return Expression(result);
};

Expression arg(const std::vector<Expression> & args) {
  double result = 0;

  if(nargs_equal(args, 1)) {
    if (args[0].isHeadComplex()) {
      std::complex<double> n = args[0].head().asComplex();
      result = std::arg(n);
    }
    else {
      throw SemanticError("Error in call to arg: not a complex argument.");
    }
  }
  else {
    throw SemanticError("Error in call to arg: invalid number of arguments.");
  }
  return Expression(result);
};

Expression conj(const std::vector<Expression> & args) {
  std::complex<double> result = 0;

  if(nargs_equal(args, 1)) {
    if (args[0].isHeadComplex()) {
      std::complex<double> n = args[0].head().asComplex();
      result = std::conj(n);
    }
    else {
      throw SemanticError("Error in call to conj: not a complex argument.");
    }
  }
  else {
    throw SemanticError("Error in call to conj: invalid number of arguments.");
  }
  return Expression(result);
};

Expression list(const std::vector<Expression> & args) {
	std::vector<Expression> result = args;
	return Expression(result);
};

Expression first(const std::vector<Expression> & args) {
	if (nargs_equal(args, 1)) {
		std::vector<Expression> result = args;
		return Expression(*result[0].tailConstBegin());
	}
	else {
		SemanticError("Error in call to first: invalid number of arguments.");
	}
};

const double PI = std::atan2(0, -1);
const double EXP = std::exp(1);
const std::complex<double> IMG (0.0,1.0);
const std::complex<double> NEG_IMG (0.0,-1.0);

Environment::Environment(){

  reset();
}

bool Environment::is_known(const Atom & sym) const{
  if(!sym.isSymbol()) return false;

  return envmap.find(sym.asSymbol()) != envmap.end();
}

bool Environment::is_exp(const Atom & sym) const{
  if(!sym.isSymbol()) return false;

  auto result = envmap.find(sym.asSymbol());
  return (result != envmap.end()) && (result->second.type == ExpressionType);
}

Expression Environment::get_exp(const Atom & sym) const{

  Expression exp;

  if(sym.isSymbol()){
    auto result = envmap.find(sym.asSymbol());
    if((result != envmap.end()) && (result->second.type == ExpressionType)){
      exp = result->second.exp;
    }
  }

  return exp;
}

void Environment::add_exp(const Atom & sym, const Expression & exp){

  if(!sym.isSymbol()){
    throw SemanticError("Attempt to add non-symbol to environment");
  }

  // error if overwriting symbol map
  if(envmap.find(sym.asSymbol()) != envmap.end()){
    throw SemanticError("Attempt to overwrite symbol in environemnt");
  }

  envmap.emplace(sym.asSymbol(), EnvResult(ExpressionType, exp));
}

bool Environment::is_proc(const Atom & sym) const{
  if(!sym.isSymbol()) return false;

  auto result = envmap.find(sym.asSymbol());
  return (result != envmap.end()) && (result->second.type == ProcedureType);
}

Procedure Environment::get_proc(const Atom & sym) const{

  //Procedure proc = default_proc;

  if(sym.isSymbol()){
    auto result = envmap.find(sym.asSymbol());
    if((result != envmap.end()) && (result->second.type == ProcedureType)){
      return result->second.proc;
    }
  }

  return default_proc;
}

/*
Reset the environment to the default state. First remove all entries and
then re-add the default ones.
 */
void Environment::reset(){

  envmap.clear();

  // Built-In value of pi
  envmap.emplace("pi", EnvResult(ExpressionType, Expression(PI)));

  // Built-In value of e
  envmap.emplace("e", EnvResult(ExpressionType, Expression(EXP)));

  // Built-In value of i
  envmap.emplace("I", EnvResult(ExpressionType, Expression(IMG)));

  // Built-In value of -i
  envmap.emplace("-I", EnvResult(ExpressionType, Expression(NEG_IMG)));

  // Procedure: add;
  envmap.emplace("+", EnvResult(ProcedureType, add));

  // Procedure: subneg;
  envmap.emplace("-", EnvResult(ProcedureType, subneg));

  // Procedure: mul;
  envmap.emplace("*", EnvResult(ProcedureType, mul));

  // Procedure: div;
  envmap.emplace("/", EnvResult(ProcedureType, div));

  // Procedure: sqrt;
  envmap.emplace("sqrt", EnvResult(ProcedureType, sqrt));

  // Procedure: pow;
  envmap.emplace("^", EnvResult(ProcedureType, pow));

  // Procedure: ln;
  envmap.emplace("ln", EnvResult(ProcedureType, ln));

  // Procedure: sin;
  envmap.emplace("sin", EnvResult(ProcedureType, sin));

  // Procedure: cos;
  envmap.emplace("cos", EnvResult(ProcedureType, cos));

  // Procedure: tan;
  envmap.emplace("tan", EnvResult(ProcedureType, tan));

  // Procedure: real;
  envmap.emplace("real", EnvResult(ProcedureType, real));

  // Procedure: imag;
  envmap.emplace("imag", EnvResult(ProcedureType, imag));

  // Procedure: mag;
  envmap.emplace("mag", EnvResult(ProcedureType, mag));

  // Procedure: arg;
  envmap.emplace("arg", EnvResult(ProcedureType, arg));

  // Procedure: conj;
  envmap.emplace("conj", EnvResult(ProcedureType, conj));

  // Procedure: list as procedure;
  envmap.emplace("list", EnvResult(ProcedureType, list));

  // Procedure: first
  envmap.emplace("first", EnvResult(ProcedureType, first));
}
