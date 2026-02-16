/**
 * Simple example demonstrating spdlogger integration in lds-gen-cpp
 *
 * This example shows how to use the ldsgen::log_with_spdlog() function
 * to log messages to a file.
 *
 * To build and run:
 *   xmake build test_final_spdlogger
 *   xmake run test_final_spdlogger
 */

#include <ldsgen/logger.hpp>
#include <ldsgen/lds.hpp>
#include <iostream>

int main() {
    std::cout << "LdsGen Spdlogger Example" << std::endl;
    std::cout << "=========================" << std::endl;

    // Example 1: Basic logging
    std::cout << "\nExample 1: Basic logging" << std::endl;
    ldsgen::log_with_spdlog("Application started");
    ldsgen::log_with_spdlog("Processing data...");

    // Example 2: Logging with sequence generation
    std::cout << "\nExample 2: Logging with sequence generation" << std::endl;
    ldsgen::VdCorput vdc(2);
    for (int i = 0; i < 5; ++i) {
        auto point = vdc.pop();
        std::cout << "  Generated point: " << point << std::endl;
    }
    ldsgen::log_with_spdlog("Sequence generation completed");

    std::cout << "\nCheck ldsgen.log for logged messages" << std::endl;

    return 0;
}