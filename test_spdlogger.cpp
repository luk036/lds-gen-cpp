#include <ldsgen/logger.hpp>
#include <spdlog/spdlog.h>
#include <spdlog/sinks/basic_file_sink.h>
#include <iostream>

int main() {
    std::cout << "Testing spdlogger integration..." << std::endl;

    // Test basic logging functionality
    std::cout << "Logging first message..." << std::endl;
    ldsgen::log_with_spdlog("Starting spdlogger test");

    std::cout << "Logged a message to ldsgen.log (in build directory)" << std::endl;

    // Test with different messages
    std::cout << "Logging second message..." << std::endl;
    ldsgen::log_with_spdlog("Testing message 2");

    std::cout << "Logging third message..." << std::endl;
    ldsgen::log_with_spdlog("Testing message 3");

    std::cout << "Check build\\windows\\x64\\debug\\ldsgen.log for the logged messages" << std::endl;

    // Also test direct spdlog usage
    std::cout << "\nTesting direct spdlog usage..." << std::endl;
    auto direct_logger = spdlog::basic_logger_mt("direct_logger", "direct_spdlog.log");
    direct_logger->info("Direct spdlog message 1");
    direct_logger->info("Direct spdlog message 2");
    direct_logger->flush();
    std::cout << "Direct spdlog messages logged to direct_spdlog.log" << std::endl;

    std::cout << "\nAll tests completed successfully!" << std::endl;

    return 0;
}