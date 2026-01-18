#include <gtest/gtest.h>
#include "LacEngine.hpp"

using namespace Lac;

// --- 深度测试：加法维度冲突 ---
TEST(ArithmeticDeepTest, AdditionDimensionInvariants) {
    LacMatrix A(3, 2, "A"); // 3x2
    LacMatrix B(3, 3, "B"); // 3x3 (行同，列不同)
    LacMatrix C(2, 2, "C"); // 2x2 (行不同，列不同)

    // 1. 测试列不匹配
    try {
        LacMatrix res = A + B;
        FAIL() << "应该抛出维度异常，但没有！";
    } catch (const LacDimensionException& e) {
        // 直接访问 public 的 i_args 数组，无需 .size()
        // 假设加法异常存入的是 {A.r, A.c, B.r, B.c}
        EXPECT_EQ(e.i_args[0], 3); // A_rows
        EXPECT_EQ(e.i_args[1], 2); // A_cols
        EXPECT_EQ(e.i_args[2], 3); // B_rows
        EXPECT_EQ(e.i_args[3], 3); // B_cols
    }

    // 2. 测试行不匹配
    try {
        LacMatrix res = A - C;
        FAIL() << "应该抛出维度异常，但没有！";
    } catch (const LacDimensionException& e) {
        EXPECT_EQ(e.i_args[0], 3); // A_rows
        EXPECT_EQ(e.i_args[2], 2); // C_rows
    }
}

// --- 深度测试：零矩阵与特殊维度 ---
TEST(ArithmeticDeepTest, ZeroMatrixBehavior) {
    LacMatrix Z(0, 0, "Empty");
    LacMatrix A(2, 2, "Normal");

    // A + Z 必须报错，不能因为 Z 是空的就跳过检查
    EXPECT_THROW({
        LacMatrix res = A + Z;
    }, LacDimensionException);
}

// --- 深度测试：连续运算的稳定性 ---
TEST(ArithmeticDeepTest, ChainedOperations) {
    LacMatrix A({{1, 1}, {1, 1}});
    LacMatrix B({{2, 2}, {2, 2}});
    LacMatrix C({{3, 3}, {3, 3}});

    // 测试 (A + B) - C 是否能正确处理中间临时变量的维度
    EXPECT_NO_THROW({
        LacMatrix res = A + B - C;
        EXPECT_DOUBLE_EQ(res(1, 1), 0.0);
        EXPECT_DOUBLE_EQ(res(2, 2), 0.0);
    });
}