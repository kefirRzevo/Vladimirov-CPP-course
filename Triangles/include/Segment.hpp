#pragma once

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
class Segment
{
    public:

        Vector<T> v1_;
        Vector<T> v2_;

        Segment():
            v1_({}), v2_({})
            {}

        Segment(const Vector<T>& v1, const Vector<T>& v2):
            v1_(v1), v2_(v2)
            {}

        bool equals(const Segment<T>& seg) const
            {
                return (v1_ == seg.v1_ && v2_ == seg.v2_) ||
                       (v1_ == seg.v2_ && v2_ == seg.v1_);
            }

        bool valid() const
            {
                return v1_ != v2_;
            }

        bool contains(const Vector<T>& v) const
            {
                Vector dir = v2_ - v1_;
                if(!dir)
                {
                    return v == v1_;
                }

                Vector offset = v - v1_;
                if(dir.collinear(offset))
                {
                    T t = offset.length() / dir.length();
                    if(greater(dot(offset, dir), T{0}))
                    {
                        return greaterEq(t, T{0}) && lessEq(t, T{1});
                    }
                    else
                    {
                        return greaterEq(-t, T{0}) && lessEq(-t, T{1});
                    }
                }
                else
                {
                    return false;
                }
            }

        bool intersects(const Segment<T>& seg) const
            {
                T a11 = v1_.x_ - v2_.x_;
                T a21 = v1_.y_ - v2_.y_;
                T a31 = v1_.z_ - v2_.z_;
                T a12 = seg.v1_.x_ - seg.v2_.x_;
                T a22 = seg.v1_.y_ - seg.v2_.y_;
                T a32 = seg.v1_.z_ - seg.v2_.z_;
                T b1 = seg.v1_.x_ - v2_.x_;
                T b2 = seg.v1_.y_ - v2_.y_;
                T b3 = seg.v1_.z_ - v2_.z_;

                T det = a11 * a22 - a12 * a21;
                T det1 = b1 * a22 - b2 * a12;
                T det2 = a11 * b2 - a21 * b1;

                if(isZero(det))
                {
                    if(!isZero(det1) || !isZero(det2))
                    {
                        return false;
                    }
                    else
                    {
                        if(!isZero(b3))
                        {
                            if(!isZero(a31) || !isZero(a32))
                            {
                                return true;
                            }
                            else
                            {
                                return false;
                            }
                        }
                        else
                        {
                            return true;
                        }
                    }
                }
                T t = det1 / det;
                T s = det2 / det;

                if(!isZero(t * a31 + s * a32 - b3))
                {
                    return false;
                }
                else
                {
                    return greaterEq(t, T{0}) && lessEq(t, T{1}) &&
                           greaterEq(s, T{0}) && lessEq(s, T{1});
                }
            }

        void dump() const
            {
                std::cout << "Vert1 {" << v1_.x_ << ", " << v1_.y_ << ", " << v1_.z_ << "}\n";
                std::cout << "Vert2 {" << v2_.x_ << ", " << v2_.y_ << ", " << v2_.z_ << "}\n";
                std::cout.flush();
            }
};

template<typename T>
class Segment2
{
    private:

        enum class Orientation
        {
            Collinear,
            Clockwise,
            Counterclockwise,
        };

        static Orientation orientation(Vector2<T> p, Vector2<T> q, Vector2<T> r)
            {
                T val = (q.y_ - p.y_) * (r.x_ - q.x_) -
                        (q.x_ - p.x_) * (r.y_ - q.y_);

                if(isZero(val))
                {
                    return Orientation::Collinear;
                }
                else
                {
                    if(greater(val, T{0}))
                    {
                        return Orientation::Clockwise;
                    }
                    else
                    {
                        return Orientation::Counterclockwise;
                    }
                }
            }

        bool containsCollinearPoint(const Vector2<T>& v) const
            {
                if(lessEq(v.x_, max(v1_.x_, v2_.x_)) && greaterEq(v.x_, min(v1_.x_, v2_.x_)) &&
                   lessEq(v.y_, max(v1_.y_, v2_.y_)) && greaterEq(v.y_, min(v1_.y_, v2_.y_)))
                {
                    return true;
                }
                else
                {
                    return false;
                }
            }

    public:

        Vector2<T> v1_;
        Vector2<T> v2_;

        Segment2():
            v1_({}), v2_({})
            {}

        Segment2(const Vector2<T>& v1, const Vector2<T>& v2):
            v1_(v1), v2_(v2)
            {}

        bool equals(const Segment2<T>& seg) const
            {
                return (v1_ == seg.v1_ && v2_ == seg.v2_) ||
                       (v1_ == seg.v2_ && v2_ == seg.v1_);
            }

        bool valid() const
            {
                return v1_ != v2_;
            }

        bool contains(const Vector2<T>& v) const
        {
            if(orientation(v1_, v2_, v) != Orientation::Collinear)
            {
                return false;
            }

            return containsCollinearPoint(v);
        }

        bool intersects(const Segment2<T>& seg) const
            {
                Orientation o1 = orientation(v1_, v2_, seg.v1_);
                Orientation o2 = orientation(v1_, v2_, seg.v2_);
                Orientation o3 = orientation(seg.v1_, seg.v2_, v1_);
                Orientation o4 = orientation(seg.v1_, seg.v2_, v2_);

                if((o1 != o2 && o3 != o4) ||
                (o1 == Orientation::Collinear && containsCollinearPoint(seg.v1_)) ||
                (o2 == Orientation::Collinear && containsCollinearPoint(seg.v2_)) ||
                (o3 == Orientation::Collinear && seg.containsCollinearPoint(v1_)) || 
                (o4 == Orientation::Collinear && seg.containsCollinearPoint(v2_)))
                {
                    return true;
                }
                else
                {
                    return false;
                }
            }

        void dump() const
            {
                std::cout << "Vert1 {" << v1_.x_ << ", " << v1_.y_ << "}\n";
                std::cout << "Vert2 {" << v2_.x_ << ", " << v2_.y_ << "}\n";
            }
};

template<typename T>
class Segment1
{
    public:

        T val1_;
        T val2_;

        Segment1():
            val1_(T{0}), val2_(T{0})
            {}

        Segment1(const T& val1, const T& val2):
            val1_(val1), val2_(val2)
            {}

        bool equals(const Segment1<T>& seg) const
            {
                return (compare::equals(val1_, seg.val1_) && compare::equals(val2_, seg.val2_)) ||
                       (compare::equals(val1_, seg.val2_) && compare::equals(val2_, seg.val1_));
            }

        bool valid() const
            {
                return !compare::equals(val1_, val2_);
            }

        bool contains(const T& val) const
            {
                return lessEq(val, max(val1_, val2_)) && greaterEq(val, min(val1_, val2_));
            }

        bool intersects(const Segment1<T>& seg) const
            {
                return greaterEq(max(val1_, val2_), min(seg.val1_, seg.val2_)) &&
                       greaterEq(max(seg.val1_, seg.val2_), min(val1_, val2_));
            }

        void dump() const
            {
                std::cout << "Val1 {" << val1_ << "}\n";
                std::cout << "Val2 {" << val2_ << "}\n";
            }
};

} //namespace triangles
