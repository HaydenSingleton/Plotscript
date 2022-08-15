#include "token.h"

Token::Token(TType type) : m_type(type)
{}

Token::Token(std::string  str) : m_type(STRING), m_value(std::move(str))
{}

Token::TType Token::type() const
{
	return m_type;
}

std::string Token::toString() const
{
	switch (m_type) {
		case OPEN:
			return "(";
		case CLOSE:
			return ")";
		default:
			return m_value;
	}
}

void save_token(std::string& token, TokenSequence& seq) 
{
	if (!token.empty()) {
		seq.emplace_back(token);
		token.clear();
	}
}

TokenSequence tokenize(std::istream& seq) {
	TokenSequence tokens;
	std::string token;

	while (true)
	{
		char c = (char) seq.get();
		if (seq.eof())
			break;

		if (c == COMMENT_CHAR) {
			// chomp until the end of the line
			while ((!seq.eof()) && (c != '\n')) {
				c = (char) seq.get();
			}
			if (seq.eof()) 
				break;
		}

		if (c == OPEN_CHAR) {
			save_token(token, tokens);
			tokens.emplace_back(Token::TType::OPEN);
		}
		else if (c == CLOSE_CHAR) {
			save_token(token, tokens);
			tokens.emplace_back(Token::TType::CLOSE);
		}
		else if (c == QUOTE_CHAR) {
			token.push_back(QUOTE_CHAR);
			c = (char) seq.get();
			while (!seq.eof() && c != QUOTE_CHAR) {
				token.push_back(c);
				c = (char) seq.get();
			}
			if (seq.eof()) {
				break;
			}
			token.push_back(QUOTE_CHAR);
			save_token(token, tokens);
		}
		else if (isspace(c)) {
			save_token(token, tokens);
		}
		else {
			token.push_back(c);
		}

	}

	save_token(token, tokens);
	return tokens;
}