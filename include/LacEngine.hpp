# pragma once
# include "LacMatrix.hpp"

namespace Lac{
//Dimension 1: override global operators
LacMatrix operator +(const LacMatrix& a, const LacMatrix& b);
LacMatrix operator -(const LacMatrix& a, const LacMatrix& b);
LacMatrix operator *(const LacMatrix& a, const LacMatrix& b);
LacMatrix operator *(const LacMatrix& a, double k);
LacMatrix operator *(double k, const LacMatrix& a);

class LacEngine{
public:
    //Dimension 2: operations and properties
    static LacMatrix transpose(const LacMatrix& mat);
    static double trace(const LacMatrix& mat);
    static int rank(const LacMatrix& mat);
    static double det(const LacMatrix& mat);
    static LacMatrix inverse(const LacMatrix& mat);
    static LacMatrix adjoint(const LacMatrix& mat);
    static LacMatrix rref(const LacMatrix& mat);
    static LacMatrix pow(const LacMatrix& mat, int exp);
    
    //Dimension 3: utilities
    static LacMatrix solve(const LacMatrix& a, const LacMatrix& b);
    static LacMatrix solveLeastSquares(const LacMatrix& a, const LacMatrix& b);
    static std::pair<LacMatrix,LacMatrix> eigenValuesSymmetric(const LacMatrix& a);
};
} //namespace Lac