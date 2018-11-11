#include "expression.hpp"

#include <sstream>
#include <list>

#include "environment.hpp"
#include "semantic_error.hpp"

Expression::Expression()
{}

Expression::Expression(const Atom & a): m_head(a)
{}

// recursive copy
Expression::Expression(const Expression & a) {

  m_head = a.m_head;
  m_type = a.m_type;
  m_properties = a.m_properties;
  for(auto e : a.m_tail){
    m_tail.push_back(e);
  }
}

// Constructor for lists
Expression::Expression(const std::vector<Expression> & a){
  m_type = ExpType::List;
  m_tail = a;
}

//Constructor for Lambda functions
Expression::Expression(const std::vector<Expression> & args, Expression & func) {
  m_type = ExpType::Lambda;
  m_tail.push_back(args);
  m_tail.push_back(func);
}

//Constructor for special cases
Expression::Expression(const Atom & head, const std::vector<Expression> & tail) : m_head(head) {
  m_type = ExpType::Graphic;
  for(auto e : tail){
    m_tail.push_back(e);
  }
}

//Constructor for plots
Expression::Expression(const Expression & a, std::string t){
  if(t == "discrete-plot") {
    m_head = a.m_head;
    m_type = ExpType::DP;
    m_properties = a.m_properties;
    for(auto e : a.m_tail){
      m_tail.push_back(e);
    }
  }
  else {
    return;
  }
}

