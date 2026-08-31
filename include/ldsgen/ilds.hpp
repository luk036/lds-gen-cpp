#pragma once

/** @file ilds.hpp
 *  @brief Integer low-discrepancy sequence generators with thread-safe runtime (ildsgen).
 */

#include <array>
#include <atomic>

#include "lds.hpp"

namespace ildsgen {

    using std::array;

    // Constants for magic numbers
    /**
     * @brief Default number of digits for the Van der Corput sequence
     *
     * This value determines the precision/scale of the integer Halton sequence.
     * Default is 10 digits.
     */
    constexpr unsigned int DEFAULT_SCALE = 10;
    constexpr unsigned int MAX_REVERSE_BITS = 64;

    /**
     * @brief Van der Corput sequence generator
     *
     * Implementation based on pre-calculating the scale factor.
     *
     */
    class VdCorput : public ldsgen::GeneratorBase<VdCorput, unsigned long> {
        unsigned long _base;  ///< Base of the number system
        std::array<unsigned long, MAX_REVERSE_BITS>
            factor_lst{};  ///< Precomputed scale factors for each digit
        static_assert(MAX_REVERSE_BITS >= sizeof(unsigned long) * 8,
                      "MAX_REVERSE_BITS must be at least the number of bits in unsigned long");

      public:
        /**
         * @brief Construct a new VdCorput object
         *
         * @param[in] base The base of the number system (default: 2)
         * @param[in] scale The number of digits (default: 10)
         */
        explicit VdCorput(unsigned long base = 2, unsigned int scale = DEFAULT_SCALE)
            : _base{base} {
            unsigned long factor = 1;
            unsigned int n = scale < MAX_REVERSE_BITS ? scale : MAX_REVERSE_BITS;
            for (unsigned int i = 0; i < n; ++i) {
                factor_lst[n - 1 - i] = factor;
                factor *= _base;
            }
        }

        /**
         * @brief Evaluate the integer sequence value at a given index (pure, no state change)
         *
         * @f[
         *     \phi_b^{\mathbb{Z}}(n) = \sum_{k=0}^{\infty} a_k(n) \cdot \mathrm{factor}_k
         * @f]
         * where \f$\mathrm{factor}_k = b^{\mathrm{scale}-1-k}\f$ scales the reversed
         * base-b digits into an integer.
         *
         * @param[in] n The sequence index.
         * @return The integer Van der Corput value for index n.
         */
        [[nodiscard]] auto value_at(unsigned long n) const -> unsigned long {
            return ldsgen::detail::vdc_digit_sum<unsigned long>(n, this->_base, this->factor_lst);
        }
    };

    /**
     * @brief Halton sequence generator
     *
     * @verbatim
     *     Integer Halton([2,3], [2,2]):
     *     pop() -> [1, 4]   (VdC_i(2,2,2), VdC_i(2,3,2))
     *     pop() -> [2, 5]   (next in each sequence)
     *     ...
     * @endverbatim
     */
    class Halton : public ldsgen::GeneratorBase<Halton, array<unsigned long, 2>> {
        VdCorput vdc0;
        VdCorput vdc1;

      public:
        /**
         * @brief Construct a new Halton object
         *
         * Constructs a Halton sequence generator with the specified bases and scale values.
         *
         * @param[in] base array of two unsigned long values representing the bases for the two Van
         * der Corput generators
         * @param[in] scale array of two unsigned int values representing the number of digits for
         * each generator
         */
        explicit Halton(const std::array<unsigned long, 2>& base,
                        const std::array<unsigned int, 2>& scale)
            : vdc0(base[0], scale[0]), vdc1(base[1], scale[1]) {}

        /**
         * @brief Evaluate the integer 2D Halton point at a given index (pure, no state change)
         *
         * @f[
         *     H(n) = (\phi_{b_1}^{\mathbb{Z}}(n), \phi_{b_2}^{\mathbb{Z}}(n))
         * @f]
         *
         * @param[in] n The sequence index.
         * @return The integer 2D Halton point for index n.
         */
        [[nodiscard]] auto value_at(unsigned long n) const -> array<unsigned long, 2> {
            return {this->vdc0.value_at(n), this->vdc1.value_at(n)};
        }
    };

    // Compile-time contract checks: the integer generators satisfy the protocol concept.
    static_assert(ldsgen::SequenceGenerator<VdCorput, unsigned long>);
    static_assert(ldsgen::SequenceGenerator<Halton, array<unsigned long, 2>>);

}  // namespace ildsgen
