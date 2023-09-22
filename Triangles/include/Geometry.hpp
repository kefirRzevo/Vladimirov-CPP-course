#pragma once

/*

Class Line. Geometry object with typical methods like intersects() or
contains(). Is parametrized with any point on the line and it's direction.

Class Plane. Is parametrized with any point on the plane and normal to it.

*/

#include "Triangle.hpp"
#include "Segment.hpp"
#include "Vector.hpp"

namespace triangles
{

template<typename T>
class Line;
template<typename T>
class Plane;

template<typename T>
class Segment;
template<typename T>
class Triangle;

template<typename T>
class Line {

    public:
        Vector<T> point_;
        Vector<T> dir_;

        Line():
            point_(), dir_() {}

        Line(const Vector<T>& point, const Vector<T>& dir):
            point_(point), dir_(dir) {}

        bool valid() const {
            return dir_;
        }

        bool equals(const Line<T>& l) const {
            if (!dir_ && !l.dir_) {
                return point_ == l.point_;
            } else if (dir_ && l.dir_) {
                return dir_.collinear(l.dir_) && dir_.collinear(point_ - l.point_);
            } else {
                return false;
            }
        }

        T squaredDistance(const Vector<T>& v) const {
            if (!dir_) {
                return point_.squaredDistance(v);
            }
            return cross(dir_, point_ - v).squaredLength() / dir_.squaredLength();
        }
        
        T distance(const Vector<T>& v) const {
            return std::sqrt(squaredDistance(v));
        }

        T distance(const Line<T>& l) const {
            if (!dir_) {
                return l.distance(point_);
            } else if (!l.dir_) {
                return distance(l.point_);
            }

            Vector dir = cross(dir_, l.dir_);
            Vector offset = point_ - l.point_;
            if (dir) {
                return std::abs(dot(dir, offset)) / dir.length();
            } else {
                Vector dist = offset - dir_.project(offset);
                return dist.length();
            }
        }

        Vector<T> project(const Vector<T>& v) const {
            if (!dir_) {
                return point_;
            }
            return point_ + dir_.project(v - point_);
        }

        std::pair<bool, T> contains(const Vector<T>& v) const {
            if (!dir_) {
                return v == point_ ? std::make_pair(true, T{0}) : std::make_pair(false, T{0});
            }

            Vector offset = v - point_;
            if (dir_.collinear(offset)) {
                T t = offset.length() / dir_.length();
                return greater(dot(offset, dir_), T{0}) ? std::make_pair(true, t) : std::make_pair(true, -t);
            } else {
                return std::make_pair(false, T{0});
            }
        }

        void dump() {
            std::cout << "Point {" << point_.x_ << ", " << point_.y_ << ", " << point_.z_ << "}\n";
            std::cout << "Dir   {" << dir_.x_   << ", " << dir_.y_   << ", " << dir_.z_   << "}";
            std::cout << std::endl;
        }

};

template<typename T>
class Plane {

    public:
        Vector<T> point_;
        Vector<T> norm_;

        Plane():
            point_(), norm_() {}

        Plane(const Vector<T>& point, const Vector<T>& norm):
            point_(point), norm_(norm) {}

        Plane(const T& a, const T& b, const T& c, const T& d):
            point_(), norm_({a, b, c}) {
            bool aGreaterB = greaterEq(a, b);
            bool bGreaterC = greaterEq(b, c);
            bool cGreaterA = greaterEq(c, a);
            if (!isZero(a) && aGreaterB && !cGreaterA) {
                point_ = Vector{(-d - b - c) / a, T{1}, T{1}};
            } else if (!isZero(b) && bGreaterC && !aGreaterB) {
                point_ = Vector{T{1}, (-d - a - c) / b, T{1}};
            } else if (!isZero(c) && cGreaterA && !bGreaterC) {
                point_ = Vector{T{1}, T{1}, (-d - a - b) / c};
            } else if (!isZero(d)) {
                point_ = Vector<T>{};
            } else {
                norm_ = Vector<T>{}; 
            }
        }

        Plane(const Vector<T>& v1, const Vector<T>& v2, const Vector<T>& v3):
            point_(v1), norm_(cross(v2 - v1, v3 - v1)) {}

        bool valid() const {
            return norm_;
        }

