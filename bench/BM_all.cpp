#define ANKERL_NANOBENCH_IMPLEMENT
#include <nanobench.h>

#include <ldsgen/lds.hpp>
#include <ldsgen/sphere_n.hpp>
#include <vector>

int main() {
    {
        ankerl::nanobench::Bench bench;
        bench.title("lds-gen-cpp (runtime, atomic) - Full Benchmark")
            .unit("op")
            .warmup(100)
            .epochs(50)
            .minEpochIterations(10000);

        ldsgen::VdCorput v(2);
        v.reseed(0);
        bench.run("VdCorput base 2 [double]", [&] {
            auto r = v.pop();
            ankerl::nanobench::doNotOptimizeAway(r);
        });

        ldsgen::Halton h(2, 3);
        h.reseed(0);
        bench.run("Halton [2,3] [double;2]", [&] {
            auto r = h.pop();
            ankerl::nanobench::doNotOptimizeAway(r);
        });

        ldsgen::Circle c(2);
        c.reseed(0);
        bench.run("Circle base 2 [double;2]", [&] {
            auto r = c.pop();
            ankerl::nanobench::doNotOptimizeAway(r);
        });

        ldsgen::Disk d(2, 3);
        d.reseed(0);
        bench.run("Disk [2,3] [double;2]", [&] {
            auto r = d.pop();
            ankerl::nanobench::doNotOptimizeAway(r);
        });

        ldsgen::Sphere s(2, 3);
        s.reseed(0);
        bench.run("Sphere [2,3] [double;3]", [&] {
            auto r = s.pop();
            ankerl::nanobench::doNotOptimizeAway(r);
        });

        ldsgen::Sphere3Hopf h3(2, 3, 5);
        h3.reseed(0);
        bench.run("Sphere3Hopf [2,3,5] [double;4]", [&] {
            auto r = h3.pop();
            ankerl::nanobench::doNotOptimizeAway(r);
        });
    }

    {
        ankerl::nanobench::Bench bench;
        bench.title("lds-gen-cpp Sphere-N (vector) generators")
            .unit("op")
            .warmup(100)
            .epochs(50)
            .minEpochIterations(1000);

        ldsgen::Sphere3 s3(std::vector<unsigned long>{2, 3, 5});
        s3.reseed(0);
        bench.run("Sphere3 [2,3,5] Vec<double>(4)", [&] {
            auto r = s3.pop();
            ankerl::nanobench::doNotOptimizeAway(r);
        });

        ldsgen::SphereN sn4(std::vector<unsigned long>{2, 3, 5, 7});
        sn4.reseed(0);
        bench.run("SphereN [2,3,5,7] Vec<double>(5)", [&] {
            auto r = sn4.pop();
            ankerl::nanobench::doNotOptimizeAway(r);
        });

        ldsgen::SphereN sn5(std::vector<unsigned long>{2, 3, 5, 7, 11});
        sn5.reseed(0);
        bench.run("SphereN [2,3,5,7,11] Vec<double>(6)", [&] {
            auto r = sn5.pop();
            ankerl::nanobench::doNotOptimizeAway(r);
        });
    }
}
