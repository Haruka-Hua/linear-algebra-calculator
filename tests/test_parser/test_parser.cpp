#include <gtest/gtest.h>
#include <unordered_map>
#include "parser/Lexer.hpp"
#include "parser/Parser.hpp"
#include "parser/ParseException.hpp"

using namespace Lac;

class ParserTest : public ::testing::Test {
protected:
    Lexer lexer;
    std::unordered_map<std::string,LacMatrix> memory;
    Parser parser{memory};

    LacMatrix evalExpr(const std::string& expr){
        auto tokens = lexer.tokenize(expr);
        // substitute (if needed) and evaluate full expression
        parser.substitute(tokens);
        return parser.eval(tokens, 0, static_cast<int>(tokens.size())-1);
    }
};

TEST_F(ParserTest, EvalSimpleNumber){
    LacMatrix res = evalExpr("42");
    // single-number expressions should produce a 1x1 matrix with that value
    ASSERT_EQ(res.rows(), 1);
    ASSERT_EQ(res.cols(), 1);
    EXPECT_DOUBLE_EQ(res(1,1), 42.0);
}

TEST_F(ParserTest, BasicArithmetic){
    LacMatrix res = evalExpr("2 + 3 * 4");
    // expect 2 + (3*4) = 14
    ASSERT_EQ(res.rows(), 1);
    ASSERT_EQ(res.cols(), 1);
    EXPECT_DOUBLE_EQ(res(1,1), 14.0);
}

TEST_F(ParserTest, ParenthesesPrecedence){
    LacMatrix res = evalExpr("( 2 + 3 ) * 4");
    // expect (2+3)*4 = 20
    ASSERT_EQ(res.rows(), 1);
    ASSERT_EQ(res.cols(), 1);
    EXPECT_DOUBLE_EQ(res(1,1), 20.0);
}

TEST_F(ParserTest, TransposeOperator){
    // transpose of [1 2] should be 2x1
    LacMatrix t = evalExpr("[ 1 2 ] '");
    EXPECT_EQ(t.rows(), 2);
    EXPECT_EQ(t.cols(), 1);
    EXPECT_DOUBLE_EQ(t(1,1), 1.0);
    EXPECT_DOUBLE_EQ(t(2,1), 2.0);
}

TEST_F(ParserTest, AssignmentAndMemoryLookup){
    // assign a scalar to X
    auto tokens1 = lexer.tokenize("X = 7");
    parser.substitute(tokens1);
    LacMatrix assigned = parser.eval(tokens1, 0, static_cast<int>(tokens1.size())-1);
    EXPECT_EQ(assigned.rows(), 1);
    EXPECT_EQ(assigned.cols(), 1);
    EXPECT_DOUBLE_EQ(assigned(1,1), 7.0);

    // use X in another expression
    auto tokens2 = lexer.tokenize("X + 3");
    parser.substitute(tokens2);
    LacMatrix res = parser.eval(tokens2, 0, static_cast<int>(tokens2.size())-1);
    EXPECT_DOUBLE_EQ(res(1,1), 10.0);
}

TEST_F(ParserTest, FunctionDeterminant){
    // det([ 1 2 ; 3 4 ]) == -2
    LacMatrix res = evalExpr("det ( [ 1 2 ; 3 4 ] )");
    ASSERT_EQ(res.rows(), 1);
    ASSERT_EQ(res.cols(), 1);
    EXPECT_DOUBLE_EQ(res(1,1), -2.0);
}

TEST_F(ParserTest, MalformedExpressionThrows){
    try{
        evalExpr("+");
        FAIL() << "Expected ParseException for malformed expression";
    } catch(const ParseException& e){
        SUCCEED();
    } catch(...){
        FAIL() << "Unexpected exception type";
    }
}
