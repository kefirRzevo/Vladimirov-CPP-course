#pragma once

#include <iostream>
#include <cmath>

#include "Utils.hpp"

namespace triangles
{

template<typename T>
class Vector;
template<typename T>
class Vector2;

template<typename T>
class Vector
{
    public:

        T x_;
        T y_;
        T z_;

        Vector():
            x_(T{0}), y_(T{0}), z_(T{0})
            {}

        Vector(const T& x, const T& y, const T& z):
            x_(x), y_(y), z_(z)
            {}

        Vector<T> operator-() const
            {
                return Vector<T>{-x_, -y_, -z_};
            }

        Vector<T>& operator+=(const Vector<T>& rhs)
            {
                x_ += rhs.x_;
                y_ += rhs.y_;
                z_ += rhs.z_;
                return *this;
            }

        Vector<T>& operator-=(const Vector<T>& rhs)
            {
                x_ -= rhs.x_;
                y_ -= rhs.y_;
                z_ -= rhs.z_;
                return *this;
            }

        Vector<T>& operator*=(const T& rhs)
            {
                x_ *= rhs;
                y_ *= rhs;
                z_ *= rhs;
                return *this;
            }

        Vector<T>& operator/=(const T& rhs)
            {
                x_ /= rhs;
                y_ /= rhs;
                z_ /= rhs;
                return *this;
            }

        Vector<T>& normalize()
            {
                T len = length();
                return operator/=(len);
            }

        bool operator==(const Vector<T>& rhs) const
            {
                return equals(x_, rhs.x_) && equals(y_, rhs.y_) && equals(z_, rhs.z_);
            }

        bool operator!=(const Vector<T>& rhs) const
            {
                return !operator==(rhs);
            }

        bool operator!() const
            {
                return operator==({});
            }

        operator bool() const
            {
                return !operator==({});
            }

        T squaredLength() const
            {
                return x_ * x_ + y_ * y_ + z_ * z_;
            }

        T length() const
            {
                return std::sqrt(squaredLength());
            }

        T getSquaredDistance(const Vector<T>& v) const
            {
                Vector<T> offset = *this - v;
                return offset.squaredLength();
            }

        T getDistance(const Vector<T>& v) const
            {
                return std::sqrt(getSquaredDistance(v));
            }

        void dump() const
            {
                std::cout << "Vector {" << x_ << ", " << y_ << ", " << z_ << "}\n";
                std::cout.flush();
            }

        bool collinear(const Vector<T>& rhs) const
            {
                return !cross(*this, rhs);
            }

        Vector<T> project(const Vector<T>& v) const
            {
                return dot(*this, v) / squaredLength() * *this;
            }
};

template<typename T>
Vector<T> operator*(const Vector<T>& lhs, const T& rhs)
{
    return {lhs.x_ * rhs, lhs.y_ * rhs, lhs.z_ * rhs};
}

template<typename T>
Vector<T> operator*(const T& lhs, const Vector<T>& rhs)
{
    return {lhs * rhs.x_, lhs * rhs.y_, lhs * rhs.z_};
}

template<typename T>
Vector<T> operator/(const Vector<T>& lhs, const T& rhs)
{
    return {lhs.x_ / rhs, lhs.y_ / rhs, lhs.z_ / rhs};
}

template<typename T>
Vector<T> operator+(const Vector<T>& lhs, const Vector<T>& rhs)
{
    return {lhs.x_ + rhs.x_, lhs.y_ + rhs.y_, lhs.z_ + rhs.z_};
}

template<typename T>
Vector<T> operator-(const Vector<T>& lhs, const Vector<T>& rhs)
{
    return {lhs.x_ - rhs.x_, lhs.y_ - rhs.y_, lhs.z_ - rhs.z_};
}

template<typename T>
T dot(const Vector<T>& v1, const Vector<T>& v2)
{
    return v1.x_ * v2.x_ + v1.y_ * v2.y_ + v1.z_ * v2.z_;
}

template<typename T>
Vector<T> cross(const Vector<T>& v1, const Vector<T>& v2)
{
    Vector<T> res{};
    res.x_ = v1.y_ * v2.z_ - v1.z_ * v2.y_;
    res.y_ = v1.z_ * v2.x_ - v1.x_ * v2.z_;
    res.z_ = v1.x_ * v2.y_ - v1.y_ * v2.x_;
    return res;
}

template<typename T>
T triple(const Vector<T>& v1, const Vector<T>& v2, const Vector<T>& v3)
{
    return dot(v1, cross(v2, v3));
}

template<typename T>
class Vector2
{
    public:

