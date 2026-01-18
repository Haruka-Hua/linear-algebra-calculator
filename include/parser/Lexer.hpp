#include <string>
#include <vector>
#include "parser/Token.hpp"

namespace Lac{

class Lexer{
public:
    std::vector<Token> tokenize(const std::string& input);
private:
    Token convert(const std::string& stoken);
    Token convert_number(const std::string& stoken);
    Token convert_identifier(const std::string& stream);
};
} //namespace Lac