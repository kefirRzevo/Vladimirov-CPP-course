#pragma once

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
            min_({}), max_({})
            {}

        BoundingBox(const Vector<T>& min, const Vector<T>& max):
            min_(min), max_(max)
            {}

        BoundingBox(const Triangle<T>& t):
            min_({min(t.v1_.x_, t.v2_.x_, t.v3_.x_), min(t.v1_.y_, t.v2_.y_, t.v3_.y_),
                  min(t.v1_.z_, t.v2_.z_, t.v3_.z_)}),
            max_({max(t.v1_.x_, t.v2_.x_, t.v3_.x_), max(t.v1_.y_, t.v2_.y_, t.v3_.y_),
                  max(t.v1_.z_, t.v2_.z_, t.v3_.z_)})
            {}

        bool valid() const
            {
                return lessEq(min_.x_, max_.x_) && lessEq(min_.y_, max_.y_) && lessEq(min_.z_, max_.z_);
            }

        bool operator==(const BoundingBox<T>& b) const
            {
                return min_ == b.min_ && max_ == b.max_;
            }

        bool operator!=(const BoundingBox<T>& b) const
            {
                return !operator==(b);
            }        

        bool intersects(const BoundingBox<T>& b) const
            {
                if(greater(min_.x_, b.max_.x_) || greater(b.min_.x_, max_.x_) ||
                   greater(min_.y_, b.max_.y_) || greater(b.min_.y_, max_.y_) ||
                   greater(min_.z_, b.max_.z_) || greater(b.min_.z_, max_.z_))
                {
                    return false;
                }
                else
                {
                    return true;
                }
            }

        bool contains(const Vector<T>& v) const
            {
                if(greaterEq(v.x_, min_.x_) && lessEq(v.x_, max_.x_) &&
                   greaterEq(v.y_, min_.y_) && lessEq(v.y_, max_.y_) &&
                   greaterEq(v.z_, min_.z_) && lessEq(v.z_, max_.z_))
                {
                    return true;
                }
                else
                {
                    return false;
                }
            }

        bool contains(const BoundingBox<T>& box) const
            {
                if(greaterEq(box.min_.x_, min_.x_) && lessEq(box.max_.x_, max_.x_) &&
                   greaterEq(box.min_.y_, min_.y_) && lessEq(box.max_.y_, max_.y_) &&
                   greaterEq(box.min_.z_, min_.z_) && lessEq(box.max_.z_, max_.z_))
                {
                    return true;
                }
                else
                {
                    return false;
                }
            }

        std::pair<bool, Quadrant> intersects(const Vector<T>& partition) const
            {
                char quadrant = 0;
                bool intersects = true;

                if(greater(partition.x_, max_.x_))
                {
                    quadrant |= 1 << 2;
                }
                else if(!less(partition.x_, min_.x_))
                {
                    intersects = false;
                }

                if(greater(partition.y_, max_.y_))
                {
                    quadrant |= 1 << 1;
                }
                else if(!less(partition.y_, min_.y_))
                {
                    intersects = false;
                }

                if(greater(partition.z_, max_.z_))
                {
                   quadrant |= 1 << 0;
                }
                else if(!less(partition.z_, min_.z_))
                {
                    intersects = false;
                }

                if(intersects)
                {
                    return std::make_pair(false, static_cast<Quadrant>(quadrant));
                }
                else
                {
                    return std::make_pair(true, Quadrant::count);
                }
            }

        Vector<T> getCenter() const
            {
                return (min_ + max_) / T{2};
            }

        void dump() const
            {
                std::cout << "Min point {" << min_.x_ << ", " << min_.y_ << ", " << min_.z_ << "}\n";
                std::cout << "Max point {" << max_.x_ << ", " << max_.y_ << ", " << max_.z_ << "}\n";
                std::cout.flush();
            }
};

} //namespace triangles
