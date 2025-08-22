#include <doctest/doctest.h>  // for ResultBuilder, TestCase, CHECK

#include <ldsgen/ilds.hpp>  // for Halton
#include <vector>           // for vector

TEST_CASE("vdc_i") { CHECK_EQ(ildsgen::vdc_i(1, 2, 10), 512); }

TEST_CASE("vdc_i with different values") {
    CHECK_EQ(ildsgen::vdc_i(0, 2, 10), 0);
    CHECK_EQ(ildsgen::vdc_i(2, 2, 10), 256);
    CHECK_EQ(ildsgen::vdc_i(3, 2, 10), 768);
}

TEST_CASE("VdCorput_i") {
    auto vgen = ildsgen::VdCorput(2, 10);
    CHECK_EQ(vgen.pop(), 512);
    vgen.reseed(0);
    CHECK_EQ(vgen.pop(), 512);
}

TEST_CASE("VdCorput_i multiple pops") {
    auto vgen = ildsgen::VdCorput(2, 10);
    vgen.reseed(0);
    CHECK_EQ(vgen.pop(), 512);
    CHECK_EQ(vgen.pop(), 256);
    CHECK_EQ(vgen.pop(), 768);
    CHECK_EQ(vgen.pop(), 128);
}

TEST_CASE("VdCorput_i reseed with different values") {
    auto vgen = ildsgen::VdCorput(2, 10);
    vgen.reseed(1);
    CHECK_EQ(vgen.pop(), 256);
    vgen.reseed(2);
    CHECK_EQ(vgen.pop(), 768);
    vgen.reseed(0);
    CHECK_EQ(vgen.pop(), 512);
}

TEST_CASE("VdCorput_i different base and scale") {
    auto vgen = ildsgen::VdCorput(3, 8);
    vgen.reseed(0);
    CHECK_EQ(vgen.pop(), 2187);  // 3^7 = 2187
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

TEST_CASE("Halton_i different bases and scales") {
    const size_t base[] = {3, 5};
    const unsigned int scale[] = {6, 4};
    auto hgen = ildsgen::Halton(base, scale);
    hgen.reseed(0);

    auto res = hgen.pop();
    CHECK_EQ(res[0], 243);  // 3^5 = 243
    CHECK_EQ(res[1], 125);  // 5^3 = 125
}
