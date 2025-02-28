#pragma once

// Require C++17

#include <cstdint>
#include <tuple>

// Inline function to simplify the iterative logic
template <typename T> constexpr std::tuple<T, T> div_mod_3_iter(T input) {
    T q = input >> 2;              // Equivalent to extracting upper bits
    T r = q + (input & 0x03);      // Equivalent to extracting lower 2 bits
    return std::make_tuple(q, r);  // Return the quotient and sum of q and r
}

// Function for u8 division by 3
constexpr std::tuple<uint8_t, uint8_t> div_mod_3_u8(uint8_t n) {
    // Perform the iterations using the inline function
    auto [q1, rem1] = div_mod_3_iter(n);     // First iteration
    auto [q2, rem2] = div_mod_3_iter(rem1);  // Second iteration
    auto [q3, rem3] = div_mod_3_iter(rem2);  // Third iteration
    auto [q4, rem4] = div_mod_3_iter(rem3);  // Fourth iteration

    // Calculate the final quotient sum
    uint8_t quotient_sum = q1 + q2 + q3 + q4;

    // Final check and output assignment
    if (rem4 == 0x03) {  // Equivalent to rem4 == 2'b11
        return {++quotient_sum,
                uint8_t(0x00)};  // Equivalent to quotient_sum + 1 and remainder 2'b00
    } else {
        return {quotient_sum, rem4};  // Equivalent to quotient_sum and rem4[1:0]
    }
}

// Function for u16 division by 3
constexpr std::tuple<uint16_t, uint16_t> div_mod_3_u16(uint16_t n) {
    // Perform the iterations using the inline function
    auto [q1, rem1] = div_mod_3_iter(n);     // First iteration
    auto [q2, rem2] = div_mod_3_iter(rem1);  // Second iteration
    auto [q3, rem3] = div_mod_3_iter(rem2);  // Third iteration
    auto [q4, rem4] = div_mod_3_iter(rem3);  // Fourth iteration
    auto [q5, rem5] = div_mod_3_iter(rem4);  // 5th iteration
    auto [q6, rem6] = div_mod_3_iter(rem5);  // 6th iteration
    auto [q7, rem7] = div_mod_3_iter(rem6);  // 7th iteration
    auto [q8, rem8] = div_mod_3_iter(rem7);  // 8th iteration

    // Calculate the final quotient sum
    uint16_t quotient_sum = q1 + q2 + q3 + q4 + q5 + q6 + q7 + q8;

    // Final check and output assignment
    if (rem8 == 0x03) {  // Equivalent to rem8 == 2'b11
        return {++quotient_sum,
                uint16_t(0x00)};  // Equivalent to quotient_sum + 1 and remainder 2'b00
    } else {
        return {quotient_sum, rem8};  // Equivalent to quotient_sum and rem8[1:0]
    }
}
