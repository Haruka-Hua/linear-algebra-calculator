#include <gtest/gtest.h>
#include "LacEngine.hpp"
#include "LacMatrix.hpp"

using namespace Lac;

/**
 * @test 验证单位矩阵的 RREF 仍然是单位矩阵
 */
TEST(RrefTest, IdentityMatrixRemainsIdentity) {
    // 使用 header.md 中的 initializer_list 构造函数
    LacMatrix m({{1.0, 0.0, 0.0}, 
                 {0.0, 1.0, 0.0}, 
                 {0.0, 0.0, 1.0}});
    
    // LacEngine::rref 是 static 方法
    LacMatrix res = LacEngine::rref(m);

    EXPECT_EQ(res.rows(), 3);
    EXPECT_EQ(res.cols(), 3);
    // 1-indexed 访问
    EXPECT_DOUBLE_EQ(res(1, 1), 1.0);
    EXPECT_DOUBLE_EQ(res(2, 2), 1.0);
    EXPECT_DOUBLE_EQ(res(3, 3), 1.0);
    EXPECT_DOUBLE_EQ(res(1, 2), 0.0);
}

/**
 * @test 验证行线性相关矩阵的消去效果
 * 矩阵：
 * [1, 2, 3]
 * [4, 5, 6]
 * [5, 7, 9] <- 此行为前两行之和
 */
TEST(RrefTest, LinearDependentRows) {
    LacMatrix m({{1.0, 2.0, 3.0}, 
                 {4.0, 5.0, 6.0}, 
                 {5.0, 7.0, 9.0}});

    LacMatrix res = LacEngine::rref(m);

    // 预期结果：
    // [1, 0, -1]
    // [0, 1,  2]
    // [0, 0,  0]
    double x = res(1,1);
    EXPECT_NEAR(res(1, 1), 1.0, 1e-12);
    x = res(1,2);
    EXPECT_NEAR(res(1, 2), 0.0, 1e-12);
    x = res(1,3);
    EXPECT_NEAR(res(1, 3), -1.0, 1e-12);
    
    x = res(2,1);
    EXPECT_NEAR(res(2, 1), 0.0, 1e-12);
    x = res(2,2);
    EXPECT_NEAR(res(2, 2), 1.0, 1e-12);
    x = res(2,3);
    EXPECT_NEAR(res(2, 3), 2.0, 1e-12);

    // 第三行应全为 0
    EXPECT_NEAR(res(3, 1), 0.0, 1e-12);
    EXPECT_NEAR(res(3, 2), 0.0, 1e-12);
    EXPECT_NEAR(res(3, 3), 0.0, 1e-12);
}

/**
 * @test 验证宽矩阵（列多于行）的处理
 */
TEST(RrefTest, WideMatrix) {
    // 2x3 矩阵
    LacMatrix m({{0.0, 2.0, 4.0}, 
                 {0.0, 1.0, 3.0}});

    LacMatrix res = LacEngine::rref(m);

    // 计算过程：
    // R1 <-> R2 (交换)
    // [0, 1, 3]
    // [0, 2, 4]
    // R2 = R2 - 2*R1
    // [0, 1, 3]
    // [0, 0, -2]
    // R2 = R2 / -2
    // [0, 1, 3]
    // [0, 0, 1]
    // R1 = R1 - 3*R2
    // [0, 1, 0]
    // [0, 0, 1]
    
    EXPECT_EQ(res.rows(), 2);
    EXPECT_EQ(res.cols(), 3);
    EXPECT_NEAR(res(1, 1), 0.0, 1e-12);
    EXPECT_NEAR(res(1, 2), 1.0, 1e-12); // 第一个主元
    EXPECT_NEAR(res(1, 3), 0.0, 1e-12);
    
    EXPECT_NEAR(res(2, 1), 0.0, 1e-12);
    EXPECT_NEAR(res(2, 2), 0.0, 1e-12);
    EXPECT_NEAR(res(2, 3), 1.0, 1e-12); // 第二个主元
}

/**
 * @test 验证全零矩阵
 */
TEST(RrefTest, AllZeroMatrix) {
    LacMatrix m(2, 2); // 默认应为 0
    m(1,1)=0; m(1,2)=0; m(2,1)=0; m(2,2)=0;

    LacMatrix res = LacEngine::rref(m);

    EXPECT_DOUBLE_EQ(res(1, 1), 0.0);
    EXPECT_DOUBLE_EQ(res(2, 2), 0.0);
}