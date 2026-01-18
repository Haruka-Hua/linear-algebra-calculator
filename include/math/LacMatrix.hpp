# pragma once
# include <Eigen/Dense>
# include "math/LacException.hpp"
namespace Lac{

class LacMatrix{
public:
    //constructors
    explicit LacMatrix(std::string name="");
    explicit LacMatrix(int rows, int cols, std::string name="");
    explicit LacMatrix(Eigen::MatrixXd other, std::string name="");
    explicit LacMatrix(std::initializer_list<std::initializer_list<double>> list, 
        std::string name = "");

    //get info
    int rows() const{ return data_.rows(); }
    int cols() const{ return data_.cols(); }

    //name handling
    const std::string& name() const{ return name_; }
    void rename(const std::string &name){ name_ = name; }

    //visitor
    const void visitCheck(int r, int c) const;
    double& operator()(int r, int c);
    const double& operator()(int r, int c) const;
    Eigen::CommaInitializer<Eigen::MatrixXd> operator<<(double val){
        return data_ << val;
    }
    const Eigen::MatrixXd& matrix() const { return data_; }
    Eigen::MatrixXd& matrix() { return data_; }

    //print
    friend std::ostream& operator << (std::ostream &os, const LacMatrix& mat);

private:
    Eigen::MatrixXd data_;
    std::string name_;
};

} // namespace Lac