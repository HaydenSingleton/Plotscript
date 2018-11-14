/*! \file expression.hpp
Defines the Expression type and assiciated functions.
 */
#ifndef EXPRESSION_HPP
#define EXPRESSION_HPP

#include <string>
#include <vector>

#include "token.hpp"
#include "atom.hpp"

#include <map>
// #include <utility>
#include <algorithm>
#include <iostream>


// forward declare Environment
class Environment;

/*! \class Expression
\brief An expression is a tree of Atoms.

An expression is an atom called the head followed by a (possibly empty)
list of expressions called the tail.
 */
class Expression {
public:

  typedef std::vector<Expression>::const_iterator ConstIteratorType;

  /// Default construct and Expression, whose type in NoneType
  Expression();

  /*! Construct an Expression with given Atom as head an empty tail
    \param atom the atom to make the head
  */
  Expression(const Atom & a);

  /// deep-copy construct an expression (recursive)
  Expression(const Expression & a);

  /// constructor for list
  Expression(const std::vector<Expression> & a);

  /// constructor for lambda type
  Expression(const std::vector<Expression> & args, Expression & func);

  /// constructor for special cases like graphics items
  Expression(const Atom & head, const std::vector<Expression> & tail);

  //Constructor for plots
  Expression(const Expression & a, std::string t);

  /// deep-copy assign an expression  (recursive)
  Expression & operator=(const Expression & a);

  /// return a reference to the head Atom
  Atom & head();

  /// return a const-reference to the head Atom
  const Atom & head() const;

  /// append Atom to tail of the expression
  void append(const Atom & a);

  /// return a pointer to the last expression in the tail, or nullptr
  Expression * tail();

  /// return the number of items in the tail vector
  size_t tailLength() const noexcept;

  /// return a const-iterator to the beginning of tail
  ConstIteratorType tailConstBegin() const noexcept;

  /// return a const-iterator to the tail end
  ConstIteratorType tailConstEnd() const noexcept;

  /// convienience member to determine if head atom is a number
  bool isHeadNumber() const noexcept;

  /// convienience member to determine if head atom is a symbol
  bool isHeadSymbol() const noexcept;

  /// convenience member to determine if the head atom is a comlex number
  bool isHeadComplex() const noexcept;

  bool isHeadString() const noexcept;

  /// member when determines if the expression has no type
  bool isNone() const noexcept;

  /// member when determines if the expression is a list
  bool isList() const noexcept;

  /// member when determines if the expression is a list
  bool isLambda() const noexcept;

  /// member when determines if the expression is actually empty
  bool isEmpty() const noexcept;

  // Check and change exp type for discrete-plots
  bool isDP() const noexcept;

  // Check and change exp type for continuous-plots
  bool isCP() const noexcept;

  /// Evaluate expression using a post-order traversal (recursive)
  Expression eval(Environment & env);

  /// equality comparison for two expressions (recursive)
  bool operator==(const Expression & exp) const noexcept;

  /// helper methods for output widget
  std::string toString() const noexcept;
  bool isPoint() const noexcept;
  bool isLine() const noexcept;
  bool isText() const noexcept;
  std::tuple<double, double, double, double, bool> getTextProperties() const noexcept;
  std::vector<Expression> asVector() const noexcept;
  double getNumericalProperty(std::string ) const noexcept;
  std::pair<double, double> getPointCoordinates() const noexcept;
  void setLineThickness(double ) noexcept;
  void setPointSize(double ) noexcept;

private:

  // the head of the expression
  Atom m_head;

  // the tail list is expressed as a vector for access efficiency
  // and cache coherence, at the cost of wasted memory.
  std::vector<Expression> m_tail;

  // state variable of the expression
  enum class ExpType {None, List, Lambda, Empty, Graphic, DP, CP};
  ExpType m_type = ExpType::None;

  // list of the expression's properties
  std::map<std::string, Expression> m_properties;

  // convenience typedef
  typedef std::vector<Expression>::iterator IteratorType;

  // internal helper methods
  Expression handle_lookup(const Atom & head, const Environment & env);
  Expression handle_define(Environment & env);
  Expression handle_begin(Environment & env);
  Expression handle_lambda();
  Expression handle_apply(Environment & env);
  Expression handle_map(Environment & env);
  Expression handle_set_property(Environment & env);
  Expression handle_get_property(Environment & env);
  Expression handle_discrete_plot(Environment & env);
  Expression handle_cont_plot(Environment & env);
};

/// Render expression to output stream
std::ostream & operator<<(std::ostream & out, const Expression & exp);

/// inequality comparison for two expressions (recursive)
bool operator!=(const Expression & left, const Expression & right) noexcept;

#endif
