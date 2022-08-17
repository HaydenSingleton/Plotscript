#include "environment.h"
#include "semantic_error.h"

Environment::Environment() {
    reset();
}

bool Environment::is_known(const Atom& sym) const
{   
    if (!sym.isSymbol()) return false;
    return env.find(sym) != env.end();
}

bool Environment::is_exp(const Atom& sym) const {
    if (!sym.isSymbol()) return false;

    auto result = env.find(sym);
    return (result != env.end()) && (result->second.type == ExpressionType);
}

bool Environment::is_proc(const Atom& sym) const {
    if (!sym.isSymbol()) return false;

    auto result = env.find(sym);
    return (result != env.end()) && (result->second.type == ProcedureType);
}

bool Environment::is_lambda(const Atom& sym) const {
    if (!sym.isSymbol()) return false;

    auto result = env.find(sym);
    if (result != env.end()) {
        return result->second.exp.head().toString() == "lambda";
    }

    return false;
}

Expression Environment::get_exp(const Atom& sym) const {

    if (sym.isSymbol()) {
        auto result = env.find(sym);
        if ((result != env.end()) && (result->second.type == ExpressionType)) {
            return result->second.exp;
        }
    }

    return {};
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

    if (env.find(sym) != env.end())
        env.erase(sym);

    env.emplace(sym, EnvResult(ExpressionType, value));
}

Expression nop(const std::vector<Expression>& args) {
    (void) args.size();
    return {};
}

Procedure Environment::get_proc(const Atom& sym) const {

    if (sym.isSymbol()) {
        auto result = env.find(sym);
        if ((result != env.end()) && (result->second.type == ProcedureType)) {
            return result->second.proc;
        }
    }

    return (Procedure) nop;
}

Expression add(const std::vector<Expression>& args) {

    double result = 0;
    double complex_part = 0;

    for (auto& a : args) {
        Atom operand = a.head();
        std::complex<double> term = operand.asComplex();

        result += term.real();
        if (term.imag() != 0) {
            complex_part += term.imag();
        }
    }


    return { std::complex<double>(result, complex_part) };
}

Expression sub_neg(const std::vector<Expression>& args) {

    if (args.size() != 1 && args.size() != 2) {
        throw SemanticError("Error: in call to sub_neg: invalid number of arguments.");
    }

    double r = args[0].head().asNumber();
    double i = args[0].head().asComplex().imag();

    if (args.size() == 1) {

        if (i == 0) {
            return { -r };
        }
        else
            return { -std::complex<double>(r, i) };
    }

    std::complex<double> result; 
    result = args[0].head().asComplex() - args[1].head().asComplex();

    return { result };
}

Expression mul(const std::vector<Expression>& args) {

    std::complex<double> result(1, 0);

    for (auto& a : args) {
        Atom operand = a.head();
        if (!operand.isComplex() && !operand.isNumber()) {
            throw SemanticError("Error: in call to mul: argument was not a number type.");

        }
        result *= operand.asComplex();
    }
    
    return { result };
}

Expression div(const std::vector<Expression>& args) {

    std::complex<double> result, first, second;

    if (args.size() == 1) {
        result = std::complex<double>(1, 0) / args[0].head().asComplex();
    }
    else if (args.size() == 2) {
        second = args[1].head().asComplex();
        first = args[0].head().asComplex();
        result = first / second;
        if (second.real() == 0 && std::isnan(result.real()))
            throw SemanticError("Error: div by zero");
    }
    else {
        throw SemanticError("Error: in call to div: invalid number of arguments.");
    }

    return { result };
}

Expression root(const std::vector<Expression>& args) {

    std::complex<double> result, first = args[0].head().asComplex();

    if (args.size() == 1) {
        result = std::sqrt(first);
    }
    else {
        throw SemanticError("Error: in call to sqrt: invalid number of arguments.");
    }
    return { result };
}

Expression pow(const std::vector<Expression>& args) {

    std::complex<double> result, first, second;

    first = args[0].head().asComplex();
    second = args[1].head().asComplex();

    if (args.size() == 2) {
        result = std::pow(first, second);
    }
    else {
        throw SemanticError("Error: in call to pow: invalid number of arguments.");
    }
    return { result };
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
    return { result };
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
    return { result };
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
    return { result };
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
    return { result };
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
    return { result };
}

Expression real(const std::vector<Expression>& args) {

    double result;
    Atom a = args[0].head();
    if (!a.isComplex())
        throw SemanticError("Error: argument to real was not a complex number");

    result = a.asNumber();
    return { result };
}

Expression imag(const std::vector<Expression>& args) {

    double result;
    Atom a = args[0].head();
    if (!a.isComplex())
        throw SemanticError("Error: argument to imag was not a complex number");

    result = a.asComplex().imag();
    return { result };
}

Expression list(const std::vector<Expression>& items) {

    return { Atom("list"), items };
}

