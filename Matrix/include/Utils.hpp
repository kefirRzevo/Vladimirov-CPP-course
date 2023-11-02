#pragma once

namespace matrix {

template<typename T>
requires std::is_floating_point_v<T>
inline constexpr T epsilon = 1.0e-5;

template<typename T>
inline bool isZero(const T& value) requires std::is_floating_point_v<T> {
    return (std::abs(value) <= epsilon<T> * std::max(std::abs(value), T{1}));
}

template<typename T>
inline bool equals(const T& lhs, const T& rhs) requires std::is_floating_point_v<T> {
    return (std::abs(lhs - rhs) <= epsilon<T> * std::max(std::max(std::abs(lhs), std::abs(rhs)), T{1}));
}

template<typename T>
inline bool greaterEq(const T& lhs, const T& rhs) requires std::is_floating_point_v<T> {
    return (lhs - rhs >= -epsilon<T> * std::max(std::max(std::abs(lhs), std::abs(rhs)), T{1}));
}

template<typename T>
inline bool lessEq(const T& lhs, const T& rhs) requires std::is_floating_point_v<T> {
    return (lhs - rhs <= epsilon<T> * std::max(std::max(std::abs(lhs), std::abs(rhs)), T{1}));
}

template<typename T>
inline bool greater(const T& lhs, const T& rhs) requires std::is_floating_point_v<T> {
    return !lessEq(lhs, rhs);
}

template<typename T>
inline bool less(const T& lhs, const T& rhs) requires std::is_floating_point_v<T> {
    return !greaterEq(lhs, rhs);
}

} //namespace matrix