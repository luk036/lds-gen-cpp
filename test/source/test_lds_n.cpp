#include <doctest/doctest.h>  // for Approx, ResultBuilder, TestCase

#include <ldsgen/lds_n.hpp>  // for halton_n
#include <vector>

TEST_CASE("HaltonN") {
    const std::vector<size_t> base = {2, 3, 5, 7};
    auto hgen = ldsgen::HaltonN(base);
    const auto res = hgen.pop();
    CHECK_EQ(res[0], doctest::Approx(0.5));
}
