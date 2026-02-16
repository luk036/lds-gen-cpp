#include <ldsgen/lds.hpp>
#include <ldsgen/logger.hpp>
#include <cxxopts.hpp>
#include <iostream>
#include <string>
#include <vector>

auto main(int argc, char** argv) -> int {
    cxxopts::Options options("LdsGen", "Low Discrepancy Sequence Generator");

    std::string sequence_type = "vdc";
    int count = 10;
    int base = 2;
    bool use_logger = false;

    // clang-format off
    options.add_options()
        ("h,help", "Show help")
        ("s,sequence", "Sequence type (vdc, halton, circle, disk, sphere)", cxxopts::value(sequence_type)->default_value("vdc"))
        ("n,count", "Number of points to generate", cxxopts::value(count)->default_value("10"))
        ("b,base", "Base for VdCorput sequence", cxxopts::value(base)->default_value("2"))
        ("l,logger", "Enable logging to file", cxxopts::value(use_logger)->default_value("false"))
    ;
    // clang-format on

    auto result = options.parse(argc, argv);

    if (result["help"].as<bool>()) {
        std::cout << options.help() << std::endl;
        return 0;
    }

    if (use_logger) {
        ldsgen::log_with_spdlog("LdsGen standalone started");
        ldsgen::log_with_spdlog("Generating " + std::to_string(count) + " points");
    }

    std::cout << "Generating " << count << " points using " << sequence_type << " sequence" << std::endl;

    if (sequence_type == "vdc") {
        ldsgen::VdCorput vdc(base);
        for (int i = 0; i < count; ++i) {
            auto point = vdc.pop();
            std::cout << "  Point " << i << ": " << point << std::endl;
        }
    } else if (sequence_type == "halton") {
        ldsgen::Halton halton(2, 3);
        for (int i = 0; i < count; ++i) {
            auto point = halton.pop();
            std::cout << "  Point " << i << ": (" << point[0] << ", " << point[1] << ")" << std::endl;
        }
    } else if (sequence_type == "circle") {
        ldsgen::Circle circle(base);
        for (int i = 0; i < count; ++i) {
            auto point = circle.pop();
            std::cout << "  Point " << i << ": (" << point[0] << ", " << point[1] << ")" << std::endl;
        }
    } else if (sequence_type == "disk") {
        ldsgen::Disk disk(2, 3);
        for (int i = 0; i < count; ++i) {
            auto point = disk.pop();
            std::cout << "  Point " << i << ": (" << point[0] << ", " << point[1] << ")" << std::endl;
        }
    } else if (sequence_type == "sphere") {
        ldsgen::Sphere sphere(2, 3);
        for (int i = 0; i < count; ++i) {
            auto point = sphere.pop();
            std::cout << "  Point " << i << ": (" << point[0] << ", " << point[1] << ", " << point[2] << ")" << std::endl;
        }
    } else {
        std::cerr << "Unknown sequence type: " << sequence_type << std::endl;
        std::cerr << "Available types: vdc, halton, circle, disk, sphere" << std::endl;
        return 1;
    }

    if (use_logger) {
        ldsgen::log_with_spdlog("Sequence generation completed");
    }

    std::cout << "Done!" << std::endl;
    return 0;
}
