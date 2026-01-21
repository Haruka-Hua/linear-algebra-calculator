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
| `double& operator()(int r, int c)` | 双偏置访问器 | 实现直接对矩阵任意格的读和写，**访问下标从1开始，符合数学习惯**。 |
| `os << matrix` | 流输出流重载 | 实现控制台自动对齐排版显示。 |
| `matrix << content` | 逗号初始化 | 提供Eigen的逗号初始化接口。 |
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

## 4. 计算异常处理（Exception Handling）
### 计算时可能出现的异常
1. `operator +`和`operator -`：相加/减的两个矩阵必须形状相同，否则应抛出异常（维度异常）
2. `operator *(const LacMatrix& a, const LacMatrix& b)`：a的列数等于b的行数，否则应当抛出异常（维度异常）
3. `trace``det```inverse``adjoint``pow``eigenValueSymmetric`：输入的矩阵应当是方阵，否则应当抛出异常（维度异常）
4. `inverse``pow(exp<0)`时应当检查矩阵是否可逆，不可逆需要抛出异常（数学异常）
5. 当`solve`无解时，应当抛出异常，由调用者决定是否调用最小二乘解（数学异常）

### 异常系统设计
1. 原则
“计算/存储/展示”业务上进行分离，并支持后续与Flutter的对接，采用 结构化数据 + 分类分段代码
2. 异常分类
    - 基类：`LacException: std::runtime_error`
        - 维度类异常：`LacDimensionException`(code: 1xxx)，处理计算时矩阵维度不匹配的问题，以及访问的越界问题
        - 数学异常：`LacMathException`(code: 2xxx)，处理矩阵不可逆/方程无解导致的数学问题
        - 状态异常：`LacStateException`(code: 3xxx)，处理其它系统问题，如非法的传参、内存溢出
3. 数据载荷：每个异常类中携带错误的具体细节，存储在固定的槽位中
    - `ErrorCode(int)`：错误码，指定了具体的错误类型
    - `Int Parameters(int args[6])`：6个存放int参数的槽位，例如在维度异常中，可以使用前四个槽位表示两个矩阵的行列数
    - `Double Parameters(double values[2])`：2个存放double的槽位，用于存储一些精度信息
    - `Formatted Message(std::string)`：返回格式化的信息
    可能的格式：    `[LAC_ERR_CODE]: Developer context description. (Details: data1 vs data2)`

---