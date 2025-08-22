#include <doctest/doctest.h>  // for Approx, ResultBuilder, TestCase, CHECK

#include <ldsgen/lds.hpp>  // for Circle, Halton, Sphere, Sphere3Hopf

TEST_CASE("vdc") { CHECK_EQ(ldsgen::vdc(11, 2), doctest::Approx(0.8125)); }

TEST_CASE("VdCorput") {
    auto vgen = ldsgen::VdCorput(2);
    CHECK_EQ(vgen.pop(), doctest::Approx(0.5));
    CHECK_EQ(vgen.pop(), doctest::Approx(0.25));
    CHECK_EQ(vgen.pop(), doctest::Approx(0.75));
    CHECK_EQ(vgen.pop(), doctest::Approx(0.125));
    vgen.reseed(0);
    CHECK_EQ(vgen.pop(), doctest::Approx(0.5));
}

TEST_CASE("VdCorput::reseed with non-zero") {
    auto vgen = ldsgen::VdCorput(2);
    vgen.reseed(5);
    CHECK_EQ(vgen.pop(), doctest::Approx(0.375));
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

TEST_CASE("Circle::pop multiple") {
    auto cgen = ldsgen::Circle(2);
    cgen.reseed(0);
    auto res = cgen.pop();
    CHECK_EQ(res[0], doctest::Approx(-1.0));
    CHECK_EQ(res[1], doctest::Approx(0.0));
    res = cgen.pop();
    CHECK_EQ(res[0], doctest::Approx(0.0));
    CHECK_EQ(res[1], doctest::Approx(1.0));
}

TEST_CASE("Circle::reseed with non-zero") {
    auto cgen = ldsgen::Circle(2);
    cgen.reseed(2);
    auto res = cgen.pop();
    CHECK_EQ(res[0], doctest::Approx(0.0));
    CHECK_EQ(res[1], doctest::Approx(-1.0));
    cgen.reseed(0);
    res = cgen.pop();
    CHECK_EQ(res[0], doctest::Approx(-1.0));
    CHECK_EQ(res[1], doctest::Approx(0.0));
}

TEST_CASE("Disk") {
    auto dgen = ldsgen::Disk(2, 3);
    const auto arr = dgen.pop();
    CHECK_EQ(arr[0], doctest::Approx(-0.5773502692));
    dgen.reseed(0);
    const auto arr2 = dgen.pop();
    CHECK_EQ(arr2[0], doctest::Approx(-0.5773502692));
}

TEST_CASE("Disk::pop multiple") {
    auto dgen = ldsgen::Disk(2, 3);
    dgen.reseed(0);
    auto res = dgen.pop();
    CHECK_EQ(res[0], doctest::Approx(-0.5773502691896257));
    CHECK_EQ(res[1], doctest::Approx(0.0));
    res = dgen.pop();
    CHECK_EQ(res[0], doctest::Approx(0.0));
    CHECK_EQ(res[1], doctest::Approx(0.816496580927726));
}

TEST_CASE("Disk::reseed with non-zero") {
    auto dgen = ldsgen::Disk(2, 3);
    dgen.reseed(2);
    auto res = dgen.pop();
    CHECK_EQ(res[0], doctest::Approx(0.0));
    CHECK_EQ(res[1], doctest::Approx(-0.3333333333333333));
    dgen.reseed(0);
    res = dgen.pop();
    CHECK_EQ(res[0], doctest::Approx(-0.5773502691896257));
    CHECK_EQ(res[1], doctest::Approx(0.0));
}

TEST_CASE("Halton") {
    auto hgen = ldsgen::Halton(2, 3);
    const auto arr = hgen.pop();
    CHECK_EQ(arr[0], doctest::Approx(0.5));
    hgen.reseed(0);
    const auto arr2 = hgen.pop();
    CHECK_EQ(arr2[0], doctest::Approx(0.5));
}

TEST_CASE("Halton::pop multiple") {
    auto hgen = ldsgen::Halton(2, 3);
    hgen.reseed(0);
    auto res = hgen.pop();
    CHECK_EQ(res[0], doctest::Approx(0.5));
    CHECK_EQ(res[1], doctest::Approx(1.0 / 3.0));
    res = hgen.pop();
    CHECK_EQ(res[0], doctest::Approx(0.25));
    CHECK_EQ(res[1], doctest::Approx(2.0 / 3.0));
}

TEST_CASE("Halton::reseed with non-zero") {
    auto hgen = ldsgen::Halton(2, 3);
    hgen.reseed(5);
    auto res = hgen.pop();
    CHECK_EQ(res[0], doctest::Approx(0.375));
    CHECK_EQ(res[1], doctest::Approx(2.0 / 9.0));
    hgen.reseed(0);
    res = hgen.pop();
    CHECK_EQ(res[0], doctest::Approx(0.5));
    CHECK_EQ(res[1], doctest::Approx(1.0 / 3.0));
}

TEST_CASE("Sphere") {
    auto sgen = ldsgen::Sphere(2, 3);
    const auto arr = sgen.pop();
    CHECK_EQ(arr[0], doctest::Approx(-0.5));
    sgen.reseed(0);
    const auto arr2 = sgen.pop();
    CHECK_EQ(arr2[0], doctest::Approx(-0.5));
}

TEST_CASE("Sphere::pop multiple") {
    auto sgen = ldsgen::Sphere(2, 3);
    sgen.reseed(0);
    auto res = sgen.pop();
    CHECK_EQ(res[0], doctest::Approx(-0.5));
    CHECK_EQ(res[1], doctest::Approx(0.8660254037844387));
    CHECK_EQ(res[2], doctest::Approx(0.0));
    res = sgen.pop();
    CHECK_EQ(res[0], doctest::Approx(-0.4330127018922197));
    CHECK_EQ(res[1], doctest::Approx(-0.75));
    CHECK_EQ(res[2], doctest::Approx(-0.5));
}

TEST_CASE("Sphere::reseed with non-zero") {
    auto sgen = ldsgen::Sphere(2, 3);
    sgen.reseed(1);
    auto res = sgen.pop();
    CHECK_EQ(res[0], doctest::Approx(-0.4330127018922197));
    CHECK_EQ(res[1], doctest::Approx(-0.75));
    CHECK_EQ(res[2], doctest::Approx(-0.5));
    sgen.reseed(0);
    res = sgen.pop();
    CHECK_EQ(res[0], doctest::Approx(-0.5));
    CHECK_EQ(res[1], doctest::Approx(0.8660254037844387));
    CHECK_EQ(res[2], doctest::Approx(0.0));
}

TEST_CASE("Sphere3Hopf") {
    auto shfgen = ldsgen::Sphere3Hopf(2, 3, 5);
    const auto arr = shfgen.pop();
    CHECK_EQ(arr[0], doctest::Approx(-0.22360679774997898));
    shfgen.reseed(0);
    const auto arr2 = shfgen.pop();
    CHECK_EQ(arr2[0], doctest::Approx(-0.22360679774997898));
}

TEST_CASE("Sphere3Hopf::pop multiple") {
    auto sgen = ldsgen::Sphere3Hopf(2, 3, 5);
    sgen.reseed(0);
    auto res = sgen.pop();
    CHECK_EQ(res[0], doctest::Approx(-0.22360679774997898));
    CHECK_EQ(res[1], doctest::Approx(0.3872983346207417));
    CHECK_EQ(res[2], doctest::Approx(0.4472135954999573));
    CHECK_EQ(res[3], doctest::Approx(-0.7745966692414837));
}

TEST_CASE("Sphere3Hopf::reseed with non-zero") {
    auto sgen = ldsgen::Sphere3Hopf(2, 3, 5);
    sgen.reseed(1);
    auto res = sgen.pop();
    CHECK_EQ(res[0], doctest::Approx(-0.3162277660168382));
    CHECK_EQ(res[1], doctest::Approx(-0.547722557505166));
    CHECK_EQ(res[2], doctest::Approx(0.6708203932499367));
    CHECK_EQ(res[3], doctest::Approx(-0.38729833462074204));
    sgen.reseed(0);
    res = sgen.pop();
    CHECK_EQ(res[0], doctest::Approx(-0.22360679774997898));
    CHECK_EQ(res[1], doctest::Approx(0.3872983346207417));
    CHECK_EQ(res[2], doctest::Approx(0.4472135954999573));
    CHECK_EQ(res[3], doctest::Approx(-0.7745966692414837));
}

TEST_CASE("dummy") { CHECK_EQ(ldsgen::dummy(15), 53); }
