#include "expression.h"
#include "environment.h"
#include "semantic_error.h"


Expression::Expression() {}

Expression::Expression(const Atom& a) {
	m_head = a;
}

Expression::Expression(const Atom& a, const std::vector<Expression>& items) {
	m_head = a;
	m_tail = items;
}

Expression& Expression::operator=(const Expression& e) {

	if (this != &e) {
		m_head = e.m_head;
		m_tail.clear();
		for (auto e : e.m_tail)
			m_tail.push_back(e);
		m_properties = e.m_properties;
	}

	return *this;
}

Expression::~Expression() {
	m_head.~Atom();
}

Atom Expression::head() const {
	return m_head;
}

Expression* Expression::tail() {
	Expression* tail = nullptr;
	if (m_tail.size() > 0)
		tail = &m_tail.back();
	return tail;
}

void Expression::setHead(const Atom& a) {
	m_head = a;
}

void Expression::appendExp(const Expression& e) {
	m_tail.emplace_back(e);
}

void Expression::append(const Atom& a) {
	m_tail.emplace_back(a);
}

Expression::ConstIteratorType Expression::tailConstBegin() const
{
	return m_tail.cbegin();
}

Expression::ConstIteratorType Expression::tailConstEnd() const
{
	return m_tail.cend();
}

void Expression::setProperty(std::string name, Expression value)
{
	if (m_properties.find(name) != m_properties.end())
		m_properties.erase(name);

	m_properties.emplace(name, value);
}

Expression Expression::getProperty(std::string name)
{
	if (m_properties.find(name) != m_properties.end())
		return m_properties.at(name);
	else
		return Expression();
}

Expression Expression::handle_lookup(const Atom& a, const Environment& env)
{
	if (env.is_exp(a)) {
		return env.get_exp(a);
	}
	if (a.isString() || a.isNumber()) {
			return Expression(a);
	}

	throw SemanticError("Unknown symbol: " + a.toString());
	
}

Expression Expression::handle_begin(Environment& env)
{
	Expression result;
	for (auto& it : m_tail) {
		result = it.eval(env);
	}
	return result;
}

Expression Expression::handle_define(Environment& env) {

	if (m_tail.size() != 2) {
		throw SemanticError("Error during handle define: Invalid number of arguments");
	}
	if (!m_tail[0].head().isSymbol()) {
		throw SemanticError("Error during handle define: first argument to define not symbol");
	}

	Atom symbol = m_tail[0].head();
	Expression value = m_tail[1].eval(env);
	try {
		env.add_exp(symbol, value);
	}
	catch (SemanticError& err) {
		std::string msg("Error in handle_define: ");
		msg += err.what();
		throw SemanticError(msg);
	}

	return value;
}

Expression Expression::handle_lambda(Environment& env) {

	std::vector<Expression> lambda;

	std::vector<Expression> args;
	args.emplace_back(Expression(m_tail[0].head()));
	for (auto e = m_tail[0].tailConstBegin(); e != m_tail[0].tailConstEnd(); e++) {
		args.emplace_back(Expression(*e));
	}

	lambda.push_back(Expression(Atom("list"), args));
	lambda.push_back(m_tail[1]);

	return Expression(Atom("lambda"), lambda);
}

Expression evaluate_lambda(const Atom& op, const std::vector<Expression>& args, const Environment& env) {

	Expression func = env.get_exp(op);
	Expression arg_list = *func.tailConstBegin();

	Environment scope = env;

	size_t count = 0;

	for (auto param = arg_list.tailConstBegin(); param != arg_list.tailConstEnd(); param++) {
		if (count == args.size())
			throw SemanticError("Error: too few args given to anonymous function " + op.toString());

		scope.add_exp(param->head(), args[count++]);
	}

	if (count != args.size())
		throw SemanticError("Error: too many args given to anonymous function " + op.toString());

	return func.tail()->eval(scope);
}

