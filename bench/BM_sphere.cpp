#define ANKERL_NANOBENCH_IMPLEMENT
#include <nanobench.h>

#include <ldsgen/sphere_n.hpp>
#include <vector>

int main() {
    {
        ankerl::nanobench::Bench bench;
        bench.title("Sphere-N Benchmarks (C++)")
            .unit("op")
            .warmup(100)
            .epochs(50)
            .minEpochIterations(1000);

        ldsgen::Sphere3 s3(std::vector<unsigned long>{2, 3, 5});
        s3.reseed(0);
        bench.run("Sphere3 [2,3,5] 4D", [&] {
            auto r = s3.pop();
            ankerl::nanobench::doNotOptimizeAway(r);
        });

        ldsgen::SphereN sn4(std::vector<unsigned long>{2, 3, 5, 7});
        sn4.reseed(0);
        bench.run("SphereN [2,3,5,7] 5D", [&] {
            auto r = sn4.pop();
            ankerl::nanobench::doNotOptimizeAway(r);
        });

        ldsgen::SphereN sn5(std::vector<unsigned long>{2, 3, 5, 7, 11});
        sn5.reseed(0);
        bench.run("SphereN [2,3,5,7,11] 6D", [&] {
            auto r = sn5.pop();
            ankerl::nanobench::doNotOptimizeAway(r);
        });
    }
}
