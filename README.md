# Linear Algebra Calculator

一个线性代数计算器，支持矩阵运算、方程求解、行列式计算等功能。

---

## 📘 用户指南

### 安装与运行

#### Windows 用户

**方式 1：直接运行可执行文件（推荐）**

1. 从发行版下载 `LAC-v1.0.0-Windows.zip`
2. 解压到任意文件夹
3. 双击 `lac_cli_app.exe` 即可运行

**方式 2：从源代码编译**

1. 安装 [CMake](https://cmake.org/download/)（3.14+）和 [MinGW](https://www.mingw-w64.org/) 或 [MSVC](https://visualstudio.microsoft.com/)
2. 在项目根目录运行：
   ```cmd
   build.bat
   ```
3. 编译完成后，可执行文件位于 `build\lac_cli_app.exe`

### 功能介绍

#### 计算功能

- **基础运算**：
  - 加减法 `+` `-`

    $$
    A + B
    $$

    $$
    A - B
    $$

    A 和 B 维度相同
  - 矩阵乘法 `*`

    $$
    A \times B
    $$

    A 的列数等于 B 的行数
  - 标量乘除法 `*` `/`

    $$
    c \times A
    $$

    $$
    A \times c
    $$

    $$
    \frac{A}{c}
    $$

    $c$ 为标量，做除法时$c \neq 0$
  - 矩阵转置 `'`

    $$
    A'
    $$
  - 矩阵幂运算 `^`

    $$
    A^n
    $$

    n 为整数，n<0时A必须可逆

> 在这个计算器中，所有的1x1矩阵都被视为标量。

- **矩阵构造**：使用方括号 `[` 和 `]`，分号 `;` 分隔行，空格分隔列
- **函数**：

  - `transpose()` - 矩阵转置
  - `trace()` - 计算迹

    $$
    \operatorname{trace}(A) = \sum_{i=1}^{n} a_{ii}
    $$

    其中 $A$ 为 $n \times n$ 方阵，$a_{ii}$ 为 $A$ 的第 $i$ 行第 $i$ 列元素
  - `rank()` - 计算秩

    $$
    \operatorname{rank}(A)
    $$
  - `det()` - 计算行列式

    $$
    |A|
    $$
  - `inverse()` - 矩阵求逆

    $$
    A^{-1}
    $$

    满足

    $$
    A \times A^{-1} = I
    $$

    其中 $|A| \neq 0$
  - `adjoint()` - 伴随矩阵

    $$
    A^{*}_{ij} = (-1)^{i+j} M_{ji}
    $$

    $$
    A \times A^{*} = |A| \times I
    $$
  - `rref()` - 行最简形式
  - `pow(matrix, n)` - 矩阵幂运算
  - `eigen()` - 特征值与特征向量（*目前只支持实对称矩阵*）
  - `solve(A, b)` - 线性方程组求解

    $$
    Ax = b
    $$

    $b$ 可以是一个矩阵（多组方程）
  - `solvelsq(A, b)` - 最小二乘解
- **表达式支持**：支持括号、嵌套函数调用与复合运算
- **变量存储**：支持命名矩阵存储与调用，自动保存计算结果到 `Ans`

#### 交互式命令

| 命令                   | 说明                 | 示例                           |
| ---------------------- | -------------------- | ------------------------------ |
| `help`               | 显示帮助信息         | `: help` 或 `: help <cmd>` |
| `memory`             | 查看所有存储的矩阵   | `: memory`                   |
| `lookup <name>`      | 查看特定矩阵的值     | `: lookup A`                 |
| `rename <old> <new>` | 重命名矩阵           | `: rename A B`               |
| `delete <name>`      | 删除矩阵             | `: delete A`                 |
| `clear`              | 清空所有存储         | `: clear`                    |
| `abort`              | 弃用当前输入的表达式 | `: abort`                    |
| `exit`               | 退出程序             | `: exit`                     |

### 使用手册

#### 基础计算

```
>>> 1 + ( 2 * 3 )
... :
Matrix 'Ans' (1x1):
7

>>> [ 1 2 3 ; 4 5 6 ] * 2
... :
Matrix 'Ans' (2x3):
 2  4  6
 8 10 12
```

#### 变量赋值与复合运算

```
>>> A = [ 1 2 3 ; 4 5 6 ]
... :
Matrix 'Ans' (2x3):
1 2 3
4 5 6

>>> A * Ans '
... :
Matrix 'Ans' (2x2):
14 32
32 77
```

#### 函数调用

```
>>> A = det ( [ 1 1 ; 4 5 ] )
:
Ans = 
1

>>> power ( inverse ( [ 1 0 ; 0 1 ] ) , 3 )
:
Ans = 
1 0
0 1
```

#### 命令操作

```
>>> : memory
Memory command invoked.
Matrix 'A' (2x3):
1 2 3
4 5 6
Matrix 'Ans' (2x2):
14 32
32 77

>>> : lookup A
Lookup command invoked.
Matrix 'A' (2x3):
1 2 3
4 5 6

>>> : rename A myMatrix
Rename command invoked.

>>> : delete myMatrix
Delete command invoked.

>>> : clear
Clear command invoked.

>>> : exit
Quiting Calculator. Goodbye!
```

#### 输入约定

1. **Token 分隔**：所有 token 必须用空格分隔

```
正确：[ 1 2 ; 3 4 ]
错误：[1 2;3 4]
```

2. **多行表达式**：输入多行后，单独输入一行 `:` 表示结束，可以在后面接入命令，使用`abort`命令可以弃用刚刚输入的表达式，否则会先求值再执行命令

```
>>> A = [
... 1 2 3 ;
... 4 5 6
... ]
... :

>>> 1 + 1
... : abort      # 使用abort，表达式被抛弃

>>> transpose_matrix = A '
... : memory     # 先计算表达式，后打印存储
```

3. **命令格式**：以冒号 `:` 开头

```
>>> : help
>>> : lookup <MatrixName>
>>> : rename <oldName> <newName>
```

---

## 👨‍💻 开发者指南

### 项目结构

```
linear-algebra-calculator/
├── include/                    # 头文件
│   ├── cli/                   # CLI 相关头文件
│   ├── math/                  # 数学库头文件
│   └── parser/                # 解析器头文件
├── src/                        # 源文件实现
│   ├── cli/                   # CLI 实现
│   ├── math/                  # 数学库实现
│   └── parser/                # 解析器实现
├── tests/                      # 单元测试
│   ├── test_math/
│   └── test_parser/
├── docs/                       # 文档
├── CMakeLists.txt             # CMake 构建配置
└── README.md                  # 本文件
```

### 技术栈

| 组件       | 技术               | 版本        |
| ---------- | ------------------ | ----------- |
| 编程语言   | C++                | C++17       |
| 线性代数库 | Eigen              | 3.4.0       |
| 构建工具   | CMake              | 3.14+       |
| 测试框架   | GoogleTest         | 1.12.1      |
| 编译器     | GCC / Clang / MSVC | 7+/5+/2017+ |

### 核心模块

#### 数学引擎 (lac_math)

- `LacMatrix`：矩阵数据结构
- `LacEngine`：线性代数运算（利用 Eigen 库）

#### 解析器 (lac_parser)

- `Lexer`：词法分析，token 生成
- `Parser`：语法分析，表达式评估，变量替换

#### CLI 交互 (lac_cli)

- `Cli`：交互式主循环
- 命令解析与执行
- 内存管理与显示

### 构建与测试

#### 构建项目

```bash
# 配置
cmake -S . -B build

# 编译
cmake --build build --config Release

# 运行
./build/bin/lac_cli_app
```

#### 运行测试

```bash
cd build
ctest

# 运行特定测试
ctest -R lac_math_tests
ctest -R lac_parser_tests
```

#### 构建选项

```bash
# 禁用测试构建
cmake -S . -B build -DBUILD_TESTS=OFF

# 调试版本
cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug
```

### 已知限制

- 单线程交互式程序，不支持并发计算
- 矩阵大小受内存限制
- 浮点数精度为 64 位双精度
- 错误提示显示功能暂不完整

### 版本信息

**v1.0.0** - 初始发行版本

- ✅ 基础矩阵运算支持
- ✅ 交互式 CLI 界面

### 后续更新计划（画饼中）

- 完善特征值与特征向量计算，支持非对称矩阵
- 优化错误提示与用户体验
- 桌面端与移动端 GUI 应用开发
- 导入与导出矩阵数据
- 增加更多线性代数功能
- 支持分数与符号计算

### 遇到问题了？

该项目为我学习编程的一个兴趣项目，目前由我一个人开发和测试，可能存在很多bug，如果在使用过程中发现问题，欢迎提交 Issue 或 Pull Request ，也可以直接发送反馈到我的邮箱：gtyl2006@163.com
