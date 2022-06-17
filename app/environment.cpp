#include "environment.h"
#include "semantic_error.h"

Environment::Environment() {
    reset();
}

bool Environment::is_known(const Atom& sym) const
{   
    if (!sym.isSymbol()) return false;
    return envmap.find(sym) != envmap.end();
}

bool Environment::is_exp(const Atom& sym) const {
    if (!sym.isSymbol()) return false;

    auto result = envmap.find(sym);
    return (result != envmap.end()) && (result->second.type == ExpressionType);
}

bool Environment::is_proc(const Atom& sym) const {
    if (!sym.isSymbol()) return false;

    auto result = envmap.find(sym);
    return (result != envmap.end()) && (result->second.type == ProcedureType);
}

bool Environment::is_lambda(const Atom& sym) const {
    if (!sym.isSymbol()) return false;

    auto result = envmap.find(sym);
    if (result != envmap.end()) {
        return result->second.exp.head().toString() == "lambda";
    }

    return false;
}

Expression Environment::evaluate_an_exp(Expression& e) {
    return e.eval(*this);
}

Expression Environment::get_exp(const Atom& sym) const {

    if (sym.isSymbol()) {
        auto result = envmap.find(sym);
        if ((result != envmap.end()) && (result->second.type == ExpressionType)) {
            return result->second.exp;
        }
    }

    return Expression();
}

void Environment::add_exp(const Atom& sym, const Expression& value) {

    if (!sym.isSymbol()) {
        throw SemanticError("Error: during add_exp: Attempt to add non-symbol to environment");
    }

    std::string s = sym.asSymbol();
    if ((s == "define") || (s == "begin") || (s == "lambda") || (s == "list")) {
        throw SemanticError("Error during add_exp: attempt to redefine a special-form");
    }
    
    if (is_proc(sym)) {
        throw SemanticError("Error during add_exp: attempt to redefine a built-in procedure");
    }

    if (envmap.find(sym) != envmap.end())
        envmap.erase(sym);

    envmap.emplace(sym, EnvResult(ExpressionType, value));
}

Expression nop(const std::vector<Expression>& args) {
    (void) args.size();
    return Expression();
}

Procedure Environment::get_proc(const Atom& sym) const {

    if (sym.isSymbol()) {
        auto result = envmap.find(sym);
        if ((result != envmap.end()) && (result->second.type == ProcedureType)) {
            return result->second.proc;
        }
    }

    return (Procedure) nop;
}

Expression add(const std::vector<Expression>& args) {

    double result = 0;
    double complex_part = 0;

    for (auto& a : args) {
        Atom arg = a.head();

        if (arg.isNumber()) {
            result += arg.asNumber();
        }
        else if (arg.isComplex()) {
            result += arg.asNumber();
            complex_part += arg.asComplex().imag();
        }
        else {
            throw SemanticError("Error in add: argument was NAN");
        }
    }

    if (complex_part == 0)
        return Expression(result);
    else
        return Expression(std::complex<double>(result, complex_part));

    
}

Expression subneg(const std::vector<Expression>& args) {

    if (args.size() == 1) {
        double r = args[0].head().asNumber();
        double i = args[0].head().asComplex().imag();

        return Expression(std::complex<double>(r * -1, i));
    }
    else if (args.size() == 2) {
        return Expression(args[0].head().asComplex() - args[1].head().asComplex());
    }
    else {
        throw SemanticError("Error: in call to subneg: invalid number of arguments.");
    }
}

Expression mul(const std::vector<Expression>& args) {

    std::complex<double> result(1, 0);

    for (auto& a : args) {
        if (a.head().isNumber()) {
            result *= a.head().asComplex();
        }
        else {
            throw SemanticError("Error: in call to mul: arg not a number");
        }
    }

    return Expression(result);
}

Expression div(const std::vector<Expression>& args) {

    std::complex<double> result, first, second;

    if (args.size() == 1) {
        result = std::complex<double>(1, 0) / args[0].head().asComplex();
    }
    else if (args.size() == 2) {
        second = args[1].head().asComplex();
        if (second.real() == 0)
            throw SemanticError("Error: div by zero");

        first = args[0].head().asComplex();
        result = first / second;
    }
    else {
        throw SemanticError("Error: in call to div: invalid number of arguments.");
    }
    return Expression(result);
}

Expression root(const std::vector<Expression>& args) {

    std::complex<double> result, first = args[0].head().asComplex();

    if (args.size() == 1) {
        result = std::sqrt(first);
    }
    else {
        throw SemanticError("Error: in call to sqrt: invalid number of arguments.");
    }
    return Expression(result);
}

