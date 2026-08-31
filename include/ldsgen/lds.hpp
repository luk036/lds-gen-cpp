#pragma once

/** @file lds.hpp
 *  @brief Low-discrepancy sequence generators with thread-safe runtime polymorphism (ldsgen).
 */

#include <array>
#include <atomic>
#include <cmath>
#include <concepts>
#include <cstddef>
#include <iterator>
#include <limits>

#ifndef M_PI
#    define M_PI 3.14159265358979323846264338327950288
#endif

namespace ldsgen {

    constexpr const auto TWO_PI = 2.0 * M_PI;

    // Constants for magic numbers
    constexpr unsigned int MAX_REVERSE_BITS = 64;
    constexpr double MAPPING_FACTOR = 2.0;

    namespace detail {

        /**
         * @brief Core base-b digit/weight summation shared by all van der Corput generators.
         *
         * Extracts the base-b digits of `n` (least significant first) and accumulates each
         * digit times its precomputed weight:
         * @f[
         *     \sum_k a_k(n) \cdot \mathrm{weights}[k], \qquad
         *     n = \sum_k a_k(n) \, b^k
         * @f]
         * The `weights` table determines the value type and scaling of the result: reverse
         * powers \f$b^{-k-1}\f$ for the floating-point generators, ascending integer powers
         * for the integer generators.
         *
         * @tparam T Accumulator/result type (e.g. double or unsigned long).
         * @tparam Table Random-access table type (e.g. std::array).
         * @param[in] n The sequence index to evaluate.
         * @param[in] base The numeric base.
         * @param[in] weights Precomputed digit weights.
         * @return The weighted digit sum for index `n`.
         */
        template <typename T, typename Table>
        constexpr auto vdc_digit_sum(unsigned long n, unsigned long base, const Table& weights)
            -> T {
            T reslt{};
            std::size_t idx = 0;
            while (n != 0) {
                const auto remainder = n % base;
                n /= base;
                reslt += static_cast<T>(remainder) * weights[idx];
                ++idx;
            }
            return reslt;
        }

    }  // namespace detail

    /**
     * @brief Forward iterator for sequence generators.
     *
     * Provides STL-compatible iterator interface for all generators.
     * Allows use in range-based for loops and STL algorithms.
     *
     * @verbatim
     * VdCorput gen(2);
     * std::vector<double> points(gen.begin(), gen.begin() + 100);
     * @endverbatim
     *
     * @tparam Generator The generator class type.
     * @tparam Value The value type (double or array).
     */
    template <typename Generator, typename Value> class GeneratorIterator {
        Generator* gen;
        unsigned long index;

      public:
        using iterator_category = std::input_iterator_tag;
        using value_type = Value;
        using difference_type = std::ptrdiff_t;
        using pointer = const value_type*;
        using reference = value_type;

        explicit GeneratorIterator(Generator* g = nullptr, unsigned long idx = 0)
            : gen{g}, index{idx} {}

        /**
         * @brief Dereference operator
         */
        auto operator*() const -> Value {
            if (gen) {
                auto temp_idx = gen->get_index();
                gen->reseed(index);
                auto value = gen->pop();
                gen->reseed(temp_idx);
                return value;
            }
            return Value{};
        }

        /**
         * @brief Pre-increment operator
         */
        auto operator++() -> GeneratorIterator& {
            ++index;
            return *this;
        }

        /**
         * @brief Post-increment operator
         */
        auto operator++(int) -> GeneratorIterator {
            auto temp = *this;
            ++index;
            return temp;
        }

        /**
         * @brief Equality comparison
         */
        auto operator==(const GeneratorIterator& other) const -> bool {
            return index == other.index;
        }

        /**
         * @brief Inequality comparison
         */
        auto operator!=(const GeneratorIterator& other) const -> bool {
            return index != other.index;
        }

        /**
         * @brief Get current index
         */
        [[nodiscard]] auto get_index() const -> unsigned long { return index; }
    };

