// tests/test_parser/test_eval_more.cpp
#include <gtest/gtest.h>
#include <unordered_map>
#include "parser/Lexer.hpp"
#include "parser/Parser.hpp"
#include "parser/ParseException.hpp"

using namespace Lac;

class EvalMoreTest : public ::testing::Test {
protected:
    Lexer lexer;
    std::unordered_map<std::string,LacMatrix> memory;
    Parser parser{memory};

    LacMatrix evalExpr(const std::string& expr){
        auto tokens = lexer.tokenize(expr);
        parser.substitute(tokens);
        return parser.eval(tokens, 0, static_cast<int>(tokens.size())-1);
    }

    void expectMatrixEq(const LacMatrix& a, const LacMatrix& b){
        ASSERT_EQ(a.rows(), b.rows());
        ASSERT_EQ(a.cols(), b.cols());
        for(int r=1;r<=a.rows();++r)
            for(int c=1;c<=a.cols();++c)
                EXPECT_DOUBLE_EQ(a(r,c), b(r,c));
    }
};

// 矩阵加法与乘法
TEST_F(EvalMoreTest, MatrixAddMultiply){
    LacMatrix res = evalExpr("[ 1 2 ; 3 4 ] + [ 0 1 ; 1 0 ]");
    LacMatrix expect({{1,3},{4,4}});
    expectMatrixEq(res, expect);
    
    LacMatrix prod = evalExpr("[ 1 2 ; 3 4 ] * [ 0 1 ; 1 0 ]");
    // [1 2] * [0 1] = [2 1]
    // [3 4]   [1 0]   [4 3]
    ASSERT_EQ(prod.rows(), 2);
    ASSERT_EQ(prod.cols(), 2);
    EXPECT_DOUBLE_EQ(prod(1,1), 2.0);
    EXPECT_DOUBLE_EQ(prod(1,2), 1.0);
    EXPECT_DOUBLE_EQ(prod(2,1), 4.0);
    EXPECT_DOUBLE_EQ(prod(2,2), 3.0);
}

// 标量乘法
TEST_F(EvalMoreTest, ScalarMultiply){
    LacMatrix res = evalExpr("2 * [ 1 2 ; 3 4 ]");
    ASSERT_EQ(res.rows(), 2);
    ASSERT_EQ(res.cols(), 2);
    EXPECT_DOUBLE_EQ(res(1,1), 2.0);
    EXPECT_DOUBLE_EQ(res(1,2), 4.0);
    EXPECT_DOUBLE_EQ(res(2,1), 6.0);
    EXPECT_DOUBLE_EQ(res(2,2), 8.0);
}

// 矩阵减法
TEST_F(EvalMoreTest, MatrixSubtract){
    LacMatrix res = evalExpr("[ 5 6 ; 7 8 ] - [ 1 2 ; 3 4 ]");
    ASSERT_EQ(res.rows(), 2);
    ASSERT_EQ(res.cols(), 2);
    EXPECT_DOUBLE_EQ(res(1,1), 4.0);
    EXPECT_DOUBLE_EQ(res(1,2), 4.0);
    EXPECT_DOUBLE_EQ(res(2,1), 4.0);
    EXPECT_DOUBLE_EQ(res(2,2), 4.0);
}

// 矩阵除法（标量）
TEST_F(EvalMoreTest, MatrixDivideByScalar){
    LacMatrix res = evalExpr("[ 2 4 ; 6 8 ] / 2");
    ASSERT_EQ(res.rows(), 2);
    ASSERT_EQ(res.cols(), 2);
    EXPECT_DOUBLE_EQ(res(1,1), 1.0);
    EXPECT_DOUBLE_EQ(res(1,2), 2.0);
    EXPECT_DOUBLE_EQ(res(2,1), 3.0);
    EXPECT_DOUBLE_EQ(res(2,2), 4.0);
}

// 逆矩阵与单位矩阵
TEST_F(EvalMoreTest, InverseMultiplicationGivesIdentity){
    LacMatrix id = evalExpr("inverse ( [ 4 7 ; 2 6 ] ) * [ 4 7 ; 2 6 ]");
    // id should be (approximately) identity
    EXPECT_EQ(id.rows(), id.cols());
    for(int i=1;i<=id.rows();++i){
        for(int j=1;j<=id.cols();++j){
            if(i==j) EXPECT_NEAR(id(i,j), 1.0, 1e-9);
            else EXPECT_NEAR(id(i,j), 0.0, 1e-9);
        }
    }
}

