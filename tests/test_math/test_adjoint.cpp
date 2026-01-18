#include <gtest/gtest.h>
#include "LacEngine.hpp"

using namespace Lac;

TEST(MatrixLogicTest, Adjoint2x2) {
    // 1. 2x2 伴随矩阵验证
    // A = [1 2; 3 4] -> adj(A) = [4 -2; -3 1]
    LacMatrix A({{1.0, 2.0}, {3.0, 4.0}});
    LacMatrix Adj = LacEngine::adjoint(A);

    EXPECT_DOUBLE_EQ(Adj(1, 1), 4.0);
    EXPECT_DOUBLE_EQ(Adj(1, 2), -2.0);
    EXPECT_DOUBLE_EQ(Adj(2, 1), -3.0);
    EXPECT_DOUBLE_EQ(Adj(2, 2), 1.0);
}

TEST(MatrixLogicTest, AdjointProperty) {
    // 2. 性质验证：A * adj(A) = det(A) * I
    // 定义一个 3x3 矩阵
    LacMatrix A({{1, 0, 2}, {4, 5, 6}, {7, 8, 9}});
    double detA = LacEngine::det(A);
    
    LacMatrix AdjA = LacEngine::adjoint(A);
    LacMatrix LeftSide = A * AdjA;
    
    // 结果应该是对角线上全是 detA 的对角阵
    for(int i=1; i<=3; ++i) {
        for(int j=1; j<=3; ++j) {
            if(i == j) {
                EXPECT_NEAR(LeftSide(i, j), detA, 1e-9);
            } else {
                EXPECT_NEAR(LeftSide(i, j), 0.0, 1e-9);
            }
        }
    }
}

TEST(MatrixLogicTest, AdjointNonSquareException) {
    // 3. 异常测试
    LacMatrix A(1, 5); // 行向量
    EXPECT_THROW({
        LacEngine::adjoint(A);
    }, LacDimensionException);
}