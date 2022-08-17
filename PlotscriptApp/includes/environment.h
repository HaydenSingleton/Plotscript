#pragma once

#include "expression.h"
#include <map>
#include <cmath>

typedef Expression (*Procedure)(const std::vector<Expression>& args);

class Environment {
public:
	Environment();

    [[nodiscard]] bool is_known(const Atom& sym) const;
    [[nodiscard]] bool is_exp(const Atom& var) const;
    [[nodiscard]] bool is_proc(const Atom& sym) const;
    [[nodiscard]] bool is_lambda(const Atom& sym) const;

	[[nodiscard]] Procedure get_proc(const Atom& sym) const;
	[[nodiscard]] Expression get_exp(const Atom& sym) const;

	void add_exp(const Atom& sym, const Expression& value);
	void reset();

private:
	enum EnvType {ExpressionType, ProcedureType};

	struct EnvResult {
		EnvType type;
		Expression exp;
		Procedure proc = nullptr;

		EnvResult(EnvType t, const Expression& e) : type(t), exp(e) {};
		EnvResult(EnvType t, Procedure p) : type(t), proc(p) {};
	};

	std::map<Atom, EnvResult> env;
};

