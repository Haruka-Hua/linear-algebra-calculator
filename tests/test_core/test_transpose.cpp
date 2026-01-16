#include <gtest/gtest.h>
#include "LacEngine.hpp"

using namespace Lac;

TEST(MatrixLogicTest, TransposeRectangular) {
    // 1. 测试长方形矩阵 (2x3 -> 3x2)
    LacMatrix A({{1, 2, 3}, {4, 5, 6}}, "A");
    LacMatrix AT = LacEngine::transpose(A);

    EXPECT_EQ(AT.rows(), 3);
    EXPECT_EQ(AT.cols(), 2);
    
    // 检查交叉位置的值 (1-based index)
    EXPECT_DOUBLE_EQ(AT(1, 2), 4.0); // 原 A(2, 1)
    EXPECT_DOUBLE_EQ(AT(3, 1), 3.0); // 原 A(1, 3)
}

TEST(MatrixLogicTest, TransposeInvolutive) {
    // 2. 测试性质: (A^T)^T == A
    LacMatrix A({{1, 5}, {2, 6}, {3, 7}}); 
    LacMatrix AT = LacEngine::transpose(A);
    LacMatrix ATT = LacEngine::transpose(AT);

    for(int i=1; i<=A.rows(); ++i) {
        for(int j=1; j<=A.cols(); ++j) {
            EXPECT_DOUBLE_EQ(A(i, j), ATT(i, j));
        }
    }
}