Expression Expression::handle_proc_to_list(Environment& env) {

	std::string cmd = m_head.toString();

	if (m_tail.size() != 2) {
		throw SemanticError("Error: Not given 2 arguments to " + cmd);
	}

	Expression list = tail()->eval(env);

	if (list.head().toString() != "list")
		throw SemanticError("Error: second argument to " + cmd + " not a list");

	Atom proc;
	if (m_tail[0].m_tail.empty())
		proc = m_tail[0].head();
	else
		proc = m_tail[0].eval(env).head();

	if ((!env.is_proc(proc) && !env.is_lambda(proc)))
		throw SemanticError("Error: first argument to " + cmd + " is not a procedure");

	try {
		if (cmd == "apply") {
			return apply(m_head, list.m_tail, env);
		}
		else if (cmd == "map") {
			std::vector<Expression> result;
			std::vector<Expression> map_args;

			for (auto& item : list.m_tail) {
				map_args.push_back(item);
				result.emplace_back(apply(proc, map_args, env));
				map_args.clear();
			}

			return apply(Atom("list"), result, env);
		}
		else {
			throw SemanticError("Unsupported operation");
		}
	}
	catch (SemanticError err) {
		std::string msg("Error during apply: ");
		msg += err.what();
		throw SemanticError(msg);
	}
	
}

Expression Expression::eval(Environment& env)
{
	std::string cmd(m_head.toString());

	if (cmd == "begin") {
		return handle_begin(env);
	}
	else if (cmd == "define") {
		return handle_define(env);;
	}
	else if (cmd == "lambda") {
		return handle_lambda(env);
	}
	else if (m_tail.empty()) {
		if (env.is_proc(m_head))
			return env.get_proc(m_head)(m_tail);
		else
			return handle_lookup(m_head, env);
	}
	if (cmd == "apply" || cmd == "map") {
		return handle_proc_to_list(env);
	}
	else {
		std::vector<Expression> args;
		for (auto &it = m_tail.begin(); it != m_tail.end(); it++) {
			args.push_back(it->eval(env));
		}
		return apply(m_head, args, env);
	}
}

Expression Expression::apply(const Atom& op, const std::vector<Expression>& args, const Environment& env) {

	if (env.is_proc(op)) {
		return env.get_proc(op)(args);
	}

	if (env.is_lambda(op)) {
		return evaluate_lambda(op, args, env);
	}

	throw SemanticError(op.toString() + " is not a procedure.");
}

std::string Expression::toString() const {
	std::ostringstream out;
	std::string head(m_head.toString());

	if (head == "") {
		out << "NONE";
	}
	else {
		out << "(";
		
		if (head == "lambda") {

			for (auto& arg = m_tail[0].tailConstBegin(); arg != m_tail[0].tailConstEnd(); arg++) {
				out << *arg;
				if (arg + 1 == m_tail[0].tailConstEnd())
					out << ")";
				else
					out << " ";
			}

			out << " (" << m_tail[1].head() << " ";
		
			for (auto& arg = m_tail[1].tailConstBegin(); arg != m_tail[1].tailConstEnd(); arg++) {
				out << *arg;

				if (arg + 1 != m_tail[1].tailConstEnd())
					out << " ";
				else
					out << ")";
			}
		}
		else {

			if (head != "list")
				out << m_head;

			for (auto& it = tailConstBegin(); it != tailConstEnd(); it++) {

				out << *it;

				if (it + 1 != tailConstEnd())
					out << " ";
			}
		}

		out << ")";
	}

	return out.str();
}

std::ostream& operator<<(std::ostream& out, const Expression& exp) {

	return out << exp.toString();
}

bool Expression::operator==(const Expression& exp) const noexcept {

	bool result = m_head == exp.m_head;

	result = result && (m_tail.size() == exp.m_tail.size());

	if (result) {
		for (auto lefte = m_tail.begin(), righte = exp.m_tail.begin();
			(lefte != m_tail.end()) && (righte != exp.m_tail.end());
			++lefte, ++righte) {
			result = result && (*lefte == *righte);
		}
	}

	return result;
}

bool operator!=(const Expression& left, const Expression& right) noexcept
{
	return !(left == right);
}