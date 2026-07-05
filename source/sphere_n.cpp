#include "ldsgen/sphere_n.hpp"

#include <algorithm>
#include <array>
#include <cmath>
#include <memory>
#include <mutex>
#include <stdexcept>
#include <vector>

namespace ldsgen {

    std::vector<double> linspace(double start, double stop, unsigned int num) {
        if (num == 1) {
            return {start};
        }
        std::vector<double> result;
        result.reserve(num);
        double step = (stop - start) / static_cast<double>(num - 1);
        for (unsigned int i = 0; i < num; ++i) {
            result.emplace_back(start + (static_cast<double>(i) * step));
        }
        return result;
    }

    double simple_interp(double x_value, std::span<const double> x_points,
                         std::span<const double> y_points) {
        if (x_points.empty() || y_points.empty() || x_points.size() != y_points.size()) {
            throw std::invalid_argument("x_points and y_points must be non-empty and same size");
        }
        if (x_value <= x_points[0]) return y_points[0];
        if (x_value >= x_points.back()) return y_points.back();

        auto it = std::ranges::upper_bound(x_points, x_value);
        auto i = static_cast<std::size_t>(std::distance(x_points.begin(), it) - 1);
        double t = (x_value - x_points[i]) / (x_points[i + 1] - x_points[i]);
        return y_points[i] + t * (y_points[i + 1] - y_points[i]);
    }

    // Precomputed tables using std::array for static allocation (no heap)
    const std::array<double, TABLE_SIZE> X = []() {
        std::array<double, TABLE_SIZE> arr{};
        double step = PI / static_cast<double>(TABLE_SIZE - 1);
        for (std::size_t i = 0; i < TABLE_SIZE; ++i) {
            arr[i] = static_cast<double>(i) * step;
        }
        return arr;
    }();

    const std::array<double, TABLE_SIZE> NEG_COSINE = []() {
        std::array<double, TABLE_SIZE> arr{};
        for (std::size_t i = 0; i < TABLE_SIZE; ++i) arr[i] = -std::cos(X[i]);
        return arr;
    }();

    const std::array<double, TABLE_SIZE> SINE = []() {
        std::array<double, TABLE_SIZE> arr{};
        for (std::size_t i = 0; i < TABLE_SIZE; ++i) arr[i] = std::sin(X[i]);
        return arr;
    }();

    const std::array<double, TABLE_SIZE> F2 = []() {
        std::array<double, TABLE_SIZE> arr{};
        for (std::size_t i = 0; i < TABLE_SIZE; ++i) {
            arr[i] = (X[i] + NEG_COSINE[i] * SINE[i]) / 2.0;
        }
        return arr;
    }();

    // Iterative helper for get_tp — avoids deep recursion and vector copies
    std::vector<double> get_tp(unsigned int n) {
        if (n == 0) {
            return std::vector<double>(X.begin(), X.end());
        }
        if (n == 1) {
            return std::vector<double>(NEG_COSINE.begin(), NEG_COSINE.end());
        }

        std::vector<double> prev2(X.begin(), X.end());
        std::vector<double> prev1(NEG_COSINE.begin(), NEG_COSINE.end());
        std::vector<double> current;
        current.reserve(TABLE_SIZE);

        for (unsigned int i = 2; i <= n; ++i) {
            current.clear();
            for (std::size_t j = 0; j < TABLE_SIZE; ++j) {
                double value = (static_cast<double>(i - 1) * prev2[j]
                                + NEG_COSINE[j] * std::pow(SINE[j], i - 1))
                               / static_cast<double>(i);
                current.emplace_back(value);
            }
            if (i < n) {
                prev2 = std::move(prev1);
                prev1 = std::move(current);
                current.reserve(TABLE_SIZE);
            }
        }
        return current;
    }

    // --- Sphere3 ---

    Sphere3::Sphere3(std::span<const unsigned long> base)
        : vdc_(base[0]), sphere2_(base[1], base[2]) {
        if (base.size() < 3) {
            throw std::invalid_argument("Sphere3 requires at least 3 bases");
        }
    }

    std::array<double, 4> Sphere3::pop() {
        std::scoped_lock lock(mutex_);
        double ti = HALF_PI * vdc_.pop();
        double xi = simple_interp(ti, F2, X);
        double cosxi = std::cos(xi);
        double sinxi = std::sin(xi);

        auto pt = sphere2_.pop();
        return {sinxi * pt[0], sinxi * pt[1], sinxi * pt[2], cosxi};
    }

    void Sphere3::reseed(unsigned long seed) {
        std::scoped_lock lock(mutex_);
        vdc_.reseed(seed);
        sphere2_.reseed(seed);
    }

    // --- SphereWrapper ---

    SphereWrapper::SphereWrapper(std::span<const unsigned long> base) : sphere_(base[0], base[1]) {}

    std::vector<double> SphereWrapper::pop() {
        std::scoped_lock lock(mutex_);
        auto arr = sphere_.pop();
        return {arr.begin(), arr.end()};
    }

    void SphereWrapper::reseed(unsigned long seed) {
        std::scoped_lock lock(mutex_);
        sphere_.reseed(seed);
    }

    // --- SphereN ---

    SphereN::SphereN(std::span<const unsigned long> base)
        : vdc_(base[0]), n_(static_cast<unsigned int>(base.size() - 1)) {
        if (n_ < 2) {
            throw std::invalid_argument("SphereN requires at least 3 bases (n >= 2)");
        }

        if (n_ == 2) {
            std::vector<unsigned long> sphere_base = {base[1], base[2]};
            s_gen_ = std::make_unique<SphereWrapper>(sphere_base);
        } else {
            std::vector<unsigned long> sub_base(base.begin() + 1, base.end());
            s_gen_ = std::make_unique<SphereN>(sub_base);
        }

        // Precompute tp_ once in constructor — avoids get_tp(n_) call on every pop()
        tp_ = get_tp(n_);
        range_ = tp_.back() - tp_.front();
    }

    std::vector<double> SphereN::pop() {
        std::scoped_lock lock(mutex_);
        if (n_ == 2) {
            double ti = HALF_PI * vdc_.pop();
            double xi = simple_interp(ti, F2, X);
            double cosxi = std::cos(xi);
            double sinxi = std::sin(xi);

            auto sub_point = s_gen_->pop();
            std::vector<double> result;
            result.reserve(sub_point.size() + 1);
            for (double s : sub_point) result.emplace_back(sinxi * s);
            result.emplace_back(cosxi);
            return result;
        }

        double vd = vdc_.pop();
        double ti = tp_.front() + range_ * vd;
        double xi = simple_interp(ti, tp_, X);
        double sinphi = std::sin(xi);

        auto sub_point = s_gen_->pop();
        std::vector<double> result;
        result.reserve(sub_point.size() + 1);
        for (double s : sub_point) result.emplace_back(s * sinphi);
        result.emplace_back(std::cos(xi));
        return result;
    }

    void SphereN::reseed(unsigned long seed) {
        std::scoped_lock lock(mutex_);
        vdc_.reseed(seed);
        s_gen_->reseed(seed);
    }

}  // namespace ldsgen