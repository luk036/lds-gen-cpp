#pragma once

#include <array>
#include <atomic>
#include <cmath>

namespace ildsgen {

    using std::array;
    
    // Constants for magic numbers
    constexpr unsigned int DEFAULT_SCALE = 10;

    /**
     * @brief Van der Corput sequence generator
     *
     * Implementation based on pre-calculating the scale factor.
     *
     */
    class VdCorput {
        size_t _base;
        std::atomic<size_t> _count;
        size_t _factor;

      public:
        /**
         * @brief Construct a new VdCorput object
         *
         * @param[in] base The base of the number system (default: 2)
         * @param[in] scale The number of digits (default: 10)
         */
explicit VdCorput(size_t base = 2, unsigned int scale = DEFAULT_SCALE)
            : _base{base}, _count{0}, _factor{static_cast<size_t>(std::pow(base, scale))} {}

        /**
         * @brief Increments count and calculates the next value in the sequence.
         *
         * @return size_t
         */
        [[nodiscard]] auto pop() -> size_t {
            this->_count.fetch_add(1, std::memory_order_relaxed);

            size_t count = this->_count.load(std::memory_order_relaxed);
            size_t reslt = 0;
            size_t factor = this->_factor;

            while (count != 0) {
                const size_t remainder = count % this->_base;
                factor /= this->_base;
                count /= this->_base;
                reslt += remainder * factor;
            }
            return reslt;
        }

        /**
         * @brief Resets the state of the sequence generator.
         *
         * @param[in] seed
         */
        auto reseed(const size_t seed) -> void {
            this->_count.store(seed, std::memory_order_relaxed);
        }

        VdCorput(VdCorput&&) noexcept = delete;
        VdCorput& operator=(VdCorput&&) noexcept = delete;
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
        explicit Halton(const std::array<size_t, 2>& base, const std::array<unsigned int, 2>& scale)
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
