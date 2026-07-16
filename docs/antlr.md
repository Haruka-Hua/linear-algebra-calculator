# Parser 迁移计划：ANTLR4 + AST

## 0. 目标

将现有手写 Lexer + 递归下降 Parser 替换为 ANTLR4 生成的 Lexer/Parser + 抽象语法树(AST) + Visitor 求值模式。

**不变**：`LacMatrix`、`LacEngine`、`LacException` 完全复用，`Cli` 的接口保持不变。
**改变**：`include/parser/` 和 `src/parser/` 下的 Lexer/Parser/Token 全部替换。

---

## 1. 语法变更

### 矩阵字面量：元素间必须用逗号分隔

```
旧: [ 1 2 3 ; 4 5 6 ]
新: [ 1 , 2 , 3 ; 4 , 5 , 6 ]
```

动机：消除"空格即逗号"的特殊规则，简化 ANTLR 文法，避免 lexer mode。

### 其余语法不变

- 运算符：`+` `-` `*` `/` `^` `'` `=`，优先级同上
- 函数调用：`det ( A )`、`pow ( A , 3 )` 等
- 矩阵下标访问：`A ( 1 , 2 )`
- 赋值：`A = expr`
- 变量名：`[a-zA-Z_][a-zA-Z0-9_]*`

---

## 2. ANTLR4 文法

### 2.1 文件位置

`src/parser/LAC.g4`

### 2.2 文法内容

```antlr
grammar LAC;

// ---- Parser Rules ----

prog
    : expr EOF
    ;

expr
    : assignment
    ;

assignment
    : addExpr (ASSIGN assignment)?
    ;

addExpr
    : mulExpr ((ADD | SUB) mulExpr)*
    ;

mulExpr
    : powExpr ((MUL | DIV) powExpr)*
    ;

powExpr
    : unaryExpr (POW powExpr)?
    ;

unaryExpr
    : TRANSPOSE unaryExpr
    | postfixExpr
    ;

postfixExpr
    : primary TRANSPOSE*
    ;

primary
    : NUMBER
    | IDENTIFIER
    | functionCall
    | matrixLiteral
    | LPAREN expr RPAREN
    ;

functionCall
    : IDENTIFIER LPAREN (expr (COMMA expr)*)? RPAREN
    ;

matrixLiteral
    : LBRACK (row (SEMICOLON row)*)? RBRACK
    ;

row
    : expr (COMMA expr)*
    ;

// ---- Lexer Rules ----

NUMBER
    : [0-9]* '.'? [0-9]+ ([eE] [-+]? [0-9]+)?
    ;

IDENTIFIER
    : [a-zA-Z_] [a-zA-Z0-9_]*
    ;

ADD     : '+' ;
SUB     : '-' ;
MUL     : '*' ;
DIV     : '/' ;
POW     : '^' ;
ASSIGN  : '=' ;
TRANSPOSE : '\'' ;
LPAREN  : '(' ;
RPAREN  : ')' ;
LBRACK  : '[' ;
RBRACK  : ']' ;
COMMA   : ',' ;
SEMICOLON : ';' ;

WS
    : [ \t\r\n]+ -> skip
    ;
```

### 2.3 代码生成配置

- ANTLR 版本：4.13.x
- 生成目标：C++ (`-Dlanguage=Cpp`)
- 输出目录：`src/parser/generated/`
- **预提交生成的代码**，构建者无需安装 JDK/ANTLR

在 `CMakeLists.txt` 中提供可选目标 `gen_antlr`（需要 JDK + ANTLR jar），仅在修改 `.g4` 后手动调用。

---

## 3. 目录与文件变更

### 3.1 新增文件

```
include/ast/
    ASTNode.hpp                    # AST 节点定义

src/ast/
    ASTBuilder.h                   # ANTLR Parse Tree → AST 转换器（头文件）
    ASTBuilder.cpp                 # 实现
    ASTEvaluator.h                 # AST → LacMatrix 求值器（头文件）
    ASTEvaluator.cpp               # 实现

src/parser/
    LAC.g4                         # ANTLR 文法

src/parser/generated/              # ANTLR 自动生成（预提交）
    LACLexer.h
    LACLexer.cpp
    LACParser.h
    LACParser.cpp
    LACBaseVisitor.h
    LACVisitor.h
    LACVisitor.cpp
```

### 3.2 删除文件

```
include/parser/Token.hpp
include/parser/Lexer.hpp
include/parser/Parser.hpp
include/parser/ParseException.hpp
src/parser/Lexer.cpp
src/parser/Parser.cpp
```