// 行列式
TEST_F(EvalMoreTest, DeterminantComputation){
    // det([1 2; 3 4]) = 1*4 - 2*3 = -2
    LacMatrix det = evalExpr("det ( [ 1 2 ; 3 4 ] )");
    ASSERT_EQ(det.rows(), 1);
    ASSERT_EQ(det.cols(), 1);
    EXPECT_DOUBLE_EQ(det(1,1), -2.0);
    
    // det([1 0 0; 0 2 0; 0 0 3]) = 6
    LacMatrix det2 = evalExpr("det ( [ 1 0 0 ; 0 2 0 ; 0 0 3 ] )");
    EXPECT_DOUBLE_EQ(det2(1,1), 6.0);
}

// 迹（对角元素和）
TEST_F(EvalMoreTest, TraceComputation){
    // trace([1 2; 3 4]) = 1 + 4 = 5
    LacMatrix tr = evalExpr("trace ( [ 1 2 ; 3 4 ] )");
    ASSERT_EQ(tr.rows(), 1);
    ASSERT_EQ(tr.cols(), 1);
    EXPECT_DOUBLE_EQ(tr(1,1), 5.0);
}

// 秩
TEST_F(EvalMoreTest, RankComputation){
    // rank([1 0; 0 1]) = 2 (full rank)
    LacMatrix r1 = evalExpr("rank ( [ 1 0 ; 0 1 ] )");
    EXPECT_DOUBLE_EQ(r1(1,1), 2.0);
    
    // rank([1 2; 2 4]) = 1 (linearly dependent rows)
    LacMatrix r2 = evalExpr("rank ( [ 1 2 ; 2 4 ] )");
    EXPECT_DOUBLE_EQ(r2(1,1), 1.0);
}

// 解线性方程组
TEST_F(EvalMoreTest, SolveLinearSystem){
    // Solve [1 2; 3 4] * x = [5 ; 11] -> x = [1;2]
    LacMatrix sol = evalExpr("solve ( [ 1 2 ; 3 4 ] , [ 5 ; 11 ] )");
    ASSERT_EQ(sol.rows(), 2);
    ASSERT_EQ(sol.cols(), 1);
    EXPECT_NEAR(sol(1,1), 1.0, 1e-9);
    EXPECT_NEAR(sol(2,1), 2.0, 1e-9);
}

// 幂运算
TEST_F(EvalMoreTest, PowerOperationScalar){
    LacMatrix p = evalExpr("pow ( [ 2 ] , 3 )");
    ASSERT_EQ(p.rows(), 1);
    ASSERT_EQ(p.cols(), 1);
    EXPECT_DOUBLE_EQ(p(1,1), 8.0);
}

// 矩阵幂运算
TEST_F(EvalMoreTest, PowerOperationMatrix){
    // [1 2; 3 4]^2 = [7 10; 15 22]
    LacMatrix p = evalExpr("pow ( [ 1 2 ; 3 4 ] , 2 )");
    ASSERT_EQ(p.rows(), 2);
    ASSERT_EQ(p.cols(), 2);
    EXPECT_DOUBLE_EQ(p(1,1), 7.0);
    EXPECT_DOUBLE_EQ(p(1,2), 10.0);
    EXPECT_DOUBLE_EQ(p(2,1), 15.0);
    EXPECT_DOUBLE_EQ(p(2,2), 22.0);
}

// 行简化阶梯形式 (RREF)
TEST_F(EvalMoreTest, RREF){
    // rref of [1 2; 2 4] should be [1 2; 0 0]
    LacMatrix r = evalExpr("rref ( [ 1 2 ; 2 4 ] )");
    ASSERT_EQ(r.rows(), 2);
    ASSERT_EQ(r.cols(), 2);
    EXPECT_NEAR(r(1,1), 1.0, 1e-9);
    EXPECT_NEAR(r(1,2), 2.0, 1e-9);
    EXPECT_NEAR(r(2,1), 0.0, 1e-9);
    EXPECT_NEAR(r(2,2), 0.0, 1e-9);
}

