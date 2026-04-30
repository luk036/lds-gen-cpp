#include <fstream>
#include <iostream>
#include <ldsgen/logger.hpp>

int main() {
    std::cout << "Testing wrapper function only...\n";

    try {
        std::cout << "Calling ldsgen::log_with_spdlog()...\n";
        ldsgen::log_with_spdlog("Test message from wrapper");
        std::cout << "Function returned successfully\n";

        std::cout << "Checking if ldsgen.log exists...\n";
        std::ifstream file("ldsgen.log");
        if (file.good()) {
            std::cout << "File exists!\n";
            std::string line;
            while (std::getline(file, line)) {
                std::cout << "  " << line << '\n';
            }
        } else {
            std::cout << "File does NOT exist\n";
        }

        return 0;
    } catch (const std::exception& e) {
        std::cerr << "Exception: " << e.what() << '\n';
        return 1;
    }
}