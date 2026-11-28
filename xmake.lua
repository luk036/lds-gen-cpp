add_rules("mode.debug", "mode.release", "mode.coverage")
add_requires("doctest", "fmt", "spdlog", "nanobench")

set_languages("c++20")
add_includedirs("include")

if is_mode("coverage") then
    add_cxflags("-ftest-coverage", "-fprofile-arcs", {force = true})
end

-- LTO for release builds
if is_mode("release") then
    if is_plat("windows") then
        add_cxflags("/GL", {force = true})
        add_ldflags("/LTCG", {force = true})
    else
        add_cxflags("-flto", {force = true})
        add_ldflags("-flto", {force = true})
    end
end

if is_plat("linux") then
    add_cxflags("-Wconversion", {force = true})
    add_syslinks("pthread")
    -- Termux/Android ships its libc++ headers under $PREFIX
    local prefix = os.getenv("PREFIX")
    if prefix then
        add_cxflags("-Wno-unused-command-line-argument", {force = true})
        add_sysincludedirs(prefix .. "/include/c++/v1", prefix .. "/include", {public = true})
    end
elseif is_plat("windows") then
    add_cxflags("/EHsc /utf-8 /W4 /WX /wd4702", {force = true})
end

target("LdsGen")
    set_kind("static")
    add_files("source/*.cpp")
    add_packages("fmt", "spdlog")

target("test_ldsgen")
    add_deps("LdsGen")
    add_files("test/source/*.cpp")
    add_packages("doctest", "fmt", "spdlog")
    add_tests("default")

target("bench_sphere")
    add_deps("LdsGen")
    add_files("bench/BM_sphere.cpp")
    add_packages("nanobench", "fmt", "spdlog")

target("bench_all")
    add_deps("LdsGen")
    add_files("bench/BM_all.cpp")
    add_packages("nanobench", "fmt", "spdlog")

target("verify_basic")
    add_deps("LdsGen")
    add_files("examples/verify_basic.cpp")
    add_packages("fmt", "spdlog")

target("verify_sphere")
    add_deps("LdsGen")
    add_files("examples/verify_sphere.cpp")
    add_packages("fmt", "spdlog")
