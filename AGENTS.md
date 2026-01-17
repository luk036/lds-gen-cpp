# AGENTS.md - Agent Guidelines for lds-gen-cpp

## Build System (CMake 3.14+)

### Build Commands
```bash
# Build and run all tests
cmake -S test -B build/test
cmake --build build/test
CTEST_OUTPUT_ON_FAILURE=1 cmake --build build/test --target test
# OR run executable directly
./build/test/LdsGenTests

# Build standalone executable
cmake -S standalone -B build/standalone
cmake --build build/standalone
./build/standalone/LdsGen --help

# Build everything (library, tests, standalone, docs)
cmake -S all -B build
cmake --build build

# Code formatting
cmake --build build/test --target format        # view changes
cmake --build build/test --target fix-format   # apply changes
```

## Build System (xmake)

This project can also be built using xmake as an alternative to CMake.

### Build Commands
```bash
# Build project
xmake

# Build and run tests
xmake test

# Build standalone executable
xmake build LdsGen

# Run standalone
xmake run LdsGen

# Clean build artifacts (DO NOT delete build/ directory directly)
xmake clean
```

### Running Single Tests
This project uses Doctest. Run specific test cases:
```bash
./build/test/LdsGenTests --test-case="VdCorput"
./build/test/LdsGenTests --test-case="Halton"
./build/test/LdsGenTests --test-case="Circle"
./build/test/LdsGenTests --test-case="Sphere"
```
Wildcards supported: `--test-case="*thread*"` to run thread-safety tests.

## Code Style Guidelines

### Formatting
- **Tool**: clang-format (Google-based style)
- **Column limit**: 100 characters
- **Indentation**: 4 spaces
- **Braces**: Attach style (opening brace on same line)
- **Namespace indentation**: Indent all namespace contents

Always format before committing:
```bash
cmake --build build/test --target fix-format
```

### Naming Conventions
| Type | Convention | Examples |
|------|------------|----------|
| Classes | PascalCase | `VdCorput`, `Halton`, `Circle`, `Sphere`, `SphereN` |
| Functions | snake_case | `vdc()`, `pop()`, `reseed()`, `simple_interp()` |
| Constants | UPPER_SNAKE_CASE | `TWO_PI`, `MAX_REVERSE_BITS`, `PRIME_TABLE` |
| Member variables | snake_case_ (trailing underscore) | `count`, `base`, `vdc_`, `s_gen_` |
| Private methods | snake_case | same as public methods |
| Parameters | snake_case | `base`, `seed`, `x_value`, `n` |

### Modern C++ Practices (C++20)
- Use `auto` for type inference when type is obvious
- Prefer `std::span` for passing arrays/views
- Use `constexpr` for compile-time constants and functions
- Use `std::array` for fixed-size arrays, `std::vector` for dynamic
- Use `std::unique_ptr` for single ownership
- Prefer range-based for loops: `for (auto x : container)`
- Use `override` keyword when overriding virtual functions

### Headers & Includes
- Use `#pragma once` for header guards (no traditional #ifndef)
- Include order: 1) Standard library, 2) Third-party (doctest, fmt), 3) Project headers
- Relative includes for project headers: `#include <ldsgen/lds.hpp>`
- Headers in `include/ldsgen/*.hpp`, implementations in `source/*.cpp`

### Member Functions
```cpp
// Use trailing return types for clarity
auto pop() -> double;
auto reseed(size_t seed) -> void;

// Mark constructors explicit unless implicit conversion intended
explicit VdCorput(size_t base);

// Delete move operations if not thread-safe/reasonable
VdCorput(VdCorput&&) noexcept = delete;
VdCorput& operator=(VdCorput&&) noexcept = delete;
```

### Thread Safety
- Use `std::atomic<T>` for thread-safe counters
- Use `std::mutex` + `std::lock_guard` for protecting sections
- Use `std::memory_order_relaxed` for atomic operations when synchronization not needed
- Always test thread safety (see test_lds.cpp for examples)

### Error Handling
- Throw `std::invalid_argument` for invalid input arguments
- Throw `std::runtime_error` or similar for runtime errors
- Check preconditions and throw early
- NO error suppression: never catch exceptions silently

### Testing Patterns (Doctest)
```cpp
#include <doctest/doctest.h>

TEST_CASE("ClassName::method_name") {
    // Setup
    auto obj = ldsgen::ClassName(params);

    // Act & Assert
    CHECK_EQ(obj.method(), expected_value);
    CHECK_GE(result, 0.0);
    CHECK(obj.pop() == doctest::Approx(0.5));
}

// Multiple assertions in one test
TEST_CASE("ClassName::pop multiple") {
    auto obj = ldsgen::ClassName(2);
    obj.reseed(0);
    auto res = obj.pop();
    CHECK_EQ(res[0], expected1);
    CHECK_EQ(res[1], expected2);
}
```

### Constants & Magic Numbers
```cpp
// Define constexpr constants in header
namespace ldsgen {
    constexpr size_t MAX_REVERSE_BITS = 64;
    constexpr double TWO_PI = 2.0 * M_PI;
    constexpr double MAPPING_FACTOR = 2.0;
}
```

### Documentation
- Use Doxygen-style comments for public APIs
- Include `@brief`, `@param[in]`, `@return` tags
- Use svgbob diagrams for visual explanations when appropriate

## Project Structure
```
include/ldsgen/    # Public headers (*.hpp)
source/             # Implementation files (*.cpp)
test/               # Test suite (doctest)
standalone/         # Executable demo
all/                # Build all targets together
```

## Important Notes
- NO `.h` files - use `.hpp` for C++ headers
- NO type error suppression - fix type issues properly
- NO empty catch blocks
- Always run format target before committing
- Tests must pass before merging
- Thread safety is a requirement for generators
- **DO NOT delete the build/ directory directly without permission** - use `xmake clean` or CMake clean targets instead
