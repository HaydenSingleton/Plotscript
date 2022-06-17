#pragma once

#include "token.h"
#include <cmath>
#include <limits>
#include <sstream>
#include <complex>

class Atom {
public:
	Atom();

	Atom(const Token& t);
	Atom(const std::string& str);
	Atom(double n);
	Atom(std::complex<double> n);

	Atom(const Atom& copy);
	Atom& operator=(const Atom& other);
	~Atom();

	bool isNone() const;
	bool isNumber() const;
	bool isSymbol() const;
	bool isComplex() const;
	bool isString() const;

	std::string asSymbol() const noexcept;
	double asNumber() const noexcept;
	std::complex<double> asComplex() const noexcept;

	std::string toString() const noexcept;
	
	bool operator==(const Atom& o) const noexcept;
	bool operator!=(const Atom&) const noexcept;
	bool operator<(const Atom& other) const noexcept;

private:
	enum Type {None, Real, Complex, Symbol};

	Type m_type;

	union {
		std::string m_symbol;
		double m_number;
		std::complex<double> m_complex;
	};
	
	void setComplex(const std::complex<double> d);
	void setNumber(const double d);
	void setSymbol(const std::string &s);
};

std::ostream& operator<<(std::ostream& out, const Atom& a);