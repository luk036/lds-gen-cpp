#include <doctest/doctest.h>  // for ResultBuilder, TestCase, CHECK

#include <ldsgen/ilds.hpp>  // for Halton

TEST_CASE("vdc_i") {
    CHECK_EQ(ildsgen::vdc_i(1, 2, 10), 512);
}

TEST_CASE("VdCorput_i") {
    auto vgen = ildsgen::VdCorput(2, 10);
    CHECK_EQ(vgen.pop(), 512);
    vgen.reseed(0);
    CHECK_EQ(vgen.pop(), 512);
}

TEST_CASE("Halton_i") {
    const size_t base[] = {2, 3};
    const unsigned int scale[] = {11, 7};
    auto hgen = ildsgen::Halton(base, scale);
    const auto arr = hgen.pop();
    CHECK_EQ(arr[0], 1024);
    CHECK_EQ(arr[1], 729);
    hgen.reseed(0);
    const auto arr2 = hgen.pop();
    CHECK_EQ(arr2[0], 1024);
    CHECK_EQ(arr2[1], 729);
}