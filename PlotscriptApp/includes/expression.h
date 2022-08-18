#pragma once

#include "atom.h"
#include <utility>
#include <iostream>
#include <vector>
#include <map>

class Environment;

class Expression {
public:
	Expression();
	/* Implicit */ Expression(const Atom&); //NOLINT
	Expression(const Atom&, const std::vector<Expression>& items);

	Expression& operator=(const Expression& e);

	[[nodiscard]] Atom head() const;
	Expression* tail();

    void setHead(const Atom &a);
    void append(const Atom &a);

    [[nodiscard]] std::vector<Expression>::const_iterator tailConstBegin() const;
    [[nodiscard]] std::vector<Expression>::const_iterator tailConstEnd() const;

	Expression eval(Environment& env);
	static Expression apply(const Atom& op, const std::vector<Expression>& args, const Environment& env);

	bool operator==(const Expression& exp) const noexcept;
	[[nodiscard]] std::string toString() const;
	void setProperty(const std::string&, const Expression&);
	Expression getProperty(const std::string&);

    [[nodiscard]] bool isEmpty() const noexcept;

private:
	Atom m_head;
	std::vector<Expression> m_tail;
	std::map<std::string, Expression*> m_properties;

	static Expression handle_lookup(const Atom&, const Environment&);
	Expression handle_begin(Environment&);
	Expression handle_define(Environment&);
	Expression handle_lambda();
	Expression handle_proc_to_list(Environment&);
};

std::ostream& operator<<(std::ostream&, const Expression&);
bool operator!=(const Expression&left, const Expression& right) noexcept;
