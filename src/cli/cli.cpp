#include "parser/Parser.hpp"
#include "parser/Lexer.hpp"
#include "cli/cli.hpp"
#include <stdexcept>
#include <iostream>

Cli::Cli():memory_(), parser(memory_){}

Cli::Cli(std::unordered_map<std::string,Lac::LacMatrix>& memory):
    memory_(memory),parser(memory_){}

void Cli::run(){
    while(true){
        std::cout << ">>> ";
        std::string line;
        std::getline(std::cin, line);
        // command
        if(line[0]==':'){
            Command cmd = parse_command(line.substr(1));
            if(!execute(cmd)) return;
        }
        //expression
        else{
            expr_ += line;
            while(true){
                std::cout << "... ";
                std::getline(std::cin, line);
                if(line[0]==':'){
                    Command cmd = parse_command(line.substr(1));
                    if(cmd.cmd_!=CommandType::ABORT){
                        try{
                            calculate(expr_);
                        } catch(std::runtime_error& e){
                            std::cout << e.what() << std::endl;
                            std::cout << "Please try again or report this problem to gtyl2006@163.com." << std::endl;
                        }
                    }
                    expr_.clear();
                    if(!execute(cmd)) return;
                    break;
                }
                expr_ += (" " + line);
            }
        }
    }
}

void Cli::calculate(std::string expr){
    std::vector<Lac::Token> tokens = Lac::Lexer::tokenize(expr);
    parser.substitute(tokens);
    Lac::LacMatrix ans = parser.eval(tokens,0,tokens.size()-1);
    ans.rename("Ans");
    memory_["Ans"] = ans;
    std::cout << ans << std::endl;
}

Command Cli::parse_command(const std::string& line){
    std::istringstream sline(line);
    std::string cmd,reader;
    std::vector<std::string> args;
    sline >> cmd;
    while (sline >> reader){
        args.push_back(std::string(reader));
    }
    if(cmd=="help") 
        return Command(CommandType::HELP,args);
    if(cmd=="memory") 
        return Command(CommandType::MEMORY,args);
    if(cmd=="lookup") 
        return Command(CommandType::LOOKUP,args);
    if(cmd=="rename") 
        return Command(CommandType::RENAME,args);
    if(cmd=="delete")
        return Command(CommandType::DELETE,args);
    if(cmd=="clear") 
        return Command(CommandType::CLEAR,args);
    if(cmd=="exit") 
        return Command(CommandType::EXIT,args);
    if(cmd=="abort")
        return Command(CommandType::ABORT,args);
    return Command(CommandType::NONE,args);
}

bool Cli::execute(const Command& cmd){
    switch (cmd.cmd_){
        case CommandType::HELP:
            std::cout << "Help command invoked." << std::endl;
            handle_help(cmd.args_); 
            return true;
        case CommandType::MEMORY:
            std::cout << "Memory command invoked." << std::endl;
            handle_memory();
            return true;
        case CommandType::LOOKUP:
            std::cout << "Lookup command invoked." << std::endl;
            handle_lookup(cmd.args_);
            return true;
        case CommandType::RENAME:
            std::cout << "Rename command invoked." << std::endl;
            handle_rename(cmd.args_);
            return true;
        case CommandType::DELETE:
            std::cout << "Delete command invoked." << std::endl;
            handle_delete(cmd.args_);
            return true;
        case CommandType::CLEAR:
            std::cout << "Clear command invoked." << std::endl;
            memory_.clear();
            return true;
        case CommandType::EXIT:
            return false;
        default:
            return true;
    }
}

void Cli::handle_help(const std::vector<std::string> args) const{
    //display help msg
    if(args.empty()){
        std::cout << "Available commands:\n"
                  << "[help]    <command>        - Show help information\n"
                  << "[memory]                   - Display all matrices in memory\n"
                  << "[lookup]  <matrix_name>    - Lookup matrices by names\n"
                  << "[rename]  <old> <new>      - Rename a matrix\n"
                  << "[delete]  <matrix_name>    - Delete matrices by names\n"
                  << "[clear]                    - Clear all matrices from memory\n"
                  << "[abort]                    - Abort expression\n"
                  << "[exit]                     - Exit the Calculator\n";
    } else {
        for(const auto& command : args){
            if(command=="help"){
                std::cout << "[help]    <command>        - Show help information\n";
            } else if(command=="memory"){
                std::cout << "[memory]                   - Display all matrices in memory\n";
            } else if(command=="lookup"){
                std::cout << "[lookup]  <matrix_name>    - Lookup matrices by names\n";
            } else if(command=="rename"){
                std::cout << "[rename]  <old> <new>      - Rename a matrix\n";
            } else if(command=="delete"){
                std::cout << "[delete]  <matrix_name>    - Delete matrices by names\n";
            } else if(command=="clear"){
                std::cout << "[clear]                    - Clear all matrices from memory\n";
            } else if(command=="exit"){
                std::cout << "[exit]                     - Exit the Calculator\n";
            } else if(command=="abort"){
                std::cout << "[abort]                    - Abort expression\n";
            } else {
                std::cout << "No help available for command: " << command << std::endl;
            }
        }
    }

}

void Cli::handle_memory() const{
    for(const auto& pair : memory_){
        std::cout << pair.second << std::endl;
    }
}

void Cli::handle_lookup(const std::vector<std::string> args) const{
    for(const auto& name: args){
        auto it = memory_.find(name);
        if(it!=memory_.end()){
            std::cout << it->second << std::endl;
        } else {
            std::cout << "Matrix " << name << " not found." << std::endl;
        }
    }
}

void Cli::handle_rename(const std::vector<std::string> args){
    //todo: rename matrix
    if(args.size()!=2){
        std::cout << "Rename command requires exactly two arguments." << std::endl;
        return;
    }
    const std::string& old_name = args[0];
    const std::string& new_name = args[1];
    auto it = memory_.find(old_name);
    if(it==memory_.end()){
        std::cout << "Matrix " << old_name << " not found." << std::endl;
        return;
    }
    if(memory_.find(new_name)!=memory_.end()){
        std::cout << "Matrix " << new_name << " already exists." << std::endl;
        return;
    }
    Lac::LacMatrix mat = it->second;
    mat.rename(new_name);
    memory_.erase(it);
    memory_[new_name] = mat;
}

void Cli::handle_delete(const std::vector<std::string> args){
    //todo: delete matrix
    for(const auto& name: args){
        auto it = memory_.find(name);
        if(it!=memory_.end()){
            memory_.erase(it);
        }
    }
}