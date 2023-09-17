#pragma once

#include "Triangle.hpp"

namespace triangles
{

template<typename T>
class BoundingBox;

template<typename T>
class BoundingBox {

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
            for (size_t i = 0; i < 3U; ++i) {
                if (greater(min_[i], b.max_[i]) || greater(b.min_[i], max_[i])) {
                    return false;
                }
            }
            return true;
        }

        bool contains(const Vector<T>& v) const {
            for (size_t i = 0; i < 3U; ++i) {
                if (!greaterEq(v[i], min_[i]) || !lessEq(v[i], max_[i])) {
                    return false;
                }
            }
            return true;
        }

        bool contains(const BoundingBox<T>& box) const {
            for (size_t i = 0; i < 3U; ++i) {
                if (!greaterEq(box.min_[i], min_[i]) || !lessEq(box.max_[i], max_[i])) {
                    return false;
                }
            }
            return true;
        }

        std::pair<bool, Quadrant> intersects(const Vector<T>& point) const {
            char quadrant = 0;
            bool intersects = true;

            for (size_t i = 0; i < 3U; ++i) {
                if (greater(point[i], max_[i])) {
                    quadrant |= 1 << (2U - i);
                } else if (!less(point[i], min_[i])) {
                    intersects = false;
                    break;
                }
            }

            if (intersects) {
                return std::make_pair(false, static_cast<Quadrant>(quadrant));
            } else {
                return std::make_pair(true, Quadrant::count);
            }
        }

        Vector<T> getCenter() const {
            return (min_ + max_) / T{2};
        }

        void dump() const {
            std::cout << "Min point {" << min_.x_ << ", " << min_.y_ << ", " << min_.z_ << "}\n";
            std::cout << "Max point {" << max_.x_ << ", " << max_.y_ << ", " << max_.z_ << "}\n";
            std::cout.flush();
        }
};

} //namespace triangles
