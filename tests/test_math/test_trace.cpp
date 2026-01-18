#include <gtest/gtest.h>
#include "LacEngine.hpp"

using namespace Lac;

TEST(MatrixLogicTest, TraceNormal) {
    // 1. 标准方阵测试
    LacMatrix A({{1, 2}, {3, 4}});
    double tr = LacEngine::trace(A);
    EXPECT_DOUBLE_EQ(tr, 5.0); // 1 + 4
}

TEST(MatrixLogicTest, TraceNonSquareException) {
    // 2. 异常测试：非方阵求迹
    LacMatrix A(2, 3, "NonSquare");
    try {
        double tr = LacEngine::trace(A);
        FAIL() << "Should throw SQUARE_MATRIX_REQUIRED exception";
    } catch (const LacDimensionException& e) {
        // 假设 code 是从你的 LacErrorCode 中定义的
        // 这里需要验证 i_args 是否记录了 [2, 3]
        EXPECT_EQ(e.i_args[0], 2);
        EXPECT_EQ(e.i_args[1], 3);
    }
}