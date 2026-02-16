#include <ldsgen/logger.hpp>
#include <iostream>
#include <fstream>

int main() {
    std::cout << "Testing wrapper function only..." << std::endl;

    try {
        std::cout << "Calling ldsgen::log_with_spdlog()..." << std::endl;
        ldsgen::log_with_spdlog("Test message from wrapper");
        std::cout << "Function returned successfully" << std::endl;

        std::cout << "Checking if ldsgen.log exists..." << std::endl;
        std::ifstream file("ldsgen.log");
        if (file.good()) {
            std::cout << "File exists!" << std::endl;
            std::string line;
            while (std::getline(file, line)) {
                std::cout << "  " << line << std::endl;
            }
        } else {
            std::cout << "File does NOT exist" << std::endl;
        }

        return 0;
    } catch (const std::exception& e) {
        std::cerr << "Exception: " << e.what() << std::endl;
        return 1;
    }
}