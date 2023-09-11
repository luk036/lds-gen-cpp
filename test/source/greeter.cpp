#include <doctest/doctest.h>
#include <ldsgen/greeter.h>
// #include <ldsgen/version.h>

#include <string>

TEST_CASE("LdsGen") {
    using namespace ldsgen;

    LdsGen ldsgen("Tests");

    CHECK(ldsgen.greet(LanguageCode::EN) == "Hello, Tests!");
    CHECK(ldsgen.greet(LanguageCode::DE) == "Hallo Tests!");
    CHECK(ldsgen.greet(LanguageCode::ES) == "¡Hola Tests!");
    CHECK(ldsgen.greet(LanguageCode::FR) == "Bonjour Tests!");
}

// TEST_CASE("LdsGen version") {
//     static_assert(std::string_view(LDSGEN_VERSION) == std::string_view("1.0"));
//     CHECK(std::string(LDSGEN_VERSION) == std::string("1.0"));
// }
