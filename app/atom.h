#pragma once

#include "token.h"
#include <cmath>
#include <limits>
#include <sstream>
#include <complex>

typedef std::complex<double> number;

class Atom {
public:
	Atom();

	Atom(const Token& t);
	Atom(const std::string& str);
	Atom(number n);
	Atom(double n);

	Atom(const Atom& copy);
	Atom& operator=(const Atom& other);
	~Atom();

	bool isNone() const;
	bool isNumber() const;
	bool isSymbol() const;
	bool isComplex() const;
	bool isString() const;

	double asNumber() const noexcept;
	std::string asSymbol() const noexcept;
	number asComplex() const noexcept;

	std::string toString() const noexcept;
	
	bool operator==(const Atom& o) const noexcept;
	bool operator!=(const Atom&) const noexcept;
	bool operator<(const Atom& other) const noexcept;

private:
	enum Type {None, Number, Symbol};

	Type m_type;

	union {
		std::string m_symbol;
		number m_number;
	};
	
	void setNumber(const number d);
	void setSymbol(const std::string &s);
};

std::ostream& operator<<(std::ostream& out, const Atom& a);