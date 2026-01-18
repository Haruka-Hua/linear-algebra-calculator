#include <gtest/gtest.h>
#include "LacEngine.hpp"
#include "LacMatrix.hpp"
#include <utility> // for std::pair

using namespace Lac;

/**
 * @test eigenValuesSymmetric: 测试实对称矩阵
 * A = [2 1; 1 2]
 * 特征值 λ1 = 3, λ2 = 1
 * 特征向量应满足 A * V = V * D
 */
TEST(EigenTest, SymmetricMatrix) {
    LacMatrix A({{2.0, 1.0}, {1.0, 2.0}});
    
    // 调用 pair 接口
    auto result = LacEngine::eigenValuesSymmetric(A);
    LacMatrix D = result.first;  // 对角特征值矩阵
    LacMatrix V = result.second; // 特征向量矩阵
    
    // 1. 验证 D 是对角阵且特征值正确
    // Eigen 的 SelfAdjointEigenSolver 结果默认是升序的 [1, 3]
    EXPECT_NEAR(D(1, 1), 1.0, 1e-12);
    EXPECT_NEAR(D(2, 2), 3.0, 1e-12);
    EXPECT_NEAR(D(1, 2), 0.0, 1e-12);
    EXPECT_NEAR(D(2, 1), 0.0, 1e-12);
    
    // 2. 验证核心定义: A * V = V * D
    // 这是测试特征分解最稳健的方法
    LacMatrix left = A * V;
    LacMatrix right = V * D;
    
    for (int i = 1; i <= 2; ++i) {
        for (int j = 1; j <= 2; ++j) {
            EXPECT_NEAR(left(i, j), right(i, j), 1e-12);
        }
    }
}

/**
 * @test 验证正交性 (实对称矩阵的特征向量矩阵应为正交阵)
 * V^T * V = I
 */
TEST(EigenTest, EigenvectorOrthogonality) {
    LacMatrix A({{1.0, 0.5}, {0.5, 1.0}});
    auto result = LacEngine::eigenValuesSymmetric(A);
    LacMatrix V = result.second;
    
    // 计算 V 的转置 * V
    // 注意：假设你的 LacMatrix 还没有 transpose()，可以手动计算或等后续实现
    // 如果有了 transpose，可以这样：
    // LacMatrix Identity = V.transpose() * V;
    // 这里我们验证第一列和第二列的点积是否为 0
    double dotProduct = V(1, 1) * V(1, 2) + V(2, 1) * V(2, 2);
    EXPECT_NEAR(dotProduct, 0.0, 1e-12);
}

/**
 * @test 异常处理：非方阵
 */
TEST(EigenTest, NonSquareMatrixThrows) {
    LacMatrix A(2, 3);
    // 验证是否抛出 checkSquaredOperation 里的异常
    EXPECT_ANY_THROW(LacEngine::eigenValuesSymmetric(A));
}

/**
 * @test 异常测试: 传入非对称矩阵应抛出 3001
 */
TEST(EigenTest, NonSymmetricThrows) {
    // 这是一个方阵，但不对称 (1 != 3)
    LacMatrix A({{1.0, 2.0}, {3.0, 4.0}});
    
    try {
        LacEngine::eigenValuesSymmetric(A);
        FAIL() << "Should throw LacStateException for non-symmetric matrix";
    } catch (const LacStateException& e) {
        EXPECT_EQ((int)e.error_code, 3001);
    }
}