#pragma once

#include <iostream>
#include <cmath>

#include "Config.hpp"

namespace triangles
{

template<typename T>
class Vector
{
    public:

        T x_;
        T y_;
        T z_;

        Vector():
            x_(0), y_(0), z_(0) {}

        Vector(const T& x, const T& y, const T& z):
            x_(x), y_(y), z_(z) {}

        Vector<T> operator-() const
            {
                return Vector<T>{-x_, -y_, -z_};
            }

        Vector<T> operator+() const
            {
                return *this;
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

        bool equals(const Vector<T>& vector) const
            {
                return  std::abs(x_ - vector.x_) <= Config<T>::epsilon &&
                        std::abs(y_ - vector.y_) <= Config<T>::epsilon &&
                        std::abs(z_ - vector.z_) <= Config<T>::epsilon;
            }

        bool equalsZero() const
            {
                return  equals({});
            }

        T squaredLength() const
            {
                return x_ * x_ + y_ * y_ + z_ * z_;
            }

        T length() const
            {
                return std::sqrt(squaredLength);
            }

        T getSquaredDistance(const Vector<T>& vector) const
            {
                Vector<T> offset{*this - vector};
                return offset.squaredLength();
            }

        T getDistance(const Vector<T>& p) const
            {
                return std::sqrt(getSquaredDistance(p));
            }

        void dump() const
            {
                std::cout << x_ << " " << y_ << " " << z_ << std::endl;
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
T dot(const Vector<T>& lhs, const Vector<T>& rhs)
{
    return lhs.x_ * rhs.x_ + lhs.y_ * rhs.y_ + lhs.z_ * rhs.z_;
}

template<typename T>
Vector<T> cross(const Vector<T>& lhs, const Vector<T>& rhs)
{
    Vector<T> res{};
    res.x_ = lhs.y_ * rhs.z_ - lhs.z_ * rhs.y_;
    res.y_ = lhs.z_ * rhs.x_ - lhs.x_ * rhs.z_;
    res.z_ = lhs.x_ * rhs.y_ - lhs.y_ * rhs.x_;
    return res;
}

template<typename T>
Vector<T> triple(const Vector<T>& a, const Vector<T>& b, const Vector<T>& c)
{
    return dot(a, cross(b, c));
}

template<typename T>
bool collinear(const Vector<T>& lhs, const Vector<T>& rhs)
{
    return cross(lhs, rhs).equalsZero();
}

} //nnamespace triangles