// tests/test_parser/test_lexer.cpp
#include <gtest/gtest.h>
#include "parser/Lexer.hpp"
#include "parser/ParseException.hpp"

using namespace Lac;

class LexerTest : public ::testing::Test {
protected:
    Lexer lexer;
    
    // 辅助函数：验证Token类型
    void expectTokenType(const Token& token, TokenType expected_type) {
        EXPECT_EQ(token.token_type_, expected_type);
    }
    
    // 辅助函数：验证Token文本
    void expectTokenText(const Token& token, const std::string& expected_text) {
        EXPECT_EQ(token.text_, expected_text);
    }
    
    // 辅助函数：验证数字值
    void expectNumberValue(const Token& token, double expected_value) {
        EXPECT_DOUBLE_EQ(token.value(), expected_value);
    }
    
    // 辅助函数：验证运算符类型
    void expectOperatorType(const Token& token, OperatorType expected_op) {
        EXPECT_EQ(token.op(), expected_op);
    }
    
    // 辅助函数：验证函数类型
    void expectFunctionType(const Token& token, FunctionType expected_func) {
        EXPECT_EQ(token.func(), expected_func);
    }
};

// 1. 基本Token识别测试
TEST_F(LexerTest, BasicTokenRecognition) {
    auto tokens = lexer.tokenize("A = 42");
    
    ASSERT_EQ(tokens.size(), 3);
    
    // 第一个Token: IDENTIFIER "A"
    expectTokenType(tokens[0], TokenType::IDENTIFIER);
    expectTokenText(tokens[0], "A");
    expectFunctionType(tokens[0], FunctionType::MATRIX);
    
    // 第二个Token: OPERATOR "="
    expectTokenType(tokens[1], TokenType::OPERATOR);
    expectTokenText(tokens[1], "=");
    expectOperatorType(tokens[1], OperatorType::ASSIGN);
    
    // 第三个Token: NUMBER "42"
    expectTokenType(tokens[2], TokenType::NUMBER);
    expectTokenText(tokens[2], "42");
    expectNumberValue(tokens[2], 42.0);
}

// 2. 数字格式测试
TEST_F(LexerTest, NumberFormats) {
    // 整数
    auto tokens1 = lexer.tokenize("123");
    ASSERT_EQ(tokens1.size(), 1);
    expectTokenType(tokens1[0], TokenType::NUMBER);
    expectNumberValue(tokens1[0], 123.0);
    
    // 小数
    auto tokens2 = lexer.tokenize("3.1415");
    ASSERT_EQ(tokens2.size(), 1);
    expectNumberValue(tokens2[0], 3.1415);
    
    // 负数
    auto tokens3 = lexer.tokenize("-5.2");
    ASSERT_EQ(tokens3.size(), 1);
    expectNumberValue(tokens3[0], -5.2);
    
    // 正数（带+号）
    auto tokens4 = lexer.tokenize("+3.14");
    ASSERT_EQ(tokens4.size(), 1);
    expectNumberValue(tokens4[0], 3.14);
    
    // 科学计数法
    auto tokens5 = lexer.tokenize("1.5e-3");
    ASSERT_EQ(tokens5.size(), 1);
    expectNumberValue(tokens5[0], 0.0015);
}

// 3. 运算符测试
TEST_F(LexerTest, OperatorRecognition) {
    auto tokens = lexer.tokenize("+ - * / ' = ^");
    
    ASSERT_EQ(tokens.size(), 7);
    
    expectOperatorType(tokens[0], OperatorType::ADD);
    expectOperatorType(tokens[1], OperatorType::SUBTRACT);
    expectOperatorType(tokens[2], OperatorType::MULTIPLY);
    expectOperatorType(tokens[3], OperatorType::DIVIDE);
    expectOperatorType(tokens[4], OperatorType::TRANSPOSE);
    expectOperatorType(tokens[5], OperatorType::ASSIGN);
    expectOperatorType(tokens[6], OperatorType::POWER);
}

// 4. 分隔符测试
TEST_F(LexerTest, DelimiterRecognition) {
    auto tokens = lexer.tokenize("( ) [ ] ;");
    
    ASSERT_EQ(tokens.size(), 5);
    
    expectTokenType(tokens[0], TokenType::L_PAREN);
    expectTokenType(tokens[1], TokenType::R_PAREN);
    expectTokenType(tokens[2], TokenType::L_BRACKET);
    expectTokenType(tokens[3], TokenType::R_BRACKET);
    expectTokenType(tokens[4], TokenType::SEMICOLON);
}

// 5. 内置函数识别测试
TEST_F(LexerTest, BuiltinFunctionRecognition) {
    auto tokens = lexer.tokenize("transpose trace rank det inverse adjoint rref pow solve solvelsq eigen");
    
    ASSERT_EQ(tokens.size(), 11);
    
    expectFunctionType(tokens[0], FunctionType::TRANSPOSE);
    expectFunctionType(tokens[1], FunctionType::TRACE);
    expectFunctionType(tokens[2], FunctionType::RANK);
    expectFunctionType(tokens[3], FunctionType::DETERMINANT);
    expectFunctionType(tokens[4], FunctionType::INVERSE);
    expectFunctionType(tokens[5], FunctionType::ADJOINT);
    expectFunctionType(tokens[6], FunctionType::RREF);
    expectFunctionType(tokens[7], FunctionType::POWER);
    expectFunctionType(tokens[8], FunctionType::SOLVE);
    expectFunctionType(tokens[9], FunctionType::SOLVELSQ);
    expectFunctionType(tokens[10], FunctionType::EIGEN);
}

