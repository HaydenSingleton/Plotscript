package lexer

import (
	"testing"
	"plotscript/token"
)

func TestNextToken(t *testing.T) {
	input := `=+(){},;[]`

	tests := []struct {
		expectedType    token.TokenType
		expectedLiteral string
	}{
		{ token.EQUAL, "=" },
		{ token.PLUS, "+" },
		{ token.LPAREN, "(" },
		{ token.RPAREN, ")" },
		{ token.LBRACE, "{" },
		{ token.RBRACE, "}" },
		{ token.COMMA, "," },
		{ token.SEMICOLON, ";" },
		{ token.LBRACKET, "[" },
		{ token.RBRACKET, "]" },
		{ token.DOT, "."},
		{ token.IDENTIFIER, "cat"},
		{ token.IDENTIFIER, "dog"},
		{ token.INT, "42"},
		{ token.ILLEGAL, "#"},
		{ token.EOF, "" },
	}

	l := New(input)

	for i, tt := range tests {
		tok := l.getNextToken()

		if tok.Type != tt.expectedType {
			t.Fatalf("tests[%d] - tokentype wrong. expected=%q, got=%q",
				i, tt.expectedType, tok.Type)
		}

		if tok.Literal != tt.expectedLiteral {
			t.Fatalf("tests[%d] - literal wrong. expected=%q, got=%q",
				i, tt.expectedLiteral, tok.Literal)
		}
	}

}