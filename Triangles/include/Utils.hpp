#pragma once

#include <algorithm> 
#include <cmath>

namespace triangles {

    namespace compare {

    template<typename T>
    constexpr T epsilon = 1.0e-4;

    template<>
    float epsilon<float> = 1.0e-5;

    template<>
    double epsilon<double> = 1.0e-6;

    template<typename T>
    T max(const T& value) {
        return value;
    }

    template<typename T, typename... Args>
    T max(const T& value, const Args&... args) {
        T max_ = max(args...);
        return (value > max_) ? value : max_;
    }

    template<typename T>
    T min(const T& value) {
        return value;
    }

    template<typename T, typename... Args>
    T min(const T& value, const Args&... args) {
        T min_ = min(args...);
        return (value < min_) ? value : min_;
    }

    template<typename T>
    bool isZero(const T& value) {
        return (std::abs(value) <= epsilon<T> * max(std::abs(value), T{1}));
    }

    template<typename T>
    bool equals(const T& lhs, const T& rhs) {
        return (std::abs(lhs - rhs) <= epsilon<T> * max(std::abs(lhs), std::abs(rhs), T{1}));
    }

    template<typename T>
    bool greaterEq(const T& lhs, const T& rhs) {
        return (lhs - rhs >= -epsilon<T> * max(std::abs(lhs), std::abs(rhs), T{1}));
    }

    template<typename T>
    bool lessEq(const T& lhs, const T& rhs) {
        return (lhs - rhs <= epsilon<T> * max(std::abs(lhs), std::abs(rhs), T{1}));
    }

    template<typename T>
    bool greater(const T& lhs, const T& rhs) {
        return !lessEq(lhs, rhs);
    }

    template<typename T>
    bool less(const T& lhs, const T& rhs) {
        return !greaterEq(lhs, rhs);
    }

    } //namespace compare

using namespace compare;

template<typename T>
class Vector2;

template<typename T>
char sign(const T& value) {
    if (isZero(value)) {
        return 0;
    } else if (less(value, T{0})) {
        return -1;
    } else {
        return 1;
    }
}

template<typename T, typename... Args>
bool sameSign(const T& value, const Args&... args) {
    char firstSign = sign(value);
    bool allSameSign = true;
    
    ((allSameSign = allSameSign && (sign(args) == firstSign)), ...);

    return allSameSign;
}

enum class IntersectionType
{
    Belongs,
    Intersects,
    Parallel,
    Undefined,
};

enum Quadrant
{
    xyz,
    xy_z,
    x_yz,
    x_y_z,
    _xyz,
    _xy_z,
    _x_yz,
    _x_y_z,
    count,
};

enum class Orientation
{
    Collinear,
    Clockwise,
    Counterclockwise,
};

template<typename T>
Orientation orientation(Vector2<T> p, Vector2<T> q, Vector2<T> r)
    {
        T val = (q.y_ - p.y_) * (r.x_ - q.x_) -
                (q.x_ - p.x_) * (r.y_ - q.y_);

        if (isZero(val)) {
            return Orientation::Collinear;
        } else if (greater(val, T{0})) {
            return Orientation::Clockwise;
        } else {
            return Orientation::Counterclockwise;
        }
    }

} //namespace triangles
