#include <gtest/gtest.h>
#include "LacEngine.hpp"
#include "LacMatrix.hpp"
#include "LacException.hpp"

using namespace Lac;

/**
 * @test 正常路径：正整数幂 A^2
 */
TEST(PowerTest, PositiveIntegerPower) {
    // A = [1 2; 3 4]
    LacMatrix A({{1.0, 2.0}, {3.0, 4.0}});
    
    // A^2 = [7 10; 15 22]
    LacMatrix res = LacEngine::pow(A, 2);
    
    EXPECT_EQ(res.rows(), 2);
    EXPECT_EQ(res.cols(), 2);
    EXPECT_NEAR(res(1, 1), 7.0, 1e-12);
    EXPECT_NEAR(res(1, 2), 10.0, 1e-12);
    EXPECT_NEAR(res(2, 1), 15.0, 1e-12);
    EXPECT_NEAR(res(2, 2), 22.0, 1e-12);
}

/**
 * @test 边界路径：零次幂 A^0
 * 任何方阵的 0 次幂应返回单位矩阵 I
 */
TEST(PowerTest, ZeroPowerReturnsIdentity) {
    LacMatrix A({{5.0, 9.0}, {1.0, 2.0}});
    
    LacMatrix res = LacEngine::pow(A, 0);
    
    // 预期结果为单位阵 [[1, 0], [0, 1]]
    EXPECT_DOUBLE_EQ(res(1, 1), 1.0);
    EXPECT_DOUBLE_EQ(res(1, 2), 0.0);
    EXPECT_DOUBLE_EQ(res(2, 1), 0.0);
    EXPECT_DOUBLE_EQ(res(2, 2), 1.0);
}

/**
 * @test 维度异常：非方阵求幂 (Error Code: 1005)
 * 根据 spec，i_args 应包含 {rows, cols}
 */
TEST(PowerTest, Throws1005ForNonSquareMatrix) {
    LacMatrix rect(2, 3); // 2行3列

    try {
        LacEngine::pow(rect, 2);
        FAIL() << "Should have thrown LacDimensionException";
    } catch (const LacDimensionException& e) {
        // 验证错误码
        EXPECT_EQ((int)e.error_code, 1005);
        // 验证数据载荷 (i_args 前两个槽位)
        EXPECT_EQ(e.i_args[0], 2);
        EXPECT_EQ(e.i_args[1], 3);
    }
}

/**
 * @test 数学异常：奇异矩阵的负幂次 (Error Code: 2001)
 * A^-1 运算要求行列式不为 0
 */
TEST(PowerTest, Throws2001ForSingularNegativePower) {
    // [1 2; 2 4] 行列式为 0
    LacMatrix singular({{1.0, 2.0}, {2.0, 4.0}});

    try {
        LacEngine::pow(singular, -1);
        FAIL() << "Should have thrown LacMathException";
    } catch (const LacMathException& e) {
        // 验证错误码
        EXPECT_EQ(e.error_code, LacErrorCode::MATRIX_SINGULAR);
        // 验证数据载荷 (d_args[0] 存储行列式)
        EXPECT_NEAR(e.d_args[0], 0.0, 1e-9);
    }
}

/**
 * @test 综合路径：负整数幂 A^-2
 * 逻辑：(A^-1)^2
 */
TEST(PowerTest, NegativeIntegerPower) {
    // A = [2 0; 0 2]
    LacMatrix A({{2.0, 0.0}, {0.0, 2.0}});
    
    // A^-1 = [0.5 0; 0 0.5]
    // A^-2 = [0.25 0; 0 0.25]
    LacMatrix res = LacEngine::pow(A, -2);
    
    EXPECT_NEAR(res(1, 1), 0.25, 1e-12);
    EXPECT_NEAR(res(2, 2), 0.25, 1e-12);
}