#include <gtest/gtest.h>
#include "LacEngine.hpp"

using namespace Lac;

TEST(MultiplyDeepTest, DimensionMismatchStrictCheck) {
    // 准备数据：A(2x3), B(2x3) -> 矩阵乘法维度不匹配
    LacMatrix A(2, 3, "MatrixA");
    LacMatrix B(2, 3, "MatrixB");

    try {
        // 在 LacEngine 中调用 checkMulOperation 时，传入的 msg 应该是 "Multiply" 
        // 或者你代码中实际传递的字符串
        LacMatrix res = A * B; 
        FAIL() << "Did not throw LacDimensionException";
    } catch (const LacDimensionException& e) {
        // 1. 验证错误码
        EXPECT_EQ(e.error_code, LacErrorCode::MULTIPLY_DIMENSION_DISMATCH);

        // 2. 验证 i_args 全量原始数据 (依据：LHS_R, LHS_C, RHS_R, RHS_C)
        EXPECT_EQ(e.i_args[0], 2);
        EXPECT_EQ(e.i_args[1], 3);
        EXPECT_EQ(e.i_args[2], 2);
        EXPECT_EQ(e.i_args[3], 3);

        // 3. 验证 what()：必须符合 "LAC-[CODE]:[MSG]" 格式
        // 假设 MULTIPLY_DIMENSION_DISMATCH 的 code 值是 1006（根据上下文推测）
        // 假设你在 checkMulOperation 传入的 msg 是 "*"
        std::string expected_what = "LAC-1004: Trying to multiply two matrices with incompatible shapes."; 
        EXPECT_STREQ(e.what(), expected_what.c_str());

        // 4. 验证 toString()：必须与源码中的字符串拼接完全一致
        // 源码逻辑："Trying to multiply two matrices with dismatched dimensions: 2x3 and 2x3"
        std::string expected_toString = "Trying to multiply two matrices with dismatched dimensions: 2x3 and 2x3";
        EXPECT_EQ(e.toString(), expected_toString);
    }
}

TEST(MultiplyDeepTest, ScalarMultiplyStrictCheck) {
    LacMatrix A({{1.0, 2.0}, {3.0, 4.0}});
    
    // 验证标量乘法不破坏维度
    LacMatrix B = A * 2.0;
    EXPECT_EQ(B.rows(), 2);
    EXPECT_EQ(B.cols(), 2);
    EXPECT_DOUBLE_EQ(B(1, 1), 2.0);
    EXPECT_DOUBLE_EQ(B(2, 2), 8.0);
}

// --- 2. 乘法结果的正确性 (数值逻辑) ---
TEST(MultiplyDeepTest, CorrectnessAndOrder) {
    // A = [1 2]  (1x2)
    //     [3 4]  (不，我们做一个 1x2 乘以 2x1)
    LacMatrix A({{1.0, 2.0}});       // 1x2 
    LacMatrix B({{3.0}, {4.0}});     // 2x1

    // 1x2 * 2x1 = 1x1 -> [1*3 + 2*4] = [11]
    LacMatrix C = A * B;
    ASSERT_EQ(C.rows(), 1);
    ASSERT_EQ(C.cols(), 1);
    EXPECT_DOUBLE_EQ(C(1, 1), 11.0);

    // 2x1 * 1x2 = 2x2 -> 外积
    LacMatrix D = B * A;
    ASSERT_EQ(D.rows(), 2);
    ASSERT_EQ(D.cols(), 2);
    EXPECT_DOUBLE_EQ(D(1, 1), 3.0);  // 3*1
    EXPECT_DOUBLE_EQ(D(2, 2), 8.0);  // 4*2
}

// --- 3. 标量乘除法 (Scalar Operations) ---
TEST(MultiplyDeepTest, ScalarOperations) {
    LacMatrix A({{1.0, 2.0}, {3.0, 4.0}});
    
    // 测试 Matrix * double
    LacMatrix B = A * 2.0;
    EXPECT_DOUBLE_EQ(B(1, 1), 2.0);
    EXPECT_DOUBLE_EQ(B(2, 2), 8.0);

    // 测试 double * Matrix (如果实现了)
    LacMatrix C = 0.5 * A;
    EXPECT_DOUBLE_EQ(C(1, 1), 0.5);

    // 测试除以标量 (你之前重载的 operator/)
    LacMatrix D = A / 2.0;
    EXPECT_DOUBLE_EQ(D(1, 1), 0.5);
}

// --- 4. 边界异常：除以零 ---
TEST(MultiplyDeepTest, DivisionByZero) {
    LacMatrix A(2, 2);
    // 故意除以一个极小的数或 0
    EXPECT_THROW({
        LacMatrix res = A / 0.0;
    }, LacMathException); // 应该匹配你的 DIVISION_BY_ZERO
}