Expression & Expression::operator=(const Expression & a){

  // prevent self-assignment
  if(this != &a){
      m_type = a.m_type;
      m_head = a.m_head;
      m_properties = a.m_properties;
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

bool Expression::isHeadString() const noexcept{
  return m_head.isString();
}

bool Expression::isNone() const noexcept {
	return (m_type == ExpType::None);
}

bool Expression::isList() const noexcept {
	return (m_type == ExpType::List);
}

bool Expression::isLambda() const noexcept {
  return (m_type == ExpType::Lambda);
}

bool Expression::isEmpty() const noexcept {
  return (m_type == ExpType::Empty);
}

bool Expression::isDP() const noexcept {
  return (m_type == ExpType::DP);
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

  //op holds a lambda
  if(env.is_exp(op)){
    Environment inner_scope = env;

    Expression lambda = inner_scope.get_exp(op);
    Expression arg_template = *lambda.tailConstBegin();

    if(args.size() != arg_template.tailLength()){
      throw SemanticError("Error: during apply: Error in call to procedure: invalid number of arguments.");
    }

    size_t count = 0;
    for(auto p = arg_template.tailConstBegin(); p != arg_template.tailConstEnd(); p++){
      inner_scope.__shadowing_helper(p->head(), args[count++]);
    }

    return lambda.tail()->eval(inner_scope);
  }//stop here if applying a lambda


  // head must be a symbol
  if(!op.isSymbol()){
    std::cout << op.asString() << std::endl;
    throw SemanticError("Error during evaluation: procedure name not symbol");
  }

  // must map to a proc
  if(!env.is_proc(op)){
    throw SemanticError("Error during evaluation: symbol does not name a procedure");
  }

    // map from symbol to proc
  Procedure proc = env.get_proc(op);

  // call proc with args
  return proc(args);
}

Expression Expression::handle_lookup(const Atom & head, const Environment & env){

    if(head.isSymbol()) { // if symbol is in env return value
      if(env.is_exp(head)) {
	      return env.get_exp(head);
      }
      else {
	      throw SemanticError("Error during handle lookup: unknown symbol " + head.asSymbol());
      }
    }
    else if(head.isNumber() || head.isComplex() || head.isString()){
      return Expression(head);
    }
    else{
      throw SemanticError("Error during handle lookup: Invalid type in terminal expression");
    }
}

Expression Expression::handle_begin(Environment & env){

  Expression result;
  for(Expression::IteratorType it = m_tail.begin(); it != m_tail.end(); ++it){
    result = it->eval(env);
  }

  // go through a whole "program" and return the last result
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
  if((s == "define") || (s == "begin") || (s == "lambda") || (s == "list")){
    throw SemanticError("Error during handle define: attempt to redefine a special-form");
  }

  if(env.is_proc(m_tail[0].head())){
    throw SemanticError("Error during handle define: attempt to redefine a built-in procedure");
  }

  // eval tail[1]
  Expression result = m_tail[1].eval(env);

  if((s=="pi"||(s=="e")||s=="I")){
    throw SemanticError("Error during handle define: attempt to redefine a built-in symbol");
  }

  //and add to env
  env.add_exp(m_tail[0].head(), result);

  return result;
}

Expression Expression::handle_lambda(){

  std::vector<Expression> argument_template;
  argument_template.emplace_back(Expression(m_tail[0].head()));
  for(auto e = m_tail[0].tailConstBegin(); e!=m_tail[0].tailConstEnd(); e++){
    argument_template.emplace_back(Expression(*e));
  }

  Expression return_exp = Expression(argument_template, m_tail[1]);
  return return_exp;
}

Expression Expression::handle_apply(Environment & env){

  if(m_tail.size() != 2){
    throw SemanticError("Error during apply: invalid number of arguments to define");
  }

  Atom op =  m_tail[0].head();

  if(!env.is_proc(op) || m_tail[0].tailLength() != 0){
    Expression arg1 = m_tail[0].eval(env);
    if(!arg1.isLambda())
      throw SemanticError("Error during apply: first argument to apply not a procedure");
  }

  Expression list_evaled = m_tail[1].eval(env);

  if(!list_evaled.isList()){
    throw SemanticError("Error during apply: second argument to apply not a list");
  }

  std::vector<Expression> list_args;
  for(auto e = list_evaled.tailConstBegin(); e != list_evaled.tailConstEnd(); e++){
    list_args.push_back(*e);
  }

  return apply(op, list_args, env);
}

Expression Expression::handle_map(Environment & env){

  if(m_tail.size() != 2){
    throw SemanticError("Error during apply: invalid number of arguments to define");
  }

  Expression list_evaled = m_tail[1].eval(env);

  if(!list_evaled.isList()){
    throw SemanticError("Error during apply: second argument to apply not a list");
  }

  std::vector<Expression> return_args;
  std::vector<Expression> temp;
  Expression temp_e;

  for(auto e = list_evaled.tailConstBegin(); e != list_evaled.tailConstEnd(); e++){
    temp.emplace_back(*e);
    temp_e = apply(m_tail[0].head(), temp, env);
    return_args.push_back(temp_e);
    temp.clear();
  }

  return Expression(return_args);
}

Expression Expression::handle_set_property(Environment & env){
   Expression result = m_tail[2].eval(env);
   if(m_tail.size()==3) {
    if(m_tail[0].isHeadString()){
      std::string key = m_tail[0].head().asString();
      if(result.m_properties.find(key) != result.m_properties.end()){
        result.m_properties.erase(key);
      }
      Expression value = m_tail[1].eval(env);
      result.m_properties[key] = value;
    }
    else{
      throw SemanticError("Error: first argument to set-property not a string.");
    }
  }
  else{
    throw SemanticError("Error invalid number of arguments for set-property.");
  }
  return result;
}

Expression Expression::handle_get_property(Environment & env){
  Expression target = m_tail[1].eval(env);
  Expression result;
  if(m_tail.size()==2) {
    if(m_tail[0].isHeadString()){
      if(!env.is_proc(target.head())){
          std::string key = m_tail[0].head().asString();
          if(target.m_properties.find(key)!= target.m_properties.end()){
            result = target.m_properties.at(key);
          }
          else {
            result.m_type = ExpType::Empty;
          }
          return result;
      }
      else{
        throw SemanticError("Error: second argument to get-property must not be a procedure.");
      }
    }
    else{
      throw SemanticError("Error: first argument to get-property not a string.");
    }
  }
  else{
    throw SemanticError("Error invalid number of arguments for get-property.");
  }
}

Expression Expression::handle_discrete_plot(Environment & env){
  if(m_tail.size()==2){
    std::vector<Expression> result;
    Expression DATA = m_tail[0].eval(env);
    Expression OPTIONS = m_tail[1].eval(env);
    if(DATA.isList() && OPTIONS.isList()) {

      // Constants used to create bounding box and graph
      double N = 20;

      // Find the max and min values of x and y inside DATA
      double xmax, xmin, ymax, ymin;
      for(auto &i : DATA.m_tail){
        if(i.m_tail[0].head().asNumber() > xmax)
          xmax = i.m_tail[0].head().asNumber();
        else if(i.m_tail[0].head().asNumber() < xmin)
          xmin = i.m_tail[0].head().asNumber();

        if(i.m_tail[1].head().asNumber() > ymax)
          ymax = i.m_tail[1].head().asNumber();
        else if(i.m_tail[1].head().asNumber() < ymin)
          ymin = i.m_tail[1].head().asNumber();
      }
      // Create scale factors using the max and min edges of the data
      double xscale = N/(xmax-xmin), yscale = N/(ymax-ymin);

      // Scale bounds of the box
      xmin *= xscale; xmax *= xscale; ymin *= yscale; ymax *= yscale;

      // Make an expression for each point of the bounding box
      Expression topLeft, topMid, topRight, midLeft, midMid, midRight, botLeft, botMid, botRight;
      std::vector<Expression> xy = {Expression(xmin), Expression(ymax)};
      topLeft = Expression(Atom("make-point"), xy);

      xy = {Expression((xmax+xmin)/2), Expression(ymax)};
      topMid = Expression(Atom("make-point"), xy);

      xy = {Expression(xmax), Expression(ymax)};
      topRight = Expression(Atom("make-point"), xy);

      xy = {Expression(xmin), Expression((ymin+ymax)/2)};
      midLeft = Expression(Atom("make-point"), xy);

      xy = {Expression((xmax+xmin)/2), Expression((ymin+ymax)/2)};
      midMid = Expression(Atom("make-point"), xy);

      xy = {Expression(xmax), Expression((ymin+ymax)/2)};
      midRight = Expression(Atom("make-point"), xy);

      xy = {Expression(xmin), Expression(ymin)};
      botLeft = Expression(Atom("make-point"), xy);

      xy = {Expression((xmax+xmin)/2), Expression(ymin)};
      botMid = Expression(Atom("make-point"), xy);

      xy = {Expression(xmax), Expression(ymin)};
      botRight = Expression(Atom("make-point"), xy);

      // Make an expression to hold each line of the bounding rect
      Expression leftLine, topLine, rightLine, botLine, xaxis, yaxis;
      std::vector<Expression> newline;

      newline = {midLeft, topLeft};
      Expression leftLine1 = Expression(Atom("make-line"), newline);

      newline = {midRight, botRight};
      Expression rightLine1 = Expression(Atom("make-line"), newline);

      newline = {botLeft, topLeft};
      leftLine = Expression(Atom("make-line"), newline);

      newline = {botMid, topMid};
      yaxis = Expression(Atom("make-line"), newline);

      newline = {botRight, topRight};
      rightLine = Expression(Atom("make-line"), newline);

      newline = {topLeft, topRight};
      topLine = Expression(Atom("make-line"), newline);

      newline = {midLeft, midRight};
      xaxis = Expression(Atom("make-line"), newline);

      newline = {botLeft, botRight};
      botLine = Expression(Atom("make-line"), newline);

      // Add all points and items to results vector
      result.push_back(topLeft.eval(env));    //1
      result.push_back(leftLine1.eval(env));  //2
      result.push_back(botRight.eval(env));   //3
      result.push_back(rightLine1.eval(env)); //4
      result.push_back(yaxis.eval(env));      //5
      result.push_back(xaxis.eval(env));      //6
      result.push_back(topLine.eval(env));    //7
      result.push_back(botLine.eval(env));    //8
      result.push_back(leftLine.eval(env));   //9
      result.push_back(rightLine.eval(env));  //10

      // Add each original point x and y value to the result individually
      std::string temp; Expression point_as_string;
      for(auto & point : DATA.m_tail){
        for(auto & value : point.m_tail){
          temp = value.head().asString();
          temp = "\"" + temp + "\"";
          point_as_string = Expression(Atom(temp));
          result.push_back(point_as_string);
        }
      }

      // Add each option to the output
      for(auto &opt : OPTIONS.m_tail){
        result.push_back(opt.m_tail[1]);
      }

      return Expression(Expression(result), "discrete-plot");

    }
    else {
      std::cout << "mtail0 " << DATA << "\n";
      std::cout << "mtail1 " << m_tail[1] << "\n";
      throw SemanticError("Error: An argument to discrete-plot is not a list");
    }
  }
  else {
    throw SemanticError("Error: invalid number of arguments for discrete-plot");
  }
}

// this is a simple recursive version. the iterative version is more
// difficult with the ast data structure used (no parent pointer).
// this limits the practical depth of our AST
Expression Expression::eval(Environment & env){

  if(m_tail.empty()){
    if (m_head.isSymbol() && m_head.asSymbol() == "list") {
      std::vector<Expression> a;
		  return Expression(a);
	  }
    if(m_head.isString()){
      return Expression(m_head);
    }
    return handle_lookup(m_head, env);
  }
  else if(m_head.isSymbol() && m_head.asSymbol() == "begin"){
    return handle_begin(env);
  }
  else if(m_head.isSymbol() && m_head.asSymbol() == "define"){
    return handle_define(env);
  }
  else if(m_head.isSymbol() && m_head.asSymbol() == "lambda"){
    return handle_lambda();
  }
  else if(m_head.isSymbol() && m_head.asSymbol() == "apply"){
    return handle_apply(env);
  }
  else if(m_head.isSymbol() && m_head.asSymbol() == "map"){
    return handle_map(env);
  }
  else if(m_head.isSymbol() && m_head.asSymbol() == "set-property"){
    return handle_set_property(env);
  }
  else if(m_head.isSymbol() && m_head.asSymbol() == "get-property"){
    return handle_get_property(env);
  }
  else if(m_head.isSymbol() && m_head.asSymbol() == "discrete-plot"){
    return handle_discrete_plot(env);
  }
  else{
    std::vector<Expression> results;
    for(Expression::IteratorType it = m_tail.begin(); it != m_tail.end(); ++it){
      results.push_back(it->eval(env));
    }
    return apply(m_head, results, env);
  }
}

std::ostream & operator<<(std::ostream & out, const Expression & exp){

  if(exp.isEmpty()){
    out << "NONE";
    return out;
  }

  if(!exp.isHeadComplex()) {
    out << "(";
  }

  out << exp.head();

  if(exp.tailLength() > 0 && exp.isNone()){
      out << " ";
  }

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

std::string Expression::toString() const noexcept{

  std::ostringstream out;

  if(this->isEmpty()){
    out << "NONE";
  }
  else {
    if(!this->isHeadComplex()) {
      out << "(";
    }

    out << this->head().asString();

    if(this->tailLength() > 0 && this->isNone()){
        out << " ";
    }

    for(auto e = this->tailConstBegin(); e != this->tailConstEnd(); ++e){
      out << *e;
      if((e + 1) != this->tailConstEnd()){
        out << " ";
      }
    }
    if(!this->isHeadComplex()) {
      out << ")";
    }
  }

  return out.str();
}

bool Expression::isPoint() const noexcept{
  for(auto &p : m_properties){
    if(p.first == "\"object-name\""){
      return p.second == Expression(Atom("\"point\""));
    }
  }
  return false;
}

bool Expression::isLine() const noexcept{
  for(auto &p : m_properties){
    if(p.first == "\"object-name\""){
      return p.second == Expression(Atom("\"line\""));
    }
  }
  return false;
}

bool Expression::isText() const noexcept{
  const std::string target_property("\"object-name\"");
  for(auto &p : m_properties){
    if(p.first == target_property){
      return p.second == Expression(Atom("\"text\""));
    }
  }
  return false;
}

std::tuple<double, double, double, double, bool> Expression::getTextProperties() const noexcept{
  double sf = 1;
  if(m_properties.find("\"text-scale\"") != m_properties.end()) {
    sf = m_properties.at("\"text-scale\"").head().asNumber();
    if(sf < 1)
      sf = 1;
  }

  double rot = 0;
  if(m_properties.find("\"text-rotation\"") != m_properties.end()) {
    rot = m_properties.at("\"text-rotation\"").head().asNumber();
  }

  if(m_properties.find("\"position\"") != m_properties.end()){
    Expression point = m_properties.at("\"position\"");
    double x = point.getPointCoordinates().first;
    double y = point.getPointCoordinates().second;
    return {x, y, sf, rot, true};
  }
  // Error case
  return {0, 0, 1, 0, false};
}

std::vector<Expression> Expression::asVector() const noexcept {
    std::vector<Expression> result;
    if(m_head.asString() != ""){
      result.emplace_back(m_head);
    }
    for(auto e : m_tail){
      result.emplace_back(e);
    }
    return result;
}

double Expression::getNumericalProperty(std::string prop) const noexcept {
  double size_value = -1;
  if(m_properties.find(prop) != m_properties.end()){
    Expression point_size = m_properties.at(prop);
    size_value = point_size.head().asNumber();
  }
  return size_value;
}

std::pair<double, double> Expression::getPointCoordinates() const noexcept {
  double x, y;
  std::string repl = this->toString();

  repl = repl.substr(2, repl.length() - 3);
  std::string xcor = repl.substr(0, repl.find_first_of(')')), ycor = repl.substr(repl.find_first_of('('), repl.find_last_of(')')).substr(1);
  ycor.pop_back();

  if(xcor.find('.')!=std::string::npos){
      x = std::stod(xcor);
  }
  else {
      x = (double)std::stoi(xcor);
  }

  if(ycor.find('.')!=std::string::npos){
      y = std::stod(ycor);
  }
  else {
      y = (double)std::stoi(ycor);
  }
  std::pair<double, double> result = {x, y};
  return result;
}