#pragma once

#include <algorithm> 
#include <cmath>

namespace triangles
{

namespace compare
{

template<typename T>
constexpr T epsilon = 1.0e-4;

template<>
float epsilon<float> = 1.0e-5;

template<>
double epsilon<double> = 1.0e-6;

template<typename T>
T max(T value)
    {
        return value;
    }

template<typename T, typename... Args>
T max(T value, Args... args)
    {
        T max_ = max(args...);
        return (value > max_) ? value : max_;
    }

template<typename T>
T min(T value)
    {
        return value;
    }

template<typename T, typename... Args>
T min(T value, Args... args)
    {
        T min_ = min(args...);
        return (value < min_) ? value : min_;
    }

template<typename T>
bool isZero(const T& value)
    {
        return (std::abs(value) <= epsilon<T> * max(std::abs(value), T{1}));
    }

template<typename T>
bool equals(const T& lhs, const T& rhs)
    {
        return (std::abs(lhs - rhs) <= epsilon<T> * max(std::abs(lhs), std::abs(rhs), T{1}));
    }

template<typename T>
bool greaterEq(const T& lhs, const T& rhs)
    {
        return (lhs - rhs >= -epsilon<T> * max(std::abs(lhs), std::abs(rhs), T{1}));
    }

template<typename T>
bool lessEq(const T& lhs, const T& rhs)
    {
        return (lhs - rhs <= epsilon<T> * max(std::abs(lhs), std::abs(rhs), T{1}));
    }

template<typename T>
bool greater(const T& lhs, const T& rhs)
    {
        return !lessEq(lhs, rhs);
    }

template<typename T>
bool less(const T& lhs, const T& rhs)
    {
        return !greaterEq(lhs, rhs);
    }

template<typename T>
char getSign(const T& val)
{
    if(isZero(val))
    {
        return 0;
    }
    else if(less(val, T{0}))
    {
        return -1;
    }
    else
    {
        return 1;
    }
}

} //namespace compare

using namespace compare;

enum class IntersectionType
{
    Belongs,
    Intersects,
    Parallels,
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

enum class Coordinate
{
    x,
    y,
    z,
};

template<typename T>
bool sameSign(const T& val1, const T& val2, const T& val3)
    {
        if((greater(val1, T{0}) && greater(val2, T{0}) && greater(val3, T{0})) ||
           (less(val1, T{0}) && less(val2, T{0}) && less(val3, T{0})))
        {
            return true;
        }
        else
        {
            return false;
        }
    }

} //namespace triangles
