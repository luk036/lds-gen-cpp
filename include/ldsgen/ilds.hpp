#pragma once

#include <array>
#include <atomic>
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
        std::atomic<size_t> _base;
        std::atomic<size_t> _count;
        std::atomic<size_t> _factor;

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
            this->_factor.store(static_cast<size_t>(std::pow(base, scale)));
        }

        /**
         * @brief Increments count and calculates the next value in the sequence.
         *
         * @return size_t
         */
        [[nodiscard]] auto pop() -> size_t {
            this->_count.fetch_add(1, std::memory_order_relaxed);

            size_t count = this->_count.load(std::memory_order_relaxed);
            size_t reslt = 0;
            size_t factor = this->_factor.load(std::memory_order_relaxed);
            size_t base = this->_base.load(std::memory_order_relaxed);

            while (count != 0) {
                // Python: factor //= self._base
                factor /= base;

                // Python: remainder = count % self._base
                const size_t remainder = count % base;

                // Python: count //= self._base
                count /= base;

                // Python: vdc += remainder * factor
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
        VdCorput& operator=(VdCorput&& other) noexcept = delete;

        // /**
        //  * @brief Move constructor
        //  */
        // VdCorput(VdCorput&& other) noexcept 
        //     : _base(other._base.load()), _count(other._count.load()), _factor(other._factor.load()) {}
        //
        // /**
        //  * @brief Move assignment operator
        //  */
        // VdCorput& operator=(VdCorput&& other) noexcept {
        //     if (this != &other) {
        //         _base.store(other._base.load());
        //         _count.store(other._count.load());
        //         _factor.store(other._factor.load());
        //     }
        //     return *this;
        // }
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