Expression pow(const std::vector<Expression>& args) {

    std::complex<double> result, first, second;

    first = args[0].head().asNumber();
    second = args[1].head().asNumber();

    if (args.size() == 2) {
        result = std::pow(first, second);
    }
    else {
        throw SemanticError("Error: in call to pow: invalid number of arguments.");
    }
    return Expression(result);
}

Expression ln(const std::vector<Expression>& args) {

    std::complex<double> result, first;
    first = args[0].head().asNumber();
    if (first.real() < 0)
        throw SemanticError("Error: negative argument to ln");

    if (args.size() == 1) {
        result = std::log(first);
    }
    else {
        throw SemanticError("Error: in call to ln: invalid number of arguments.");
    }
    return Expression(result);
}

Expression log(const std::vector<Expression>& args) {

    std::complex<double> result, first, second;
    first = args[0].head().asNumber();

    if (first.real() < 0)
        throw SemanticError("Error: negative argument to log");

    if (args.size() == 1) {
        result = std::log10(first);
    }
    else if (args.size() == 2) {
        second = args[1].head().asNumber();
        result = std::log(first) / std::log(second);
    }
    else {
        throw SemanticError("Error: in call to log: invalid number of arguments.");
    }
    return Expression(result);
}

Expression sin(const std::vector<Expression>& args) {

    std::complex<double> result, first;
    first = args[0].head().asNumber();

    if (args.size() == 1) {
        result = std::sin(first);
    }
    else {
        throw SemanticError("Error: in call to sin: invalid number of arguments.");
    }
    return Expression(result);
}

Expression cos(const std::vector<Expression>& args) {

    std::complex<double> result, first;
    first = args[0].head().asNumber();

    if (args.size() == 1) {
        result = std::cos(first);
    }
    else {
        throw SemanticError("Error: in call to cos: invalid number of arguments.");
    }
    return Expression(result);
}

Expression tan(const std::vector<Expression>& args) {

    std::complex<double> result, first;
    first = args[0].head().asNumber();

    if (args.size() == 1) {
        result = std::tan(first);
    }
    else {
        throw SemanticError("Error: in call to tan: invalid number of arguments.");
    }
    return Expression(result);
}

Expression real(const std::vector<Expression>& args) {

    double result;
    Atom a = args[0].head();
    if (!a.isComplex())
        throw SemanticError("Error: argument to real was not a complex number");

    result = a.asNumber();
    return Expression(result);
}

Expression imag(const std::vector<Expression>& args) {

    double result;
    Atom a = args[0].head();
    if (!a.isComplex())
        throw SemanticError("Error: argument to imag was not a complex number");

    result = a.asComplex().imag();
    return Expression(result);
}

Expression list(const std::vector<Expression>& items) {

    return Expression(Atom("list"), items);
}

Expression first(const std::vector<Expression>& args) {

    if (args.size() != 1)
        throw SemanticError("Error: more than one argument in call to first.");

    Expression list = args[0];
  
    if (list.head().toString() != "list")
        throw SemanticError("Error: argument to first was not a list");

    if (list.tailConstBegin() == list.tailConstEnd())
        throw SemanticError("Error: argument to first was empty list");
    
    return Expression(*args[0].tailConstBegin());
}

Expression rest(const std::vector<Expression>& args) {
    if (args.size() > 1)
        throw SemanticError("Error: more than one argument in call to rest.");

    Expression list = args[0];

    if (list.head().toString() != "list")
        throw SemanticError("Error: argument to rest was not a list");

    if (list.tailConstBegin() == list.tailConstEnd())
        throw SemanticError("Error: argument to rest was empty list");

    std::vector<Expression> items;
    for (auto it = list.tailConstBegin(); it != list.tailConstEnd(); it++) {
        if (it == list.tailConstBegin())
            continue;
        items.push_back(*it);
    }

    return Expression(Atom("list"), items);
}

Expression length(const std::vector<Expression>& args) {
    if (args.size() > 1)
        throw SemanticError("Error: more than one argument in call to length.");

    Expression list = args[0];

    if (list.head().toString() != "list")
        throw SemanticError("Error: argument to length was not a list");

    int len = 0;
    for (auto it = list.tailConstBegin(); it != list.tailConstEnd(); it++)
        len++;

    return Expression(len);
}

Expression append(const std::vector<Expression>& args) {
    if (args.size() != 2)
        throw SemanticError("Error: not given 2 arguments to append.");

    Expression list = args[0];

    if (list.head().toString() != "list")
        throw SemanticError("Error: first argument to append was not a list");

    std::vector<Expression> items;
    for (auto it = list.tailConstBegin(); it != list.tailConstEnd(); it++) {
        items.push_back(*it);
    }
    items.push_back(args[1]);

    return Expression(Atom("list"), items);
}

