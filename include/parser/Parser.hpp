#pragma once
#include "parser/Token.hpp"
#include <vector>
#include <variant>
#include <unordered_map>

namespace Lac{
class Parser{
public:
    Parser(std::unordered_map<std::string,LacMatrix>& memory):memory_(memory){}
    void substitute(std::vector<Token> &tokens);
    LacMatrix eval(const std::vector<Token>& tokens, int l, int r);
private:
    std::unordered_map<std::string, LacMatrix>& memory_;
    bool check_parentheses(const std::vector<Token>& tokens, int l, int r);
    int find_main_operator(const std::vector<Token>& tokens, int l, int r);
    int find_comma(const std::vector<Token>& tokens, int l, int r);
};
} //namespace Lac