### 3.3 修改文件

| 文件 | 变更 |
|---|---|
| `include/cli/cli.hpp` | 移除 `#include "parser/Parser.hpp"`，替换为 AST 相关头文件；`Parser parser` 成员替换为 AST 相关成员 |
| `src/cli/cli.cpp` | `calculate()` 改为：ANTLR InputStream → Lexer → Parser → ASTBuilder → ASTEvaluator |
| `CMakeLists.txt` | 移除 `lac_parser` 库，新增 `lac_ast` 库；链接 ANTLR4 C++ runtime |
| `tests/test_parser/` | 迁移测试用例适配新 AST API（语法变更部分需更新 golden） |

---

## 4. AST 节点设计

```cpp
// include/ast/ASTNode.hpp

#pragma once
#include <memory>
#include <string>
#include <vector>
#include "math/LacMatrix.hpp"
#include "math/LacEngine.hpp"

namespace Lac::ast {

enum class NodeType {
    Number,
    Variable,
    MatrixLiteral,
    BinaryOp,
    UnaryOp,
    FunctionCall,
    Assign
};

struct ASTNode {
    NodeType type;
    virtual ~ASTNode() = default;
};

struct NumberNode : ASTNode {
    NumberNode() { type = NodeType::Number; }
    double value;
};

struct VariableNode : ASTNode {
    VariableNode() { type = NodeType::Variable; }
    std::string name;
};

struct MatrixLiteralNode : ASTNode {
    MatrixLiteralNode() { type = NodeType::MatrixLiteral; }
    std::vector<std::vector<std::unique_ptr<ASTNode>>> rows;
};

struct BinaryOpNode : ASTNode {
    BinaryOpNode() { type = NodeType::BinaryOp; }
    OperatorType op;  // 复用现有 OperatorType 枚举
    std::unique_ptr<ASTNode> left;
    std::unique_ptr<ASTNode> right;
};

struct UnaryOpNode : ASTNode {
    UnaryOpNode() { type = NodeType::UnaryOp; }
    OperatorType op;
    std::unique_ptr<ASTNode> operand;
};

struct FunctionCallNode : ASTNode {
    FunctionCallNode() { type = NodeType::FunctionCall; }
    FunctionType func;  // 复用现有 FunctionType 枚举
    std::string name;
    std::vector<std::unique_ptr<ASTNode>> args;
};

struct AssignNode : ASTNode {
    AssignNode() { type = NodeType::Assign; }
    std::string name;
    std::unique_ptr<ASTNode> value;
};

} // namespace Lac::ast
```

> `OperatorType` 和 `FunctionType` 枚举从当前 `Token.hpp` 中移出，放到独立的公共头文件（或直接留在新位置）。

---

## 5. ASTBuilder

### 5.1 职责

继承 ANTLR 生成的 `LACBaseVisitor`，遍历 Parse Tree，构建 AST。

### 5.2 关键映射

| ANTLR Context | AST Node |
|---|---|
| `NumberContext` | `NumberNode` |
| `IdentifierContext` | `VariableNode` |
| `MatrixLiteralContext` | `MatrixLiteralNode`（递归构建行） |
| `AddExprContext / MulExprContext` | `BinaryOpNode`（根据 operator token 设置 op） |
| `PowExprContext` | `BinaryOpNode` |
| `UnaryExprContext` (TRANSPOSE + expr) | `UnaryOpNode` |
| `PostfixExprContext` (expr + TRANSPOSE) | `UnaryOpNode` |
| `FunctionCallContext` | `FunctionCallNode` |
| `AssignmentContext` | `AssignNode` |

### 5.3 函数标识符处理

ANTLR 文法中 `functionCall` 由 `IDENTIFIER LPAREN ...` 匹配。在 `ASTBuilder` 中，通过 `IDENTIFIER` 的文本判断是否为内置函数名（与当前 `Lexer::convert_identifier()` 逻辑一致）。若是内置函数则创建 `FunctionCallNode` 并设置对应 `FunctionType`；若是变量名则视为矩阵下标访问（也走 `FunctionCallNode`，`FunctionType::MATRIX`）。

---

## 6. ASTEvaluator

### 6.1 职责

遍历 AST，调用 `LacEngine` 完成求值。持有 `memory_` 引用，处理变量存储。

### 6.2 求值规则

