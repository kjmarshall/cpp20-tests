#include "gtest/gtest.h"
#include <array>
#include <compare>
#include <format>
#include <future>
#include <iomanip>
#include <iostream>
#include <numbers>
#include <span>
#include <string>
#include <syncstream>

// Designated Initializers
TEST(cpp20, designatedInitializers) {
    struct Point {
        double x;
        double y;
        double z;
        std::string label;
    };

    Point p{
        .x = 100,
        .y = 50,
        .label = "myPoint",
    };
    std::cout << std::format("({}, {}, {})\n", p.x, p.y, p.z);
}

// Spaceship operator <=>
// Needs #include <compare>
TEST(cpp20, spaceShip) {
    struct VertexIndex {
        int value;
        constexpr VertexIndex(int value) : value{value} {}
        constexpr bool operator==(const VertexIndex &rhs) const noexcept { return value == rhs.value; }
        constexpr bool operator!=(const VertexIndex &rhs) const noexcept { return !(*this == rhs); }
        constexpr bool operator<(const VertexIndex &rhs) const noexcept { return value < rhs.value; }
        constexpr bool operator<=(const VertexIndex &rhs) const noexcept { return !(rhs < *this); }
        constexpr bool operator>(const VertexIndex &rhs) const noexcept { return rhs < *this; }
        constexpr bool operator>=(const VertexIndex &rhs) const noexcept { return !(*this < rhs); }
    };
    static_assert(VertexIndex(0) < VertexIndex(1));

    struct VertexIndex2 {
        int value;
        constexpr VertexIndex2(int value) : value{value} {}
        // auto deduces to std::strong_ordering
        auto operator<=>(const VertexIndex2 &) const = default;
    };
    static_assert(VertexIndex2(0) < VertexIndex2(1));

    // Also works for more complex types (compiler can recursively expand structs and arrays and compare element by
    // element) Overloading specific instances of ==, !=, <, <=, >, >= take precedence during compiler prefers overloads
    // that are not rewritten or syntehsized

    struct Basics {
        int i;
        char c;
        float f;
        double d;
        auto operator<=>(const Basics &) const = default;
    };

    struct Arrays {
        int ai[1];
        char ac[2];
        float af[3];
        double ad[2][2];
        auto operator<=>(const Arrays &) const = default;
    };

    struct Bases : Basics, Arrays {
        auto operator<=>(const Bases &) const = default;
    };

    constexpr Bases a = {{0, 'c', 1.f, 1.}, {{1}, {'a', 'b'}, {1.f, 2.f, 3.f}, {{1., 2.}, {3., 4.}}}};
    constexpr Bases b = {{0, 'c', 1.f, 1.}, {{1}, {'a', 'b'}, {1.f, 2.f, 3.f}, {{1., 2.}, {3., 4.}}}};
    static_assert(a == b);
    static_assert(!(a != b));
    static_assert(!(a < b));
    static_assert(a <= b);
    static_assert(!(a > b));
    static_assert(a >= b);
}

// Init-statements and initializers in range-for
TEST(cpp20, rangedForInit) {
    struct Data {
        Data() {
            for (int i = 0; i < 10; ++i) {
                data.push_back(i);
            }
        }
        std::vector<int> data;
    };
    // Bad (has lifetime problems????)
    for (auto &i : Data().data) {
        std::cout << std::format("i = {}\n", i);
    }
    // Good
    for (auto d = Data(); auto &i : d.data) {
        std::cout << std::format("i = {}\n", i);
    }
}

// Immediate functions: functions with consteval must produce a constant
consteval int sqr(int n) { return n * n; }
TEST(cpp20, immediateFunction) {
    constexpr int r = sqr(100);
    EXPECT_EQ(r, 10000);
}

// using enum
enum class Direction : unsigned { UP, DOWN, LEFT, RIGHT };
// Before
std::string_view to_string1(Direction dir) {
    switch (dir) {
    case Direction::UP:
        return "up";
    case Direction::DOWN:
        return "down";
    case Direction::LEFT:
        return "left";
    case Direction::RIGHT:
        return "right";
    }
    return "";
}
// After
std::string_view to_string2(Direction dir) {
    switch (dir) {
        using enum Direction;
    case UP:
        return "up";
    case DOWN:
        return "down";
    case LEFT:
        return "left";
    case RIGHT:
        return "right";
    }
    return "";
}

