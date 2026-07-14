#include <cstdio>
#include <ldsgen/sphere_n.hpp>

int main() {
    // Sphere3 = 4D
    ldsgen::Sphere3 s3(std::vector<unsigned long>{2, 3, 5});
    s3.reseed(0);
    std::printf("Sphere3 [2,3,5] seed=0, first 3 points:\n");
    for (int i = 0; i < 3; ++i) {
        auto p = s3.pop();
        std::printf("  %d: [%.16f, %.16f, %.16f, %.16f]\n", i, p[0], p[1], p[2], p[3]);
    }

    // SphereN with 4 bases -> 5D
    ldsgen::SphereN sn4(std::vector<unsigned long>{2, 3, 5, 7});
    sn4.reseed(0);
    std::printf("\nSphereN [2,3,5,7] -> 5D, seed=0, first 3 points:\n");
    for (int i = 0; i < 3; ++i) {
        auto p = sn4.pop();
        std::printf("  %d: [", i);
        for (std::size_t j = 0; j < p.size(); ++j) {
            if (j > 0) std::printf(", ");
            std::printf("%.16f", p[j]);
        }
        std::printf("]\n");
    }

    // SphereN with 5 bases -> 6D
    ldsgen::SphereN sn5(std::vector<unsigned long>{2, 3, 5, 7, 11});
    sn5.reseed(0);
    std::printf("\nSphereN [2,3,5,7,11] -> 6D, seed=0, first 3 points:\n");
    for (int i = 0; i < 3; ++i) {
        auto p = sn5.pop();
        std::printf("  %d: [", i);
        for (std::size_t j = 0; j < p.size(); ++j) {
            if (j > 0) std::printf(", ");
            std::printf("%.16f", p[j]);
        }
        std::printf("]\n");
    }
}
