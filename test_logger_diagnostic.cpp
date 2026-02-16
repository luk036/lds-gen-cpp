#include <spdlog/spdlog.h>
#include <spdlog/sinks/basic_file_sink.h>
#include <iostream>
#include <memory>
#include <fstream>

int main() {
    std::cout << "=== Logger Diagnostic Test ===" << std::endl;

    // Test 1: Create a fresh logger
    std::cout << "\n[Test 1] Creating fresh logger..." << std::endl;
    try {
        auto logger1 = spdlog::basic_logger_mt("test1", "test1.log");
        logger1->set_level(spdlog::level::info);
        logger1->info("Test 1 message");
        logger1->flush();
        std::cout << "  - Logger created and message logged" << std::endl;
    } catch (const spdlog::spdlog_ex& ex) {
        std::cerr << "  - Error: " << ex.what() << std::endl;
    }

    // Test 2: Try to get the same logger
    std::cout << "\n[Test 2] Getting existing logger..." << std::endl;
    try {
        auto logger2 = spdlog::get("test1");
        if (logger2) {
            logger2->info("Test 2 message");
            logger2->flush();
            std::cout << "  - Logger retrieved and message logged" << std::endl;
        } else {
            std::cout << "  - Logger not found" << std::endl;
        }
    } catch (const spdlog::spdlog_ex& ex) {
        std::cerr << "  - Error: " << ex.what() << std::endl;
    }

    // Test 3: Check if file exists
    std::cout << "\n[Test 3] Checking file existence..." << std::endl;
    std::ifstream file("test1.log");
    if (file.good()) {
        std::cout << "  - test1.log exists" << std::endl;
        std::string line;
        int count = 0;
        while (std::getline(file, line)) {
            std::cout << "    Line " << ++count << ": " << line << std::endl;
        }
    } else {
        std::cout << "  - test1.log NOT found" << std::endl;
    }

    std::cout << "\n=== Diagnostic complete ===" << std::endl;
    return 0;
}