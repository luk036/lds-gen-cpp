#include <chrono>
#include <cstdio>
#include <ldsgen/sphere_n.hpp>

template <typename F>
void bench(const char* name, F&& f, int iterations = 50000) {
    for (int i = 0; i < 1000; ++i) f();
    auto start = std::chrono::steady_clock::now();
    for (int i = 0; i < iterations; ++i) f();
    auto end = std::chrono::steady_clock::now();
    auto ns = std::chrono::duration<double, std::nano>(end - start).count() / iterations;
    std::printf("  %-30s %8.1f ns/op  (%d iters)\n", name, ns, iterations);
}

int main() {
    std::printf("=== Sphere-N Benchmarks (C++) ===\n");

    ldsgen::Sphere3 s3(std::vector<unsigned long>{2, 3, 5});
    s3.reseed(0);
    bench("Sphere3 [2,3,5] 4D", [&]() { s3.pop(); });

    ldsgen::SphereN sn4(std::vector<unsigned long>{2, 3, 5, 7});
    sn4.reseed(0);
    bench("SphereN [2,3,5,7] 5D", [&]() { sn4.pop(); });

    ldsgen::SphereN sn5(std::vector<unsigned long>{2, 3, 5, 7, 11});
    sn5.reseed(0);
    bench("SphereN [2,3,5,7,11] 6D", [&]() { sn5.pop(); });
}