    /**
     * @brief Concept for the sequence generator protocol
     *
     * Requires the uniform strategy interface shared by every generator: pop/peek/skip/
     * reseed/get_index. Every generator class in this library satisfies it, so generic code
     * can be written against the protocol without knowing the concrete generator.
     *
     * @tparam G The candidate generator type.
     * @tparam V The value type it produces.
     */
    template <typename G, typename V>
    concept SequenceGenerator = requires(G g, unsigned long n) {
        { g.pop() } -> std::convertible_to<V>;
        { g.peek() } -> std::convertible_to<V>;
        g.skip(static_cast<unsigned int>(n));
        g.reseed(n);
        { g.get_index() } -> std::convertible_to<unsigned long>;
    };

    /**
     * @brief CRTP base implementing the sequence generator protocol
     *
     * Implements the shared stateful protocol (pop/peek/skip/reseed/get_index) in terms of
     * a single pure computation `value_at(n)` supplied by the derived class. The sequence
     * counter is a `std::atomic<unsigned long>` so the protocol stays thread-safe: `pop()`
     * claims a unique index with a relaxed `fetch_add` and evaluates `value_at` on immutable
     * generator data.
     *
     * @note Template Method pattern via CRTP: the base fixes the protocol skeleton —
     * `pop()` evaluates `value_at(fetch_add(1) + 1)` and `peek()` evaluates
     * `value_at(load() + 1)` — while the derived class supplies only the pure index-to-value
     * computation. This guarantees pop/peek consistency by construction, centralizes the
     * sequence state in one atomic counter (making composite pop() atomic as a point), and
     * removes the duplicated protocol from every concrete generator.
     *
     * @tparam Derived The CRTP-derived generator class.
     * @tparam Value The value type produced by pop()/peek().
     */
    template <typename Derived, typename Value> class GeneratorBase {
      public:
        /**
         * @brief Generate the next value in the sequence (advances state).
         *
         * Atomically claims the next index and evaluates the pure value_at computation.
         *
         * @return The value at the incremented sequence index.
         */
        auto pop() -> Value {
            auto count_value
                = this->count_.fetch_add(1, std::memory_order_relaxed) + 1;  // ignore 0
            return derived().value_at(count_value);
        }

        /**
         * @brief Peek at the next value without advancing state.
         *
         * @return The value at the next sequence index.
         */
        [[nodiscard]] auto peek() -> Value {
            auto count_value = this->count_.load(std::memory_order_relaxed) + 1;
            return derived().value_at(count_value);
        }

        /**
         * @brief Skip n values in the sequence.
         *
         * @param[in] n number of values to skip
         */
        auto skip(unsigned int n) -> void { this->count_.fetch_add(n, std::memory_order_relaxed); }

        /**
         * @brief Reset the generator to a specific seed value.
         *
         * @param[in] seed the seed value to reset the sequence generator to
         */
        auto reseed(const unsigned long& seed) -> void {
            this->count_.store(seed, std::memory_order_relaxed);
        }

        /**
         * @brief Get current index in the sequence.
         *
         * @return unsigned long current index in the sequence
         */
        [[nodiscard]] auto get_index() const -> unsigned long {
            return this->count_.load(std::memory_order_relaxed);
        }

      protected:
        std::atomic<unsigned long> count_{0};  ///< Current sequence index (single source of state)

      private:
        friend Derived;  // for classes deriving directly from GeneratorBase (ilds.hpp, lds_n.hpp)
        template <typename, typename> friend class GeneratorIterable;
        GeneratorBase() = default;

        auto derived() -> Derived& { return static_cast<Derived&>(*this); }
    };