Expression join(const std::vector<Expression>& args) {

    if (args.size() == 0)
        throw SemanticError("Error: nothing to join");

    std::vector<Expression> items;

    for (auto arg : args) {
        if (arg.head().toString() != "list")
            throw SemanticError("Error: argument to join not a list");
        for (auto it = arg.tailConstBegin(); it != arg.tailConstEnd(); it++) {
            items.push_back(*it);
        }
    }

    return Expression(Atom("list"), items);
}

Expression range(const std::vector<Expression>& args) {
    double start, stop, step;

    if (args.size() < 2)
        throw SemanticError("Error: number of args to range");

    for (auto arg : args) {
        if (!arg.head().isNumber())
            throw SemanticError("Error: argument to range was not a number");
    }

    start = args[0].head().asNumber();
    stop = args[1].head().asNumber();

    if (start > stop)
        throw SemanticError("Error: begin greater than end in range");

    if (args.size() == 3) {
        step = args[2].head().asNumber();
        if (step <= 0)
            throw SemanticError("Error: negative or zero increment in range");
    }
    else
        step = 1;

    std::vector<Expression> items;

    for (double d = start; d < stop; d += step) {
        items.emplace_back(d);
    }

    return Expression(Atom("list"), items);
}

Expression set_prop(const std::vector<Expression>& args) {
    if (args.size() != 3)
        throw SemanticError("Error: not given 3 args to set-property");

    if (!args[0].head().isString())
        throw SemanticError("Error: first argument to set-property was not a string");

    std::string key = args[0].head().toString();
    Expression value = args[1];
    
    Expression result = args[2];

    result.setProperty(key, value);

    return result;
}

Expression get_prop(const std::vector<Expression>& args) {
    if (args.size() != 2)
        throw SemanticError("Error: not given 2 args to get-property");

    if (!args[0].head().isString())
        throw SemanticError("Error: first argument to get-property was not a string");

    std::string key = args[0].head().toString();
    Expression obj = args[1];
    
    return obj.getProperty(key);
}


void Environment::reset()
{
    envmap.clear();

    envmap.emplace(Atom("pi"), EnvResult(ExpressionType, Expression(std::atan2(0, -1))));
    envmap.emplace(Atom("e"), EnvResult(ExpressionType, Expression(std::exp(1))));
    Atom atomI(std::complex<double>(0, 1));
    envmap.emplace(Atom("I"), EnvResult(ExpressionType, Expression(atomI)));
    envmap.emplace(Atom("-I"), EnvResult(ExpressionType, Expression(std::complex<double>(0, -1))));

    envmap.emplace(Atom("+"), EnvResult(ProcedureType, add));
    envmap.emplace(Atom("-"), EnvResult(ProcedureType, subneg));
    envmap.emplace(Atom("*"), EnvResult(ProcedureType, mul));
    envmap.emplace(Atom("/"), EnvResult(ProcedureType, div));

    envmap.emplace(Atom("sqrt"), EnvResult(ProcedureType, root));
    envmap.emplace(Atom("^"), EnvResult(ProcedureType, pow));
    envmap.emplace(Atom("ln"), EnvResult(ProcedureType, ln));
    envmap.emplace(Atom("log"), EnvResult(ProcedureType, log));
    envmap.emplace(Atom("sin"), EnvResult(ProcedureType, sin));
    envmap.emplace(Atom("cos"), EnvResult(ProcedureType, cos));
    envmap.emplace(Atom("tan"), EnvResult(ProcedureType, tan));
    envmap.emplace(Atom("real"), EnvResult(ProcedureType, real));
    envmap.emplace(Atom("imag"), EnvResult(ProcedureType, imag));

    envmap.emplace(Atom("list"), EnvResult(ProcedureType, list));
    envmap.emplace(Atom("first"), EnvResult(ProcedureType, first));
    envmap.emplace(Atom("rest"), EnvResult(ProcedureType, rest));
    envmap.emplace(Atom("length"), EnvResult(ProcedureType, length));
    envmap.emplace(Atom("append"), EnvResult(ProcedureType, append));
    envmap.emplace(Atom("join"), EnvResult(ProcedureType, join));
    envmap.emplace(Atom("range"), EnvResult(ProcedureType, range));
    
    envmap.emplace(Atom("apply"), EnvResult(ProcedureType, nop));
    envmap.emplace(Atom("map"), EnvResult(ProcedureType, nop));

    envmap.emplace(Atom("set-property"), EnvResult(ProcedureType, set_prop));
    envmap.emplace(Atom("get-property"), EnvResult(ProcedureType, get_prop));
}