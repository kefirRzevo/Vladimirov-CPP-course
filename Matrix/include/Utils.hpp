#pragma once

#include <initializer_list>

namespace matrix {

template<typename T>
requires std::is_floating_point_v<T>
inline constexpr T epsilon = 1.0e-4;

template<typename T>
inline bool isZero(const T& value) requires std::is_integral_v<T> {
    return value == T{0};
}

template<typename T>
inline bool isZero(const T& value) requires std::is_floating_point_v<T> {
    return (std::abs(value) <= epsilon<T> * std::max(std::abs(value), T{1}));
}

template<typename T>
inline bool equals(const T& lhs, const T& rhs) requires std::is_integral_v<T> {
    return lhs == rhs;
}

template<typename T>
inline bool equals(const T& lhs, const T& rhs) requires std::is_floating_point_v<T> {
    return (std::abs(lhs - rhs) <= epsilon<T> * std::max(std::max(std::abs(lhs), std::abs(rhs)), T{1}));
}

template<typename T>
inline size_t maxNestedLstSize(const std::initializer_list<std::initializer_list<T>>& lst) {
    size_t maxSize = 0U;
    for (const auto& nestedLst: lst) {
        if (nestedLst.size() > maxSize) {
            maxSize = nestedLst.size();
        }
    }
    return maxSize;
}

} //namespace matrix
