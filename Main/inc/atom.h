#pragma once

#include "token.h"
#include <cmath>
#include <limits>
#include <sstream>
#include <complex>
#include <variant>

class Atom { //NOLINT
public:
	Atom();
	explicit Atom(const std::string&);
	Atom(double); //NOLINT
	Atom(std::complex<double>); //NOLINT

	[[nodiscard]] std::string asSymbol() const noexcept;
	[[nodiscard]] double asNumber() const noexcept;
	[[nodiscard]] std::complex<double> asComplex() const noexcept;

    [[nodiscard]] bool isNone() const;
    [[nodiscard]] bool isNumber() const;
    [[nodiscard]] bool isSymbol() const;
    [[nodiscard]] bool isString() const ;
    [[nodiscard]] bool isComplex() const;

    [[nodiscard]] std::string toString() const noexcept;
	
	bool operator==(const Atom&) const noexcept;
	bool operator!=(const Atom&) const noexcept;
	bool operator<(const Atom&) const noexcept;

private:
    enum class Type {None, Number, Symbol, Complex};
	Type m_type;

    typedef std::variant<double, std::string> ATOM_DATA;
    ATOM_DATA m_data;

    double m_imag = 0;
};

std::ostream& operator<<(std::ostream&, const Atom&);