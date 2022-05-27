#include "token.h"

Token::Token(TType type) : m_type(type)
{}

Token::Token(const std::string& str) : m_value(str), m_type(STRING)
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

TokenSequence tokenize(std::istream& seq) 
{
	TokenSequence tokens;
	std::string token;

	while (true)
	{
		char c = seq.get();
		if (seq.eof())
			break;

		if (c == COMMENTCHAR) {
			// chomp until the end of the line
			while ((!seq.eof()) && (c != '\n')) {
				c = seq.get();
			}
			if (seq.eof()) 
				break;
		}

		if (c == OPENCHAR) {
			save_token(token, tokens);
			tokens.push_back(Token::TType::OPEN);
		}
		else if (c == CLOSECHAR) {
			save_token(token, tokens);
			tokens.push_back(Token::TType::CLOSE);
		}
		else if (c == QUOTECHAR) {
			token.push_back(QUOTECHAR);
			c = seq.get();
			while (!seq.eof() && c != QUOTECHAR) {
				token.push_back(c);
				c = seq.get();
			}
			if (seq.eof()) {
				break;
			}
			token.push_back(QUOTECHAR);
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