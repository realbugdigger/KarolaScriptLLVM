#pragma once

#include <vector>

#include "Token.h"

inline std::vector<Token> tokens;

void initLexer(const char* source);
Token scanToken();
std::vector<Token> scanTokens();