#pragma once
#include <string>
#include <vector>
#include "parser/Token.hpp"

namespace Lac{

class Lexer{
public:
    static std::vector<Token> tokenize(const std::string& input);
private:
    static Token convert(const std::string& stoken);
    static Token convert_number(const std::string& stoken);
    static Token convert_identifier(const std::string& stream);
};
} //namespace Lac