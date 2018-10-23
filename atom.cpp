#include "atom.hpp"

#include <sstream>
#include <cctype>
#include <cmath>
#include <iostream>

Atom::Atom(): m_type(NoneKind) {}

Atom::Atom(double value){

  setNumber(value);
}

Atom::Atom(std::complex<double> value): Atom() {
  setComplex(value);
}

Atom::Atom(const Token & token): Atom(){

  // is token a number?
  double temp;
  std::istringstream iss(token.asString());
  if(iss >> temp){
    // check for trailing characters if >> succeeds
    if(iss.rdbuf()->in_avail() == 0){
      setNumber(temp);
    }
  }
  else if(!std::isdigit(token.asString()[0]) && token.asString()[0]=='"'){ // check if literal string
    setString(token.asString());
  }
  else{ // else assume symbol
    // make sure does not start with number
    if(!std::isdigit(token.asString()[0]) )
      setSymbol(token.asString());
    }
    // std::cout << "Atom type = " << m_type << "\n";
}

Atom::Atom(const std::string & value): Atom() {
  if(value[0] == '"'){
    setString(value);
  }
  else{
    setSymbol(value);
  }
}

Atom::Atom(const Atom & x): Atom(){
  if(x.isNumber()){
    setNumber(x.numberValue);
  }
  else if(x.isSymbol()){
    setSymbol(x.stringValue);
  }
  else if(x.isComplex()){
    setComplex(x.complexValue);
  }
  else if(x.isString()){
    setString(x.stringValue);
  }
}

Atom & Atom::operator=(const Atom & x){

  if(this != &x){
    if(x.m_type == NoneKind){
      m_type = NoneKind;
    }
    else if(x.m_type == NumberKind){
      setNumber(x.numberValue);
    }
    else if(x.m_type == SymbolKind){
      setSymbol(x.stringValue);
    }
    else if(x.m_type == ComplexKind){
      setComplex(x.complexValue);
    }
    else if(x.m_type==StringKind){
      setString(x.stringValue);
    }
  }
  return *this;
}

Atom::~Atom(){

  // we need to ensure the destructor of the symbol string is called
  if(m_type == SymbolKind || m_type == StringKind){
    stringValue.~basic_string();
  }
}

bool Atom::isNone() const noexcept{
  return m_type == NoneKind;
}

bool Atom::isNumber() const noexcept{
  return m_type == NumberKind;
}

bool Atom::isSymbol() const noexcept{
  return m_type == SymbolKind;
}

bool Atom::isComplex() const noexcept{
  return m_type == ComplexKind;
}

bool Atom::isString() const noexcept{
  return m_type == StringKind;
}

void Atom::setNumber(double value){

  m_type = NumberKind;
  numberValue = value;
}

void Atom::setSymbol(const std::string & value){

  // we need to ensure the destructor of the symbol string is called
  if(m_type == SymbolKind || m_type == StringKind){
    stringValue.~basic_string();
  }

  m_type = SymbolKind;

  // copy construct in place
  new (&stringValue) std::string(value);
}

void Atom::setString(const std::string & value){

  // we need to ensure the destructor of the symbol string is called
  if(m_type == SymbolKind || m_type == StringKind){
    stringValue.~basic_string();
  }

  m_type = StringKind;

  // copy construct in place
  new (&stringValue) std::string(value);
}

void Atom::setComplex(const std::complex<double> & value){
  m_type = ComplexKind;
  complexValue = value;
}

double Atom::asNumber() const noexcept{
  if (m_type == ComplexKind) {
    return complexValue.real();
  }
  return (m_type == NumberKind) ? numberValue : 0.0;
}


std::string Atom::asSymbol() const noexcept{

  std::string result;

  if(m_type == SymbolKind || m_type == StringKind){
    result = stringValue;
  }

  return result;
}

std::string Atom::asString() const noexcept{

  std::ostringstream os;

  if(m_type == StringKind || m_type == SymbolKind){
    os << stringValue;
  }
  else if (m_type == NumberKind){
    os << numberValue;
  }
  else if (m_type == ComplexKind){
    os << complexValue;
  }

  return os.str();
}

std::complex<double> Atom::asComplex() const noexcept{
  if (m_type == NumberKind) {
    std::complex<double> number2complex(numberValue, 0.0);
    return number2complex;
  }
  return (m_type == ComplexKind) ? complexValue : (std::complex<double>)(0);
}

bool Atom::operator==(const Atom & right) const noexcept{

  if(m_type != right.m_type) return false;

  switch(m_type){
  case NoneKind:
    if(right.m_type != NoneKind) return false;
    break;
  case NumberKind:
    {
      if(right.m_type != NumberKind) return false;
      double dleft = numberValue;
      double dright = right.numberValue;
      double diff = fabs(dleft - dright);
      if(std::isnan(diff) || (diff > (std::numeric_limits<double>::epsilon()*2) ) )
        return false;
    }
    break;
  case SymbolKind:
    {
      if(right.m_type != SymbolKind) return false;
      return stringValue == right.stringValue;
    }
    break;
  case StringKind:
    {
      if(right.m_type != StringKind) return false;
      return stringValue == right.stringValue;
    }
    break;
  case ComplexKind:
	{
    if(right.m_type != ComplexKind) return false;
    std::complex<double> diff;
    diff = (complexValue - right.complexValue);
    double realPart = std::fabs(diff.real());
    double imagPart = std::fabs(diff.imag());
    if(realPart > std::numeric_limits<double>::epsilon()*2 || imagPart > std::numeric_limits<double>::epsilon()*2)
      return false;
	}
	break;
  default:
    return false;
  }

  return true;
}

bool operator!=(const Atom & left, const Atom & right) noexcept{

  return !(left == right);
}


std::ostream & operator<<(std::ostream & out, const Atom & a){

  if(a.isNumber()){
    out << a.asNumber();
  }
  if(a.isSymbol()||a.isString()){
    out << a.asSymbol();
  }
  if(a.isComplex()){
    out << a.asComplex();
  }
  return out;
}
