#pragma once

#include "expression.h"
#include "token.h"

Expression parse(const TokenSequence& tokens) noexcept;
