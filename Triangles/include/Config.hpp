#pragma once

#include <cmath>

namespace triangles
{

template <typename T>
class Config
{
    public:

        static T epsilon = 1.0e-5;
        static bool equalsZero(const T& value)
            {
                return std::abs(value) <= epsilon;
            }
};

} //namespace triangles
