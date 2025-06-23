#include <doctest/doctest.h>  // for Approx, ResultBuilder, TestCase, CHECK

#include <ldsgen/lds.hpp>  // for Circle, Halton, Sphere, Sphere3Hopf

TEST_CASE("Circle") {
    auto cgen = ldsgen::Circle(2);
    const auto arr = cgen.pop();
    CHECK_EQ(arr[1], doctest::Approx(0.0));
}

TEST_CASE("Disk") {
    auto dgen = ldsgen::Disk(2, 3);
    const auto arr = dgen.pop();
    CHECK_EQ(arr[0], doctest::Approx(-0.57735));
}

TEST_CASE("Halton") {
    auto hgen = ldsgen::Halton(2, 3);
    const auto arr = hgen.pop();
    CHECK_EQ(arr[0], doctest::Approx(0.5));
}

TEST_CASE("Sphere") {
    auto sgen = ldsgen::Sphere(2, 3);
    const auto arr = sgen.pop();
    CHECK_EQ(arr[1], doctest::Approx(0.8660254038));
}

TEST_CASE("Sphere3Hopf") {
    auto shfgen = ldsgen::Sphere3Hopf(2, 3, 5);
    const auto arr = shfgen.pop();
    CHECK_EQ(arr[0], doctest::Approx(-0.2236067977));
}

TEST_CASE("dummy") { CHECK_EQ(ldsgen::dummy(15), 53); }