// 6. 矩阵字面量测试
TEST_F(LexerTest, MatrixLiteralParsing) {
    auto tokens = lexer.tokenize("[ 1 2 ; 3 4 ]");
    
    ASSERT_EQ(tokens.size(), 7);
    
    expectTokenType(tokens[0], TokenType::L_BRACKET);
    expectTokenType(tokens[1], TokenType::NUMBER);
    expectNumberValue(tokens[1], 1.0);
    expectTokenType(tokens[2], TokenType::NUMBER);
    expectNumberValue(tokens[2], 2.0);
    expectTokenType(tokens[3], TokenType::SEMICOLON);
    expectTokenType(tokens[4], TokenType::NUMBER);
    expectNumberValue(tokens[4], 3.0);
    expectTokenType(tokens[5], TokenType::NUMBER);
    expectNumberValue(tokens[5], 4.0);
    expectTokenType(tokens[6], TokenType::R_BRACKET);
}

// 7. 复杂表达式测试
TEST_F(LexerTest, ComplexExpression) {
    auto tokens = lexer.tokenize("Result = [ 1 2 ; 3 4 ] ' * transpose ( Ans )");
    
    ASSERT_EQ(tokens.size(), 15);
    
    // 验证关键Token
    expectTokenText(tokens[0], "Result");
    expectTokenText(tokens[1], "=");
    expectTokenText(tokens[2], "[");
    expectTokenText(tokens[5], ";");
    expectTokenText(tokens[9], "'");
    expectTokenText(tokens[10], "*");
    expectTokenText(tokens[11], "transpose");
}

// 8. 异常测试：非法字符
TEST_F(LexerTest, InvalidCharacterThrowsException) {
    try {
        lexer.tokenize("A @ B");
        FAIL() << "Should throw ParseException for invalid character '@'";
    } catch (const ParseException& e) {
        // 验证异常消息包含相关信息
        std::string whatMsg = e.what();
        EXPECT_NE(whatMsg.find("@"), std::string::npos);
    } catch (...) {
        FAIL() << "Caught unexpected exception type";
    }
}

// 9. 异常测试：格式错误数字
TEST_F(LexerTest, MalformedNumberThrowsException) {
    try {
        lexer.tokenize("1.2.3");
        FAIL() << "Should throw ParseException for malformed number '1.2.3'";
    } catch (const ParseException& e) {
        std::string whatMsg = e.what();
        EXPECT_NE(whatMsg.find("number"), std::string::npos);
    } catch (...) {
        FAIL() << "Caught unexpected exception type";
    }
}

// 10. 异常测试：非法标识符字符
TEST_F(LexerTest, InvalidIdentifierCharacterThrowsException) {
    try {
        lexer.tokenize("my@matrix");
        FAIL() << "Should throw ParseException for invalid character in identifier";
    } catch (const ParseException& e) {
        std::string whatMsg = e.what();
        EXPECT_NE(whatMsg.find("identifier"), std::string::npos);
        EXPECT_NE(whatMsg.find("@"), std::string::npos);
    } catch (...) {
        FAIL() << "Caught unexpected exception type";
    }
}

// 11. 边界情况：空输入
TEST_F(LexerTest, EmptyInputReturnsEmptyVector) {
    auto tokens = lexer.tokenize("");
    EXPECT_TRUE(tokens.empty());
    
    tokens = lexer.tokenize("   ");  // 只有空格
    EXPECT_TRUE(tokens.empty());
}

// 12. 边界情况：单元素矩阵
TEST_F(LexerTest, SingleElementMatrix) {
    auto tokens = lexer.tokenize("[ 5 ]");
    
    ASSERT_EQ(tokens.size(), 3);
    expectTokenType(tokens[0], TokenType::L_BRACKET);
    expectTokenType(tokens[1], TokenType::NUMBER);
    expectNumberValue(tokens[1], 5.0);
    expectTokenType(tokens[2], TokenType::R_BRACKET);
}

// 13. 边界情况：空矩阵
TEST_F(LexerTest, EmptyMatrix) {
    auto tokens = lexer.tokenize("[ ]");
    
    ASSERT_EQ(tokens.size(), 2);
    expectTokenType(tokens[0], TokenType::L_BRACKET);
    expectTokenType(tokens[1], TokenType::R_BRACKET);
}

// 14. 标识符命名规则测试
TEST_F(LexerTest, IdentifierNamingRules) {
    // 有效标识符
    EXPECT_NO_THROW(lexer.tokenize("matrix_name"));
    EXPECT_NO_THROW(lexer.tokenize("Matrix123"));
    EXPECT_NO_THROW(lexer.tokenize("_private"));
    EXPECT_NO_THROW(lexer.tokenize("A1_B2_C3"));
    
    // 无效标识符（应该抛出异常）
    EXPECT_THROW(lexer.tokenize("123matrix"), ParseException);  // 以数字开头
    EXPECT_THROW(lexer.tokenize("my-matrix"), ParseException);  // 包含连字符
}

// 15. 混合测试：完整计算表达式
TEST_F(LexerTest, FullCalculationExpression) {
    auto tokens = lexer.tokenize("det ( M ) = A ' * B + 3.14");
    
    // 验证Token数量
    EXPECT_EQ(tokens.size(), 11);
    
    // 验证关键Token
    expectTokenText(tokens[0], "det");
    expectFunctionType(tokens[0], FunctionType::DETERMINANT);
    
    expectTokenText(tokens[2], "M");
    expectFunctionType(tokens[2], FunctionType::MATRIX);
    
    expectTokenText(tokens[5], "A");
    expectTokenText(tokens[6], "'");
    expectTokenText(tokens[8], "B");
    expectTokenText(tokens[10], "3.14");
}