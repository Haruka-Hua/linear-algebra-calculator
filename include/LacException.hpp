#pragma once
#include <stdexcept>
#include <string>

namespace Lac{
enum class LacErrorCode{
    BAD_ALLOCATION = 1001,                  //i_arg = {alloc_r, alloc_c};
    INDEX_OUT_OF_RANGE = 1002,              //i_arg = {target_r, target_c, matrix_r, matrix_c};
    ADD_DIMENSION_DISMATCH = 1003,          //i_arg = {a_r, a_c, b_r, b_c};
    MULTIPLY_DIMENSION_DISMATCH = 1004,     //i_arg = {a_r, a_c, b_r, b_c};
    SQUARE_DIMENSION_DISMATCH = 1005,       //i_arg = {r, c};
    LHS_RHS_DISMATCH = 1006,                //i_arg = {lhs_r, lhs_c, rhs_r, rhs_c};

    DIVISION_BY_ZERO = 2001,    //d_arg = {dividor};
    MATRIX_SINGULAR = 2002,     //i_arg = {rank(A), matrix_r_c}; d_arg = {determinant};
    NO_SOLUTION = 2003,         //i_arg = {rank(A), rank(A,b)}
    INFINITE_SOLUTION = 2004,   //i_arg = {rank(A), a_c};

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
    const std::string toString() const;
};
class LacDimensionException : public LacException{
public:
    LacDimensionException(LacErrorCode code, const std::string& msg,
    std::initializer_list<int> int_args):LacException(code,msg){
        int i = 0;
        for(auto val : int_args){
            if(i<6) i_args[i++] = val;
        }
    }
};
class LacMathException : public LacException{
public:
    LacMathException(LacErrorCode code, const std::string& msg, 
        std::initializer_list<int> int_args, std::initializer_list<double> double_args):
        LacException(code,msg){
        int i = 0;
        for(auto val : int_args){
            if(i<6) i_args[i++] = val;
        }
        int d = 0;
        for(auto val : double_args){
            if(d<2) d_args[d++] = val;
        }
    }
};
class LacStateException : public LacException{
    using LacException::LacException;
};
} //namespace Lac