        T x_;
        T y_;

        Vector2():
            x_(T{0}), y_(T{0})
            {}

        Vector2(const T& x, const T& y):
            x_(x), y_(y)
            {}

        Vector2<T> operator-() const
            {
                return Vector2<T>{-x_, -y_};
            }

        Vector2<T>& operator+=(const Vector2<T>& rhs)
            {
                x_ += rhs.x_;
                y_ += rhs.y_;
                return *this;
            }

        Vector2<T>& operator-=(const Vector2<T>& rhs)
            {
                x_ -= rhs.x_;
                y_ -= rhs.y_;
                return *this;
            }

        Vector2<T>& operator*=(const T& rhs)
            {
                x_ *= rhs;
                y_ *= rhs;
                return *this;
            }

        Vector2<T>& operator/=(const T& rhs)
            {
                x_ /= rhs;
                y_ /= rhs;
                return *this;
            }

        Vector2<T>& normalize()
            {
                T len = length();
                return operator/=(len);
            }

        bool operator==(const Vector2<T>& rhs) const
            {
                return equals<T>(x_, rhs.x_) && equals<T>(y_, rhs.y_);
            }

        bool operator!=(const Vector2<T>& rhs) const
            {
                return !operator==(rhs);
            }

        bool operator!() const
            {
                return operator==({});
            }

        operator bool() const
            {
                return !operator==({});
            }

        T squaredLength() const
            {
                return x_ * x_ + y_ * y_;
            }

        T length() const
            {
                return std::sqrt(squaredLength());
            }

        T getSquaredDistance(const Vector2<T>& v) const
            {
                Vector2<T> offset = *this - v;
                return offset.squaredLength();
            }

        T getDistance(const Vector2<T>& v) const
            {
                return std::sqrt(getSquaredDistance(v));
            }

        void dump() const
            {
                std::cout << "Vector2 {" << x_ << ", " << y_ << "}\n";
                std::cout.flush();
            }

        bool collinear(const Vector2<T>& rhs) const
            {
                return equals(x_ * rhs.y_, y_ * rhs.x_);
            }

        Vector2<T> project(const Vector2<T>& v) const
            {
                return dot2(*this, v) / squaredLength() * *this;
            }
};

template<typename T>
Vector2<T> operator*(const Vector2<T>& lhs, const T& rhs)
{
    return {lhs.x_ * rhs, lhs.y_ * rhs};
}

template<typename T>
Vector2<T> operator*(const T& lhs, const Vector2<T>& rhs)
{
    return {lhs * rhs.x_, lhs * rhs.y_};
}

template<typename T>
Vector2<T> operator/(const Vector2<T>& lhs, const T& rhs)
{
    return {lhs.x_ / rhs, lhs.y_ / rhs};
}

template<typename T>
Vector2<T> operator+(const Vector2<T>& lhs, const Vector2<T>& rhs)
{
    return {lhs.x_ + rhs.x_, lhs.y_ + rhs.y_};
}

template<typename T>
Vector2<T> operator-(const Vector2<T>& lhs, const Vector2<T>& rhs)
{
    return {lhs.x_ - rhs.x_, lhs.y_ - rhs.y_};
}

template<typename T>
T dot2(const Vector2<T>& v1, const Vector2<T>& v2)
{
    return v1.x_ * v2.x_ + v1.y_ * v2.y_;
}

} //namespace triangles
