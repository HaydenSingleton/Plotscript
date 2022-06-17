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
	m_type = Real;
	m_number = n;

	if (fabs(n) < std::numeric_limits<double>::epsilon() * 2)
		m_number = 0;
}

Atom::Atom(std::complex<double> complex) : Atom() {
	m_type = Complex;
	m_complex = complex;

	if (fabs(complex.real()) < std::numeric_limits<double>::epsilon() * 2)
		m_complex = std::complex<double>(0, m_complex.imag());
	if (fabs(complex.imag()) < std::numeric_limits<double>::epsilon() * 2)
		m_complex = std::complex<double>(m_complex.real(), 0);
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
		else if (other.m_type == Real) {
			setNumber(other.m_number);
		}
		else if (other.m_type == Symbol) {
			setSymbol(other.m_symbol);
		}
		else {
			setComplex(other.m_complex);
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
	return m_type == Real;
}

bool Atom::isSymbol() const {
	return m_type == Symbol;
}

bool Atom::isString() const {
	return m_type == Symbol && !m_symbol.empty() && m_symbol.front() == '\"' && m_symbol.back() == '\"';
}

bool Atom::isComplex() const
{
	return m_type == Complex;
}

double Atom::asNumber() const noexcept {
	return m_type == Real ? m_number : m_type == Complex ? m_complex.real() : 0;
}

std::complex<double> Atom::asComplex() const noexcept {
	return m_type == Real ? std::complex<double>(m_number, 0) : m_type == Complex ? m_complex : 0;
}

std::string Atom::asSymbol() const noexcept {
	return m_type == Symbol ? m_symbol : "_";
}

std::string Atom::toString() const noexcept {
	std::ostringstream os;

	os << *this;
	
	return os.str();
}

void Atom::setComplex(const std::complex<double> n) {
	m_type = Complex;
	m_complex = n;

	if (fabs(n.real()) < std::numeric_limits<double>::epsilon() * 2)
		m_complex = std::complex<double>(0, m_complex.imag());
	if (fabs(n.imag()) < std::numeric_limits<double>::epsilon() * 2)
		m_complex = std::complex<double>(m_complex.real(), 0);
}


void Atom::setNumber(const double n) {

	m_type = Real;
	m_number = n;

	if (fabs(m_number) < std::numeric_limits<double>::epsilon() * 2)
		m_number = 0;
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
		case Real:
			diff = fabs(m_number - other.m_number);
			return diff < std::numeric_limits<double>::epsilon() * 2;
			break;
		case Complex:
			diff = fabs(m_complex.imag() - other.m_complex.imag());
			return !std::isnan(diff) && (std::numeric_limits<double>::epsilon() * 2) < diff;
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
		case Real:
			return m_number < other.m_number;
			break;
		case Complex:
			if (m_complex.real() < other.m_complex.real())
				if (m_complex.imag() < other.m_complex.imag())
					return true;
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
	else if (a.isComplex()) {
		out << a.asComplex().real() << ", " << a.asComplex().imag();
	}
	else if (a.isSymbol()) {
		out << a.asSymbol();
	}

	return out;
}
