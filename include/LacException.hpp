#pragma once
#include <stdexcept>
#include <string>

namespace Lac{
enum class LacErrorCode{
    INDEX_OUT_OF_RANGE = 1001,
    ADD_DIMENSION_DISMATCH = 1002, 
    MULTIPLY_DIMENSION_DISMATCH = 1003,
    SQUARE_DIMENSION_DISMATCH = 1004,

    MATRIX_SINGULAR = 2001,
    NO_SOLUTION = 2002,
    INFINETE_SOLUTION = 2003,

    INVALID_ARGUMENT = 3001,
    OUT_OF_MEMORY = 3002,
    UNKNOWN_ERROR = 3999
};
class LacException : public std::runtime_error{
public:
    const LacErrorCode error_code;
    int i_args[6] = {0};
    double d_args[2] = {0.0};

    LacException(LacErrorCode code, const std::string& msg);
    std::string toString();
};
class LacDimensionException : public LacException{
public:
    LacDimensionException(LacErrorCode code, const std::string& msg,
    int rA, int cA, int rB=0, int cB=0):LacException(code,msg){
        i_args[0] = rA;
        i_args[1] = cA;
        i_args[2] = rB;
        i_args[3] = cB;
    }
};
class LacMathException : public LacException{
public:
    LacMathException(LacErrorCode code, const std::string& msg, double value = 0):LacException(code,msg){
        d_args[0] = value;
    }
};
class LacStateException : public LacException{
    using LacException::LacException;
};
} //namespace Lac