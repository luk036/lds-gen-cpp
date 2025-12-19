#include <ldsgen/lds.hpp>
#include <iostream>
#include <thread>
#include <vector>

int main() {
    // Test that VdCorput compiles with atomic changes
    ldsgen::VdCorput vgen(2);
    
    std::cout << "Testing atomic VdCorput:" << std::endl;
    for (int i = 0; i < 5; ++i) {
        std::cout << "pop() = " << vgen.pop() << std::endl;
    }
    
    vgen.reseed(0);
    std::cout << "\nAfter reseed(0):" << std::endl;
    for (int i = 0; i < 3; ++i) {
        std::cout << "pop() = " << vgen.pop() << std::endl;
    }
    
    // Test thread safety
    std::cout << "\nTesting thread safety..." << std::endl;
    ldsgen::VdCorput sharedVgen(3);
    std::vector<std::thread> threads;
    std::vector<double> results;
    
    for (int t = 0; t < 4; ++t) {
        threads.emplace_back([&sharedVgen, &results]() {
            for (int i = 0; i < 3; ++i) {
                results.push_back(sharedVgen.pop());
            }
        });
    }
    
    for (auto& t : threads) {
        t.join();
    }
    
    std::cout << "Got " << results.size() << " results from threads" << std::endl;
    
    return 0;
}