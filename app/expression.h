#pragma once

#include "atom.h"
#include <utility>
#include <iostream>
#include <vector>
#include <unordered_map>

class Environment;


class Expression {
public:
	Expression();
	Expression(const Atom&);
	Expression(const Atom&, const std::vector<Expression>& items);

	Expression& operator=(const Expression& e);
	~Expression();

	Atom head() const;
	void setHead(const Atom& a);

	typedef std::vector<Expression>::const_iterator ConstIteratorType;

	Expression* tail();
	void appendExp(const Expression&);
	void append(const Atom&);

	ConstIteratorType tailConstBegin() const;
	ConstIteratorType tailConstEnd() const;

	Expression eval(Environment& env);
	Expression apply(const Atom& op, const std::vector<Expression>& args, const Environment& env);

	bool operator==(const Expression& exp) const noexcept;
	std::string toString() const;
	void setProperty(std::string, Expression);
	Expression getProperty(std::string);

private:
	Atom m_head;
	std::vector<Expression> m_tail;
	std::unordered_map<std::string, Expression*> m_properties;

	Expression handle_lookup(const Atom&, const Environment&);
	Expression handle_begin(Environment&);
	Expression handle_define(Environment&);
	Expression handle_lambda();
	Expression handle_proc_to_list(Environment&);

	typedef std::vector<Expression>::iterator IteratorType;
};

std::ostream& operator<<(std::ostream&, const Expression&);
bool operator!=(const Expression&left, const Expression& right) noexcept;
