#include <gtest/gtest.h>
#include "LacEngine.hpp"
#include "LacMatrix.hpp"

using namespace Lac;
// 命名空间：测试套件名, 测试用例名
TEST(RankTest, BasicMatrices) {
    LacEngine engine;

    // 1. 单位矩阵测试
    LacMatrix eye(3, 3);
    eye(1,1)=1; eye(2,2)=1; eye(3,3)=1;
    EXPECT_EQ(engine.rank(eye), 3);

    // 2. 零矩阵测试
    LacMatrix zero(4, 4);
    EXPECT_EQ(engine.rank(zero), 0);
}

TEST(RankTest, RectangularMatrices) {
    LacEngine engine;

    // 3. 长方形矩阵测试 (2x3)
    // [ 1 0 5 ]
    // [ 0 1 6 ] -> Rank: 2
    LacMatrix rect(2, 3);
    rect(1,1)=1; rect(1,3)=5; rect(2,2)=1; rect(2,3)=6;
    EXPECT_EQ(engine.rank(rect), 2);
}

TEST(RankTest, DependencyAndSingular) {
    LacEngine engine;

    // 4. 线性相关测试
    // [ 1 2 ]
    // [ 2 4 ] -> Rank: 1
    LacMatrix singular(2, 2);
    singular(1,1)=1; singular(1,2)=2;
    singular(2,1)=2; singular(2,2)=4;
    EXPECT_EQ(engine.rank(singular), 1);
}

// 对应 spec 中的 1-indexed 访问测试
TEST(RankTest, IndexingCheck) {
    LacEngine engine;
    LacMatrix m(3, 1); // 3行1列
    m(1,1) = 1.0;
    m(2,1) = 2.0;
    m(3,1) = 3.0;
    EXPECT_EQ(engine.rank(m), 1);
}