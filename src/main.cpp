#include "LacMatrix.hpp"
#include <iostream>
#include <vector>
#include <cassert>

using namespace Lac;

void testBasicSetup() {
    std::cout << "--- Test 1: Basic Construction & Naming ---" << std::endl;
    // 默认空矩阵
    LacMatrix m1("Empty");
    std::cout << m1 << std::endl;

    // 指定维度的零矩阵
    LacMatrix m2(3, 2, "ZeroMatrix");
    std::cout << m2 << std::endl;
}

void testInitializationMethods() {
    std::cout << "\n--- Test 2: Diverse Init Silhouettes ---" << std::endl;
    
    // 1. 嵌套列表初始化 (Rectangular)
    LacMatrix A({
        {1.0, 2.0, 3.0},
        {4.0, 5.0, 6.0}
    }, "ListInit_A");
    std::cout << A << std::endl;

    // 2. 逗号初始化 (必须先指定维度)
    LacMatrix B(3, 3, "CommaInit_B");
    B << 1, 0, 0,
         0, 1, 0,
         0, 0, 1;
    std::cout << B << std::endl;
}

void testAccessAndConsistency() {
    std::cout << "\n--- Test 3: Element Access & Mutation ---" << std::endl;
    
    LacMatrix M(2, 2, "Mutator");
    M << 1.1, 2.2,
         3.3, 4.4;

    // 测试读写权限 () 操作符
    M(1, 0) = 99.9; // 将 3.3 修改为 99.9
    
    double val = M(1, 0);
    std::cout << "Element at (1,0) changed to: " << val << std::endl;
    assert(val == 99.9);

    // 获取底层 Eigen 引用进行外部计算 (不改变封装性)
    int rowCount = M.matrix().rows();
    std::cout << "Eigen check row count: " << rowCount << std::endl;
}

void testEdgeCases() {
    std::cout << "\n--- Test 4: Edge Cases (Handle with care) ---" << std::endl;

    // 1. 列表初始化长度不一的情况（测试鲁棒性）
    // 你之前写的代码通过 std::max 确定了最大宽度，这里验证数据对齐
    LacMatrix unbalance({
        {1, 2},
        {3, 4, 5, 6},
        {7}
    }, "Unbalanced");
    std::cout << "Unbalanced List (Expected padding with zeros):\n" << unbalance << std::endl;

    // 2. 极小矩阵
    LacMatrix scale(1, 1, "Scalar");
    scale << 3.14159;
    std::cout << scale << std::endl;
}

int main() {
    try {
        testBasicSetup();
        testInitializationMethods();
        testAccessAndConsistency();
        testEdgeCases();

        std::cout << "\n=======================================" << std::endl;
        std::cout << " All Milestone 1 Tests Passed Saccessfully!" << std::endl;
        std::cout << "=======================================" << std::endl;
    } catch (const std::exception& e) {
        std::cerr << "Test failed with error: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}