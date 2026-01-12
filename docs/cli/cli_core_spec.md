# Core Design Specification: Linear Algebra Calculator (LAC)

**版本：** v0.1-Alpha (MVP Stage)
**目标：** 实现 C++ 核心库，为命令行工具（CLI）和未来的移动端（Flutter）提供稳定的计算逻辑。

---

## 1. 核心架构：Data-Logic Separation
系统采用严格的数据与逻辑分离模式，利用 [Eigen 3.4+](https://eigen.tuxfamily.org/) 作为底层数值内核。

### 1.1 `LacMatrix` (数据载体)
- **职责**：封装 Eigen 矩阵对象，存储数据名、维度信息，并提供对流式操作和访问器的重载。
- **内存模型**：深拷贝语义。数据发生变化时不进行昂贵的缓存策略，直接按需重新计算。

### 1.2 `Engine` (逻辑中枢)
- **职责**：不拥有任何状态。所有计算方法均为对 `LacMatrix` 的无状态操作。
- **输入输出**：接受 `(const LacMatrix&)`，返回新的 `LacMatrix` 或数值结果。

---

## 2. LacMatrix 对象规范

### 内部实现
- 容器：使用 `Eigen::MatrixXd`（双精度浮点型动态矩阵）。
- 属性：`std::string name` (标识符)。

### 操作符契约 (Minimal Set)
| 语法 | 说明 | 目的 |
| :--- | :--- | :--- |
| `double& operator()(int r, int c)` | 双偏置访问器 | 实现直接对矩阵任意格的读和写。 |
| `os << matrix` | 流输出流重载 | 实现控制台自动对齐排版显示。 |
| `is >> matrix` | 流输入流重载 | 实现高效批量填充数据（按行列顺序）。 |

---

## 3. Engine 计算清单 (MVP 范围)

`Engine` 类将导出以下静态函数库：

### A. 算术与变换 (Arithmetic)
- `add(A, B)`, `sub(A, B)`, `mul(A, B)` (矩阵运算)
- `scale(A, factor)` (数乘)
- `transpose(A)` (转置)
- `power(A,p)`（幂运算）

### B. 常规分析 (Basic Analysis)
- `determinant(A)`: 行列式
- `rank(A)`: 矩阵的秩
- `trace(A)`: 矩阵的迹
- `rref(A)`: 行最简阶梯形（利用高斯消元）

### C. 特征与解算 (Linear Systems)
- `inverse(A)`: 逆矩阵
- `solve(A, B)`: 精确解方程组 $Ax = B$
- `solveLeastSquares(A, B)` (预留): 扫描无常情况下的最小二乘解

---

## 4. 后续扩展 (Stage 2)
1. **表达式解析 (Expression Parser)**: 允许用户直接输入字符公式（如 `(A+B)*C`）。
2. **Flutter FFI**: 将 C 风格指针接口暴露，支持双平台通信。
3. **缓存系统**: 对特大矩阵计算耗时项目进行元数据标记缓存。

---

## 5. 开发约束 (Development Constraints)
- **Error Handling**: 在这一阶段，若遇到奇异阵取逆或维度不匹配，统一打印错误信息或抛出显式的 `std::runtime_error`。
- **Precision**: 默认内部采用 `double` 计算以满足大部分通用场合。