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
     * @brief Van der Corput sequence
     *
     * @param[in] k
     * @param[in] base
     * @param[in] scale
     * @return size_t
     */
    inline auto vdc_i(size_t k, size_t base, unsigned int scale) -> size_t {
        size_t vdc{0};
        auto factor = size_t(std::pow(base, scale));
        while (k != 0U) {
            const auto remainder = k % base;
            factor /= base;
            k /= base;
            vdc += remainder * factor;
        }
        return vdc;
    }

    /**
     * @brief Van der Corput sequence generator
     *
     */
    class VdCorput {
        size_t count;
        size_t base;
        unsigned int scale;

      public:
        /**
         * @brief Construct a new Vdcorputorput object
         *
         * @param[in] base
         * @param[in] scale
         */
        CONSTEXPR14 explicit VdCorput(size_t base, unsigned int scale)
            : count{0}, base{base}, scale{scale} {}

        /**
         * @brief
         *
         * @return size_t
         */
        inline auto pop() -> size_t {
            this->count += 1;
            return vdc_i(this->count, this->base, this->scale);
        }

        /**
         * @brief
         *
         * @param[in] seed
         * @return auto
         */
        CONSTEXPR14 auto reseed(size_t seed) -> void { this->count = seed; }
    };

    /**
     * @brief Halton sequence generator
     *
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
        CONSTEXPR14 explicit Halton(const size_t base[], const unsigned int scale[])
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
        CONSTEXPR14 auto reseed(size_t seed) -> void {
            this->vdc0.reseed(seed);
            this->vdc1.reseed(seed);
        }
    };

}  // namespace ildsgen
