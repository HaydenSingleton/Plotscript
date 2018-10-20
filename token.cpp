#include "token.hpp"

// system includes
#include <cctype>
#include <iostream>

// define constants for special characters
const char OPENCHAR = '(';
const char CLOSECHAR = ')';
const char COMMENTCHAR = ';';
const char QUOTECHAR = '"';

Token::Token(TokenType t): m_type(t){}

Token::Token(const std::string & str): value(str) {
  if(str[0] != QUOTECHAR){
    m_type = STRING;
  }
  else
    m_type = QUOTE;
}

Token::TokenType Token::type() const{
  return m_type;
}


std::string Token::asString() const{
  switch(m_type){
    case OPEN:
      return "(";
    case CLOSE:
      return ")";
    case STRING:
      return value;
    case QUOTE:
      return value;
    }
    return "";
}


// add token to sequence unless it is empty, clears token
void store_ifnot_empty(std::string & token, TokenSequenceType & seq){
  if(!token.empty()){
    seq.emplace_back(token);
    token.clear();
  }
}

TokenSequenceType tokenize(std::istream & seq){
  TokenSequenceType tokens;
  std::string token;

  while(true){
    char c = seq.get();
    if(seq.eof()) break;

    if(c == COMMENTCHAR){
      // chomp until the end of the line
      while((!seq.eof()) && (c != '\n')){
	      c = seq.get();
      }
      if(seq.eof()) break;
    }
    else if(c == OPENCHAR){ // c == (
      store_ifnot_empty(token, tokens);
      tokens.push_back(Token::TokenType::OPEN);
    }
    else if(c == CLOSECHAR){ // c == )
      store_ifnot_empty(token, tokens);
      tokens.push_back(Token::TokenType::CLOSE);
    }
    else if(c == QUOTECHAR){ // c == " (BEGIN SPECIAL CASE)
      token.push_back(QUOTECHAR);
      c = seq.get();
      while(!seq.eof() && c != QUOTECHAR){
	      token.push_back(c);
        c = seq.get();
      }
      if(seq.eof()) break;
      token.push_back(QUOTECHAR); // push last ending quote
      store_ifnot_empty(token, tokens);
    }
    else if(isspace(c)){ // c == ' '
      store_ifnot_empty(token, tokens);
    }
    else{ // c == any other character
      token.push_back(c);
    }
  }
  store_ifnot_empty(token, tokens);

  return tokens;
}
