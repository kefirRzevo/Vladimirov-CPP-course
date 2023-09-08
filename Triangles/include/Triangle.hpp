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
class Plane;

template<typename T>
class Triangle
{
    private:

        static void renumber(std::pair<T, Vector<T>>(& distVert)[3])
            {
                char signs[3] = {};
                for(int i = 0; i < 3; i++)
                {
                    signs[i] = getSign(distVert[i].first);
                }

                if(signs[0] + signs[1] + signs[2] == 0)
                {
                    if(signs[1] != 0)
                    {
                        std::swap(distVert[0], distVert[1]);
                    }
                    return;
                }

                int i = 0;
                while(signs[0] != signs[1] && i < 3)
                {
                    std::swap(signs[i % 3], signs[(i + 1) % 3]);
                    std::swap(distVert[i % 3], distVert[(i + 1) % 3]);
                    i++;
                }
            }

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

        bool operator==(const Triangle<T>& t) const
            {
                return v1_ == t.v1_ && v2_ == t.v2_ && v3_ == t.v3_;
            }

        bool operator!=(const Triangle<T>& t) const
            {
                return !operator==(t);
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

        Triangle2<T> projectOnCoordinatePlane(Coordinate coord) const
            {
                switch(coord)
                {
                    case Coordinate::x:
                    {
                        return Triangle2<T>{{v1_.y_, v1_.z_}, {v2_.y_, v2_.z_}, {v3_.y_, v3_.z_}};
                    }
                    case Coordinate::y:
                    {
                        return Triangle2<T>{{v1_.z_, v1_.x_}, {v2_.z_, v2_.x_}, {v3_.z_, v3_.x_}};
                    }
                    case Coordinate::z:
                    {
                        return Triangle2<T>{{v1_.x_, v1_.y_}, {v2_.x_, v2_.y_}, {v3_.x_, v3_.y_}};
                    }
                    default:
                    {
                        assert(0 && "Unreachable");
                        return Triangle2<T>{};
                    }
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
                    return seg.contains(point);
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

        bool intersects(const Triangle<T>& t) const
            {
                auto [pointFlag, point] = isPoint();
                if(pointFlag)
                {
                    return t.contains(point);
                }

                auto [segFlag, seg] = isSegment();
                if(segFlag)
                {
                    return t.intersects(seg);
                }

                Plane<T> p1{v1_, v2_, v3_};
                Plane<T> p2{t.v1_, t.v2_, t.v3_};

                auto [type, line] = p1.intersects(p2);

                switch(type)
                {
                    case IntersectionType::Parallels:
                    {
                        return false;
                    }
                    case IntersectionType::Belongs:
                    {
                        auto greatestCoord = p1.norm_.greatestComponent();
                        Triangle2<T> flatT1 = projectOnCoordinatePlane(greatestCoord);
                        Triangle2<T> flatT2 = t.projectOnCoordinatePlane(greatestCoord);
                        return flatT1.intersects(flatT2);
                    }
                    case IntersectionType::Intersects:
                    {
                        std::pair<T, Vector<T>> distVert1[3] = {
                            std::make_pair(p2.getSignedDistance(v1_), v1_),
                            std::make_pair(p2.getSignedDistance(v3_), v3_),
                            std::make_pair(p2.getSignedDistance(v2_), v2_),
                        };
                        if(sameSign(distVert1[0].first, distVert1[1].first, distVert1[2].first))
                        {
                            return false;
                        }
                        renumber(distVert1);

                        std::pair<T, Vector<T>> distVert2[3] = {
                            std::make_pair(p1.getSignedDistance(t.v1_), t.v1_),
                            std::make_pair(p1.getSignedDistance(t.v3_), t.v3_),
                            std::make_pair(p1.getSignedDistance(t.v2_), t.v2_),
                        };
                        if(sameSign(distVert2[0].first, distVert2[1].first, distVert2[2].first))
                        {
                            return false;
                        }
                        renumber(distVert2);

                        Vector<T> p = line.point_;
                        Vector<T> d = line.dir_;

                        T v11_ = dot(d, distVert1[0].second - p);
                        T v12_ = dot(d, distVert1[1].second - p);
                        T v13_ = dot(d, distVert1[2].second - p);
                        T t11 = v11_ + (v13_ - v11_) * distVert1[0].first / (distVert1[0].first - distVert1[2].first);
                        T t12 = v12_ + (v13_ - v12_) * distVert1[1].first / (distVert1[1].first - distVert1[2].first);

                        T v21_ = dot(d, distVert2[0].second - p);
                        T v22_ = dot(d, distVert2[1].second - p);
                        T v23_ = dot(d, distVert2[2].second - p);
                        T t21 = v21_ + (v23_ - v21_) * distVert2[0].first / (distVert2[0].first - distVert2[2].first);
                        T t22 = v22_ + (v23_ - v22_) * distVert2[1].first / (distVert2[1].first - distVert2[2].first);

                        Segment1<T> seg1{t11, t12};
                        Segment1<T> seg2{t21, t22};
                        return seg1.intersects(seg2);
                    }
                    default:
                    {
                        assert(0 && "Unreachable");
                        return false;
                    }
                }
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