TEST(cpp20, usingEnum) {
    using enum Direction;
    for (auto const &dir : {UP, DOWN, LEFT, RIGHT}) {
        auto s1 = to_string1(dir);
        auto s2 = to_string2(dir);
        std::cout << std::format("{}\n", s1);
        std::cout << std::format("{}\n", s2);
        EXPECT_EQ(s1, s2);
    }
}

// Capture structured bindings in another lambda...
TEST(cpp20, captureStructuredBinding) {
    auto foo = []() { return std::make_pair(1, 2); };
    auto [a, b] = foo();
    auto bar = [&]() { std::cout << std::format("({}, {})\n", a, b); };
    bar();
}

// Thread save std::cout
// Needs #include <syncstream>
TEST(cpp20, syncOutput) {

    auto foo = []() {
        for (int i = 0; i < 3; ++i) {
            std::cout << std::format("Hello world from foo()\n");
        }
    };

    // TODO KM: ???????????????????????
    auto bar = []() {
        for (int i = 0; i < 3; ++i) {
            std::osyncstream{std::cout} << std::format("Hello world from foo()\n");
        }
    };

    std::vector<std::future<void>> f;
    for (int i = 0; i < 10; ++i) {
        f.push_back(std::async(std::launch::async, foo));
    }
}

// std::span
// needs #include <span>
TEST(cpp20, stdSpan) {
    // Get rid of gsl span?
    auto print = [](std::span<const int> v) {
        for (auto e : v) {
            std::cout << e << " ";
        }
        std::cout << "\n";
    };
    std::vector<int> v{1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
    print(v);
    print(std::vector<int>{1, 2, 3, 4, 5, 6, 7, 8, 9, 10});
    print(std::array<int, 10>{1, 2, 3, 4, 5, 6, 7, 8, 9, 10});
}

// Mathematical constants
// Needs #include <numbers>
TEST(cpp20, mathConstants) {
    /*
    inline constexpr double e          = e_v<double>;
    inline constexpr double log2e      = log2e_v<double>;
    inline constexpr double log10e     = log10e_v<double>;
    inline constexpr double pi         = pi_v<double>;
    inline constexpr double inv_pi     = inv_pi_v<double>;
    inline constexpr double inv_sqrtpi = inv_sqrtpi_v<double>;
    inline constexpr double ln2        = ln2_v<double>;
    inline constexpr double ln10       = ln10_v<double>;
    inline constexpr double sqrt2      = sqrt2_v<double>;
    inline constexpr double sqrt3      = sqrt3_v<double>;
    inline constexpr double inv_sqrt3  = inv_sqrt3_v<double>;
    inline constexpr double egamma     = egamma_v<double>;
    inline constexpr double phi
    */
    std::cout << std::format("e = {}\n", std::numbers::e);
    std::cout << std::format("log2e = {}\n", std::numbers::log2e);
    std::cout << std::format("log10e = {}\n", std::numbers::log10e);
    std::cout << std::format("pi = {}\n", std::numbers::pi);
    std::cout << std::format("inv_pi = {}\n", std::numbers::inv_pi);
    std::cout << std::format("inv_sqrtpi = {}\n", std::numbers::inv_sqrtpi);
    std::cout << std::format("ln2 = {}\n", std::numbers::ln2);
    std::cout << std::format("ln10 = {}\n", std::numbers::ln10);
    std::cout << std::format("sqrt2 = {}\n", std::numbers::sqrt2);
    std::cout << std::format("sqrt3 = {}\n", std::numbers::sqrt3);
    std::cout << std::format("inv_sqrt3 = {}\n", std::numbers::inv_sqrt3);
    std::cout << std::format("egamma = {}\n", std::numbers::egamma);
    std::cout << std::format("phi = {}\n", std::numbers::phi);
}