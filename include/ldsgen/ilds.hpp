#pragma once

/** @file ilds.hpp
 *  @brief Integer low-discrepancy sequence generators with thread-safe runtime (ildsgen).
 */

#include <array>
#include <atomic>

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
    class VdCorput {
        unsigned long _base;                ///< Base of the number system
        std::atomic<unsigned long> _count;  ///< Current count in the sequence
        // unsigned long _factor;              ///< Precomputed scale factor (base^scale)
        std::array<unsigned long, MAX_REVERSE_BITS>
            factor_lst;  ///< Precomputed scale factors for each digit
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
            : _base{base}, _count{0}, factor_lst{} {
            unsigned long factor = 1;
            unsigned int n = scale < MAX_REVERSE_BITS ? scale : MAX_REVERSE_BITS;
            for (unsigned int i = 0; i < n; ++i) {
                factor_lst[n - 1 - i] = factor;
                factor *= _base;
            }
        }

        /**
         * @brief Increments count and calculates the next value in the sequence.
         *
         * @return unsigned long
         */
        [[nodiscard]] auto pop() -> unsigned long {
            this->_count.fetch_add(1, std::memory_order_relaxed);

            unsigned long count = this->_count.load(std::memory_order_relaxed);
            unsigned long reslt = 0;
            unsigned int idx = 0;
            while (count != 0) {
                const unsigned long remainder = count % this->_base;
                count /= this->_base;
                reslt += remainder * this->factor_lst[idx];
                ++idx;
            }
            return reslt;
        }

        /**
         * @brief Resets the state of the sequence generator.
         *
         * @param[in] seed
         */
        auto reseed(const unsigned long& seed) -> void {
            this->_count.store(seed, std::memory_order_relaxed);
        }

        VdCorput(VdCorput&&) noexcept = delete;
        VdCorput& operator=(VdCorput&&) noexcept = delete;
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
    class Halton {
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
         * @brief Generate the next point in the Halton sequence
         *
         * Returns the next point in the Halton sequence as an array of two unsigned long values.
         *
         * @return array<unsigned long, 2> the next point in the sequence
         */
        inline auto pop() -> array<unsigned long, 2> {  //
            return {this->vdc0.pop(), this->vdc1.pop()};
        }

        /**
         * @brief Reset the state of the Halton sequence generator
         *
         * Resets the state of the sequence generator to a specific seed value.
         *
         * @param[in] seed the seed value to reset the sequence generator to
         */
        auto reseed(const unsigned long& seed) -> void {
            this->vdc0.reseed(seed);
            this->vdc1.reseed(seed);
        }
    };

}  // namespace ildsgen
