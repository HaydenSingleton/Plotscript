#include "expression.h"
#include "environment.h"
#include "semantic_error.h"

Expression::Expression() = default;

Expression::Expression(const Atom& a) {
	m_head = a;
}

Expression::Expression(const Atom& a, const std::vector<Expression>& items) {
	m_head = a;
	m_tail = items;
}

Expression& Expression::operator=(const Expression& exp) {

	if (this != &exp) {
		m_head = exp.m_head;
		m_tail.clear();
		for (const auto& e : exp.m_tail)
			m_tail.push_back(e);

		m_properties = exp.m_properties;
	}

	return *this;
}

Atom Expression::head() const {
	return m_head;
}

Expression* Expression::tail() {
	Expression* tail = nullptr;
	if (!m_tail.empty())
		tail = &m_tail.back();
	return tail;
}

void Expression::setHead(const Atom& a) {
	m_head = a;
}

void Expression::append(const Atom& a) {
	m_tail.emplace_back(a);
}

std::vector<Expression>::const_iterator Expression::tailConstBegin() const
{
	return m_tail.cbegin();
}

std::vector<Expression>::const_iterator Expression::tailConstEnd() const
{
	return m_tail.cend();
}

void Expression::setProperty(const std::string& name, const Expression& value)
{
	if (m_properties.find(name) != m_properties.end())
		m_properties.erase(name);

	auto p = new Expression(value);
	m_properties.emplace(name, p);
}

Expression Expression::getProperty(const std::string& name)
{
	if (m_properties.find(name) != m_properties.end())
		return *m_properties.at(name);
	else
		return {};
}

Expression Expression::handle_lookup(const Atom& a, const Environment& env)
{
	if (env.is_exp(a)) {
		return env.get_exp(a);
	}

	if (a.isString() || a.isNumber()) {
			return { a };
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

Expression Expression::handle_lambda() {

	std::vector<Expression> lambda;

	std::vector<Expression> args;
	args.emplace_back(Expression(m_tail[0].head()));
	for (auto e = m_tail[0].tailConstBegin(); e != m_tail[0].tailConstEnd(); e++) {
		args.emplace_back(Expression(*e));
	}

	lambda.emplace_back(Atom("list"), args);
	lambda.emplace_back(m_tail[1]);

	return { Atom("lambda"), lambda };
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
	catch (SemanticError& err) {
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
		return handle_define(env);
	}
	else if (cmd == "lambda") {
		return handle_lambda();
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
		for (auto & it : m_tail){
			args.push_back(it.eval(env));
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

    if (head == "lambda") {

        for (auto arg = m_tail[0].tailConstBegin(); arg != m_tail[0].tailConstEnd(); arg++) {
            out << *arg;
            if (arg + 1 == m_tail[0].tailConstEnd())
                out << ")";
            else
                out << " ";
        }

        out << " (" << m_tail[1].head() << " ";

        for (auto arg = m_tail[1].tailConstBegin(); arg != m_tail[1].tailConstEnd(); arg++) {
            out << *arg;

            if (arg + 1 != m_tail[1].tailConstEnd())
                out << " ";
            else
                out << ")";
        }
    }
    else {
        if (head != "list" )
            out << m_head;
        for (const auto& e : m_tail) {
            out << e;
            if (e != m_tail.back())
                out << " ";
        }
    }

	return out.str();
}

std::ostream& operator<<(std::ostream& out, const Expression& exp) {

	return out << "(" << exp.toString() << ")";
}

bool Expression::operator==(const Expression& exp) const noexcept {

	bool result = m_head == exp.m_head;

	result = result && (m_tail.size() == exp.m_tail.size());

	if (result) {
		for (int i = 0 ; i < m_tail.size(); i++) {
			result &= (m_tail[i] == exp.m_tail[i]);
		}
	}

	return result;
}

bool Expression::isEmpty() const noexcept {
    return m_head.isNone() && m_tail.empty();
}

bool operator!=(const Expression& left, const Expression& right) noexcept {
	return !(left == right);
}