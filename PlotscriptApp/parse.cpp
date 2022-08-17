#include "parse.h"
#include <stack>

Atom createAtom(const Token& t) {
    std::istringstream iss(t.toString());
    double number;
    Atom a;
    if (iss >> number) {
        if (iss.rdbuf()->in_avail() == 0)
            a = Atom(number);
    }
    else
        a = Atom(t.toString());
    return a;
}

Expression parse(const TokenSequence& tokens) noexcept
{
    Expression ast;
    bool at_head = false;

    std::stack<Expression*> stack;
    std::size_t num_tokens_seen = 0;

    for (auto& t : tokens) {

        if (t.type() == Token::OPEN) {
            at_head = true;
        }
        else if (t.type() == Token::CLOSE) {
            if (stack.empty()) {
                return {};
            }
            stack.pop();

            if (stack.empty()) {
                num_tokens_seen += 1;
                break;
            }
        }
        else {
            if (at_head) {
                if (stack.empty()) {
                    Atom next = createAtom(t);
                    if (next.isNone())
                        return {};
                    ast.setHead(next);
                    stack.push(&ast);
                }
                else {
                    if (stack.empty()) {
                        return {};
                    }
                    Atom next = createAtom(t);
                    if (next.isNone())
                        return {};
                    stack.top()->append(next);
                    stack.push(stack.top()->tail());
                }
                at_head = false;
            }
            else {
                if (stack.empty()) {
                    return {};
                }
                Atom next = createAtom(t);
                if (next.isNone())
                    return {};
                stack.top()->append(next);
            }
        }
        num_tokens_seen += 1;
    }

    if (stack.empty() && (num_tokens_seen == tokens.size())) {
        return ast;
    }

    return {};
}
