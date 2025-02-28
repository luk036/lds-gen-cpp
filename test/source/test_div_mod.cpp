#include <doctest/doctest.h>  // for ResultBuilder, TestCase, CHECK

#include <cstdint>
#include <ldsgen/div_mod.hpp>  // for div_mod_3
#include <tuple>

// Unit tests using doctest
TEST_CASE("Test div_mod_3_u8") {
    auto [q1, r1] = div_mod_3_u8(10);
    CHECK_EQ(q1, 3);
    CHECK_EQ(r1, 1);

    auto [q2, r2] = div_mod_3_u8(12);
    CHECK_EQ(q2, 4);
    CHECK_EQ(r2, 0);
}

TEST_CASE("Test div_mod_3_u16") {
    auto [q1, r1] = div_mod_3_u16(10000);
    CHECK_EQ(q1, 3333);
    CHECK_EQ(r1, 1);

    auto [q2, r2] = div_mod_3_u16(10002);
    CHECK_EQ(q2, 3334);
    CHECK_EQ(r2, 0);
}
