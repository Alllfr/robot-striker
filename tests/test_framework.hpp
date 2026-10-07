#pragma once
// Framework unit test mini (Standard Library saja).
#include <cmath>
#include <functional>
#include <string>
#include <vector>

struct TestCase {
    std::string name;
    std::function<void()> fn;
};
struct TestFailure {
    std::string message;
};

inline std::vector<TestCase>& testRegistry() {
    static std::vector<TestCase> registry;
    return registry;
}
struct TestRegistrar {
    TestRegistrar(const char* name, std::function<void()> fn) { testRegistry().push_back({name, std::move(fn)}); }
};

#define TEST(name)                                         \
    static void name();                                    \
    static TestRegistrar registrar_##name(#name, name);    \
    static void name()

#define TEST_LOCATION (std::string(__FILE__) + ":" + std::to_string(__LINE__))

#define CHECK(cond) \
    do { if (!(cond)) throw TestFailure{TEST_LOCATION + "  CHECK gagal: " #cond}; } while (0)

#define CHECK_NEAR(a, b, eps) \
    do { if (std::fabs((a) - (b)) > (eps)) throw TestFailure{TEST_LOCATION + "  CHECK_NEAR gagal: " #a " != " #b}; } while (0)

#define CHECK_THROWS(expr, ExType)                                                       \
    do {                                                                                 \
        bool caught_ = false;                                                            \
        try { expr; } catch (const ExType&) { caught_ = true; } catch (...) {}           \
        if (!caught_) throw TestFailure{TEST_LOCATION + "  Exception " #ExType " tidak terlempar: " #expr}; \
    } while (0)
