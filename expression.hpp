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
#include <utility>
#include <algorithm>
#include <iostream>
#include <numeric>
#include <cassert>
#include <csignal>
#include <cstdlib>
extern sig_atomic_t global_status_flag;

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
  Expression(const std::vector<Expression> & listItems);

  /// constructor for lambda functions
  Expression(const std::vector<Expression> & args, Expression & func);

  /// constructor for graphics items
  // Expression( );

  /// Constructor for plots
  Expression(std::string type, const std::vector<Expression> & back, size_t num);

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

  /// return a vector of the items in the Expression's tail
  std::vector<Expression> contents() const noexcept;

  /// return the number of items in the tail vector
  size_t tailLength() const noexcept;

  /// return a const-iterator to the beginning of tail
  ConstIteratorType tailConstBegin() const noexcept;

  /// return a const-iterator to the tail end
  ConstIteratorType tailConstEnd() const noexcept;

  /// member when determines if the expression has no type
  bool isNone() const noexcept;

  /// member when determines if the expression is a list
  bool isList() const noexcept;

  /// member when determines if the expression is a Lambda function
  bool isLambda() const noexcept;

  /// member when determines if the expression is actually empty
  bool isEmpty() const noexcept;

  // Check exp type for discrete-plots
  bool isDP() const noexcept;

  // Check exp type for continuous-plots
  bool isCP() const noexcept;

  /// Evaluate expression using a post-order traversal (recursive)
  Expression eval(Environment & env);

  /// equality comparison for two expressions (recursive)
  bool operator==(const Expression & exp) const noexcept;

  /// helper methods for output widget
  bool checkProperty(std::string key, std::string value) const noexcept;
  double getNumericalProperty(std::string) const noexcept;
  std::tuple<double, double, double, double> getTextProperties() const noexcept;
  Atom getProperty(std::string p) {
    if (m_properties.find(p) != m_properties.end()) {
      return m_properties.at(p).head();
    }
    return Atom();
  };

  void setLineThickness(double ) noexcept;
  void setPointSize(double ) noexcept;
  void setTextPosition(Expression p , double r = 0) noexcept;

private:

  // the head of the expression
  Atom m_head;

  std::vector<Expression> m_tail;

  // state variable of the expression
  enum class ExpType {None, Singleton, List, Lambda, Graphic, Plot};
  ExpType m_type;

  // list of the expression's properties
  std::map<std::string, Expression> m_properties;

  // convenience typedef
  typedef std::vector<Expression>::iterator IteratorType;

  // internal helper methods
  Expression handle_lookup(const Atom & head, const Environment & env);
  Expression handle_define(Environment & env);
  Expression handle_begin(Environment & env);
  Expression handle_list(Environment & env);
  Expression handle_lambda(Environment & env);
  Expression handle_apply(Environment & env);
  Expression handle_map(Environment & env);
  Expression handle_set_property(Environment & env);
  Expression handle_get_property(Environment & env);
  Expression handle_discrete_plot(Environment & env);
  Expression handle_cont_plot(Environment & env);

  /* Returns the matching Expression from the interal properties map
  or the empty Expression if not found
  @param key The string to search for */
  Expression __getProperty(std::string key) const ;
};

/// Render expression to output stream
std::ostream & operator<<(std::ostream & out, const Expression & exp);

/// inequality comparison for two expressions (recursive)
bool operator!=(const Expression & left, const Expression & right) noexcept;

#endif
