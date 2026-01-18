#include <gtest/gtest.h>
#include "LacEngine.hpp"
#include "LacMatrix.hpp"
#include "LacException.hpp"

using namespace Lac;

/**
 * @test 正常路径：计算 2x2 矩阵的逆
 * A = [1, 2; 3, 4], det = -2
 * A^-1 = [-2, 1; 1.5, -0.5]
 */
TEST(InverseTest, NormalInverseCalculation) {
    LacMatrix A({{1.0, 2.0}, {3.0, 4.0}});
    
    LacMatrix inv = LacEngine::inverse(A);
    
    // 验证维度
    EXPECT_EQ(inv.rows(), 2);
    EXPECT_EQ(inv.cols(), 2);
    
    // 验证数值 (使用 EXPECT_NEAR 处理浮点数精度)
    EXPECT_NEAR(inv(1, 1), -2.0, 1e-12);
    EXPECT_NEAR(inv(1, 2),  1.0, 1e-12);
    EXPECT_NEAR(inv(2, 1),  1.5, 1e-12);
    EXPECT_NEAR(inv(2, 2), -0.5, 1e-12);
}

/**
 * @test 验证单位矩阵的逆仍为单位矩阵
 */
TEST(InverseTest, IdentityMatrixInverse) {
    LacMatrix I({{1.0, 0.0}, {0.0, 1.0}});
    LacMatrix inv = LacEngine::inverse(I);
    
    EXPECT_DOUBLE_EQ(inv(1, 1), 1.0);
    EXPECT_DOUBLE_EQ(inv(2, 2), 1.0);
    EXPECT_DOUBLE_EQ(inv(1, 2), 0.0);
}

/**
 * @test 维度异常：尝试对非方阵求逆 (Error Code: 1005)
 */
TEST(InverseTest, Throws1005ForNonSquare) {
    LacMatrix rect(2, 3); // 2x3 矩阵
    
    try {
        LacEngine::inverse(rect);
        FAIL() << "Should throw LacDimensionException with code 1005";
    } catch (const LacDimensionException& e) {
        EXPECT_EQ((int)e.error_code, 1005);
        // 校验 i_args 是否存入了行和列
        EXPECT_EQ(e.i_args[0], 2);
        EXPECT_EQ(e.i_args[1], 3);
    }
}

/**
 * @test 数学异常：奇异矩阵不可逆 (Error Code: 2002)
 * A = [1, 2; 2, 4], det = 0
 */
TEST(InverseTest, Throws2002ForSingularMatrix) {
    LacMatrix singular({{1.0, 2.0}, {2.0, 4.0}});
    
    try {
        LacEngine::inverse(singular);
        FAIL() << "Should throw LacMathException with code 2002";
    } catch (const LacMathException& e) {
        // 校验你指正的 2002 错误码
        EXPECT_EQ((int)e.error_code, 2002);
        // 按照 spec，d_args[0] 应该存放计算出的行列式值（接近 0）
        EXPECT_NEAR(e.d_args[0], 0.0, 1e-12);
    }
}