cmake_minimum_required(VERSION 3.14)
project(LdsGenInstalledTest LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Find dependencies first - the installed config references fmt.
find_package(fmt REQUIRED)
find_package(spdlog REQUIRED)
find_package(LdsGen REQUIRED)

add_executable(ldsgen_installed_test main.cpp)
target_link_libraries(ldsgen_installed_test PRIVATE LdsGen::LdsGen)

enable_testing()
add_test(NAME ldsgen_installed_test COMMAND ldsgen_installed_test)