    /**
     * @brief CRTP mixin adding STL iterator support to a GeneratorBase.
     *
     * Provides begin()/end() so generators can be consumed with range-for, std::begin/
     * std::end and STL algorithms. Combined with GeneratorIterator, this realizes the
     * Iterator pattern for every generator without repeating the iterator boilerplate.
     *
     * @tparam Derived The CRTP-derived generator class.
     * @tparam Value The value type produced by the generator.
     */
    template <typename Derived, typename Value> class GeneratorIterable
        : public GeneratorBase<Derived, Value> {
      private:
        friend Derived;
        GeneratorIterable() = default;

      public:
        /**
         * @brief Get iterator to beginning
         *
         * @return GeneratorIterator<Derived, Value>
         */
        auto begin() -> GeneratorIterator<Derived, Value> {
            return GeneratorIterator<Derived, Value>(static_cast<Derived*>(this));
        }

        /**
         * @brief Get iterator to end (infinite sequence)
         *
         * For infinite sequences, you typically use begin() + n to get a specific position
         *
         * @return GeneratorIterator<Derived, Value>
         */
        [[nodiscard]] auto end() const -> GeneratorIterator<Derived, Value> {
            return GeneratorIterator<Derived, Value>(nullptr,
                                                     std::numeric_limits<unsigned long>::max());
        }
    };

    /**
     * @brief Van der Corput sequence
     *
     * The `vdc` function is calculating the Van der Corput sequence value for a
     * given index `count` and base `base`. It returns a `double` value.
     *
     * @verbatim
     *     Base 2 Example:
     *     count=1 -> 0.5  (0.1 in base 2)
     *     count=2 -> 0.25 (0.01 in base 2)
     *     count=3 -> 0.75 (0.11 in base 2)
     *     count=4 -> 0.125(0.001 in base 2)
     * @endverbatim
     *
     * @param[in] count index of the sequence
     * @param[in] base base of the sequence
     * @return double
     */
    constexpr auto vdc(unsigned long count, unsigned long base) -> double {
        auto reslt = 0.0;
        auto denom = 1.0;
        while (count != 0) {
            const auto remainder = count % base;
            count /= base;
            denom *= static_cast<double>(base);
            reslt += static_cast<double>(remainder) / denom;
        }
        return reslt;
    }

    /**
     * @brief Van der Corput sequence generator
     *
     * `VdCorput` is a class that generates the Van der Corput sequence. The Van der
     * Corput sequence is a low-discrepancy sequence that is commonly used in
     * quasi-Monte Carlo methods. The sequence is generated by iterating over a base
     * and calculating the fractional part of the number in that base. The
     * `VdCorput` class keeps track of the current count and base, and provides a
     * `pop()` method that returns the next value in the sequence.
     *
     * @verbatim
     *     VdCorput(2) sequence:
     *     pop() -> 0.5   (0.1 base 2)
     *     pop() -> 0.25  (0.01 base 2)
     *     pop() -> 0.75  (0.11 base 2)
     *     pop() -> 0.125 (0.001 base 2)
     *     ...
     * @endverbatim
     *
     * @dot
     *   digraph vdc_flow {
     *     rankdir=LR; bgcolor="transparent";
     *     node [shape=box, style=filled, fillcolor="#d4e6f1"];
     *     n [label="Input n\n(index)", fillcolor="#a9cce3"];
     *     base [label="Base b", fillcolor="#a9cce3"];
     *     digits [label="Extract base-b\ndigits a_k"];
     *     ratio [label="Compute\nratio a_k / b^{k+1}"];
     *     sum [label="Sum\nphi_b(n)", fillcolor="#7fb3d8"];
     *     n -> digits; base -> digits;
     *     digits -> ratio -> sum;
     *   }
     * @enddot
     */
    class VdCorput : public GeneratorIterable<VdCorput, double> {
        unsigned long base;
        std::array<double, MAX_REVERSE_BITS> rev_lst{};
        static_assert(MAX_REVERSE_BITS >= sizeof(unsigned long) * 8,
                      "MAX_REVERSE_BITS must be at least the number of bits in unsigned long");

      public:
        /**
         * @brief Construct a new VdCorput object
         *
         * The `VdCorput(unsigned long base)` constructor is initializing a `VdCorput`
         * object with a given base. The base is used to generate the Van der Corput
         * sequence.
         *
         * @param[in] base the base of the Van der Corput sequence
         */
        explicit VdCorput(const unsigned long base) : base{base} {
            double reverse = 1.0;
            for (unsigned int i = 0; i < MAX_REVERSE_BITS; ++i) {
                reverse /= static_cast<double>(base);
                this->rev_lst[i] = reverse;
            }
        }

