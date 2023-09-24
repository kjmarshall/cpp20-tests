#include "gtest/gtest.h"
#include <concepts>

// Clunky and complicated until we get std::generator???
// Concepts allow you to put constraints on template parameters
// Advantanges: improve readability of code, speed up compilation time, better compiler error messages

template <typename T>
concept isIntegral = std::is_integral_v<T>;

template <typename T>
concept POD = std::is_trivial<T>::value && std::is_standard_layout<T>::value;

template <typename Iter> struct Foo {
    // The requires expression always evalutes to a boolean.
    static_assert(
        requires(Iter i) { ++i; }, "No increment operator");
};

template <typename T>
concept HasMember = requires(T v) {
                        v.f(); // member function;
                    };

template <typename T>
concept HasMemberReturnsInt = requires(T v) {
                                  { v.f() } -> std::convertible_to<int>; // member function;
                              };

template <typename T>
concept HasMemberReturnsExactlyInt = requires(T v) {
                                         { v.f() } -> std::same_as<int>; // member function;
                                         // Reads as "std::same_as<decltype(v.f(i)), int> must be satisfied"
                                         // Can't do (RHS needs to be a concept)
                                         // { v.f() } -> int;
                                     };

struct Point3d {
    double x;
    double y;
    double z;
};

struct Point2d {
    double x;
    double y;
};

// Define a concept that requires an object of type T to have a member function
// called getPoints(), which returns something convertable to a std::vector<Point3d>>
template <typename T>
concept ConPoint2d = requires(T obj) {
                         obj.x;
                         obj.y;
                     };

template <typename T>
concept ConPoint3d = ConPoint2d<T> && requires(T obj) { obj.z; };

template <typename T>
concept ConGetPoints = requires(T obj) {
                           { obj.getPoints() } -> std::same_as<std::vector<Point3d> const &>;
                       };
template <typename PointType>
    requires ConPoint3d<PointType> && ConPoint3d<PointType>
class Surface3d {
  public:
    std::vector<Point3d> const &getPoints() { return _points; }

  private:
    std::vector<Point3d> _points;
};

Point3d getVector3d(ConPoint3d auto const &a, ConPoint3d auto b) { return Point3d{b.x - a.x, b.y - a.y, b.z - a.z}; }

TEST(cpp20, concepts1) {
    // ConPoint2d auto point2d = Point2d{};
    ConPoint2d auto point2d = Point3d{};
    //ConPoint3d auto point3d = Point3d{};
    ConPoint3d auto point3d = Point2d{};
    ConGetPoints auto surf3d_a = Surface3d<Point3d>{};
    // ConGetPoints auto surf3d_b = Surface3d<Point2d>{};

    auto v = getVector3d(Point3d{1, 1, 1}, Point3d{2, 2, 2});
    // auto v2 = getVector3d(Point2d{1, 1}, Point3d{2, 2});
}