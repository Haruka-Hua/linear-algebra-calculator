#include <LacException.hpp>

namespace Lac{
LacException::LacException(const LacErrorCode code, const std::string& msg):
    std::runtime_error("LAC-"+std::to_string((int)code)+":"+msg),error_code(code){}

std::string LacException::toString(){
    switch(error_code){
        case LacErrorCode::INDEX_OUT_OF_RANGE: 
            // 1-based
            return 
                "Trying to access matrix element out of range: ("
                + std::to_string(i_args[0]) + ", " + std::to_string(i_args[1]) + ")"
                + " in matrix of size "
                + std::to_string(i_args[2]) + "x" + std::to_string(i_args[3]);

        case LacErrorCode::ADD_DIMENSION_DISMATCH:
            return 
                "Trying to add two matrices with dismatched dimensions: "
                + std::to_string(i_args[0]) + "x" + std::to_string(i_args[1])
                + " and "
                + std::to_string(i_args[2]) + "x" + std::to_string(i_args[3]);

        case LacErrorCode::MULTIPLY_DIMENSION_DISMATCH:
            return 
                "Trying to multiply two matrices with dismatched dimensions: "
                + std::to_string(i_args[0]) + "x" + std::to_string(i_args[1])
                + " and "
                + std::to_string(i_args[2]) + "x" + std::to_string(i_args[3]);

        case LacErrorCode::SQUARE_DIMENSION_DISMATCH:
            return 
                "Trying to perform operation that requires square matrix on a non-square matrix: "
                + std::to_string(i_args[0]) + "x" + std::to_string(i_args[1]);

        case LacErrorCode::MATRIX_SINGULAR:
            return 
                "Matrix is singular, cannot perform operation. Determinant: "
                + std::to_string(d_args[0]);

        case LacErrorCode::NO_SOLUTION:
            return 
                "The system of equations has no solution. Try using least squares method.";

        case LacErrorCode::INFINETE_SOLUTION:
            return 
                "The system of equations has infinite solutions.";

        case LacErrorCode::INVALID_ARGUMENT:
            return 
                "Invalid argument provided to function.";

        case LacErrorCode::OUT_OF_MEMORY:
            return 
                "Memory allocation failed! Try again.";
        
        default:
            return
                "Oops, some unknown error occurred.";
    }
}
} // namespace Lac