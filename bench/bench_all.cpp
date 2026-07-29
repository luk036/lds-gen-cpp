#include <chrono>
#include <cmath>
#include <cstdio>
#include <ldsgen/sphere_n.hpp>

template <typename F> void bench(const char* name, F&& f, int iterations) {
    for (int i = 0; i < 1000; ++i) f();
    auto start = std::chrono::steady_clock::now();
    for (int i = 0; i < iterations; ++i) f();
    auto end = std::chrono::steady_clock::now();
    auto ns = std::chrono::duration<double, std::nano>(end - start).count() / iterations;
    std::printf("  %-35s %8.1f ns/op  (%d iters)\n", name, ns, iterations);
}

int main() {
    std::printf("=== lds-gen-cpp (runtime, atomic) - Full Benchmark ===\n\n");

    {
        ldsgen::VdCorput v(2);
        v.reseed(0);
        bench(
            "VdCorput base 2 [double]",
            [&]() {
                volatile auto r = v.pop();
                (void)r;
            },
            1000000);
    }
    {
        ldsgen::Halton h(2, 3);
        h.reseed(0);
        bench(
            "Halton [2,3] [double;2]",
            [&]() {
                volatile auto r = h.pop();
                (void)r;
            },
            500000);
    }
    {
        ldsgen::Circle c(2);
        c.reseed(0);
        bench(
            "Circle base 2 [double;2]",
            [&]() {
                volatile auto r = c.pop();
                (void)r;
            },
            500000);
    }
    {
        ldsgen::Disk d(2, 3);
        d.reseed(0);
        bench(
            "Disk [2,3] [double;2]",
            [&]() {
                volatile auto r = d.pop();
                (void)r;
            },
            500000);
    }
    {
        ldsgen::Sphere s(2, 3);
        s.reseed(0);
        bench(
            "Sphere [2,3] [double;3]",
            [&]() {
                volatile auto r = s.pop();
                (void)r;
            },
            500000);
    }
    {
        ldsgen::Sphere3Hopf h3(2, 3, 5);
        h3.reseed(0);
        bench(
            "Sphere3Hopf [2,3,5] [double;4]",
            [&]() {
                volatile auto r = h3.pop();
                (void)r;
            },
            500000);
    }
    {
        ldsgen::Sphere3 s3(std::vector<unsigned long>{2, 3, 5});
        s3.reseed(0);
        bench(
            "Sphere3 [2,3,5] Vec<double>(4)",
            [&]() {
                volatile auto r = s3.pop();
                (void)r;
            },
            50000);
    }
    {
        ldsgen::SphereN sn4(std::vector<unsigned long>{2, 3, 5, 7});
        sn4.reseed(0);
        bench(
            "SphereN [2,3,5,7] Vec<double>(5)",
            [&]() {
                volatile auto r = sn4.pop();
                (void)r;
            },
            50000);
    }
    {
        ldsgen::SphereN sn5(std::vector<unsigned long>{2, 3, 5, 7, 11});
        sn5.reseed(0);
        bench(
            "SphereN [2,3,5,7,11] Vec<double>(6)",
            [&]() {
                volatile auto r = sn5.pop();
                (void)r;
            },
            50000);
    }
}
