#pragma once

/*

Class Segment. Geometric object with methods intersects(Segment), contains(point), etc.
Is parameterized with begin and end.

Class Segment2. Similar to Segment, but points are in 2d space.

Class Segment1(Interval).

*/

#include "Vector.hpp"

namespace triangles
{

template<typename T>
class Segment;
template<typename T>
class Segment2;
template<typename T>
class Segment1;

template<typename T>
Orientation orientation(Vector2<T> p, Vector2<T> q, Vector2<T> r);

template<typename T>
class Segment
{
    public:
        Vector<T> v1_;
        Vector<T> v2_;

        Segment():
            v1_(), v2_() {}

        Segment(const Vector<T>& v1, const Vector<T>& v2):
            v1_(v1), v2_(v2) {}

        bool valid() const {
            return v1_ != v2_;
        }

        bool equals(const Segment<T>& seg) const {
            return (v1_ == seg.v1_ && v2_ == seg.v2_) ||
                    (v1_ == seg.v2_ && v2_ == seg.v1_);
        }

        Segment2<T> projectOnAxis(size_t axis) const {
            switch (axis)
            {
            case 0U:
                return Segment2<T>{{v1_.y_, v1_.z_}, {v2_.y_, v2_.z_}};
            case 1U:
                return Segment2<T>{{v1_.z_, v1_.x_}, {v2_.z_, v2_.x_}};
            case 2U:
                return Segment2<T>{{v1_.x_, v1_.y_}, {v2_.x_, v2_.y_}};
            default:
                throw std::out_of_range("Incorrect axis index");
            }
        }

        bool contains(const Vector<T>& v) const {
            Vector dir = v2_ - v1_;
            if (!dir) {
                return v == v1_;
            }
            Vector offset = v - v1_;
            if (!dir.collinear(offset)) {
                return false;
            }

            T t = offset.length() / dir.length();
            if (greater(dot(offset, dir), T{0})) {
                return greaterEq(t, T{0}) && lessEq(t, T{1});
            } else {
                return greaterEq(-t, T{0}) && lessEq(-t, T{1});
            }
        }

        bool intersects(const Segment<T>& seg) const {
            Vector<T> offset1 = seg.v1_ - v1_;
            Vector<T> offset2 = seg.v2_ - v1_;
            Vector<T> offset3 = v2_ - v1_;
            if (!isZero(triple(offset1, offset2, offset3))) {
                return false;
            }
            Vector<T> norm = cross(offset1, offset2);
            if (!norm) {
                norm = cross(offset2, offset3);
            }

            size_t axis = norm.greatestComponent();
            Segment2 seg1{v1_.projectOnAxis(axis), v2_.projectOnAxis(axis)};
            Segment2 seg2{seg.v1_.projectOnAxis(axis), seg.v2_.projectOnAxis(axis)};
            return seg1.intersects(seg2);
        }

        void dump() const {
            std::cout << "Vert1 {" << v1_.x_ << ", " << v1_.y_ << ", " << v1_.z_ << "}\n";
            std::cout << "Vert2 {" << v2_.x_ << ", " << v2_.y_ << ", " << v2_.z_ << "}";
            std::cout << std::endl;
        }
};

template<typename T>
class Segment2
{
    private:
        bool containsCollinearPoint(const Vector2<T>& v) const {
            if (lessEq(v.x_, max(v1_.x_, v2_.x_)) && greaterEq(v.x_, min(v1_.x_, v2_.x_)) &&
                lessEq(v.y_, max(v1_.y_, v2_.y_)) && greaterEq(v.y_, min(v1_.y_, v2_.y_))) {
                return true;
            }
            return false;
        }

    public:
        Vector2<T> v1_;
        Vector2<T> v2_;

        Segment2():
            v1_(), v2_() {}

        Segment2(const Vector2<T>& v1, const Vector2<T>& v2):
            v1_(v1), v2_(v2) {}

        bool valid() const {
            return v1_ != v2_;
        }

        bool equals(const Segment2<T>& seg) const {
            return (v1_ == seg.v1_ && v2_ == seg.v2_) ||
                    (v1_ == seg.v2_ && v2_ == seg.v1_);
        }

        T signedDistance(const Vector2<T>& v) const {
            Vector2<T> seg = v2_ - v1_;
            Vector2<T> offset = v - v1_;
            Vector2<T> perp = (offset - seg.project(offset));
            T len = perp.length();
            return greater(dot2(seg, perp), T{0}) ? -len : len;
        }

        bool contains(const Vector2<T>& v) const {
            if (orientation(v1_, v2_, v) != Orientation::Collinear) {
                return false;
            }
            return containsCollinearPoint(v);
        }

        bool intersects(const Segment2<T>& seg) const {
            Orientation o1 = orientation(v1_, v2_, seg.v1_);
            Orientation o2 = orientation(v1_, v2_, seg.v2_);
            Orientation o3 = orientation(seg.v1_, seg.v2_, v1_);
            Orientation o4 = orientation(seg.v1_, seg.v2_, v2_);

            if ((o1 != o2 && o3 != o4) ||
                (o1 == Orientation::Collinear && containsCollinearPoint(seg.v1_)) ||
                (o2 == Orientation::Collinear && containsCollinearPoint(seg.v2_)) ||
                (o3 == Orientation::Collinear && seg.containsCollinearPoint(v1_)) ||
                (o4 == Orientation::Collinear && seg.containsCollinearPoint(v2_))) {
                return true;
            }
            return false;
        }

        void dump() const {
            std::cout << "Vert1 {" << v1_.x_ << ", " << v1_.y_ << "}\n";
            std::cout << "Vert2 {" << v2_.x_ << ", " << v2_.y_ << "}";
            std::cout << std::endl;
        }
};

template<typename T>
class Segment1
{
    public:
        T val1_;
        T val2_;

        Segment1():
            val1_(T{0}), val2_(T{0}) {}

        Segment1(const T& val1, const T& val2):
            val1_(val1), val2_(val2) {}

        bool valid() const {
            return !::triangles::equals(val1_, val2_);
        }

        bool equals(const Segment1<T>& seg) const {
            return (::triangles::equals(val1_, seg.val1_) && ::triangles::equals(val2_, seg.val2_)) ||
                    (::triangles::equals(val1_, seg.val2_) && ::triangles::equals(val2_, seg.val1_));
        }

        bool contains(const T& val) const {
            return lessEq(val, max(val1_, val2_)) && greaterEq(val, min(val1_, val2_));
        }

        bool intersects(const Segment1<T>& seg) const {
            return greaterEq(max(val1_, val2_), min(seg.val1_, seg.val2_)) &&
                    greaterEq(max(seg.val1_, seg.val2_), min(val1_, val2_));
        }

        void dump() const {
            std::cout << "Val1 {" << val1_ << "}\n";
            std::cout << "Val2 {" << val2_ << "}";
            std::cout << std::endl;
        }
};

} //namespace triangles
