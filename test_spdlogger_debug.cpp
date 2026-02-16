#include <ldsgen/logger.hpp>
#include <spdlog/spdlog.h>
#include <spdlog/sinks/basic_file_sink.h>
#include <iostream>
#include <fstream>

int main() {
    std::cout << "=== Testing spdlogger integration with debug output ===" << std::endl;

    // Test 1: Using the wrapper function
    std::cout << "\n[Test 1] Using ldsgen::log_with_spdlog()..." << std::endl;
    try {
        ldsgen::log_with_spdlog("Wrapper test message 1");
        std::cout << "  - Message 1 logged successfully" << std::endl;

        ldsgen::log_with_spdlog("Wrapper test message 2");
        std::cout << "  - Message 2 logged successfully" << std::endl;

        ldsgen::log_with_spdlog("Wrapper test message 3");
        std::cout << "  - Message 3 logged successfully" << std::endl;
    } catch (const std::exception& e) {
        std::cerr << "  - Error: " << e.what() << std::endl;
        return 1;
    }

    // Test 2: Direct spdlog usage (as a control)
    std::cout << "\n[Test 2] Using direct spdlog..." << std::endl;
    try {
        auto logger = spdlog::basic_logger_mt("test_logger", "test_debug_spdlog.log");
        logger->set_level(spdlog::level::info);
        logger->info("Direct test message 1");
        logger->info("Direct test message 2");
        logger->flush();
        std::cout << "  - Direct messages logged successfully" << std::endl;
    } catch (const spdlog::spdlog_ex& ex) {
        std::cerr << "  - Error: " << ex.what() << std::endl;
        return 1;
    }

    // Check if log files exist
    std::cout << "\n[Verification] Checking for log files..." << std::endl;

    auto check_file = [](const char* filename) {
        std::ifstream file(filename);
        if (file.good()) {
            std::cout << "  - " << filename << " exists" << std::endl;
            std::string line;
            int line_count = 0;
            while (std::getline(file, line)) {
                line_count++;
            }
            std::cout << "    Contains " << line_count << " lines" << std::endl;
            return true;
        } else {
            std::cout << "  - " << filename << " NOT found" << std::endl;
            return false;
        }
    };

    check_file("ldsgen.log");
    check_file("test_debug_spdlog.log");

    std::cout << "\n=== All tests completed ===" << std::endl;
    return 0;
}