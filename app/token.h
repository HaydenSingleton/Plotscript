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

	Token(TType type);
	~Token() = default;
	Token(const std::string&);

	TType type() const;
	std::string toString() const;

private:
	TType m_type;
	std::string m_value;
};

using TokenSequence = std::deque<Token>;

TokenSequence tokenize(std::istream&);

const char OPENCHAR = '(';
const char CLOSECHAR = ')';
const char COMMENTCHAR = ';';
const char QUOTECHAR = '"';