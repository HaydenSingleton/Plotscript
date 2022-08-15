#include "atom.h"

Atom::Atom() {
    m_type = Atom::Type::None;
}

Atom::Atom(const std::string &s) {
    m_type = Type::Symbol;
    m_data = s;
}

bool roundsToZero(double value) {
	return fabs(value) < std::pow(1, -10);
}

Atom::Atom(double n) : m_type(Type::Number) {
	m_data = roundsToZero(n) ?  0 : n;
}

Atom::Atom(std::complex<double> complex) : Atom(complex.real()) {
    m_imag = complex.imag();
	if (!roundsToZero(m_imag))
		m_type = Type::Complex;
	else
		m_imag = 0;
}

double Atom::asNumber() const noexcept {
    return m_type == Type::Number || m_type == Type::Complex ? *std::get_if<double>(&m_data) : 0;
}

std::complex<double> Atom::asComplex() const noexcept {
	
	if (isComplex())
		return std::complex<double>(std::get<double>(m_data), m_imag);
	else
		return isNumber() ? std::complex<double>(std::get<double>(m_data), 0) : 0;
}

std::string Atom::asSymbol() const noexcept {
	return m_type == Type::Symbol ? std::get<std::string>(m_data) : "";
}

bool Atom::isNone() const {
    return m_type == Type::None;
}

bool Atom::isNumber() const {
    return m_type == Type::Number;
}

bool Atom::isSymbol() const {
    return m_type == Type::Symbol;
}

bool Atom::isString() const {
    return m_type == Type::Symbol && std::get<std::string>(m_data).starts_with('"');
}

bool Atom::isComplex() const {
    return m_type == Type::Complex;
}

bool Atom::operator==(const Atom& other) const noexcept {
	if (m_type != other.m_type)
		return false;

    double diff;
    switch (m_type) {
		case Type::Number:
			diff = fabs(asNumber() - other.asNumber());
			return roundsToZero(diff);
		case Type::Complex:
			diff = fabs(asNumber() - other.asNumber());
			diff += fabs(asComplex().imag() - other.asComplex().imag());
			return !std::isnan(diff) && roundsToZero(diff);
		case Type::Symbol:
			return asSymbol() == other.asSymbol();
		default:
			return false;
	}
}

bool Atom::operator!=(const Atom& right) const noexcept
{
	return !(*this == right);
}

bool Atom::operator<(const Atom& other) const noexcept
{
	switch (m_type) {
		case Type::Number:
        case Type::Complex:
            return asNumber() < other.asNumber();
		case Type::Symbol:
			if (other.m_type == Type::Symbol) {
				return asSymbol() < other.asSymbol();
			}
			else
				return false;
		default:
			return false;
	}
}

std::string Atom::toString() const noexcept {
    std::ostringstream os;
    os << *this;
    return os.str();
}

std::ostream& operator<<(std::ostream& out, const Atom& a) {

    if (a.isNumber())
        out << a.asNumber();
    if (a.isSymbol())
        out << a.asSymbol();
    if (a.isComplex())
        out << a.asNumber() << ", " << a.asComplex().imag();

	return out;
}