Expression first(const std::vector<Expression>& args) {

    if (args.size() != 1)
        throw SemanticError("Error: more than one argument in call to first.");

    const Expression& list = args[0];
  
    if (list.head().toString() != "list")
        throw SemanticError("Error: argument to first was not a list");

    if (list.tailConstBegin() == list.tailConstEnd())
        throw SemanticError("Error: argument to first was empty list");
    
    return { *args[0].tailConstBegin() };
}

Expression rest(const std::vector<Expression>& args) {
    if (args.size() > 1)
        throw SemanticError("Error: more than one argument in call to rest.");

    const Expression& list = args[0];

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

    return { Atom("list"), items };
}

Expression length(const std::vector<Expression>& args) {
    if (args.size() > 1)
        throw SemanticError("Error: more than one argument in call to length.");

    const Expression& list = args[0];

    if (list.head().toString() != "list")
        throw SemanticError("Error: argument to length was not a list");

    int len = 0;
    for (auto it = list.tailConstBegin(); it != list.tailConstEnd(); it++)
        len++;

    return { Atom(len) };
}

Expression append(const std::vector<Expression>& args) {
    if (args.size() != 2)
        throw SemanticError("Error: not given 2 arguments to append.");

    const Expression& list = args[0];

    if (list.head().toString() != "list")
        throw SemanticError("Error: first argument to append was not a list");

    std::vector<Expression> items;
    for (auto it = list.tailConstBegin(); it != list.tailConstEnd(); it++) {
        items.push_back(*it);
    }
    items.push_back(args[1]);

    return { Atom("list"), items };
}

Expression join(const std::vector<Expression>& args) {

    if (args.empty())
        throw SemanticError("Error: nothing to join");

    std::vector<Expression> items;

    for (const auto& arg : args) {
        if (arg.head().toString() != "list")
            throw SemanticError("Error: argument to join not a list");
        for (auto it = arg.tailConstBegin(); it != arg.tailConstEnd(); it++) {
            items.push_back(*it);
        }
    }

    return { Atom("list"), items };
}

Expression range(const std::vector<Expression>& args) {
    double start, stop, step;

    if (args.size() < 2)
        throw SemanticError("Error: number of args to range");

    for (const auto& arg : args) {
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
    int steps = static_cast<int>((stop - start) / step);

    for (int i = 0; i <= steps; i++) {
        double part = start + static_cast<double>(i) * (stop - start) / static_cast<double>(steps);
        items.emplace_back(Expression(part));
    }

    return { Atom("list"), items };
}

Expression set_prop(const std::vector<Expression>& args) {
    if (args.size() != 3)
        throw SemanticError("Error: not given 3 args to set-property");

    if (!args[0].head().isString())
        throw SemanticError("Error: first argument to set-property was not a string");

    std::string key = args[0].head().toString();
    const Expression& value = args[1];
    
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
    env.clear();

    env.emplace(Atom("pi"), EnvResult(ExpressionType, Expression(std::atan2(0, -1))));
    env.emplace(Atom("e"), EnvResult(ExpressionType, Expression(std::exp(1))));
    env.emplace(Atom("I"), EnvResult(ExpressionType, Expression(std::complex<double>(0, 1))));
    env.emplace(Atom("-I"), EnvResult(ExpressionType, Expression(std::complex<double>(0, -1))));

    env.emplace(Atom("+"), EnvResult(ProcedureType, add));
    env.emplace(Atom("-"), EnvResult(ProcedureType, sub_neg));
    env.emplace(Atom("*"), EnvResult(ProcedureType, mul));
    env.emplace(Atom("/"), EnvResult(ProcedureType, div));

    env.emplace(Atom("sqrt"), EnvResult(ProcedureType, root));
    env.emplace(Atom("^"), EnvResult(ProcedureType, pow));
    env.emplace(Atom("pow"), EnvResult(ProcedureType, pow));
    env.emplace(Atom("ln"), EnvResult(ProcedureType, ln));
    env.emplace(Atom("log"), EnvResult(ProcedureType, log));
    env.emplace(Atom("sin"), EnvResult(ProcedureType, sin));
    env.emplace(Atom("cos"), EnvResult(ProcedureType, cos));
    env.emplace(Atom("tan"), EnvResult(ProcedureType, tan));
    env.emplace(Atom("real"), EnvResult(ProcedureType, real));
    env.emplace(Atom("imag"), EnvResult(ProcedureType, imag));

    env.emplace(Atom("list"), EnvResult(ProcedureType, list));
    env.emplace(Atom("first"), EnvResult(ProcedureType, first));
    env.emplace(Atom("rest"), EnvResult(ProcedureType, rest));
    env.emplace(Atom("length"), EnvResult(ProcedureType, length));
    env.emplace(Atom("append"), EnvResult(ProcedureType, append));
    env.emplace(Atom("join"), EnvResult(ProcedureType, join));
    env.emplace(Atom("range"), EnvResult(ProcedureType, range));
    
    env.emplace(Atom("apply"), EnvResult(ProcedureType, nop));
    env.emplace(Atom("map"), EnvResult(ProcedureType, nop));

    env.emplace(Atom("set-property"), EnvResult(ProcedureType, set_prop));
    env.emplace(Atom("get-property"), EnvResult(ProcedureType, get_prop));
}