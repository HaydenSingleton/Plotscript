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

  std::complex<double> result;
  bool noComplexArgs = true;

  if(nargs_equal(args,1)){
	  if (args[0].isHeadComplex())
		  noComplexArgs = false;
	  result = std::complex<double>(1.0,0) / args[0].head().asComplex();
  }
  else {
	  if(is_num_type(args[0]))
		  result = args[0].head().asComplex() * args[0].head().asComplex();
	  for (auto & a : args) {
		  if (a.isHeadNumber()) {
			  result /= a.head().asNumber();
		  }
		  else if (a.isHeadComplex()) {
			  noComplexArgs = false;
			  result /= a.head().asComplex();
		  }
		  else {
			  throw SemanticError("Error in call to division, argument not a number");
		  }
	  }
  }

  if (noComplexArgs) {
	  return Expression(result.real());
  }
  else {
	  return Expression(result);
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
  // assert(Expression(result).isList());
	return Expression(result);
};

Expression first(const std::vector<Expression> & args) {

	if (nargs_equal(args, 1)) {
		if(args[0].isList()) {
			if (args[0] != Expression()) {
				Expression result = *args[0].tailConstBegin();
				return Expression(result);
			}
			else
				throw SemanticError("Error: argument to first is an empty list.");
		}
		else
			throw SemanticError("Error: argument to first is not a list.");
	}
	else {
		throw SemanticError("Error: more than one argument in call to first.");
	}
};

Expression rest(const std::vector<Expression> & args) {
  if(nargs_equal(args, 1)) {
    if(!args[0].isList())
      throw SemanticError("Error in call to rest: not a list argument.");
    else {
      if(args[0] != Expression()) {
        std::vector<Expression> result;
        auto e = args[0].tailConstBegin();
        e++;
        while(e != args[0].tailConstEnd()){
          result.emplace_back(*e++);
        }
        return Expression(result);
      }
      else
        throw SemanticError("Error argument to rest is an empty list.");
    }
  }
    else
      throw SemanticError("Error in call to rest: invalid number of arguments.");
};

Expression length(const std::vector<Expression> & args) {
  if(nargs_equal(args, 1)) {
    if(!args[0].isList())
      throw SemanticError("Error argument to length is not a list.");
    else {
        double len = (double)args[0].tailLength();
        return Expression(len);
      }
  }
  else
    throw SemanticError("Error invalid number of arguments for length.");
};

Expression append(const std::vector<Expression> & args) {
  if(nargs_equal(args, 2)) {
    if(!args[0].isList())
      throw SemanticError("Error first argument not a list.");
    else {
      std::vector<Expression> result;
      for(auto e = args[0].tailConstBegin(); e != args[0].tailConstEnd(); e++){
        result.emplace_back(*e);
      }
      result.emplace_back(args[1]);
      return Expression(result);
    }
  }
  else
    throw SemanticError("Error invalid number of arguments for append.");
};

Expression join(const std::vector<Expression> & args) {
  if(nargs_equal(args, 2)) {
    if(!args[0].isList() || !args[1].isList())
      throw SemanticError("Error: an argument to join not a list.");
    else {
      std::vector<Expression> result;
      for(auto e = args[0].tailConstBegin(); e != args[0].tailConstEnd(); e++){
        result.emplace_back(*e);
      }
      for(auto e = args[1].tailConstBegin(); e != args[1].tailConstEnd(); e++){
        result.emplace_back(*e);
      }
      return Expression(result);
    }
  }
  else
    throw SemanticError("Error invalid number of arguments to join.");
};

Expression range(const std::vector<Expression> & args) {
  if(nargs_equal(args, 3)) {
    if(!args[0].isHeadNumber() || !args[1].isHeadNumber() || !args[2].isHeadNumber())
      throw SemanticError("Error an argument is not a number.");
    else if (args[0].head().asNumber() > args[1].head().asNumber())
      throw SemanticError("Error: begin greater than end in range");
    else if (args[2].head().asNumber() <= 0)
      throw SemanticError("Error: negative or zero increment in range");
    else {
      std::vector<Expression> result;
      for(double i = args[0].head().asNumber(); i <= args[1].head().asNumber(); i += args[2].head().asNumber()){
        result.emplace_back(Expression(Atom(i)));
      }
      return Expression(result);
    }
  }
  else
    throw SemanticError("Error invalid number of arguments for range function.");
};

const double PI = std::atan2(0, -1);
const double EXP = std::exp(1);
const std::complex<double> IMG (0.0,1.0);
const std::complex<double> NEG_IMG (0.0,-1.0);

Environment::Environment(){

  reset();
}

Environment::Environment(const Environment & a){

  envmap = a.envmap;

}

Environment & Environment::operator=(const Environment & a){

  envmap = a.envmap;
  return *this;
}

bool Environment::is_known(const Atom & sym) const{
  if(!sym.isSymbol()) return false;

  return envmap.find(sym.asSymbol()) != envmap.end();
}

void Environment::__shadowing_helper(const Atom & sym, const Expression new_sym_val){

  if(this->envmap.find(sym.asSymbol()) != envmap.end()){
    this->envmap.erase(sym.asSymbol());
    }

    this->add_exp(sym, new_sym_val);
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

// bool Environment::is_proc(const Expression & sym) const{
//   if(!sym.head().isSymbol()) return false;

//   auto result = envmap.find(sym.head().asSymbol());
//   return (result != envmap.end()) && (result->second.type == ProcedureType);
// }

Procedure Environment::get_proc(const Atom & sym) const{

  // Procedure proc = default_proc;

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

  // Procedure: first;
  envmap.emplace("first", EnvResult(ProcedureType, first));

  // Procedure: rest;
  envmap.emplace("rest", EnvResult(ProcedureType, rest));

  // Procedure: length;
  envmap.emplace("length", EnvResult(ProcedureType, length));

  // Procedure: append;
  envmap.emplace("append", EnvResult(ProcedureType, append));

  // Procedure: join;
  envmap.emplace("join", EnvResult(ProcedureType, join));

  // Procedure: range;
  envmap.emplace("range", EnvResult(ProcedureType, range));
}
