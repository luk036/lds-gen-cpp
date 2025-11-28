#pragma once

#if __cpp_constexpr >= 201304
#    define CONSTEXPR14 constexpr
#else
#    define CONSTEXPR14 inline
#endif

#include <array>
#include <cmath>

namespace ildsgen {

    using std::array;

    /**
     * @brief Van der Corput sequence generator
     *
     * Implementation based on pre-calculating the scale factor.
     *
     */
    class VdCorput {
        size_t _base;
        size_t _count;
        size_t _factor;

      public:
        /**
         * @brief Construct a new VdCorput object
         *
         * @param[in] base The base of the number system (default: 2)
         * @param[in] scale The number of digits (default: 10)
         */
        explicit VdCorput(size_t base = 2, unsigned int scale = 10)
            : _base{base}, _count{0} {
            // Python: self._factor = base**scale
            // We use static_cast because std::pow returns double
            this->_factor = static_cast<size_t>(std::pow(base, scale));
        }

        /**
         * @brief Increments count and calculates the next value in the sequence.
         *
         * @return size_t
         */
        [[nodiscard]] auto pop() -> size_t {
            this->_count += 1;
            
            size_t k = this->_count;
            size_t vdc = 0;
            size_t factor = this->_factor;

            while (k != 0) {
                // Python: factor //= self._base
                factor /= this->_base;
                
                // Python: remainder = k % self._base
                const size_t remainder = k % this->_base;
                
                // Python: k //= self._base
                k /= this->_base;
                
                // Python: vdc += remainder * factor
                vdc += remainder * factor;
            }
            return vdc;
        }

        /**
         * @brief Resets the state of the sequence generator.
         *
         * @param[in] seed
         */
        auto reseed(const size_t seed) -> void {
            this->_count = seed;
        }
    };

    /**
     * @brief Halton sequence generator
     *
     * ```svgbob
     *     Integer Halton([2,3], [2,2]):
     *     pop() -> [1, 4]   (VdC_i(2,2,2), VdC_i(2,3,2))
     *     pop() -> [2, 5]   (next in each sequence)
     *     ...
     * ```
     */
    class Halton {
        VdCorput vdc0;
        VdCorput vdc1;

      public:
        /**
         * @brief Construct a new Halton object
         *
         * @param[in] base
         * @param[in] scale
         */
        explicit Halton(const size_t base[], const unsigned int scale[])
            : vdc0(base[0], scale[0]), vdc1(base[1], scale[1]) {}

        /**
         * @brief
         *
         * @return array<size_t, 2>
         */
        inline auto pop() -> array<size_t, 2> {  //
            return {this->vdc0.pop(), this->vdc1.pop()};
        }

        /**
         * @brief
         *
         * @param[in] seed
         */
        auto reseed(const size_t seed) -> void {
            this->vdc0.reseed(seed);
            this->vdc1.reseed(seed);
        }
    };

}  // namespace ildsgen
