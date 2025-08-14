#include <doctest/doctest.h>  // for Approx, ResultBuilder, TestCase

#include <ldsgen/lds_n.hpp>  // for halton_n
#include <vector>

TEST_CASE("HaltonN") {
    const std::vector<size_t> base = {2, 3, 5, 7};
    auto hgen = ldsgen::HaltonN(base);
    const auto res = hgen.pop();
    CHECK_EQ(res[0], doctest::Approx(0.5));
    CHECK_EQ(res[1], doctest::Approx(1.0/ 3.0));
    CHECK_EQ(res[2], doctest::Approx(0.2));
    CHECK_EQ(res[3], doctest::Approx(0.14285714285714285));
    hgen.reseed(0);
    const auto res2 = hgen.pop();
    CHECK_EQ(res2[0], doctest::Approx(0.5));
}
