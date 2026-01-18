#include "math/LacEngine.hpp"
#include <iostream>
#include <vector>
#include <cassert>

using namespace Lac;

void testBasics() {
    Eigen::MatrixXd d1(2, 2);
    d1 << 1, 2, 3, 4;
    LacMatrix A(d1, "A");

    Eigen::MatrixXd d2(2, 2);
    d2 << 5, 6, 7, 8;
    LacMatrix B(d2, "B");

    LacEngine engine;

    auto res1 = A+B;
    res1.rename("A+B");
    auto res2 = A*B;
    res2.rename("A*B");
    std::cout << res1 << std::endl;
    std::cout << res2 << std::endl;
}

void testAnalysis() {
    Eigen::MatrixXd d(3, 3);
    d << 1, 2, 3, 
         0, 1, 4, 
         5, 6, 0;
    LacMatrix M(d, "M");

    LacEngine engine;

    std::cout << "Det: \n" << engine.det(M) << std::endl; // 预期: 1
    std::cout << "Rank: \n" << engine.rank(M) << std::endl;       // 预期: 3
    std::cout << "Inverse of M\n" << engine.inverse(M) << std::endl;               // 验证 inv 是否全为实数
}

void testSolve() {
    // 方程组: 
    // x + y = 3
    // x - y = 1  => (x=2, y=1)
    Eigen::MatrixXd m_a(2, 2);
    m_a << 1, 1, 1, -1;
    Eigen::MatrixXd m_b(2, 1);
    m_b << 3, 1;

    LacMatrix A(m_a);
    LacMatrix B(m_b);
    LacEngine engine;

    std::cout << "x solution\n" << engine.solve(A,B) << std::endl;
}

void testEigenSymmetric() {
    // 经典的 2x2 对称矩阵
    // [ 2  1 ]
    // [ 1  2 ]
    // 特征值应为 3 和 1
    Eigen::MatrixXd sym(2, 2);
    sym << 2, 1, 1, 2;
    LacMatrix A(sym);

    LacEngine engine;
    auto [values, vectors] = engine.eigenValuesSymmetric(A);

    std::cout << "Eigenvalues (Diagonal)\n" <<  values << std::endl;
    std::cout << "Eigenvectors (Columns)\n" << vectors << std::endl;
}

int main() {
    testBasics();
    testAnalysis();
    testEigenSymmetric();
    testSolve();
    return 0;
}