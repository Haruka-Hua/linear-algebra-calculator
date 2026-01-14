# include "LacMatrix.hpp"
# include <algorithm>

namespace Lac{

LacMatrix::LacMatrix(std::string name):name_(std::move(name)),data_(){}

LacMatrix::LacMatrix(int rows, int cols, 
    std::string name):data_(rows,cols),name_(std::move(name)){
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

double& LacMatrix::operator()(int r, int c){
    return data_(r,c);
}

const double& LacMatrix::operator()(int r, int c) const{
    return data_(r,c);
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