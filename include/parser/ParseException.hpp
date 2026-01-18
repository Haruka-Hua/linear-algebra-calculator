#pragma once
#include <stdexcept>
#include <string>

namespace Lac{

enum class ParseErrorCode{
    // Lexical Error 4xxx;
    INVALID_CHARACTER = 1001,
    MALFORMED_NUMBER = 1002,

    // Syntax Error 5xxx;


    // Calculation Error 6xxx;

    // System Error 7xxx;
};

class ParseException: public std::runtime_error{
public:
    ParseException(const std::string& msg):std::runtime_error(msg){}
};
} // namespace Lac