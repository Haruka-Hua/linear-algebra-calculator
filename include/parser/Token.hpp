#pragma once
#include <string>
#include <variant>
#include "math/LacMatrix.hpp"

namespace Lac{

enum class TokenType{
    IDENTIFIER, 
    NUMBER, 
    OPERATOR, 
    L_BRACKET, 
    R_BRACKET, 
    L_PAREN, 
    R_PAREN, 
    SEMICOLON,
    COMMA,
    MATRIX,
    FUNCTION
};

enum class OperatorType{
    ADD,
    SUBTRACT,
    MULTIPLY,
    DIVIDE,
    ASSIGN,
    TRANSPOSE,
    POWER
};

enum class FunctionType{
    MATRIX,
    TRANSPOSE,
    TRACE,
    RANK,
    DETERMINANT,
    INVERSE,
    ADJOINT,
    RREF,
    POWER,
    SOLVE,
    SOLVELSQ,
    EIGEN
};

struct Token {
    TokenType token_type_;
    std::string text_;
    std::variant<
        std::monostate,
        std::string, 
        FunctionType, 
        OperatorType, 
        double, 
        LacMatrix
    > content_;

    //constructors
    //for brackets, parens, commas, semicolons;
    Token(TokenType token_type, std::string text): 
        token_type_(token_type),text_(text),content_(std::monostate{}){}
    //for identifiers;
    Token(TokenType token_type, std::string text, FunctionType function):
        token_type_(token_type),text_(text),content_(function){}
    //for numbers;
    Token(TokenType token_type, std::string text, double value):
        token_type_(token_type),text_(text),content_(value){}
    //for operators;
    Token(TokenType token_type, std::string text, OperatorType _operator):
        token_type_(token_type),text_(text),content_(_operator){}
    //for matrices;
    Token(TokenType token_type, std::string text, const LacMatrix& matrix):
        token_type_(token_type),text_(text),content_(matrix){}

    //visit detail
    //for identifiers
    const FunctionType func() const {
        return std::get<FunctionType>(content_);
    }
    //for numbers
    const double value() const {
        return std::get<double>(content_);
    }
    //for operators
    const OperatorType op() const {
        return std::get<OperatorType>(content_);
    }
    //for matrices;
    const LacMatrix& matrix() const {
        return std::get<LacMatrix>(content_);
    }
};
} // namespace Lac