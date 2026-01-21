#include "cli/cli.hpp"
#include <chrono>
#include <thread>
#include <iostream>

int main(){
    Cli cli;
    cli.run();
    std::cout << "Quiting Calculator. Goodbye!" << std::endl;
    // sleep for a while to show message
    std::this_thread::sleep_for(std::chrono::seconds(1));
    return 0;
}