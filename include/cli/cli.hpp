#pragma once
#include "parser/Parser.hpp"
#include "parser/Lexer.hpp"
#include <unordered_map>
#include <string>

enum class CommandType{
    HELP,
    MEMORY,
    LOOKUP,
    RENAME,
    DELETE,
    CLEAR,
    EXIT,
    ABORT,
    NONE
};

class Command{
public:
    Command(CommandType cmd, std::initializer_list<std::string> args={}):
        cmd_(cmd),args_(args){}
    Command(CommandType cmd, const std::vector<std::string>& args={}):
        cmd_(cmd),args_(args){}
    CommandType cmd_;
    std::vector<std::string> args_;
};

class Cli{
public:
    Cli();
    Cli(std::unordered_map<std::string,Lac::LacMatrix> &memory);
    void run();
    void calculate(std::string expr);
    static Command parse_command(const std::string& line);
    // false for exit
    bool execute(const Command& cmd);
    void handle_help(const std::vector<std::string> args) const;
    void handle_memory() const;
    void handle_lookup(const std::vector<std::string> args) const;
    void handle_rename(const std::vector<std::string> args);
    void handle_delete(const std::vector<std::string> args);
    std::unordered_map<std::string,Lac::LacMatrix>& get_memory(){ return memory_; }
private:
    std::unordered_map<std::string,Lac::LacMatrix> memory_;
    Lac::Parser parser;
    std::string expr_;
};
