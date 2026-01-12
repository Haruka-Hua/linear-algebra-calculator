# Linear Algebra Calculator 重写计划
使用C++完全重写原仓库，直接使用Eigen的算法实现

## 核心功能
### 核心基础运算
计算器的根基，解决最直观的合并和简单变换
1. 矩阵加法、减法、乘法
2. 矩阵的数乘
3. 矩阵转置
4. 求迹
### 核心分析功能
1. 求行列式
2. 求行最简形(RREF)
3. 求逆/伴随矩阵
4. 求秩
### 高级线性系统处理
1. 求解线性方程组 $Ax = B$ 的精确解
2. 求解线性方程组 $Ax = B$ 的最小二乘解（如果无解）
3. 求特征值和特征向量 
<!-- ### 进阶分解与判定
TODO: 等我补学了相关知识以后再做 -->

## 预期产出

1. 先完全使用C++写一个命令行版本，确保基本功能的实现（存储、处理表达式、计算）
2. 利用flutter做一个图形界面，可以使用计算器按键进行交互，通过表格输入和输出矩阵
3. 实现网页版本
4. 实现移动端APP版本

## 技术栈

### core（核心计算层）
- **C++17/20**：高性能计算核心语言
- **Eigen 3.4+**：线性代数计算库
- **CMake**：跨平台构建系统
- **WebAssembly**：用于网页端编译目标

### 应用层（统一Flutter方案）
- **Flutter 3.0+**：跨平台UI框架
- **Dart 3.0+**：编程语言
- **平台支持**：
  - **桌面**：Windows, macOS, Linux
  - **网页**：Flutter Web（CanvasKit渲染器）
  - **移动**：iOS, Android

### 集成与构建
- **FFI（Foreign Function Interface）**：桌面端C++调用
- **dart:js** 和 **package:js**：Web端JavaScript互操作
- **WebAssembly**：C++核心的网页端运行环境
- **Emscripten**：C++到WebAssembly的编译工具链

### 开发工具
- **Visual Studio Code**：开发环境
- **Git**：版本控制
- **GitHub Actions**：持续集成
