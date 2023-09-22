#pragma once

/*

Class Vector. Implementation of good known math object with sume useful methods.

Class Vector2. Same to class Vector, but in 2d space.

*/

#include <cmath>
#include <iostream>
#include <stdexcept>
#include "Utils.hpp"

namespace triangles {

template<typename T>
class Vector;
template<typename T>
class Vector2;

template<typename T>
class Vector {

    public:
        T x_;
        T y_;
        T z_;

        Vector():
            x_(T{0}), y_(T{0}), z_(T{0}) {}

        Vector(const T& x, const T& y, const T& z):
            x_(x), y_(y), z_(z) {}

        Vector<T>& operator+=(const Vector<T>& rhs) {
            x_ += rhs.x_;
            y_ += rhs.y_;
            z_ += rhs.z_;
            return *this;
        }

        Vector<T>& operator-=(const Vector<T>& rhs) {
            x_ -= rhs.x_;
            y_ -= rhs.y_;
            z_ -= rhs.z_;
            return *this;
        }

        Vector<T>& operator*=(const T& rhs) {
            x_ *= rhs;
            y_ *= rhs;
            z_ *= rhs;
            return *this;
        }

        Vector<T>& operator/=(const T& rhs) {
            x_ /= rhs;
            y_ /= rhs;
            z_ /= rhs;
            return *this;
        }

        Vector<T> operator-() const {
            return Vector<T>{-x_, -y_, -z_};
        }

        bool operator==(const Vector<T>& rhs) const {
            return equals(x_, rhs.x_) && equals(y_, rhs.y_) && equals(z_, rhs.z_);
        }

        bool operator!=(const Vector<T>& rhs) const {
            return !operator==(rhs);
        }

        bool operator!() const {
            return operator==({});
        }

        operator bool() const {
            return !operator==({});
        }

        T& operator[](size_t index) {
            switch (index)
            {
            case 0U:
                return x_;
            case 1U:
                return y_;
            case 2U:
                return z_;
            default:
                throw std::out_of_range("Incorrect index");
            }
        }

        const T& operator[](size_t index) const {
            switch (index)
            {
            case 0U:
                return x_;
            case 1U:
                return y_;
            case 2U:
                return z_;
            default:
                throw std::out_of_range("Incorrect index");
            }
        }

        bool valid() const {
            return x_ == x_ && y_ == y_ && z_ == z_;
        }

        Vector<T>& normalize() {
            T len = length();
            if (isZero(len)) {
                return *this;
            }
            return operator/=(len);
        }

        T squaredLength() const {
            return x_ * x_ + y_ * y_ + z_ * z_;
        }

        T length() const {
            return std::sqrt(squaredLength());
        }

        T squaredDistance(const Vector<T>& v) const {
            Vector<T> offset = *this - v;
            return offset.squaredLength();
        }

        T distance(const Vector<T>& v) const {
            return std::sqrt(squaredDistance(v));
        }

        bool collinear(const Vector<T>& rhs) const {
            return !cross(*this, rhs);
        }

        Vector<T> project(const Vector<T>& v) const {
            T squaredLen = squaredLength();
            if (isZero(squaredLen)) {
                return Vector<T>{};
            }
            return dot(*this, v) / squaredLen * *this;
        }

        size_t greatestComponent() const {
            if (greaterEq(x_, y_) && greaterEq(x_, z_)) {
                return 0U;
            } else if (greaterEq(y_, x_) && greaterEq(y_, z_)) {
                return 1U;
            } else {
                return 2U;
            }
        }

        Vector2<T> projectOnAxis(size_t axis) const {
            switch (axis)
            {
            case 0U:
                return Vector2{y_, z_};
            case 1U:
                return Vector2{x_, z_};
            case 2U:
                return Vector2{x_, y_};
            default:
                throw std::out_of_range("Incorrect axis index");
            }
        }

        void dump() const {
            std::cout << "Vector {" << x_ << ", " << y_ << ", " << z_ << "}";
            std::cout << std::endl;
        }
};

template<typename T>
Vector<T> operator*(const Vector<T>& lhs, const T& rhs) {
    return {lhs.x_ * rhs, lhs.y_ * rhs, lhs.z_ * rhs};
}

template<typename T>
Vector<T> operator*(const T& lhs, const Vector<T>& rhs) {
    return {lhs * rhs.x_, lhs * rhs.y_, lhs * rhs.z_};
}

template<typename T>
Vector<T> operator/(const Vector<T>& lhs, const T& rhs) {
    return {lhs.x_ / rhs, lhs.y_ / rhs, lhs.z_ / rhs};
}

template<typename T>
Vector<T> operator+(const Vector<T>& lhs, const Vector<T>& rhs) {
    return {lhs.x_ + rhs.x_, lhs.y_ + rhs.y_, lhs.z_ + rhs.z_};
}

template<typename T>
Vector<T> operator-(const Vector<T>& lhs, const Vector<T>& rhs) {
    return {lhs.x_ - rhs.x_, lhs.y_ - rhs.y_, lhs.z_ - rhs.z_};
}

template<typename T>
T dot(const Vector<T>& v1, const Vector<T>& v2)
{
    return v1.x_ * v2.x_ + v1.y_ * v2.y_ + v1.z_ * v2.z_;
}

template<typename T>
Vector<T> cross(const Vector<T>& v1, const Vector<T>& v2) {
    Vector<T> res{};
    res.x_ = v1.y_ * v2.z_ - v1.z_ * v2.y_;
    res.y_ = v1.z_ * v2.x_ - v1.x_ * v2.z_;
    res.z_ = v1.x_ * v2.y_ - v1.y_ * v2.x_;
    return res;
}

template<typename T>
T triple(const Vector<T>& v1, const Vector<T>& v2, const Vector<T>& v3) {
    return dot(v1, cross(v2, v3));
}

template<typename T>
class Vector2 {

