#include "parser/Parser.hpp"
#include "parser/ParseException.hpp"
#include "math/LacEngine.hpp"

namespace Lac{

void Parser::substitute(std::vector<Token> &tokens){
    for (int i=0; i < tokens.size();i++){
        //identifiers 
        //function: id ( 
        //variable: id 
        if (tokens[i].token_type_==TokenType::IDENTIFIER){
            bool isFunction = false;
            if(i+1 < tokens.size()){
                if(tokens[i+1].token_type_ == TokenType::L_PAREN){
                    isFunction = true;
                    tokens[i].token_type_ = TokenType::FUNCTION;
                }
            }
            if(!isFunction){
                auto find_pair = memory_.find(tokens[i].text_);
                LacMatrix mat;
                if(find_pair != memory_.end()){
                    mat = find_pair->second;
                }
                else{
                    mat = LacMatrix();
                }
                Token mat_token(TokenType::MATRIX, tokens[i].text_, mat);
                tokens.erase(tokens.begin()+i);
                tokens.insert(tokens.begin()+i, mat_token);
            }
        }
        // l_bracket, scan for a whole matrix and replace
        else if (tokens[i].token_type_==TokenType::L_BRACKET){
            int j;
            std::vector<std::vector<double>> mat_data;
            std::vector<double> line;
            for(j=i+1;j<tokens.size();j++){
                //number: push this number into the line
                if(tokens[j].token_type_==TokenType::NUMBER){
                    line.push_back(tokens[j].value());
                }
                //semicolon: push current line into matrix and clear line
                else if(tokens[j].token_type_==TokenType::SEMICOLON){
                    mat_data.push_back(std::vector<double>(line));
                    line.clear();
                }
                //r_bracket: push current line into matrix, jump out
                else if(tokens[j].token_type_==TokenType::R_BRACKET){
                    mat_data.push_back(std::move(line));
                    break;
                }
                else {
                    throw ParseException("Invalid token inside a matrix: " + tokens[j].text_);
                }
            }
            if (j==tokens.size()){
                throw ParseException("Unclosed matrix.");
            }
            tokens.erase(tokens.begin()+i,tokens.begin()+j+1);
            LacMatrix mat(mat_data);
            Token mat_token(TokenType::MATRIX,"tmp matrix",mat);
            tokens.insert(tokens.begin()+i,mat_token);
        }
    }
}
LacMatrix Parser::eval(const std::vector<Token> &tokens, int l, int r){
    if (l > r){
        throw ParseException("Invalid expression: empty subexpression.");
    }
    if (l == r){
        if(tokens[l].token_type_==TokenType::NUMBER){
            return LacMatrix(tokens[l].value());
        }
        if(tokens[r].token_type_==TokenType::MATRIX){
            return LacMatrix(tokens[l].matrix());
        }
        throw ParseException("SyntaxError-Unable to evaluate single token: " + tokens[l].text_);
    }
    //wrapped by a pair of parentheses
    if (check_parentheses(tokens,l,r)) {
        return eval(tokens,l+1,r-1);
    }
    int op_pos = find_main_operator(tokens,l,r);
    const Token& op = tokens[op_pos];
    //assignment statement
    //operators
    if(op.token_type_==TokenType::OPERATOR){
        switch (op.op()){
            case OperatorType::ADD: 
                return eval(tokens,l,op_pos-1) + eval(tokens,op_pos+1,r);
            case OperatorType::SUBTRACT:
                return eval(tokens,l,op_pos-1) - eval(tokens,op_pos+1,r);
            case OperatorType::MULTIPLY:
                return eval(tokens,l,op_pos-1) * eval(tokens,op_pos+1,r);
            case OperatorType::DIVIDE:
                return eval(tokens,l,op_pos-1) / eval(tokens,op_pos+1,r);
            case OperatorType::TRANSPOSE:
                if(op_pos!=r) throw ParseException("Operator \' is a unary operator.");
                return LacEngine::transpose(eval(tokens,l,op_pos-1));
            case OperatorType::POWER:
                return LacEngine::pow(eval(tokens,l,op_pos-1), static_cast<int>(eval(tokens,op_pos+1,r)));
            case OperatorType::ASSIGN:
                if(op_pos == l+1 && tokens[l].token_type_==TokenType::MATRIX
                    && tokens[l].text_!=""){
                    LacMatrix res = eval(tokens,op_pos+1,r);
                    memory_[tokens[l].text_] = res;
                    return res;
                }
                throw ParseException("Assignment failure.");
            throw ParseException("Unknown operator.");
        }
    }
    //functions
    if(op.token_type_==TokenType::FUNCTION){
        if(op_pos != l){
            throw ParseException("Operator required between "+ tokens[op_pos-1].text_
                 + " and " + tokens[op_pos].text_);
        }
        if(tokens[op_pos+1].token_type_!=TokenType::L_PAREN || tokens[r].token_type_!=TokenType::R_PAREN){
            throw ParseException("Parentheses required for arguments.");
        }
        switch (op.func()){
            case FunctionType::MATRIX:{
                auto find_pair = memory_.find(op.text_);
                if(find_pair == memory_.end()){
                    throw ParseException("Matrix not found: " + tokens[op_pos].text_);
                }
                int comma_pos = find_comma(tokens,l,r);
                double value = find_pair->second(
                    static_cast<int>(eval(tokens,op_pos+2,comma_pos-1)),
                    static_cast<int>(eval(tokens,comma_pos+1,r-1))
                );
                return LacMatrix(value);
            }
            case FunctionType::TRANSPOSE:
                return LacEngine::transpose(eval(tokens,op_pos+2,r-1));
            case FunctionType::TRACE:
                return LacEngine::trace(eval(tokens,op_pos+2,r-1));
            case FunctionType::RANK:
                return LacEngine::rank(eval(tokens,op_pos+2,r-1));
            case FunctionType::DETERMINANT:
                return LacEngine::det(eval(tokens,op_pos+2,r-1));
            case FunctionType::INVERSE:
                return LacEngine::inverse(eval(tokens,op_pos+2,r-1));
            case FunctionType::ADJOINT:
                return LacEngine::adjoint(eval(tokens,op_pos+2,r-1));
            case FunctionType::RREF:
                return LacEngine::rref(eval(tokens,op_pos+2,r-1));
            case FunctionType::POWER:{
                int comma_pos = find_comma(tokens,l,r);
                return LacEngine::pow(
                    eval(tokens,op_pos+2,comma_pos-1),
                    static_cast<int>(eval(tokens,comma_pos+1,r-1))
                );
            }
            case FunctionType::SOLVE:{
                int comma_pos = find_comma(tokens,l,r);
                return LacEngine::solve(
                    eval(tokens,op_pos+2,comma_pos-1),
                    eval(tokens,comma_pos+1,r-1)
                );
            }
            case FunctionType::SOLVELSQ:{
                int comma_pos = find_comma(tokens,l,r);
                return LacEngine::solveLeastSquares(
                    eval(tokens,op_pos+2,comma_pos-1),
                    eval(tokens,comma_pos+1,r-1)
                );
            }
            // returns a eigen vector matrix
            case FunctionType::EIGEN:{
                // todo: switch to a common version later
                auto pair = LacEngine::eigenValuesSymmetric(eval(tokens,op_pos+2,r-1));
                memory_["eigen_val_Ans"] = pair.first;
                memory_["eigen_vec_Ans"] = pair.second;
                return pair.second;
            }
        }
    }
    throw ParseException("Unknown eval error");
}
bool Parser::check_parentheses(const std::vector<Token>& tokens, int l, int r){
    if(tokens[r].token_type_!=TokenType::R_PAREN || 
        tokens[l].token_type_!=TokenType::L_PAREN)
        return false;
    int index, cnt=0;
    for (index=l;index<r;index++){
        if(tokens[index].token_type_==TokenType::L_PAREN) 
            cnt++;
        else if(tokens[index].token_type_==TokenType::R_PAREN) 
            cnt--;
        if(cnt<0)
            throw ParseException("Parentheses dismatch.");
        else if(cnt==0)
            return false;
    }
    cnt--;
    if(cnt==0) return true;
    else throw ParseException("Parentheses dismatch.");
}
int Parser::find_main_operator(const std::vector<Token>& tokens, int l, int r){
    // calculate order: () -->  func --> ' --> ^ --> */ --> +- --> =
    std::vector<bool> in_paren(r-l+1);
    int cnt = 0;
    for(int i=l; i<=r; i++){
        auto& token = tokens[i];
        if(token.token_type_==TokenType::L_PAREN){
            cnt++;
            in_paren[i] = true;
        }
        else if(token.token_type_==TokenType::R_PAREN){
            cnt--;
            in_paren[i] = true;
        }
        else if(cnt>0)
            in_paren[i] = true;
        else
            in_paren[i] = false;
    }
    //check =
    for(int i=l;i<=r;i++){
        const Token& token = tokens[i];
        if(!in_paren[i] && token.token_type_==TokenType::OPERATOR && 
            token.op()==OperatorType::ASSIGN){
            return i;
        }
    }
    //check +-
    for(int i=r;i>=l;i--){
        const Token& token = tokens[i];
        if(!in_paren[i] && token.token_type_==TokenType::OPERATOR && 
            (token.op()==OperatorType::ADD || token.op()==OperatorType::SUBTRACT)){
            return i;
        }
    }
    //check */
    for(int i=r;i>=l;i--){
        const Token& token = tokens[i];
        if(!in_paren[i] && token.token_type_==TokenType::OPERATOR && 
            (token.op()==OperatorType::MULTIPLY || token.op()==OperatorType::DIVIDE)){
            return i;
        }
    }
    //check ^
    for(int i=l;i<=r;i++){
        const Token& token = tokens[i];
        if(!in_paren[i] && token.token_type_==TokenType::OPERATOR && 
            token.op()==OperatorType::POWER){
            return i;
        }
    }
    //check '
    for(int i=l;i<=r;i++){
        const Token& token = tokens[i];
        if(!in_paren[i] && token.token_type_==TokenType::OPERATOR && 
            token.op()==OperatorType::TRANSPOSE){
            return i;
        }
    }
    //check function
    for(int i=r;i>=l;i--){
        const Token& token = tokens[i];
        if(!in_paren[i] && token.token_type_==TokenType::FUNCTION){
            return i;
        }
    }
    throw ParseException("Cannot find main operator.");
}
int Parser::find_comma(const std::vector<Token>& tokens, int l, int r){
    for (int i=l; i<=r; i++){
        if(tokens[i].token_type_==TokenType::COMMA){
            return i;
        }
    }
    throw ParseException("Comma required to seperate arguments.");
}
} // namespace Lac