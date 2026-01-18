#include <gtest/gtest.h>
#include "LacEngine.hpp"

using namespace Lac;

class DetTest : public ::testing::Test {
protected:
    LacEngine engine;
};

// 1. 业务逻辑测试
TEST_F(DetTest, CalculationLogic) {
    // 3x3 单位矩阵
    LacMatrix m3(3, 3);
    m3(1, 1) = 1.0; m3(2, 2) = 1.0; m3(3, 3) = 1.0;
    EXPECT_DOUBLE_EQ(engine.det(m3), 1.0);

    // 2x2 奇异矩阵
    LacMatrix m2(2, 2);
    m2(1, 1) = 1.0; m2(1, 2) = 2.0;
    m2(2, 1) = 2.0; m2(2, 2) = 4.0;
    EXPECT_NEAR(engine.det(m2), 0.0, 1e-15);
}

// 2. 异常校验：严格匹配你的 LacException 成员定义
TEST_F(DetTest, NonSquareMatrixThrowsException) {
    int testRows = 2;
    int testCols = 3;
    LacMatrix rect(testRows, testCols);

    try {
        engine.det(rect);
        FAIL() << "Should throw LacDimensionException";
    } catch (const LacDimensionException& e) {
        // A. 直接访问 public 成员 error_code
        // 根据 header.md，非方阵应对应 SQUARE_DIMENSION_DISMATCH (1004)
        EXPECT_EQ(e.error_code, LacErrorCode::SQUARE_DIMENSION_DISMATCH);

        // B. 直接访问 public 数组 i_args
        // 根据 header.md 的注释：SQUARE_DIMENSION_DISMATCH 的 i_arg = {r, c}
        EXPECT_EQ(e.i_args[0], testRows);
        EXPECT_EQ(e.i_args[1], testCols);

        // C. 校验 what() 格式：LAC-[code]:msg
        std::string whatMsg = e.what();
        std::string expectedPrefix = "LAC-1005:";
        EXPECT_EQ(whatMsg.substr(0, expectedPrefix.length()), expectedPrefix);

        // D. 校验 toString() 的输出
        // 你的 LacException.cpp 中 SQUARE_DIMENSION_DISMATCH 的 toString 实现应包含维度
        std::string detail = e.toString();
        EXPECT_NE(detail.find("2"), std::string::npos);
        EXPECT_NE(detail.find("3"), std::string::npos);

    } catch (...) {
        FAIL() << "Caught unexpected exception type";
    }
}