        /**
         * @brief Evaluate the sequence value at a given index (pure, no state change)
         *
         * Computes the Van der Corput value for index \f$n\f$:
         * @f$ \phi_b(n) = \sum_{k=0}^{\infty} a_k(n) \, b^{-k-1} @f$
         * where @f$ a_k(n) @f$ are the base-@f$ b @f$ digits of @f$ n @f$.
         *
         * @param[in] n The sequence index.
         * @return The Van der Corput value for index n.
         */
        auto value_at(unsigned long n) const -> double {
            return detail::vdc_digit_sum<double>(n, this->base, this->rev_lst);
        }
    };

    /**
     * @brief Halton sequence generator
     *
     * The `Halton` class is a sequence generator that generates points in a
     * 2-dimensional space using the Halton sequence. The Halton sequence is a
     * low-discrepancy sequence that is commonly used in quasi-Monte Carlo methods.
     * It is generated by iterating over two different bases and calculating the
     * fractional parts of the numbers in those bases. The `Halton` class keeps
     * track of the current count and bases, and provides a `pop()` method that
     * returns the next point in the sequence as a `std::array<double, 2>`.
     *
     * @verbatim
     *     Halton(2,3) sequence:
     *     pop() -> (0.5, 0.333)  (VdC(2) -> 0.5, VdC(3) -> 0.333)
     *     pop() -> (0.25, 0.666) (VdC(2) -> 0.25, VdC(3) -> 0.666)
     *     pop() -> (0.75, 0.111) (VdC(2) -> 0.75, VdC(3) -> 0.111)
     *     ...
     * @endverbatim
     *
     * @dot
     *   digraph halton_flow {
     *     rankdir=LR; bgcolor="transparent";
     *     node [shape=box, style=filled, fillcolor="#d4e6f1"];
     *     vdc0 [label="VdC(b0)", fillcolor="#a9cce3"];
     *     vdc1 [label="VdC(b1)", fillcolor="#a9cce3"];
     *     combine [label="Combine:\n(phi_b0(n), phi_b1(n))", fillcolor="#f9e79f"];
     *     point [label="Halton point\n(x, y)", fillcolor="#7fb3d8"];
     *     vdc0 -> combine; vdc1 -> combine;
     *     combine -> point;
     *   }
     * @enddot
     */
    class Halton : public GeneratorIterable<Halton, std::array<double, 2>> {
        VdCorput vdc0;
        VdCorput vdc1;

      public:
        /**
         * @brief Construct a new Halton object
         *
         * Constructs a Halton sequence generator with the specified bases for the
         * two dimensions.
         *
         * @param[in] base0 the base for the first dimension
         * @param[in] base1 the base for the second dimension
         */
        Halton(const unsigned long base0, const unsigned long base1) : vdc0(base0), vdc1(base1) {}

        /**
         * @brief Evaluate the 2D Halton point at a given index (pure, no state change)
         *
         * @f$ H(n) = \bigl(\phi_{b_0}(n),\; \phi_{b_1}(n)\bigr) @f$
         *
         * @param[in] n The sequence index.
         * @return The 2D Halton point for index n.
         */
        auto value_at(unsigned long n) const -> std::array<double, 2> {
            return {this->vdc0.value_at(n), this->vdc1.value_at(n)};
        }
    };

