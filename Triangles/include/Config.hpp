#pragma once

#include <concepts>
#include <cmath>

namespace triangles
{

template <typename T, typename = std::enable_if_t<std::is_floating_point_v<T>>>
class Config
{
    public:

        static constexpr T epsilon = 1.0e-5;
        static bool equalsZero(const T& value)
            {
                return std::abs(value) <= epsilon;
            }
};

} //namespace triangles