```cpp
class ASTEvaluator {
public:
    LacMatrix evaluate(const ASTNode& node);

private:
    std::unordered_map<std::string, LacMatrix>& memory_;

    LacMatrix visit(const NumberNode& node);        // → LacMatrix(value)
    LacMatrix visit(const VariableNode& node);       // → memory_[name]
    LacMatrix visit(const MatrixLiteralNode& node);  // 递归求值每行 → LacMatrix
    LacMatrix visit(const BinaryOpNode& node);       // left op right → LacEngine operators
    LacMatrix visit(const UnaryOpNode& node);        // op operand → LacEngine
    LacMatrix visit(const FunctionCallNode& node);   // 参数求值 → LacEngine::det/inverse/...
    LacMatrix visit(const AssignNode& node);         // 求值右手边 → 存入 memory_ → 返回结果
};
```

### 6.3 与 LacEngine 的映射

| AST 操作 | LacEngine 调用 |
|---|---|
| `+` (ADD) | `LacEngine::operator+` 或全局 `operator+` |
| `-` (SUB) | `LacEngine::operator-` 或全局 `operator-` |
| `*` (MUL) | `operator*` |
| `/` (DIV) | `operator/` |
| `^` (POW) | `LacEngine::pow()` |
| `'` (TRANSPOSE) | `LacEngine::transpose()` |
| `det()` | `LacEngine::det()` |
| `inverse()` | `LacEngine::inverse()` |
| `trace()` | `LacEngine::trace()` |
| `rank()` | `LacEngine::rank()` |
| `adjoint()` | `LacEngine::adjoint()` |
| `rref()` | `LacEngine::rref()` |
| `transpose()` | `LacEngine::transpose()` |
| `solve()` | `LacEngine::solve()` |
| `solvelsq()` | `LacEngine::solveLeastSquares()` |
| `eigen()` | `LacEngine::eigenValuesSymmetric()` |
| `pow()` (函数) | `LacEngine::pow()` |
| 矩阵下标 `A(r,c)` | `LacMatrix::operator()` |

---

## 7. ANTLR4 C++ Runtime 集成

### 7.1 CMake 配置

```cmake
# 添加 ANTLR4 C++ runtime（FetchContent 或 find_package）
FetchContent_Declare(
    antlr4_runtime
    GIT_REPOSITORY https://github.com/antlr/antlr4.git
    GIT_TAG v4.13.1
    SOURCE_SUBDIR runtime/Cpp
)
FetchContent_MakeAvailable(antlr4_runtime)

# AST 静态库
add_library(lac_ast STATIC
    src/ast/ASTBuilder.cpp
    src/ast/ASTEvaluator.cpp
    src/parser/generated/LACLexer.cpp
    src/parser/generated/LACParser.cpp
    src/parser/generated/LACVisitor.cpp
)
target_include_directories(lac_ast PUBLIC
    ${CMAKE_SOURCE_DIR}/include/ast
    ${CMAKE_SOURCE_DIR}/src/parser/generated
)
target_link_libraries(lac_ast PUBLIC lac_math antlr4_static)
```

### 7.2 预提交策略

ANTLR 生成的 `src/parser/generated/` 目录须作为源码提交到 Git。仅当修改 `LAC.g4` 时才重新生成：

```bash
# 仅在修改文法时执行（需要 JDK + ANTLR jar）
java -jar antlr-4.13.1-complete.jar -Dlanguage=Cpp -o src/parser/generated -no-listener -visitor src/parser/LAC.g4
```

可在 `CMakeLists.txt` 中提供自定义目标：

```cmake
add_custom_target(gen_antlr
    COMMAND java -jar ${CMAKE_SOURCE_DIR}/tools/antlr-4.13.1-complete.jar
            -Dlanguage=Cpp
            -o ${CMAKE_SOURCE_DIR}/src/parser/generated
            -no-listener -visitor
            ${CMAKE_SOURCE_DIR}/src/parser/LAC.g4
    COMMENT "Generating ANTLR4 C++ code from LAC.g4"
)
```

---

## 8. 测试迁移

### 8.1 需要更新的测试文件

- `tests/test_parser/test_lexer.cpp` → 删除（Lexer 由 ANTLR 生成，不再需要单独测试）
- `tests/test_parser/test_parser.cpp` → 重写为 AST 集成测试
- `tests/test_parser/test_eval_more.cpp` → 重写为 AST 评估测试

### 8.2 测试输入变更

由于语法变更（矩阵内必须用逗号），需全局替换：

```diff
- [ 1 2 ; 3 4 ]
+ [ 1 , 2 ; 3 , 4 ]
```

其他测试输入不变。

### 8.3 测试目标

