#include <gtest/gtest.h>
#include "LacEngine.hpp"
#include "LacMatrix.hpp"
#include "LacException.hpp"

using namespace Lac;

/**
 * @test solve: 唯一解情况
 * x + y = 3
 * 0x + y = 2
 * 解得 x=1, y=2
 */
TEST(SolveTest, UniqueSolution) {
    LacMatrix A({{1.0, 1.0}, {0.0, 1.0}});
    LacMatrix B({{3.0}, {2.0}});
    
    LacMatrix x = LacEngine::solve(A, B);
    
    EXPECT_NEAR(x(1, 1), 1.0, 1e-12);
    EXPECT_NEAR(x(2, 1), 2.0, 1e-12);
}

/**
 * @test solve: 无解情况 (NO_SOLUTION)
 * x + y = 1
 * x + y = 2
 * rank(A)=1, rank(A|B)=2
 */
TEST(SolveTest, ThrowsNoSolution) {
    LacMatrix A({{1.0, 1.0}, {1.0, 1.0}});
    LacMatrix B({{1.0}, {2.0}});

    try {
        LacEngine::solve(A, B);
        FAIL() << "Should have thrown NO_SOLUTION";
    } catch (const LacMathException& e) {
        EXPECT_EQ(e.error_code, LacErrorCode::NO_SOLUTION);
        // 校验 i_args: {rank_a, rank_augmentation}
        EXPECT_EQ(e.i_args[0], 1);
        EXPECT_EQ(e.i_args[1], 2);
    }
}

/**
 * @test solve: 无穷多解情况 (INFINITE_SOLUTION)
 * x + y = 2
 * 2x + 2y = 4
 * rank(A)=1, rank(A|B)=1, cols=2
 */
TEST(SolveTest, ThrowsInfiniteSolution) {
    LacMatrix A({{1.0, 1.0}, {2.0, 2.0}});
    LacMatrix B({{2.0}, {4.0}});

    try {
        LacEngine::solve(A, B);
        FAIL() << "Should have thrown INFINITE_SOLUTION";
    } catch (const LacMathException& e) {
        EXPECT_EQ(e.error_code, LacErrorCode::INFINITE_SOLUTION);
        // 校验 i_args: {rank_a, cols}
        EXPECT_EQ(e.i_args[0], 1);
        EXPECT_EQ(e.i_args[1], 2);
    }
}

/**
 * @test solveLeastSquares: 超定方程的最优解
 * 找到一个 x 使得 ||Ax - B|| 最小
 */
TEST(SolveTest, LeastSquaresCalculation) {
    // 这是一个简单的超定系统
    LacMatrix A({{1.0}, {1.0}});
    LacMatrix B({{1.0}, {2.0}});
    
    // 对于这个系统，最小二乘解应该是 (1+2)/2 = 1.5
    LacMatrix x = LacEngine::solveLeastSquares(A, B);
    
    EXPECT_NEAR(x(1, 1), 1.5, 1e-12);
}

/**
 * @test 维度检查: solve 矩阵维度不匹配
 * A(2x2) * x = B(3x1) -> 无法计算
 */
TEST(SolveTest, ThrowsDimensionMismatch) {
    LacMatrix A(2, 2);
    LacMatrix B(3, 1);

    // 假设 checkEquationDimension 抛出的是维度异常
    EXPECT_THROW(LacEngine::solve(A, B), LacDimensionException);
}

/**
 * @test solve: 多列 B 的情况 (AX = B)
 * A = [1 1; 0 1], B = [3 5; 2 4]
 * 相当于同时解：
 * [1 1; 0 1]x1 = [3; 2] -> x1 = [1; 2]
 * [1 1; 0 1]x2 = [5; 4] -> x2 = [1; 4]
 * 结果 X 应为 [1 1; 2 4]
 */
TEST(SolveTest, MultiColumnSolution) {
    LacMatrix A({{1.0, 1.0}, {0.0, 1.0}});
    LacMatrix B({{3.0, 5.0}, {2.0, 4.0}});
    
    LacMatrix X = LacEngine::solve(A, B);
    
    EXPECT_EQ(X.rows(), 2);
    EXPECT_EQ(X.cols(), 2);
    
    // 第一列
    EXPECT_NEAR(X(1, 1), 1.0, 1e-12);
    EXPECT_NEAR(X(2, 1), 2.0, 1e-12);
    // 第二列
    EXPECT_NEAR(X(1, 2), 1.0, 1e-12);
    EXPECT_NEAR(X(2, 2), 4.0, 1e-12);
}