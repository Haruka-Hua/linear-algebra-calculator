#include <gtest/gtest.h>
#include "LacEngine.hpp"

using namespace Lac;

/**
 * 专门针对 LacMatrix 容器本身的鲁棒性测试
 * 目标：确保在涉及到 Engine 计算之前，数据载体不会崩溃
 */

// --- 1. 维度边界测试 (Dimensional Extremes) ---

TEST(MatrixRobustness, DegenerateDimensions) {
    // 极端情况 A: 0阶矩阵
    // 虽无数学意义，但在动态解析中可能出现。确保构造函数不崩溃。
    EXPECT_NO_THROW({
        LacMatrix zeroMat(0, 0, "Zero");
        EXPECT_EQ(zeroMat.rows(), 0);
    });

    // 极端情况 B: 负数维度 (非法输入)
    // 你的构造函数内部应该对 rows < 0 或 cols < 0 抛出异常
    EXPECT_THROW({
        LacMatrix negMat(-1, 5);
    }, LacDimensionException);
}

// --- 2. 访问器越界测试 (Accessor Bounds - 1-Based) ---

TEST(MatrixRobustness, AccessorInvariants) {
    LacMatrix A(2, 2, "Test");

    // 极端情况 C: 零索引 (1-based 下的 0 是非法的)
    EXPECT_THROW(A(0, 1), LacDimensionException);
    EXPECT_THROW(A(1, 0), LacDimensionException);

    // 极端情况 D: 负数索引
    EXPECT_THROW(A(-1, -1), LacDimensionException);

    // 极端情况 E: 远超边界
    EXPECT_THROW(A(100, 2), LacDimensionException);
    
    // 极端情况 F: 对 0x0 矩阵尝试访问 (1,1)
    LacMatrix empty(0, 0);
    EXPECT_THROW(empty(1, 1), LacDimensionException);
}

// --- 3. 初始化列表安全性测试 (Initializer List Safety) ---

TEST(MatrixRobustness, InitializerListMismatch) {
    // 极端情况 G: 每一行的长度不一致
    // {{1, 2}, {3}} -> 第一行2个元素，第二行1个，空位置默认置为0
    LacMatrix A({{1.0, 2.0}, {3.0}});
    EXPECT_EQ(A(2,2),0.0);

    // 极端情况 H: 空列表构造
    EXPECT_NO_THROW({
        LacMatrix B({}); 
        EXPECT_EQ(B.rows(), 0);
    });
}

// --- 4. 内存与深拷贝坚固性 (Memory & Deep Copy) ---

TEST(MatrixRobustness, MemorySafety) {
    // 极端情况 I: 自赋值 (Self-assignment)
    LacMatrix A({{1, 2}, {3, 4}});
    EXPECT_NO_THROW({
        A = A; 
    });
    EXPECT_DOUBLE_EQ(A(1, 1), 1.0);

    // 极端情况 J: 作用域销毁
    LacMatrix* ptr = new LacMatrix(10, 10);
    int c = (*ptr).cols();
    int r = (*ptr).rows();
    LacMatrix B = *ptr; // 深拷贝
    c = B.cols();
    r = B.rows();
    delete ptr;         // 销毁原对象
    // B 应该依然能正常访问，不受外界销毁影响
    c = B.cols();
    r = B.rows();
    EXPECT_NO_THROW({
        B(1, 1) = 99.0;
        EXPECT_EQ(B.rows(), 10);
    });
}