#include <doctest/doctest.h>  // for Approx, ResultBuilder, TestCase, CHECK

#include <ldsgen/lds.hpp>  // for Circle, Halton, Sphere, Sphere3Hopf

TEST_CASE("vdc") {
    CHECK_EQ(ldsgen::vdc(11, 2), doctest::Approx(0.8125));
}

TEST_CASE("VdCorput") {
    auto vgen = ldsgen::VdCorput(2);
    CHECK_EQ(vgen.pop(), doctest::Approx(0.5));
    CHECK_EQ(vgen.pop(), doctest::Approx(0.25));
    CHECK_EQ(vgen.pop(), doctest::Approx(0.75));
    CHECK_EQ(vgen.pop(), doctest::Approx(0.125));
    vgen.reseed(0);
    CHECK_EQ(vgen.pop(), doctest::Approx(0.5));
}

TEST_CASE("Circle") {
    auto cgen = ldsgen::Circle(2);
    const auto arr = cgen.pop();
    CHECK_EQ(arr[0], doctest::Approx(-1.0));
    CHECK_EQ(arr[1], doctest::Approx(0.0));
    cgen.reseed(0);
    const auto arr2 = cgen.pop();
    CHECK_EQ(arr2[0], doctest::Approx(-1.0));
    CHECK_EQ(arr2[1], doctest::Approx(0.0));
}

TEST_CASE("Disk") {
    auto dgen = ldsgen::Disk(2, 3);
    const auto arr = dgen.pop();
    CHECK_EQ(arr[0], doctest::Approx(-0.5773502692));
    dgen.reseed(0);
    const auto arr2 = dgen.pop();
    CHECK_EQ(arr2[0], doctest::Approx(-0.5773502692));
}

TEST_CASE("Halton") {
    auto hgen = ldsgen::Halton(2, 3);
    const auto arr = hgen.pop();
    CHECK_EQ(arr[0], doctest::Approx(0.5));
    hgen.reseed(0);
    const auto arr2 = hgen.pop();
    CHECK_EQ(arr2[0], doctest::Approx(0.5));
}

TEST_CASE("Sphere") {
    auto sgen = ldsgen::Sphere(2, 3);
    const auto arr = sgen.pop();
    CHECK_EQ(arr[0], doctest::Approx(-0.5));
    sgen.reseed(0);
    const auto arr2 = sgen.pop();
    CHECK_EQ(arr2[0], doctest::Approx(-0.5));
}

TEST_CASE("Sphere3Hopf") {
    auto shfgen = ldsgen::Sphere3Hopf(2, 3, 5);
    const auto arr = shfgen.pop();
    CHECK_EQ(arr[0], doctest::Approx(-0.22360679774997898));
    shfgen.reseed(0);
    const auto arr2 = shfgen.pop();
    CHECK_EQ(arr2[0], doctest::Approx(-0.22360679774997898));
}

TEST_CASE("dummy") { CHECK_EQ(ldsgen::dummy(15), 53); }