    /**
     * @brief Circle sequence generator
     *
     * The `Circle` class is a sequence generator that generates points on a unit
     * circle using the Van der Corput sequence. It uses the `VdCorput` class to
     * generate the sequence values and maps them to points on the unit circle. The
     * `pop()` method returns the next point on the unit circle as a
     * `std::array<double, 2>`, where the first element represents the x-coordinate
     * and the second element represents the y-coordinate of the point. The
     * `reseed()` method is used to reset the state of the sequence generator to a
     * specific seed value.
     *
     * @verbatim
     *     Unit Circle:
     *         (0,1)
     *           *
     *    (-1,0) *   * (1,0)
     *           *
     *        (0,-1)
     *
     *     Points distributed more evenly
     *     than random sampling
     * @endverbatim
     *
     * @dot
     *   digraph circle_flow {
     *     rankdir=LR; bgcolor="transparent";
     *     node [shape=box, style=filled, fillcolor="#d4e6f1"];
     *     vdc [label="VdC(n)\nin base b", fillcolor="#a9cce3"];
     *     angle [label="theta =\n2pi * VdC", fillcolor="#a9dfbf"];
     *     point [label="(cos(theta),\nsin(theta))", fillcolor="#7fb3d8"];
     *     vdc -> angle -> point;
     *   }
     * @enddot
     */
    class Circle : public GeneratorIterable<Circle, std::array<double, 2>> {
        VdCorput vdc;

      public:
        /**
         * @brief Construct a new Circle object
         *
         * Constructs a Circle sequence generator with the specified base for generating
         * the Van der Corput sequence, which is then mapped to points on the unit circle.
         *
         * @param[in] base the base for the Van der Corput sequence generator
         */
        explicit Circle(const unsigned long base) : vdc(base) {}

        /**
         * @brief Evaluate the point on the unit circle at a given index (pure, no state change)
         *
         * @f$ \theta = 2\pi \cdot \phi_b(n), \quad P(n) = (\cos\theta,\; \sin\theta) @f$
         *
         * @param[in] n The sequence index.
         * @return The point on the unit circle for index n.
         */
        auto value_at(unsigned long n) const -> std::array<double, 2> {
            auto theta = this->vdc.value_at(n) * TWO_PI;  // map to [0, 2*pi];
            return {std::cos(theta), std::sin(theta)};
        }
    };

    /**
     * @brief Disk sequence generator
     *
     * The `Disk` class is a sequence generator that generates points in a
     * 2-dimensional space using the Disk sequence. The Disk sequence is a
     * low-discrepancy sequence that is commonly used in quasi-Monte Carlo methods.
     * It is generated by iterating over two different bases and calculating the
     * fractional parts of the numbers in those bases. The `Disk` class keeps
     * track of the current count and bases, and provides a `pop()` method that
     * returns the next point in the sequence as a `std::array<double, 2>`.
     *
     * @verbatim
     *     Unit Disk:
     *         *****
     *      ***     ***
     *    **         **
     *   *             *
     *   *             *  More evenly
     *   *             *  distributed
     *    **         **   than random
     *      ***     ***
     *         *****
     * @endverbatim
     *
     * @dot
     *   digraph disk_flow {
     *     rankdir=LR; bgcolor="transparent";
     *     node [shape=box, style=filled, fillcolor="#d4e6f1"];
     *     vdc0 [label="VdC(b0)", fillcolor="#a9cce3"];
     *     vdc1 [label="VdC(b1)", fillcolor="#a9cce3"];
     *     polar [label="theta = 2pi*VdC(b0)\nr = sqrt(VdC(b1))", fillcolor="#f9e79f"];
     *     point [label="(r cos(theta),\nr sin(theta))", fillcolor="#7fb3d8"];
     *     vdc0 -> polar; vdc1 -> polar;
     *     polar -> point;
     *   }
     * @enddot
     */
    class Disk : public GeneratorIterable<Disk, std::array<double, 2>> {
        VdCorput vdc0;
        VdCorput vdc1;

      public:
        /**
         * @brief Construct a new Disk object
         *
         * Constructs a Disk sequence generator with the specified bases for the two dimensions.
         *
         * @param[in] base0 the base for the first dimension (angle)
         * @param[in] base1 the base for the second dimension (radius)
         */
        Disk(const unsigned long base0, const unsigned long base1) : vdc0(base0), vdc1(base1) {}

