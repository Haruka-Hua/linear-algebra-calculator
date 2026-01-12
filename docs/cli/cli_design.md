# CLI 设计规范 (Spec: CLI-V1)

本规范详细说明由 C++ 编写的 `LAC` (Linear Algebra Calculator) 命令行前端的分词、语法及交互准则。

## 1. 运行模式 (The REPL Loop)
计算器以一种循环等待（REPL）的模式运行。
*   **提示符**: 使用 `>>> ` 作为等待输入的提示。
*   **状态保持**: 所有定义的变量在整个会话（Session）中有效，直至程序进程退出。

## 2. 词法与语法描述 (Grammar & Syntax)

### 2.1 矩阵定义 (Matrix Definition)
使用类 MATLAB 风格语法：
- **符号**: 中括号 `[]` 包裹矩阵体。
- **列分隔**: 使用 `空格` 或 `,` 分隔同一行的元素。
- **行分隔**: 使用分号 `;` 指定换行。
- **示例**: `A = [1 2 3 ; 4, 5, 6]`

### 2.2 定义式 (Assignment)
- **命名规范**: 与 C++/Dart 一致，允许 `字母`、`数字`、`下划线`，必须以字母开头。
- **内建关键字 (Reserved)**:
    - `Ans`: 最新一次计算的最终结果。
    - `I`: 用于单位矩阵的操作（待定是否根据维度自动适配渲染）。
    ```bash
    >>> A = [1 0; 0 1]
    >>> B = A + [2 2; 2 2]
    >>> Ans
    [ 3 2 ; 2 3 ]
    ```

### 2.3 表达式解析 (Expressions)
支持递归嵌套的递归上升解析（Support Recursive Nesting using Shunting-yard or AST Parser）：
- **基元**: 数字、命名变量、矩阵字面量。
- **一元操作符**: `'` (转置)。
- **二元操作符**: `+`, `-`, `*` (自动根据对等性判断数乘或矩阵乘)。
- **优先级管理**: 允许使用圆括号 `()`。

## 3. 会话命令 (Administrative Commands)
非计算用途的指令，用于管理当前内存状态：
- `list` / `ls`: 打印当前 `std::map` 中存储的所有变量名、维度及概览。
- `del <variant_name>`: 释放指定矩阵占用的内存空间。
- `read <path> as <alias>`: 读取磁盘上的 CSV 数据并存入变量。
- `write <variant_name> to <path>`: 将矩阵变量序列化后持久化存储。
- `clear`: 清除屏幕输出。
- `exit` / `quit`: 关闭计算器。

## 4. 异常与回显 (Errors & Feedback)

### 4.1 输出控制
- **格式**: V1 阶段使用 Eigen 默认矩阵输出格式。
- **统计**: 输出结果后，额外提供一行提示物理运算细节（如有必要）。

### 4.2 错误反馈
采用 **Exception-Handling** 捕获各类逻辑错误并在 CLI 截断报错，严禁程序直接卡死（Panic）：
- **语法错误**: 如 `[1 2 3` 少了右括号。
- **线性错误**: `DIMENSION_MISMATCH`（加法/乘法不满足维度要求）、`SINGULAR_NOT_INVERTIBLE`（计算逆矩阵且行列式为0）。

## 5. 底层数据结构 (Backend Implementation)
- **容器**: `std::map<std::string, Eigen::MatrixXd>`。之所以选 `MatrixXd (Dynamic)`，是为了支持会话期间不同维数的重新赋值逻辑。
- **引擎**: 使用 Eigen 核心算法执行具体的 `+`, `-`, `*` 操作。