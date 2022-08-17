#pragma once

#include <istream>
#include <sstream>
#include <deque>

class Token {
public:
	enum TType
	{
		OPEN, CLOSE, STRING
	};

	explicit Token(TType type);
	~Token() = default;
	explicit Token(std::string);

	[[nodiscard]] TType type() const;
	[[nodiscard]] std::string toString() const;

private:
	TType m_type;
	std::string m_value;
};

using TokenSequence = std::deque<Token>;

TokenSequence tokenize(std::istream& s);

const char OPEN_CHAR = '(';
const char CLOSE_CHAR = ')';
const char COMMENT_CHAR = ';';
const char QUOTE_CHAR = '"';