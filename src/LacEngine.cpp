#include "LacEngine.hpp"
#include <Eigen/LU>

namespace Lac{
//Dimension 1: override global operators
void checkAddOperation(const LacMatrix& a, const LacMatrix& b, const std::string& op){
    if(a.rows()!=b.rows() || a.cols()!=b.cols())
        throw LacDimensionException(LacErrorCode::ADD_DIMENSION_DISMATCH,
        "Trying to " + op + " two matrices with different shapes.",
        a.rows(),a.cols(),b.rows(),b.cols());
}
void checkMultiplyOperation(const LacMatrix& a, const LacMatrix& b){
    if(a.cols()!=b.rows())
        throw LacDimensionException(LacErrorCode::MULTIPLY_DIMENSION_DISMATCH,
        "Trying to multiply two matrices with incompatible shapes.",
        a.rows(),a.cols(),b.rows(),b.cols());
}
LacMatrix operator +(const LacMatrix& a, const LacMatrix& b){
    checkAddOperation(a,b,"add");
    return LacMatrix(a.matrix() + b.matrix());
}
LacMatrix operator -(const LacMatrix& a, const LacMatrix& b){
    checkAddOperation(a,b,"subtract");
    return LacMatrix(a.matrix() - b.matrix());
}
LacMatrix operator *(const LacMatrix& a, const LacMatrix& b){
    checkMultiplyOperation(a,b);
    return LacMatrix(a.matrix() * b.matrix());
}
LacMatrix operator *(const LacMatrix& a, double k){
    return LacMatrix(a.matrix() * k);
}
LacMatrix operator *(double k, const LacMatrix& a){
    return LacMatrix(k * a.matrix());
}

//Dimension 2: operations and properties
void LacEngine::checkSquaredOperation(const LacMatrix& mat, const std::string& op){
    if(mat.rows()!=mat.cols())
        throw LacDimensionException(LacErrorCode::SQUARE_DIMENSION_DISMATCH,
        "Trying to find the " + op + " of a non-square matrix.",mat.rows(),mat.cols());
}
LacMatrix LacEngine::transpose(const LacMatrix& mat){
    return LacMatrix(mat.matrix().transpose());
}
double LacEngine::trace(const LacMatrix& mat){
    checkSquaredOperation(mat,"trace");
    return mat.matrix().trace();
}
int LacEngine::rank(const LacMatrix& mat){
    Eigen::FullPivLU<Eigen::MatrixXd> lu(mat.matrix());
    // lu.setThreshold(1e-9);
    return lu.rank();
}
double LacEngine::det(const LacMatrix& mat){
    checkSquaredOperation(mat,"determinant");
    Eigen::FullPivLU<Eigen::MatrixXd> lu(mat.matrix());
    return lu.determinant();
}
LacMatrix LacEngine::inverse(const LacMatrix& mat){
    checkSquaredOperation(mat,"inverse");
    Eigen::FullPivLU<Eigen::MatrixXd> lu(mat.matrix());
    return LacMatrix(lu.inverse());
}
LacMatrix LacEngine::adjoint(const LacMatrix& mat){
    checkSquaredOperation(mat,"adjoint");
    Eigen::FullPivLU<Eigen::MatrixXd> lu(mat.matrix());
    return LacMatrix(lu.adjoint());
}
LacMatrix LacEngine::rref(const LacMatrix& mat){
    Eigen::MatrixXd A = mat.matrix();
    int rows = A.rows();
    int cols = A.cols();
    double epsilon = 1e-9;
    int pivot_row = 0;
    for(int c = 0; (c<cols)&&(pivot_row<rows);c++){
        int max_row = pivot_row;
        double max_val = std::abs(A(pivot_row,c));
        for(int r = pivot_row+1; r < rows; r++){
            if(std::abs(A(r,c)) > max_val){
                max_val = std::abs(A(r,c));
                max_row = r;
            }
        }
        if(max_val < epsilon){
            for(int r = pivot_row; r < rows; r++){
                A(r,c) = 0.0;
                continue;
            }
        }
        if(max_row != pivot_row){
            A.row(pivot_row).swap(A.row(max_row));
        }
        double pivot_val = A(pivot_row,c);
        A.row(pivot_row) /= pivot_val;
        for(int r=pivot_row+1; r<rows; r++){
            double factor = A(r,c);
            A.row(r) -= A.row(pivot_row) * factor;
        }
        pivot_row++;
    }
    for(int r=0;r<rows;r++){
        for(int c=0;c<cols;c++){
            if(std::abs(A(r,c)) < epsilon){
                A(r,c) = 0.0;
            }
        }
    }
    return LacMatrix(A);
}
LacMatrix LacEngine::pow(const LacMatrix& mat, int exp){
    checkSquaredOperation(mat,"power");
    if(exp<0) 
        return pow(inverse(mat),-exp);
    if(exp==0) 
        return LacMatrix(Eigen::MatrixXd::Identity(mat.rows(),mat.cols()));
    else{
        Eigen::MatrixXd res = Eigen::MatrixXd::Identity(mat.rows(),mat.cols());
        Eigen::MatrixXd base = mat.matrix();
        while(exp>0){
            if(exp%2==1) res *= base;
            base *= base;
            exp /= 2;
        }
        return LacMatrix(res);
    }
}

//Dimension 3: utilities
LacMatrix LacEngine::solve(const LacMatrix& a, const LacMatrix& b){
    auto solver = a.matrix().fullPivLu();
    if(!solver.isInvertible()){
        //todo: handle special cases
        return solveLeastSquares(a,b);
    }
    Eigen::MatrixXd x = solver.solve(b.matrix());
    return LacMatrix(x);
}
LacMatrix LacEngine::solveLeastSquares(const LacMatrix& a, const LacMatrix& b){
    Eigen::MatrixXd x = a.matrix().colPivHouseholderQr().solve(b.matrix());
    return LacMatrix(x);
}
std::pair<LacMatrix,LacMatrix> LacEngine::eigenValuesSymmetric(const LacMatrix& mat){
    checkSquaredOperation(mat,"eigen values and vectors");
    Eigen::SelfAdjointEigenSolver<Eigen::MatrixXd> solver(mat.matrix());
    return {
        LacMatrix(solver.eigenvalues().asDiagonal()),
        LacMatrix(solver.eigenvectors())
    };
}

} //namespace Lac