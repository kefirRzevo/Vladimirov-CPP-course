#pragma once

/*

Class BoundingBox. This class is used to speed up checking whether 2 objects
doesn't intersects. The algorithm is very simple and fast.

*/

#include "Triangle.hpp"

namespace triangles
{

template<typename T>
class BoundingBox;

template<typename T>
class BoundingBox
{
    public:
        Vector<T> min_;
        Vector<T> max_;

        BoundingBox():
            min_(), max_() {}

        BoundingBox(const Vector<T>& min, const Vector<T>& max):
            min_(min), max_(max) {}

        BoundingBox(const Triangle<T>& t):
            min_({min(t.v1_.x_, t.v2_.x_, t.v3_.x_), min(t.v1_.y_, t.v2_.y_, t.v3_.y_),
                  min(t.v1_.z_, t.v2_.z_, t.v3_.z_)}),
            max_({max(t.v1_.x_, t.v2_.x_, t.v3_.x_), max(t.v1_.y_, t.v2_.y_, t.v3_.y_),
                  max(t.v1_.z_, t.v2_.z_, t.v3_.z_)}) {}

        bool valid() const {
            return lessEq(min_.x_, max_.x_) && lessEq(min_.y_, max_.y_) && lessEq(min_.z_, max_.z_);
        }

        bool operator==(const BoundingBox<T>& b) const {
            return min_ == b.min_ && max_ == b.max_;
        }

        bool operator!=(const BoundingBox<T>& b) const {
            return !operator==(b);
        }        

        bool intersects(const BoundingBox<T>& b) const {
            std::array<size_t, 3U> axis{0U, 1U, 2U};
            auto intersects = [&](size_t i) { return greaterEq(max_[i], b.min_[i]) && lessEq(min_[i], b.max_[i]); };
            return std::all_of(axis.cbegin(), axis.cend(), intersects);
        }

        bool contains(const Vector<T>& v) const {
            std::array<size_t, 3U> axis{0U, 1U, 2U};
            auto contains = [&](size_t i) { return greaterEq(v[i], min_[i]) && lessEq(v[i], max_[i]); };
            return std::all_of(axis.cbegin(), axis.cend(), contains);
        }

        bool contains(const BoundingBox<T>& box) const {
            std::array<size_t, 3U> axis{0U, 1U, 2U};
            auto contains = [&](size_t i) { return greaterEq(box.min_[i], min_[i]) && lessEq(box.max_[i], max_[i]); };
            return std::all_of(axis.cbegin(), axis.cend(), contains);
        }

        std::pair<bool, Quadrant> intersects(const Vector<T>& sepPoint) const {
            std::array<size_t, 3U> axis{0U, 1U, 2U};
            char quadrant = 0;

            auto intersects = [&](size_t i) {
                if (greater(sepPoint[i], max_[i])) {
                    quadrant |= 1 << (2U - i);
                    return false;
                } else if (less(sepPoint[i], min_[i])) {
                    return false;
                } else {
                    return true;
                }
            };

            if (std::any_of(axis.cbegin(), axis.cend(), intersects)) {
                return std::make_pair(true, Quadrant::count);
            } else {
                return std::make_pair(false, static_cast<Quadrant>(quadrant));
            }
        }

        Vector<T> getCenter() const {
            return (min_ + max_) / T{2};
        }

        BoundingBox<T> split(const Vector<T>& sepPoint, Quadrant quadrant) const {
            if (!contains(sepPoint)) {
                return BoundingBox<T>{};
            }
            BoundingBox<T> res;
            switch (quadrant)
            {
            case Quadrant::xyz:
                res = BoundingBox{sepPoint, max_}; break;
            case Quadrant::xy_z:
                res = BoundingBox{{sepPoint.x_, sepPoint.y_, min_.z_}, {max_.x_, max_.y_, sepPoint.z_}}; break;
            case Quadrant::x_yz:
                res = BoundingBox{{sepPoint.x_, min_.y_, sepPoint.z_}, {max_.x_, sepPoint.y_, max_.z_}}; break;
            case Quadrant::x_y_z:
                res = BoundingBox{{sepPoint.x_, min_.y_, min_.z_}, {max_.x_, sepPoint.y_, sepPoint.z_}}; break;
            case Quadrant::_xyz:
                res = BoundingBox{{min_.x_, sepPoint.y_, sepPoint.z_}, {sepPoint.x_, max_.y_, max_.z_}}; break;
            case Quadrant::_xy_z:
                res = BoundingBox{{min_.x_, sepPoint.y_, min_.z_}, {sepPoint.x_, max_.y_, sepPoint.z_}}; break;
            case Quadrant::_x_yz:
                res = BoundingBox{{min_.x_, min_.y_, sepPoint.z_}, {sepPoint.x_, sepPoint.y_, max_.z_}}; break;
            case Quadrant::_x_y_z:
                res = BoundingBox{min_, sepPoint}; break;
            default:
                throw std::out_of_range("Incorrect index");
            }
            return res.valid() ? res : BoundingBox<T>{};
        }

        void dump() const {
            std::cout << "Min point {" << min_.x_ << ", " << min_.y_ << ", " << min_.z_ << "}\n";
            std::cout << "Max point {" << max_.x_ << ", " << max_.y_ << ", " << max_.z_ << "}";
            std::cout << std::endl;
        }
};

} //namespace triangles
