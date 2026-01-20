#include "parser/Lexer.hpp"
#include "parser/ParseException.hpp"
#include <sstream>

namespace Lac{

std::vector<Token> Lexer::tokenize(const std::string& input){
    std::string stoken;
    std::istringstream sinput(input);
    std::vector<Token> tokens;
    while(sinput >> stoken){
        Token token = convert(stoken);
        tokens.push_back(std::move(token));
    }
    return tokens;
}

Token Lexer::convert(const std::string& stoken){
    //operators: + - * / ' = ^
    if (stoken=="+") 
        return Token(TokenType::OPERATOR,stoken,OperatorType::ADD);
    if (stoken=="-")
        return Token(TokenType::OPERATOR,stoken,OperatorType::SUBTRACT);
    if (stoken=="*")
        return Token(TokenType::OPERATOR,stoken,OperatorType::MULTIPLY);
    if (stoken=="/")
        return Token(TokenType::OPERATOR,stoken,OperatorType::DIVIDE);
    if (stoken=="\'")
        return Token(TokenType::OPERATOR,stoken,OperatorType::TRANSPOSE);
    if (stoken=="=")
        return Token(TokenType::OPERATOR,stoken,OperatorType::ASSIGN);
    if (stoken=="^")
        return Token(TokenType::OPERATOR,stoken,OperatorType::POWER);
    //other symbols: ( ) [ ] ;
    if (stoken=="(")
        return Token(TokenType::L_PAREN,stoken);
    if (stoken==")")
        return Token(TokenType::R_PAREN,stoken);
    if (stoken=="[")
        return Token(TokenType::L_BRACKET,stoken);
    if (stoken=="]")
        return Token(TokenType::R_BRACKET,stoken);
    if (stoken==";")
        return Token(TokenType::SEMICOLON,stoken);
    if (stoken==",")
        return Token(TokenType::COMMA,stoken);
    //numbers
    char sbegin = stoken.at(0);
    if (isdigit(sbegin) || sbegin=='-' || sbegin=='+'){
        return convert_number(stoken);
    }
    //identifiers
    if (isalpha(sbegin) || sbegin=='_'){
        return convert_identifier(stoken);
    }
    throw ParseException("Invalid character: " + std::string(1,sbegin));
}

Token Lexer::convert_number(const std::string& stoken){
    // check if the number is valid
    char *endptr;
    double value = strtod(stoken.c_str(), &endptr);
    if (*endptr!='\0'){
        throw ParseException("Invalid character in number: " + std::string(1,*endptr));
    }
    return Token(TokenType::NUMBER,stoken,value);
}

Token Lexer::convert_identifier(const std::string& stoken){
    // reserved
    if (stoken=="transpose")
        return Token(TokenType::IDENTIFIER,stoken,FunctionType::TRANSPOSE);
    if (stoken=="trace")
        return Token(TokenType::IDENTIFIER,stoken,FunctionType::TRACE);
    if (stoken=="rank")
        return Token(TokenType::IDENTIFIER,stoken,FunctionType::RANK);
    if (stoken=="det")
        return Token(TokenType::IDENTIFIER,stoken,FunctionType::DETERMINANT);
    if (stoken=="inverse")
        return Token(TokenType::IDENTIFIER,stoken,FunctionType::INVERSE);
    if (stoken=="adjoint")
        return Token(TokenType::IDENTIFIER,stoken,FunctionType::ADJOINT);
    if (stoken=="rref")
        return Token(TokenType::IDENTIFIER,stoken,FunctionType::RREF);
    if (stoken=="pow")
        return Token(TokenType::IDENTIFIER,stoken,FunctionType::POWER);
    if (stoken=="solve")
        return Token(TokenType::IDENTIFIER,stoken,FunctionType::SOLVE);
    if (stoken=="solvelsq")
        return Token(TokenType::IDENTIFIER,stoken,FunctionType::SOLVELSQ);
    if (stoken=="eigen")
        return Token(TokenType::IDENTIFIER,stoken,FunctionType::EIGEN);
    // matrix names
    for(char c : stoken){
        if (!isalnum(c) && c!='_'){
            throw ParseException("Invalid character in identifier: " + std::string(1,c));
        }
    }
    return Token(TokenType::IDENTIFIER,stoken,FunctionType::MATRIX);
}
} // namespace Lac