        /**
         * @brief Evaluate the point in the unit disk at a given index (pure, no state change)
         *
         * @f$ \theta = 2\pi \cdot \phi_{b_0}(n), \; r = \sqrt{\phi_{b_1}(n)} @f$
         * @f$ P(n) = \bigl(r\cos\theta,\; r\sin\theta\bigr) @f$
         *
         * @param[in] n The sequence index.
         * @return The point in the unit disk for index n.
         */
        auto value_at(unsigned long n) const -> std::array<double, 2> {
            auto theta = this->vdc0.value_at(n) * TWO_PI;  // map to [0, 2*pi];
            auto radius = std::sqrt(this->vdc1.value_at(n));
            return {radius * std::cos(theta), radius * std::sin(theta)};
        }
    };

    /**
     * @brief Sphere sequence generator
     *
     * The `Sphere` class is a sequence generator that generates points on a unit
     * sphere using the Van der Corput sequence. It uses the `VdCorput` class to
     * generate the sequence values and maps them to points on the unit sphere. The
     * `pop()` method returns the next point on the unit sphere as a
     * `std::array<double, 3>`, where the first element represents the x-coordinate,
     * the second element represents the y-coordinate of the point, and the third
     * element represents the z-coordinate of the point. The
     * `reseed()` method is used to reset the state of the sequence generator to a
     * specific seed value.
     *
     * @verbatim
     *     Unit Sphere:
     *          *****
     *       **       **
     *     **           **
     *    *               *
     *    *      O        *  Points distributed
     *    *               *  evenly on surface
     *     **           **
     *       **       **
     *          *****
     * @endverbatim
     *
     * @dot
     *   digraph sphere_flow {
     *     rankdir=LR; bgcolor="transparent";
     *     node [shape=box, style=filled, fillcolor="#d4e6f1"];
     *     vdc [label="VdC(b0)\nz = 2*VdC - 1", fillcolor="#a9cce3"];
     *     circle [label="Circle(b1)\n(cos, sin)", fillcolor="#a9cce3"];
     *     combine [label="P = (sin(theta)*cos(phi),\nsin(theta)*sin(phi),\ncos(theta))",
     * fillcolor="#f9e79f"]; point [label="Sphere point\n(x, y, z)", fillcolor="#7fb3d8"]; vdc ->
     * combine; circle -> combine; combine -> point;
     *   }
     * @enddot
     */
    class Sphere : public GeneratorIterable<Sphere, std::array<double, 3>> {
        VdCorput vdcgen;
        Circle cirgen;

      public:
        /**
         * @brief Construct a new Sphere object
         *
         * Constructs a Sphere sequence generator with the specified bases for generating
         * points on the unit sphere.
         *
         * @param[in] base0 the base for the Van der Corput generator (phi coordinate)
         * @param[in] base1 the base for the Circle generator (theta coordinate)
         */
        Sphere(const unsigned long base0, const unsigned long base1)
            : vdcgen(base0), cirgen(base1) {}

        /**
         * @brief Evaluate the point on the unit sphere at a given index (pure, no state change)
         *
         * @f$ \phi = 2\pi \cdot \phi_{b_1}(n), \quad \cos\theta = 2\phi_{b_0}(n) - 1 @f$
         * @f$ P(n) = \bigl(\sin\theta\cos\phi,\; \sin\theta\sin\phi,\; \cos\theta\bigr) @f$
         *
         * @param[in] n The sequence index.
         * @return The point on the unit sphere for index n.
         */
        auto value_at(unsigned long n) const -> std::array<double, 3> {
            auto cosphi = (MAPPING_FACTOR * this->vdcgen.value_at(n)) - 1.0;  // map to [-1, 1];
            auto sinphi = std::sqrt(1.0 - (cosphi * cosphi));
            auto arr = this->cirgen.value_at(n);
            return {sinphi * arr[0], sinphi * arr[1], cosphi};
        }
    };

