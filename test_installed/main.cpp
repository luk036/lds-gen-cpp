#include <ldsgen/version.h>

#include <iostream>

auto main() -> int {
    const auto ok = (LDSGEN_VERSION_MAJOR >= 1);
    std::cout << "ldsgen installed test: version " << LDSGEN_VERSION << "\n";
    return ok ? 0 : 1;
}
