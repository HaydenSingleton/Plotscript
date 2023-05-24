package token

type TokenType string

type Token struct {
	Type    TokenType
	Literal string
}

const (
	ILLEGAL = "ILLEGAL"
	EOF     = "EOF"

	// Variables
	IDENTIFIER = "IDENT"

	// Number literals
	INT = "INT"

	// Operators
	EQUAL     = "="
	PLUS      = "+"
	COMMA     = ","
	SEMICOLON = ";"
	DOT       = "."

	// Grouping
	LPAREN   = "("
	RPAREN   = ")"
	LBRACE   = "{"
	RBRACE   = "}"
	LBRACKET = "["
	RBRACKET = "]"
)

var keywords = map[string]TokenType{
	"func": ILLEGAL,
}

func lookupKeyword(ident string) TokenType {
	if tok, ok := keywords[ident]; ok {
		return tok
	}
	return IDENTIFIER
}