// 特征值与特征向量（对称矩阵）
TEST_F(EvalMoreTest, EigenStoresValuesAndVectors){
    // eigen should store eigenvalues in memory under "eigen_val_Ans"
    evalExpr("eigen ( [ 2 0 ; 0 3 ] )");
    auto it = memory.find("eigen_val_Ans");
    ASSERT_NE(it, memory.end());
    LacMatrix eigvals = it->second;
    ASSERT_EQ(eigvals.rows(), 2);
    ASSERT_EQ(eigvals.cols(), 2);
    // diag should contain 2 and 3 (order may vary)
    std::vector<double> diag;
    diag.push_back(eigvals(1,1));
    diag.push_back(eigvals(2,2));
    EXPECT_TRUE((std::abs(diag[0]-2.0) < 1e-9 && std::abs(diag[1]-3.0) < 1e-9) ||
                (std::abs(diag[0]-3.0) < 1e-9 && std::abs(diag[1]-2.0) < 1e-9));
}

// 转置操作
TEST_F(EvalMoreTest, TransposeMatrix){
    // [1 2 3]' = [1; 2; 3]
    LacMatrix t = evalExpr("[ 1 2 3 ] '");
    EXPECT_EQ(t.rows(), 3);
    EXPECT_EQ(t.cols(), 1);
    EXPECT_DOUBLE_EQ(t(1,1), 1.0);
    EXPECT_DOUBLE_EQ(t(2,1), 2.0);
    EXPECT_DOUBLE_EQ(t(3,1), 3.0);
}

// 复杂表达式：(A + B) * C
TEST_F(EvalMoreTest, ComplexExpression){
    LacMatrix res = evalExpr("( [ 1 2 ; 3 4 ] + [ 0 1 ; 1 0 ] ) * [ 1 0 ; 0 1 ]");
    // ([1 2; 3 4] + [0 1; 1 0]) * I = [1 3; 4 4]
    ASSERT_EQ(res.rows(), 2);
    ASSERT_EQ(res.cols(), 2);
    EXPECT_DOUBLE_EQ(res(1,1), 1.0);
    EXPECT_DOUBLE_EQ(res(1,2), 3.0);
    EXPECT_DOUBLE_EQ(res(2,1), 4.0);
    EXPECT_DOUBLE_EQ(res(2,2), 4.0);
}

// 变量赋值与重用
TEST_F(EvalMoreTest, AssignmentAndReuse){
    // M = [1 2; 3 4]
    evalExpr("M = [ 1 2 ; 3 4 ]");
    
    // M * M = [7 10; 15 22]
    LacMatrix res = evalExpr("M * M");
    ASSERT_EQ(res.rows(), 2);
    ASSERT_EQ(res.cols(), 2);
    EXPECT_DOUBLE_EQ(res(1,1), 7.0);
    EXPECT_DOUBLE_EQ(res(1,2), 10.0);
    EXPECT_DOUBLE_EQ(res(2,1), 15.0);
    EXPECT_DOUBLE_EQ(res(2,2), 22.0);
}

// 多步计算
TEST_F(EvalMoreTest, MultiStepComputation){
    // A = [1 2; 3 4]
    evalExpr("A = [ 1 2 ; 3 4 ]");
    
    // B = det(A) = -2
    evalExpr("B = det ( A )");
    
    // 3 * B = -6
    LacMatrix res = evalExpr("3 * B");
    EXPECT_DOUBLE_EQ(res(1,1), -6.0);
}

// 零矩阵处理
TEST_F(EvalMoreTest, ZeroMatrixAddition){
    LacMatrix res = evalExpr("[ 1 2 ; 3 4 ] + [ 0 0 ; 0 0 ]");
    ASSERT_EQ(res.rows(), 2);
    ASSERT_EQ(res.cols(), 2);
    EXPECT_DOUBLE_EQ(res(1,1), 1.0);
    EXPECT_DOUBLE_EQ(res(1,2), 2.0);
    EXPECT_DOUBLE_EQ(res(2,1), 3.0);
    EXPECT_DOUBLE_EQ(res(2,2), 4.0);
}

// 单位矩阵乘法
TEST_F(EvalMoreTest, IdentityMultiplication){
    LacMatrix res = evalExpr("[ 1 0 ; 0 1 ] * [ 5 6 ; 7 8 ]");
    ASSERT_EQ(res.rows(), 2);
    ASSERT_EQ(res.cols(), 2);
    EXPECT_DOUBLE_EQ(res(1,1), 5.0);
    EXPECT_DOUBLE_EQ(res(1,2), 6.0);
    EXPECT_DOUBLE_EQ(res(2,1), 7.0);
    EXPECT_DOUBLE_EQ(res(2,2), 8.0);
}
