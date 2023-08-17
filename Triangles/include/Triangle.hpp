#pragma once

#include "Vector.hpp"
#include "Segment.hpp"
#include "Geometry.hpp"

namespace triangles
{

template<typename T>
class Triangle;
template<typename T>
class Triangle2;

template<typename T>
class Line;

template<typename T>
class Triangle
{
    public:

        Vector<T> v1_;
        Vector<T> v2_;
        Vector<T> v3_;

        Triangle():
            v1_({}), v2_({}), v3_({})
            {}

        Triangle(const Vector<T>& v1, const Vector<T>& v2, const Vector<T>& v3):
            v1_(v1), v2_(v2), v3_(v3)
            {}

        bool equals(const Triangle<T>& t) const
            {
                return  (v1_ == t.v1_ && v2_ == t.v2_ && v3_ == t.v3_) ||
                        (v1_ == t.v1_ && v2_ == t.v3_ && v3_ == t.v2_) ||
                        (v1_ == t.v2_ && v2_ == t.v3_ && v3_ == t.v1_) ||
                        (v1_ == t.v2_ && v2_ == t.v1_ && v3_ == t.v3_) ||
                        (v1_ == t.v3_ && v2_ == t.v1_ && v3_ == t.v2_) ||
                        (v1_ == t.v3_ && v2_ == t.v2_ && v3_ == t.v1_);
            }

        bool valid() const
            {
                Vector s1 = v1_ - v3_;
                Vector s2 = v2_ - v3_;
                if(!s1 || !s2)
                {
                    return false;
                }
                else
                {
                    return !s1.collinear(s2); 
                }
            }

        std::pair<bool, Vector<T>> isPoint() const
            {
                if(v1_ == v2_ && v2_ == v3_)
                {
                    return std::make_pair(true, v1_);
                }
                else
                {
                    return std::make_pair(false, Vector<T>{});
                }
            }

        std::pair<bool, Segment<T>> isSegment() const
            {
                Vector s1 = v1_ - v3_;
                Vector s2 = v2_ - v3_;

                if(s1.collinear(s2))
                {
                    Line line{v3_, s1};

                    auto [flag, param] = line.contains(v2_);
                    if(lessEq(param, T{0}))
                    {
                        return std::make_pair(true, Segment{v2_, v1_});
                    }
                    else if(greaterEq(param, T{1}))
                    {
                        return std::make_pair(true, Segment{v3_, v2_});
                    }
                    else
                    {
                        return std::make_pair(true, Segment{v3_, v1_});
                    }
                }
                else
                {
                    return std::make_pair(false, Segment<T>{});
                }

            }

        bool contains(const Vector<T>& p) const
            {
                auto [pointFlag, point] = isPoint();
                if(pointFlag)
                {
                    return point == p;
                }

                auto[segFlag, seg] = isSegment();
                if(segFlag)
                {
                    return seg.contains(p);
                }

                Vector a = v1_ - p;
                Vector b = v2_ - p;
                Vector c = v3_ - p;

                Vector u = cross(b, c);
                Vector v = cross(c, a);
                Vector w = cross(a, b);

                if(less(dot(u, v), T{0}) || less(dot(u, w), T{0}))
                {
                    return false;
                }
                else
                {
                    return true;
                }
            }

        bool intersects(const Segment<T>& seg) const
            {
                auto [pointFlag, point] = isPoint();
                if(pointFlag)
                {
                    return seg.intersects(Segment{point, point});
                }

                auto[segmentFlag, segment] = isSegment();
                if(segmentFlag)
                {
                    return seg.intersects(segment);
                }

                Vector e1 = v2_ - v1_;
                Vector e2 = v3_ - v1_;
                Vector dir = seg.v2_ - seg.v1_;

                Vector norm = cross(e1, e2);
                if(isZero(dot(norm, dir)))
                {
                    Segment<T> seg1{v2_, v3_};
                    Segment<T> seg2{v3_, v1_};
                    Segment<T> seg3{v1_, v2_};

                    return seg1.intersects(seg) || seg2.intersects(seg) || seg3.intersects(seg);
                }

                Vector p = cross(dir, e2);
                T temp1 = dot(p, e1);
                if(isZero(temp1))
                {
                    return false;
                }

                T temp2 = T{1} / temp1;
                Vector s = seg.v1_ - v1_;

                T u = temp2 * dot(s, p);
                if(less(u, T{0}) || greater(u, T{1}))
                {
                    return false;
                }

                Vector q = cross(s, e1);
                T v = temp2 * dot(s, q);
                if(less(v, T{0}) || greater(v, T{1}))
                {
                    return false;
                }

                T t = temp2 * dot(e2, q);
                return greaterEq(t, T{0}) && lessEq(t, T{1});
            }

        void dump() const
            {
                std::cout << "Vert1 {" << v1_.x_ << ", " << v1_.y_ << ", " << v1_.z_ << "}\n";
                std::cout << "Vert2 {" << v2_.x_ << ", " << v2_.y_ << ", " << v2_.z_ << "}\n";
                std::cout << "Vert3 {" << v3_.x_ << ", " << v3_.y_ << ", " << v3_.z_ << "}\n";
                std::cout.flush();
            }
};

template<typename T>
class Triangle2
{
    public:

        Vector2<T> v1_;
        Vector2<T> v2_;
        Vector2<T> v3_;

        Triangle2():
            v1_({}), v2_({}), v3_({})
            {}

        Triangle2(const Vector2<T>& v1, const Vector2<T>& v2, const Vector2<T>& v3):
            v1_(v1), v2_(v2), v3_(v3)
            {}

        bool equals(const Triangle2<T>& t) const
            {
                return  (v1_ == t.v1_ && v2_ == t.v2_ && v3_ == t.v3_) ||
                        (v1_ == t.v1_ && v2_ == t.v3_ && v3_ == t.v2_) ||
                        (v1_ == t.v2_ && v2_ == t.v3_ && v3_ == t.v1_) ||
                        (v1_ == t.v2_ && v2_ == t.v1_ && v3_ == t.v3_) ||
                        (v1_ == t.v3_ && v2_ == t.v1_ && v3_ == t.v2_) ||
                        (v1_ == t.v3_ && v2_ == t.v2_ && v3_ == t.v1_);
            }

        bool valid() const
            {
                Vector2 s1 = v1_ - v3_;
                Vector2 s2 = v2_ - v3_;
                if(!s1 || !s2)
                {
                    return false;
                }
                else
                {
                    return !s1.collinear(s2); 
                }
            }

        bool intersects(const Triangle2<T>& t) const
            {
                Segment2<T> seg11{v2_, v3_};
                Segment2<T> seg12{v3_, v1_};
                Segment2<T> seg13{v1_, v2_};

                Segment2<T> seg21{t.v2_, t.v3_};
                Segment2<T> seg22{t.v3_, t.v1_};
                Segment2<T> seg23{t.v1_, t.v2_};

                if(seg11.intersects(seg21) || seg11.intersects(seg22) || seg11.intersects(seg23) ||
                   seg12.intersects(seg21) || seg12.intersects(seg22) || seg12.intersects(seg23) ||
                   seg13.intersects(seg21) || seg13.intersects(seg22) || seg13.intersects(seg23))
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
                std::cout << "Vert3 {" << v3_.x_ << ", " << v3_.y_ << "}\n";
            }
};

} //namespace triangles
