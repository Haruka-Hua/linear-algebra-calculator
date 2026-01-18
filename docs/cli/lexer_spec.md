## 0. 输入约定
在命令行版本中，用户输入的所有token之间应该用空格分隔

## 1. Token（词法符号）定义清单

Lexer 识别出的所有 Token 必须属于以下类型之一：

| Token 类型 | 示例 | 说明 |
| :--- | :--- | :--- |
| **IDENTIFIER** | `mat_name`,`Ans`,`transpose`,<br>`trace`,`rank`,`det`,`inverse`,<br>`adjoint`,`rref`,`pow`,`solve`,<br>`solvelsq`,`eigen` | 除了矩阵名外全是保留字，要求矩阵名符合 C 命名规范：字母/下划线开头，支持字母数字下划线。 |
| **NUMBER** | `42`, `3.1415`, `1.2e-5` | **支持科学计数法**，包含整数和浮点数。 |
| **OPERATOR** | `+`,`-`,`*`,`/`,`=`,`'`，`^` | 基本四则运算、赋值、转置和幂运算。 |
| **L_BRACKET / R_BRACKET** | `[`, `]` | 用于定义矩阵字面量边界。 |
| **L_PAREN / R_PAREN** | `(`, `)` | 用于函数调用和控制计算优先级。 |
| **SEMICOLON** | `;` | 矩阵行分隔符。 |
| **MATRIX** | `[ 1 , 2 ; 3 , 4 ]` | 矩阵token，封装了一个匿名矩阵，便于parser进行替换 |

---

## 2. 核心匹配规则 (Scanning Rules)

### 2.1 标识符（矩阵名与保留字）
- **模式**：`[a-zA-Z_][a-zA-Z0-9_]*`
- **处理方式**：Lexer 不主动区分“变量名”和“内置函数名”，但是对于保留的函数名，Lexer会提前在token中记录函数类型（枚举内置函数/矩阵）。
  - *例如*：`det`, `transpose`, `inv` 会被标记为 `IDENTIFIER`。
  - *由 Parser 决定*：如果该标识符后跟着 `(`，则 Parser 将其视为函数（矩阵名也是函数，mat_name(r,c)可以访问矩阵元素），否则视为变量。

### 2.2 数字逻辑（预留科学计数法）
- **模式**：`[0-9]*\.?[0-9]+([eE][-+]?[0-9]+)?`
- **规则**：
  - 必须支持 `1.2`, `10`。
  - 为未来扩展考虑，Lexer 应能一次性识别 `1e-10` 这种格式，避免将其拆碎。

### 2.3 矩阵上下文逻辑 (The "Space" Rule)
这是本设计的关键。Lexer 需要维护一个内部状态位 `bool in_matrix_context`，当遇到 `[` 时设为 true，遇到 `]` 时设为 false。

- **在 `[]` 内部**：
  - **空格 (Space)**：被视为有效分割符，Lexer 应生成一个标记为 `COMMA` 的 Token，或在生成数字流时作为断点。
  - **分号 (Semicolon)**：被视为行切换符。
- **在 `[]` 外部**：
  - **空格**：直接跳过（Skip），不产生任何 Token。
  - **分号**：视为语句结束标志。

### 2.4 转置操作 (Transpose)
- **隐式转置**：单引号 `'` 必须紧跟在标识符、右括号 `)` 或右中括号 `]` 之后。
- **显式转置**：支持 `transpose ( A ) `，此时 `transpose` 识别为 `IDENTIFIER`，后续处理由 Parser 完成。

---

## 3. 状态转换示例 (Tracing)

**输入语句：** 
`Result = [ 1 2 ; 3 4 ] ' * transpose ( Ans )`

**解析过程：**

1. `Result` $\rightarrow$ **IDENTIFIER**
2. `=` $\rightarrow$ **OPERATOR (ASSIGN)**
3. `[` $\rightarrow$ **L_BRACKET** (进入矩阵模式)
4. `1` $\rightarrow$ **NUMBER**
5. ` ` (空格) $\rightarrow$ **COMMA** (矩阵模式下的特殊处理)
6. `2` $\rightarrow$ **NUMBER**
7. `;` $\rightarrow$ **SEMICOLON**
8. `3` $\rightarrow$ **NUMBER**
9. ` ` (空格) $\rightarrow$ **COMMA**
10. `4` $\rightarrow$ **NUMBER**
11. `]` $\rightarrow$ **R_BRACKET** (退出矩阵模式)
12. `'` $\rightarrow$ **OPERATOR (TRANSPOSE)**
13. `*` $\rightarrow$ **OPERATOR (MULTIPLY)**
14. `transpose` $\rightarrow$ **IDENTIFIER**
15. `(` $\rightarrow$ **L_PAREN**
16. `Ans` $\rightarrow$ **IDENTIFIER**
17. `)` $\rightarrow$ **R_PAREN**

---

## 4. 异常捕获规范 (Error Handling)

Lexer 在扫描阶段需识别并抛出以下异常：
- **Invalid Character**: 遇到无法识别的字符（如 `@`, `#`）。
- **Malformed Number**: 数字格式错误（如 `1.2.3` 或 `1e--5`）。
- **Implicit Transpose Misplaced**: `'` 前面不是合法的操作对象（如 `+ '`）。

---

## 5. 其它

**贪婪模式**：Lexer 应当始终尝试匹配最长的字符串（例如 `transpose` 不会被切成 `trans` 和 `pose`）。
**运算符优先级**
从高到低：
1. `'`（转置） - 右结合
2. `^`（幂） - 右结合
3. `*`, `/` - 左结合
4. `+`, `-` - 左结合
5. `=`（赋值） - 右结合

## 6. 测试用例

### 6.1 正常输入
```
A = [ 1 2 ; 3 4 ]
B = A ' * transpose ( C ) + 1.5e-3
det ( M ) = 42
```

### 6.2 边界情况
```
# 空矩阵
E = [ ]

# 单元素矩阵
F = [ 5 ]

# 科学计数法
G = 1.2e+10 * H

# 嵌套转置
I = A ' '  # 双重转置
```

### 6.3 错误输入
```
# 非法字符
J = [ 1 @ 2 ]

# 格式错误数字
K = 1.2.3

# 错误转置位置
L = + ' M

---