# include "math/LacMatrix.hpp"
# include <algorithm>

namespace Lac{

LacMatrix::LacMatrix(std::string name):name_(std::move(name)),data_(){}

LacMatrix::LacMatrix(int rows, int cols, 
    std::string name):name_(std::move(name)){
        if (rows<0 || cols<0)
            throw LacDimensionException(LacErrorCode::BAD_ALLOCATION,
            "Trying to allocate a matrix with a invalid size.",
            {rows,cols});
        data_ = Eigen::MatrixXd(rows,cols);
        data_.setZero();
}

LacMatrix::LacMatrix(Eigen::MatrixXd other, 
    std::string name):data_(std::move(other)),name_(std::move(name)){}

LacMatrix::LacMatrix(std::initializer_list<std::initializer_list<double>> list, 
    std::string name):name_(std::move(name)){
    int rows = list.size();
    if(rows==0){
        data_.resize(0,0);
        return;
    }
    
    int cols = 0;
    for(auto& col : list){
        cols = std::max(cols,static_cast<int>(col.size()));
    }
    data_.resize(rows,cols);
    data_.setZero();
    int r = 0;
    for(auto& row : list){
        int c = 0;
        for(auto& val : row){
            data_(r,c) = val;
            c++;
        }
        r++;
    }
}

const void LacMatrix::visitCheck(int r, int c) const{
    if(r > data_.rows() || r < 1 || c > data_.cols() || c < 1)
        throw LacDimensionException(LacErrorCode::INDEX_OUT_OF_RANGE,
            "Trying to access element out of range.",{r,c,static_cast<int>(data_.rows()),static_cast<int>(data_.cols())});
}

double& LacMatrix::operator()(int r, int c){
    // visit matrix starting from index 1
    visitCheck(r,c);
    return data_(r-1,c-1);
}

const double& LacMatrix::operator()(int r, int c) const{
    visitCheck(r,c);
    return data_(r-1,c-1);
}

std::ostream& operator << (std::ostream &os, const LacMatrix& mat){
    if (!mat.name_.empty()) {
        os << "Matrix '" << mat.name_ << "' (" << 
            mat.rows() << "x" << mat.cols() << "):\n";
    }
    os << mat.data_;
    return os;
}
} //namespace Lac