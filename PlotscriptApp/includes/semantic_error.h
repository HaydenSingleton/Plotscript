#pragma once

#include <exception>
#include <stdexcept>

class SemanticError : public std::runtime_error {
public:
	SemanticError(const std::string& msg) : runtime_error(msg) {};
};