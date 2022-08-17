#pragma once

#include "expression.h"
#include <map>
#include <cmath>

typedef Expression (*Procedure)(const std::vector<Expression>& args);

class Environment {
public:
	Environment();

	bool is_known(const Atom& sym) const;
	bool is_exp(const Atom& var) const;
	bool is_proc(const Atom& sym) const;
	bool is_lambda(const Atom& sym) const;

	Procedure get_proc(const Atom& sym) const;

	Expression get_exp(const Atom& sym) const;
	void add_exp(const Atom& sym, const Expression& value);
	Expression evaluate_an_exp(Expression& e);
	void reset();

private:
	enum EnvType {ExpressionType, ProcedureType};

	struct EnvResult {
		EnvType type;
		Expression exp;
		Procedure proc = nullptr;

		EnvResult(EnvType t, Expression e) : type(t), exp(e) {};
		EnvResult(EnvType t, Procedure p) : type(t), proc(p) {};
	};

	std::map<Atom, EnvResult> envmap;
};

