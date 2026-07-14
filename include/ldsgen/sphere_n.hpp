#pragma once

/** @file sphere_n.hpp
 *  @brief N-dimensional sphere sequence generators (Sphere3, SphereN, SphereWrapper).
 */

#include <array>
#include <memory>
#include <mutex>
#include <span>
#include <vector>

#include "lds.hpp"

#ifndef M_PI
#    define M_PI 3.14159265358979323846264338327950288
#endif

namespace ldsgen {

    constexpr double PI = M_PI;
    constexpr double HALF_PI = M_PI / 2.0;

    /// @brief Number of interpolation points for sphere mapping tables
    constexpr std::size_t TABLE_SIZE = 300;

    /// @brief Precomputed interpolation tables (static storage, no heap)
    extern const std::array<double, TABLE_SIZE> X;
    extern const std::array<double, TABLE_SIZE> NEG_COSINE;
    extern const std::array<double, TABLE_SIZE> SINE;
    extern const std::array<double, TABLE_SIZE> F2;

    /**
     * @brief Generate evenly spaced numbers over a specified interval
     */
    std::vector<double> linspace(double start, double stop, unsigned int num);

    /**
     * @brief Perform one-dimensional linear interpolation
     */
    double simple_interp(double x_value, std::span<const double> x_points,
                         std::span<const double> y_points);

    /**
     * @brief Compute Tp(n) recurrence for sphere mapping.
     * @param n The recurrence index.
     * @return Vector of TABLE_SIZE interpolated values.
     */
    std::vector<double> get_tp(unsigned int n);

    /**
     * @brief Base class for sphere generators
     *
     * Provides a common interface for all sphere sequence generators.
     */
    class SphereGen {
      public:
        virtual ~SphereGen() = default;
        virtual std::vector<double> pop() = 0;
        virtual void reseed(unsigned long seed) = 0;
    };

    /**
     * @brief 3-Sphere sequence generator (standalone, returns std::array).
     *
     * Generates points on a 3-sphere using a combination of Van der Corput and Sphere generators.
     * Thread-safe (internal mutex). Returns std::array<double, 4> to avoid heap allocation per
     * pop().
     */
    class Sphere3 {
      public:
        explicit Sphere3(std::span<const unsigned long> base);
        std::array<double, 4> pop();
        void reseed(unsigned long seed);

      private:
        VdCorput vdc_;
        Sphere sphere2_;
        mutable std::mutex mutex_;
    };

    /**
     * @brief Wrapper class to make Sphere compatible with SphereGen interface.
     */
    class SphereWrapper : public SphereGen {
      public:
        explicit SphereWrapper(std::span<const unsigned long> base);
        std::vector<double> pop() override;
        void reseed(unsigned long seed) override;

      private:
        Sphere sphere_;
        mutable std::mutex mutex_;
    };

    /**
     * @brief N-dimensional sphere sequence generator.
     *
     * Precomputes tp_ in the constructor to avoid calling get_tp on every pop().
     */
    class SphereN : public SphereGen {
      public:
        explicit SphereN(std::span<const unsigned long> base);
        std::vector<double> pop() override;
        void reseed(unsigned long seed) override;

      private:
        VdCorput vdc_;
        std::unique_ptr<SphereGen> s_gen_;
        unsigned int n_;
        std::vector<double> tp_;
        double range_;
        mutable std::mutex mutex_;
    };
}  // namespace ldsgen