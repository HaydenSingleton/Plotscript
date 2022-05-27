#include "atom.h"


Atom::Atom() : m_type(None), m_symbol(std::string()) {}

Atom::Atom(const Token& token) : Atom() {
	std::istringstream iss(token.toString());
	double num;
	if (iss >> num) {
		if (iss.rdbuf()->in_avail() == 0) {
			setNumber(num);
		}
	}
	else {
		setSymbol(token.toString());
	}
}


Atom::Atom(const std::string& str) : Atom() {
	setSymbol(str);
}


Atom::Atom(double n) : Atom() {
	m_type = Number;
	m_number = number(n, 0);

	if (fabs(n) < std::numeric_limits<double>::epsilon() * 2)
		m_number = number(0, 0);
}

Atom::Atom(number complex) : Atom() {
	m_type = Number;
	m_number = complex;

	if (fabs(complex.real()) < std::numeric_limits<double>::epsilon() * 2)
		m_number = number(0, m_number.imag());
	if (fabs(complex.imag()) < std::numeric_limits<double>::epsilon() * 2)
		m_number = number(m_number.real(), 0);
}


Atom::Atom(const Atom& copy) {
	if (copy.isNumber()) {
		setNumber(copy.m_number);
	}
	else if (copy.isSymbol()) {
		setSymbol(copy.m_symbol);
	}
	else {
		if (m_type == Symbol) {
			m_symbol.~basic_string();
		}

		m_type = Symbol;
		new (&m_symbol) std::string(copy.m_symbol);
	}
}


Atom& Atom::operator=(const Atom& other) {

	if (this != &other) {
		if (other.m_type == None) {
			m_type = None;
		}
		else if (other.m_type == Number) {
			setNumber(other.m_number);
		}
		else if (other.m_type == Symbol) {
			setSymbol(other.m_symbol);
		}
	}

	return *this;
}


Atom::~Atom() {
	if (m_type == Symbol)
		m_symbol.~basic_string();
}


bool Atom::isNone() const {
	return m_type == None;
}


bool Atom::isNumber() const {
	return m_type == Number;
}


bool Atom::isSymbol() const {
	return m_type == Symbol;
}


bool Atom::isString() const {
	return m_type == Symbol && !m_symbol.empty() && m_symbol.front() == '\"' && m_symbol.back() == '\"';
}


bool Atom::isComplex() const
{
	return m_type == Number && m_number.imag() != 0;
}


double Atom::asNumber() const noexcept {
	return m_type == Number ? m_number.real() : 0;
}

number Atom::asComplex() const noexcept {
	return m_type == Number ? m_number : 0;
}


std::string Atom::asSymbol() const noexcept {
	return m_type == Symbol ? m_symbol : "_";
}


std::string Atom::toString() const noexcept {
	std::ostringstream os;

	os << *this;
	
	return os.str();
}

void Atom::setNumber(const number n) {

	m_type = Number;
	m_number = n;

	if (fabs(n.real()) < std::numeric_limits<double>::epsilon() * 2)
		m_number = number(0, m_number.imag());
	if (fabs(n.imag()) < std::numeric_limits<double>::epsilon() * 2)
		m_number = number(m_number.real(), 0);
}

void Atom::setSymbol(const std::string &str) {

	if (isSymbol())
		m_symbol.~basic_string();
	else
		m_type = Symbol;

	new (&m_symbol) std::string(str);
}


bool Atom::operator==(const Atom& other) const noexcept {
	if (m_type != other.m_type)
		return false;

	switch (m_type) {
		double diff;
		case Number:
			diff = fabs(m_number.imag() - other.m_number.imag());
			if (isComplex() && other.isComplex() && std::isnan(diff) || (std::numeric_limits<double>::epsilon() * 2) < diff ) {
				return true;
			}
			diff = fabs(m_number.real() - other.m_number.real());
			return diff < std::numeric_limits<double>::epsilon() * 2;
			break;
		case Symbol:
			return m_symbol == other.m_symbol;
			break;
		default:
			return true;
	}
}


bool Atom::operator!=(const Atom& right) const noexcept
{
	return !(*this == right);
}


bool Atom::operator<(const Atom& other) const noexcept
{

	switch (m_type) {
		case Number:
			if (other.m_type == Number) {
				double diff = fabs(m_number.imag() - other.m_number.imag());
				if (isComplex() && other.isComplex() && std::isnan(diff) || (std::numeric_limits<double>::epsilon() * 2) < diff) {
					return diff > 0;
				}
				diff = fabs(m_number.real() - other.m_number.real());
				return diff > std::numeric_limits<double>::epsilon() * 2;
			}
			else if (other.m_type == Symbol) {
				return false;
			}
			else
				return false;
			break;
		case Symbol:
			if (other.m_type == Symbol) {
				return m_symbol < other.m_symbol;
			}
			else
				return false;
			break;
		case None:
			if (other.m_type == None)
				return m_symbol < other.m_symbol;
		default:
			return true;
	}
}


std::ostream& operator<<(std::ostream& out, const Atom& a) {

	
	if (a.isNumber()) {
		out << a.asNumber();
	}
	if (a.isComplex()) {
		out << ", " << a.asComplex().imag();
	}
	else if (a.isSymbol()) {
		out << a.asSymbol();
	}

	return out;
}