    /**
     * @brief S(3) sequence generator by Hopf fibration
     *
     * The `Sphere3Hopf` class is a sequence generator that generates points on a
     * 3-sphere using the Hopf fibration. It uses three instances of the `VdCorput`
     * class to generate the sequence values and maps them to points on the
     * 3-sphere. The `pop()` method returns the next point on the 3-sphere as a
     * `std::array<double, 4>`, where the first three elements represent the x, y,
     * and z coordinates of the point, and the fourth element represents the w
     * coordinate. The `reseed()` method is used to reset the state of the sequence
     * generator to a specific seed value.
     *
     * @verbatim
     *     3-Sphere (S3) visualization:
     *     A 4D hypersphere where points (x,y,z,w)
     *     satisfy x²+y²+z²+w² = 1
     *
     *         4D hypersurface
     *            _____
     *         .-'     '-.
     *       ,'           ',
     *      /               \
     *     |        O        |  (4D analog of sphere)
     *      \               /
     *       '.           .'
     *         '-.....-'
     * @endverbatim
     */
    class Sphere3Hopf : public GeneratorIterable<Sphere3Hopf, std::array<double, 4>> {
        VdCorput vdc0;
        VdCorput vdc1;
        VdCorput vdc2;

      public:
        /**
         * @brief Construct a new Sphere 3 Hopf object
         *
         * Constructs a 3-sphere sequence generator using the Hopf fibration with the specified
         * bases.
         *
         * @param[in] base0 the base for the first Van der Corput generator (phi coordinate)
         * @param[in] base1 the base for the second Van der Corput generator (psi coordinate)
         * @param[in] base2 the base for the third Van der Corput generator (eta coordinate)
         */
        Sphere3Hopf(const unsigned long base0, const unsigned long base1, const unsigned long base2)
            : vdc0(base0), vdc1(base1), vdc2(base2) {}

        /**
         * @brief Evaluate the point on the 3-sphere at a given index (pure, no state change)
         *
         * Uses the Hopf fibration parametrization:
         * @f[
         *     \begin{aligned}
         *     x &= \cos\eta\cos\psi \\
         *     y &= \cos\eta\sin\psi \\
         *     z &= \sin\eta\cos(\phi + \psi) \\
         *     w &= \sin\eta\sin(\phi + \psi)
         *     \end{aligned}
         * @f]
         * where @f$ \phi = 2\pi\phi_{b_0}(n) @f$, @f$ \psi = 2\pi\phi_{b_1}(n) @f$,
         * and @f$ \eta = \arccos\sqrt{\phi_{b_2}(n)} @f$.
         *
         * @param[in] n The sequence index.
         * @return The point on the 3-sphere for index n.
         */
        auto value_at(unsigned long n) const -> std::array<double, 4> {
            auto phi = this->vdc0.value_at(n) * TWO_PI;  // map to [0, 2*pi];
            auto psy = this->vdc1.value_at(n) * TWO_PI;  // map to [0, 2*pi];
            auto vdc = this->vdc2.value_at(n);
            auto cos_eta = std::sqrt(vdc);
            auto sin_eta = std::sqrt(1.0 - vdc);
            return {
                cos_eta * std::cos(psy),
                cos_eta * std::sin(psy),
                sin_eta * std::cos(phi + psy),
                sin_eta * std::sin(phi + psy),
            };
        }
    };

    // Compile-time contract checks: every generator satisfies the protocol concept.
    static_assert(SequenceGenerator<VdCorput, double>);
    static_assert(SequenceGenerator<Halton, std::array<double, 2>>);
    static_assert(SequenceGenerator<Circle, std::array<double, 2>>);
    static_assert(SequenceGenerator<Disk, std::array<double, 2>>);
    static_assert(SequenceGenerator<Sphere, std::array<double, 3>>);
    static_assert(SequenceGenerator<Sphere3Hopf, std::array<double, 4>>);

    /**
     * @brief Dummy function (placeholder, not yet implemented).
     * @param[in] index The input index.
     * @return The result of the dummy operation.
     */
    extern unsigned long dummy(unsigned int index);
}  // namespace ldsgen