        bool equals(const Plane& p) {
            if (!norm_ && !p.norm_) {
                return point_ == p.point_;
            } else if (norm_ && p.norm_) {
                return norm_.collinear(p.norm_) && isZero(dot(point_ - p.point_, norm_));
            } else {
                return false;
            }
        }

        T signedDistance(const Vector<T>& v) const {
            if (!norm_) {
                return point_.distance(v);
            }
            return dot(v - point_, norm_) / norm_.length();
        }

        T distance(const Vector<T>& v) const {
            return std::abs(signedDistance(v));
        }

        Vector<T> project(const Vector<T>& v) const {
            if (!norm_) {
                return point_;
            }
            return v + dot(norm_, point_ - v) * norm_ / norm_.squaredLength();
        }

        Line<T> project(const Line<T>& l) const {
            if (!norm_) {
                return Line{point_, Vector<T>{}};
            }
            return Line{project(l.point_), l.dir_ - norm_.project(l.dir_)};
        }

        Triangle<T> project(const Triangle<T>& t) const {
            Vector<T> v1 = project(t.v1_);
            Vector<T> v2 = project(t.v2_);
            Vector<T> v3 = project(t.v3_);
            return Triangle{v1, v2, v3};
        }

        std::pair<IntersectionType, Vector<T>> intersects(const Line<T>& l) const {
            T dot1 = dot(l.dir_, norm_);
            T dot2 = dot(point_ - l.point_, norm_);
            if (!norm_ || !l.dir_) {
                return std::make_pair(IntersectionType::Undefined, Vector<T>{});
            }

            if (isZero(dot1)) {
                if (isZero(dot2)) {
                    return std::make_pair(IntersectionType::Belongs, Vector<T>{});
                } else {
                    return std::make_pair(IntersectionType::Parallel, Vector<T>{});
                }
            }

            T t = dot2 / dot1;
            return std::make_pair(IntersectionType::Intersects, l.point_ + t * l.dir_);;
        }

        std::pair<IntersectionType, Line<T>> intersects(const Plane<T>& p) const {
            if (!norm_ || !p.norm_) {
                return std::make_pair(IntersectionType::Undefined, Line<T>{});
            }
            Vector<T> dir = cross(norm_, p.norm_);
            if (!dir) {
                if (isZero(dot(point_ - p.point_, norm_))) {
                    return std::make_pair(IntersectionType::Belongs, Line<T>{});
                } else {
                    return std::make_pair(IntersectionType::Parallel, Line<T>{});
                }
            }

            T norm = dot(norm_, p.norm_);
            T s1 = dot(point_, norm_);
            T s2 = dot(p.point_, p.norm_);
            T l1 = norm_.squaredLength();
            T l2 = p.norm_.squaredLength();
            T denominator = norm * norm - l1 * l2;
            T a = (s2 * norm - s1 * l2) / denominator;
            T b = (s1 * norm - s2 * l1) / denominator;
            return std::make_pair(IntersectionType::Intersects, Line{a * norm_ + b * p.norm_, dir});
        }

        std::pair<IntersectionType, Vector<T>> intersects(const Segment<T>& seg) const {
            if (!norm_) {
                return std::make_pair(IntersectionType::Undefined, Vector<T>{});
            }

            T dist1 = signedDistance(seg.v1_);
            T dist2 = signedDistance(seg.v2_);
            if (isZero(dist1) && isZero(dist2)) {
                return std::make_pair(IntersectionType::Belongs, Vector<T>{});
            } else if (sameSign(dist1, dist2)) {
                return std::make_pair(IntersectionType::Parallel, Vector<T>{});
            }

            Vector<T> intersection = (dist1 / (dist1 - dist2)) * (seg.v2_ - seg.v1_) + seg.v1_;
            return std::make_pair(IntersectionType::Intersects, intersection);
        }

        void dump() const {
            std::cout << "Point  {" << point_.x_ << ", " << point_.y_ << ", " << point_.z_ << "}\n";
            std::cout << "Normal {" << norm_.x_  << ", " << norm_.y_  << ", " << norm_.z_  << "}";
            std::cout << std::endl;
        }

        void dumpABCD() const {
            std::cout << "a " << norm_.x_ << "\n";
            std::cout << "b " << norm_.y_ << "\n";
            std::cout << "c " << norm_.z_ << "\n";
            std::cout << "d " << -dot(norm_, point_);
            std::cout << std::endl;
        }
};

} //namespace triangles