    public:
        T x_;
        T y_;

        Vector2():
            x_(T{0}), y_(T{0}) {}

        Vector2(const T& x, const T& y):
            x_(x), y_(y) {}

        Vector2<T>& operator+=(const Vector2<T>& rhs) {
            x_ += rhs.x_;
            y_ += rhs.y_;
            return *this;
        }

        Vector2<T>& operator-=(const Vector2<T>& rhs) {
            x_ -= rhs.x_;
            y_ -= rhs.y_;
            return *this;
        }

        Vector2<T>& operator*=(const T& rhs) {
            x_ *= rhs;
            y_ *= rhs;
            return *this;
        }

        Vector2<T>& operator/=(const T& rhs) {
            x_ /= rhs;
            y_ /= rhs;
            return *this;
        }

        Vector2<T> operator-() const {
            return Vector2<T>{-x_, -y_};
        }

        bool operator==(const Vector2<T>& rhs) const {
            return equals<T>(x_, rhs.x_) && equals<T>(y_, rhs.y_);
        }

        bool operator!=(const Vector2<T>& rhs) const {
            return !operator==(rhs);
        }

        bool operator!() const {
            return operator==({});
        }

        operator bool() const {
            return !operator==({});
        }

        T& operator[](size_t index) {
            switch (index)
            {
            case 0U:
                return x_;
            case 1U:
                return y_;
            default:
                throw std::out_of_range("Incorrect index");
            }
        }

        const T& operator[](size_t index) const {
            switch (index)
            {
            case 0U:
                return x_;
            case 1U:
                return y_;
            default:
                throw std::out_of_range("Incorrect index");
            }
        }

        bool valid() const {
            return x_ == x_ && y_ == y_;
        }

        Vector2<T>& normalize() {
            T len = length();
            if (isZero(len)) {
                return *this;
            }
            return operator/=(len);
        }

        T squaredLength() const {
            return x_ * x_ + y_ * y_;
        }

        T length() const {
            return std::sqrt(squaredLength());
        }

        T squaredDistance(const Vector2<T>& v) const {
            Vector2<T> offset = *this - v;
            return offset.squaredLength();
        }

        T distance(const Vector2<T>& v) const {
            return std::sqrt(getSquaredDistance(v));
        }

        bool collinear(const Vector2<T>& rhs) const {
            return equals(x_ * rhs.y_, y_ * rhs.x_);
        }

        Vector2<T> project(const Vector2<T>& v) const {
            T squaredLen = squaredLength();
            if (isZero(squaredLen)) {
                return Vector2<T>{};
            }
            return dot2(*this, v) / squaredLen * *this;
        }

        size_t greatestComponent() const {
            if (greaterEq(x_, y_)) {
                return 0U;
            } else {
                return 1U;
            }
        }

        void dump() const {
            std::cout << "Vector2 {" << x_ << ", " << y_ << "}";
            std::cout << std::endl;
        }
};

template<typename T>
Vector2<T> operator*(const Vector2<T>& lhs, const T& rhs) {
    return {lhs.x_ * rhs, lhs.y_ * rhs};
}

template<typename T>
Vector2<T> operator*(const T& lhs, const Vector2<T>& rhs) {
    return {lhs * rhs.x_, lhs * rhs.y_};
}

template<typename T>
Vector2<T> operator/(const Vector2<T>& lhs, const T& rhs) {
    return {lhs.x_ / rhs, lhs.y_ / rhs};
}

template<typename T>
Vector2<T> operator+(const Vector2<T>& lhs, const Vector2<T>& rhs) {
    return {lhs.x_ + rhs.x_, lhs.y_ + rhs.y_};
}

template<typename T>
Vector2<T> operator-(const Vector2<T>& lhs, const Vector2<T>& rhs) {
    return {lhs.x_ - rhs.x_, lhs.y_ - rhs.y_};
}

template<typename T>
T dot2(const Vector2<T>& v1, const Vector2<T>& v2) {
    return v1.x_ * v2.x_ + v1.y_ * v2.y_;
}

} //namespace triangles
