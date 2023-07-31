#pragma once

#include "Vector.hpp"

namespace triangles
{

template<typename T>
class Line
{
    public:

        Vector<T> p_;
        Vector<T> n_;

        Line():
            p_({}), n_({}) {}

        Line(const Vector<T&> p, const Vector<T>& n):
            p_(p), n_(n) {}

        bool equals(const Line<T>& line) const
            {
                return collinear(n_, line.n_) && collinear(n_, p_ - line.p_);
            }

        bool equalsZero() const
            {
                return n_.equalsZero();
            }

        bool getSquaredDistance(const Vector<T>& p) const
            {
                return cross(n_, p_ - p).getSquaredLength() /
                                      n_.getSquaredLength();
            }
        
        bool getDistance(const Vector<T>& p) const
            {
                return std::sqrt(getSquaredDistance(p));
            }

        T getSquaredDistance(const Line<T>& line) const
            {
                Vector<T> n = cross(n_, line.n_);
                return dot(n, p_ - line.p_) / n.squaredLength();
            }

        T getDistance(const Line<T>& line) const
            {
                return std::sqrt(getSquaredDistance(line));
            }

        Vector<T> project(const Vector<T>& p)
            {
                Vector<T> p_p = p_ - p;
                return p_ + dot(p_p, n_) / dot (n_, n_) * n_;
            }
};

template<typename T>
class Plane
{
    public:

        Vector<T> n_;
        Vector<T> p_;

        Plane():
            n_({}), p_({}) {} 

        Plane(const Vector<T>& n, const Vector<T>& p):
            n_(n), p_(p) {}

        Plane(const T& a, const T& b, const T& c, const T& d):
            n_({a, b, c}), p_({})
            {
                if(!Config<T>.equalsZero(a))
                {
                    p_ = Vector<T>{(-d - b - c) / a, 1, 1};
                }
                else if(!Config<T>.equalsZero(b))
                {
                    p_ = Vector<T>{1, (-d - a - c) / b, 1};
                }
                else if(!Config<T>.equalsZero(c))
                {
                    p_ = Vector<T>{1, 1, (-d - a - b) / c};
                }
                else if(Config<T>.equalsZero(d))
                {
                    p_ = Vector<T>{};
                }
                else
                {
                    n_ = Vector<T>{}; 
                }
            }


        Plane(const Vector<T>& p1, const Vector<T>& p2, const Vector<T>& p3):
            n_(cross(p2 - p1, p3 - p1)), p_(p1)
            {
                if(n_.equalsZero())
                {
                    n_ = Vector<T>{};
                    p_ = Vector<T>{};
                }
            }

        bool equals(const Plane& plane)
            {
                return Config<T>::equalsZero(dot(p_ - plane.p_, n_)) &&
                                                collinear(n_, plane.n_);
            }

        bool equalsZero() const
            {
                return n_.equalsZero();
            }
};

} //namespace triangles
