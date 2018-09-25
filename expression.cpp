#include "expression.hpp"

#include <sstream>
#include <list>

#include "environment.hpp"
#include "semantic_error.hpp"

Expression::Expression():m_type(None) {}

Expression::Expression(const Atom & a): Expression() {

  m_head = a;
}

// recursive copy
Expression::Expression(const Expression & a) {

  m_head = a.m_head;
  m_type = a.m_type ;
  for(auto e : a.m_tail){
    m_tail.push_back(e);
  }
}

// Constructor to get everything after the word "list"
Expression::Expression(const std::vector<Expression> & a){
  m_type = List;
  m_tail = a;
}

// Take an expression and change it to type List
void Expression::makeList() {
  m_type = List;
}

Expression & Expression::operator=(const Expression & a){

  // prevent self-assignment
  if(this != &a){
      m_head = a.m_head;
      m_tail.clear();
      for(auto e : a.m_tail)
        m_tail.push_back(e);
  }

  return *this;
}


Atom & Expression::head(){
  return m_head;
}

const Atom & Expression::head() const{
  return m_head;
}

bool Expression::isHeadNumber() const noexcept{
  return m_head.isNumber();
}

bool Expression::isHeadSymbol() const noexcept{
  return m_head.isSymbol();
}

bool Expression::isHeadComplex() const noexcept{
  return m_head.isComplex();
}

bool Expression::isList() const noexcept {
	return (m_type == List);
}

void Expression::append(const Atom & a){
  m_tail.emplace_back(a);
}


Expression * Expression::tail(){
  Expression * ptr = nullptr;

  if(m_tail.size() > 0){
    ptr = &m_tail.back();
  }

  return ptr;
}

size_t Expression::tailLength() const noexcept{
  return m_tail.size();
}

Expression::ConstIteratorType Expression::tailConstBegin() const noexcept{
  return m_tail.cbegin();
}

Expression::ConstIteratorType Expression::tailConstEnd() const noexcept{
  return m_tail.cend();
}

Expression apply(const Atom & op, const std::vector<Expression> & args, const Environment & env){

  // head must be a symbol
  if(!op.isSymbol()){
    throw SemanticError("Error during evaluation: procedure name not symbol");
  }

  // must map to a proc
  if(!env.is_proc(op)){
    throw SemanticError("Error during evaluation: symbol does not name a procedure");
  }

  // map from symbol to proc
  Procedure proc = env.get_proc(op);

  // call proc with args
  Expression a;
  a = proc(args);

  if(op.asSymbol() == "list")
    a.makeList();
  return a;
}

Expression Expression::handle_lookup(const Atom & head, const Environment & env){
    if(head.isSymbol()) { // if symbol is in env return value
      if(env.is_exp(head)) {
	      return env.get_exp(head);
      }
      else {
	      throw SemanticError("Error during handle lookup: unknown symbol");
      }
    }
    else if(head.isNumber() || head.isComplex()){
      return Expression(head);
    }
    else{
      throw SemanticError("Error during handle lookup: Invalid type in terminal expression");
    }
}

Expression Expression::handle_begin(Environment & env){

  if(m_tail.size() == 0){
    throw SemanticError("Error during handle begin: zero arguments to begin");
  }

  // evaluate each arg from tail, return the last
  Expression result;
  for(Expression::IteratorType it = m_tail.begin(); it != m_tail.end(); ++it){
    result = it->eval(env);
  }

  return result;
}


Expression Expression::handle_define(Environment & env){

  // check expected tail size
  if(m_tail.size() != 2){
    throw SemanticError("Error during handle define: invalid number of arguments to define");
  }

  // tail[0] must be symbol
  if(!m_tail[0].isHeadSymbol()){
    throw SemanticError("Error during handle define: first argument to define not symbol");
  }

  // but tail[0] must not be a special-form or procedure
  std::string s = m_tail[0].head().asSymbol();
  if((s == "define") || (s == "begin")){
    throw SemanticError("Error during handle define: attempt to redefine a special-form");
  }

  if(env.is_proc(m_head)){
    throw SemanticError("Error during handle define: attempt to redefine a built-in procedure");
  }

  // eval tail[1]
  Expression result = m_tail[1].eval(env);

  if(env.is_exp(m_head)){
    throw SemanticError("Error during handle define: attempt to redefine a previously defined symbol");
  }

  //and add to env
  env.add_exp(m_tail[0].head(), result);

  return result;
}

// this is a simple recursive version. the iterative version is more
// difficult with the ast data structure used (no parent pointer).
// this limits the practical depth of our AST
Expression Expression::eval(Environment & env){

  if(m_tail.empty()){
	  if (m_head.isSymbol() && (m_head.asSymbol() == "list")) {
      std::vector<Expression> a;
		  return Expression(a);
	  }
    return handle_lookup(m_head, env);
  }
  // handle begin special-form
  else if(m_head.isSymbol() && m_head.asSymbol() == "begin"){
    return handle_begin(env);
  }
  // handle define special-form
  else if(m_head.isSymbol() && m_head.asSymbol() == "define"){
    return handle_define(env);
  }
  // else attempt to treat as procedure
  else{
    std::vector<Expression> results;
    for(Expression::IteratorType it = m_tail.begin(); it != m_tail.end(); ++it){
      results.push_back(it->eval(env));
    }
    return apply(m_head, results, env);
  }
}


std::ostream & operator<<(std::ostream & out, const Expression & exp){
  if(!exp.isHeadComplex()) {
    out << "(";
  }
  out << exp.head();

  for(auto e = exp.tailConstBegin(); e != exp.tailConstEnd(); ++e){
    out << *e;
    if((e + 1) != exp.tailConstEnd()){
      out << " ";
    }
  }
  if(!exp.isHeadComplex()) {
    out << ")";
  }

  return out;
}

bool Expression::operator==(const Expression & exp) const noexcept{

  bool result = (m_head == exp.m_head);

  result = result && (m_tail.size() == exp.m_tail.size());

  if(result){
    for(auto lefte = m_tail.begin(), righte = exp.m_tail.begin();
	(lefte != m_tail.end()) && (righte != exp.m_tail.end());
	++lefte, ++righte){
      result = result && (*lefte == *righte);
    }
  }

  return result;
}

bool operator!=(const Expression & left, const Expression & right) noexcept{

  return !(left == right);
}