确保所有现有测试用例在新 parser 下通过，包括：
- 原子数值求值
- 四则运算与运算符优先级
- 括号分组
- 转置操作（一元 / 后缀两种形式）
- 变量赋值与重用
- 13 种内置函数调用
- 矩阵字面量
- 复杂嵌套表达式
- 多步计算
- 异常 / 错误输入

---

## 9. 实现步骤清单

### Phase 1: 基础设施
- [ ] 在 `CMakeLists.txt` 中添加 ANTLR4 C++ runtime 依赖
- [ ] 编写 `src/parser/LAC.g4` 文法文件
- [ ] 下载 ANTLR jar 到 `tools/`，配置 `gen_antlr` 自定义目标
- [ ] 首次生成代码到 `src/parser/generated/`，提交到 Git
- [ ] 验证 `lac_ast` 库可成功编译链接

### Phase 2: AST 节点
- [ ] 将 `OperatorType`、`FunctionType` 枚举从 `Token.hpp` 移到独立公共头文件 `include/parser/LacTypes.hpp`
- [ ] 实现 `include/ast/ASTNode.hpp` 所有节点类
- [ ] 编译验证

### Phase 3: ASTBuilder
- [ ] 实现 `src/ast/ASTBuilder.h` / `ASTBuilder.cpp`
- [ ] 编写单元测试：传入简单表达式、验证生成的 AST 结构

### Phase 4: ASTEvaluator
- [ ] 实现 `src/ast/ASTEvaluator.h` / `ASTEvaluator.cpp`
- [ ] 对每个节点类型编写 visit 方法
- [ ] 集成测试：对比新旧 parser 的求值结果

### Phase 5: CLI 集成
- [ ] 修改 `include/cli/cli.hpp`：替换 `Parser parser` 为 AST 相关成员
- [ ] 修改 `src/cli/cli.cpp::calculate()`：实现 InputStream → AST → Evaluate 流程
- [ ] 保持 `memory_`、`"Ans"` 行为不变

### Phase 6: 测试更新
- [ ] 更新所有测试用例（矩阵字面量语法变更）
- [ ] 确保全部 `lac_math_tests` + `lac_parser_tests` 通过
- [ ] 添加新 parser 单元测试

### Phase 7: 清理
- [ ] 删除旧 Lexer/Parser/Token 源文件
- [ ] 更新 `CMakeLists.txt` 移除 `lac_parser` 库
- [ ] 更新 `docs/cli/parser.md` 文法文档

---

## 10. 风险与缓解

| 风险 | 缓解 |
|---|---|
| ANTLR4 C++ runtime 版本兼容问题 | 使用 FetchContent 锁定 `v4.13.1` |
| 生成的代码与编译器警告冲突 | 在 CMake 中对 `generated/` 目录单独设置 `-w` 或 `/W0` |
| 矩阵内逗号分隔破坏用户习惯 | 在 CLI 启动时打印语法提示；考虑提供兼容模式下"空格自动转逗号"的输入预处理（可选） |

---

## 11. 目录结构预览（完成后）

```
linear-algebra-calculator/
├── include/
│   ├── ast/
│   │   └── ASTNode.hpp
│   ├── cli/
│   │   └── cli.hpp
│   ├── math/
│   │   ├── LacEngine.hpp
│   │   ├── LacException.hpp
│   │   └── LacMatrix.hpp
│   └── parser/
│       └── LacTypes.hpp              # OperatorType / FunctionType 枚举
├── src/
│   ├── ast/
│   │   ├── ASTBuilder.h
│   │   ├── ASTBuilder.cpp
│   │   ├── ASTEvaluator.h
│   │   └── ASTEvaluator.cpp
│   ├── cli/
│   │   ├── cli.cpp
│   │   └── main.cpp
│   ├── math/
│   │   ├── LacEngine.cpp
│   │   ├── LacException.cpp
│   │   └── LacMatrix.cpp
│   └── parser/
│       ├── LAC.g4                     # ANTLR 文法源文件
│       └── generated/                 # ANTLR 生成（预提交）
│           ├── LACLexer.h
│           ├── LACLexer.cpp
│           ├── LACParser.h
│           ├── LACParser.cpp
│           ├── LACBaseVisitor.h
│           ├── LACVisitor.h
│           └── LACVisitor.cpp
├── tests/
│   ├── test_math/                     # 不变
│   └── test_parser/                   # 测试用例更新语法
├── tools/
│   └── antlr-4.13.1-complete.jar      # ANTLR 工具（用于 gen_antlr）
├── CMakeLists.txt
└── docs/
    ├── antlr.md                       # 本文件
    └── cli/
        ├── cli.md
        ├── math.md
        ├── parser.md
        └── spec.md
```
