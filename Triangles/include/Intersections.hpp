#pragma once

#include "Triangle.hpp"
#include "Geometry.hpp"
#include "Segment.hpp"

namespace triangles
{

namespace intersection
{

template<typename T>
static bool sameSign(const T& val1, const T& val2, const T& val3)
    {
        if((greater(val1, T{0}) && greater(val2, T{0}) && greater(val3, T{0})) ||
           (less(val1, T{0}) && less(val2, T{0}) && less(val3, T{0})))
        {
            return true;
        }
        else
        {
            return false;
        }
    }

enum class Coordinate
{
    x,
    y,
    z,
};

template<typename T>
static Coordinate greatestComponent(const Vector<T>& v)
    {
        if(greaterEq(v.x_, v.y_) && greaterEq(v.x_, v.z_))
        {
            return Coordinate::x;
        }
        else if(greaterEq(v.y_, v.x_) && greaterEq(v.y_, v.z_))
        {
            return Coordinate::y;
        }
        else
        {
            return Coordinate::z;
        }
    }

template<typename T>
static Triangle2<T> projectOnCoordinatePlane(const Triangle<T>& t, Coordinate coord)
    {
        switch(coord)
        {
            case Coordinate::x:
            {
                return Triangle2<T>{{t.v1_.y_, t.v1_.z_}, {t.v2_.y_, t.v2_.z_}, {t.v3_.y_, t.v3_.z_}};
            }
            case Coordinate::y:
            {
                return Triangle2<T>{{t.v1_.z_, t.v1_.x_}, {t.v2_.z_, t.v2_.x_}, {t.v3_.z_, t.v3_.x_}};
            }
            case Coordinate::z:
            {
                return Triangle2<T>{{t.v1_.x_, t.v1_.y_}, {t.v2_.x_, t.v2_.y_}, {t.v3_.x_, t.v3_.y_}};
            }
            default:
            {
                assert(0 && "Unreachable");
                return Triangle2<T>{};
            }
        }
    }

template<typename T>
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

//nice site for visualization
//https://www.matheretter.de/geoservant/ru
template<typename T>
bool intersects(const Triangle<T>& t1, const Triangle<T>& t2)
    {
        auto [pointFlag, point] = t1.isPoint();
        if(pointFlag)
        {
            return t2.contains(point);
        }

        auto [segFlag, seg] = t1.isSegment();
        if(segFlag)
        {
            return t2.intersects(seg);
        }

        Plane<T> p1{t1.v1_, t1.v2_, t1.v3_};
        Plane<T> p2{t2.v1_, t2.v2_, t2.v3_};

        auto [type, line] = p1.intersects(p2);

        switch(type)
        {
            case IntersectionType::Parallels:
            {
                return false;
            }
            case IntersectionType::Belongs:
            {
                auto greatestCoord = greatestComponent(p1.norm_);
                Triangle2<T> flatT1 = projectOnCoordinatePlane(t1, greatestCoord);
                Triangle2<T> flatT2 = projectOnCoordinatePlane(t2, greatestCoord);
                return flatT1.intersects(flatT2);
            }
            case IntersectionType::Intersects:
            {
                std::pair<T, Vector<T>> distVert1[3] = {
                    std::make_pair(p2.getSignedDistance(t1.v1_), t1.v1_),
                    std::make_pair(p2.getSignedDistance(t1.v3_), t1.v3_),
                    std::make_pair(p2.getSignedDistance(t1.v2_), t1.v2_),
                };
                if(sameSign(distVert1[0].first, distVert1[1].first, distVert1[2].first))
                {
                    return false;
                }
                renumber(distVert1);

                std::pair<T, Vector<T>> distVert2[3] = {
                    std::make_pair(p1.getSignedDistance(t2.v1_), t2.v1_),
                    std::make_pair(p1.getSignedDistance(t2.v3_), t2.v3_),
                    std::make_pair(p1.getSignedDistance(t2.v2_), t2.v2_),
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
} // namespace intersection

} // namespace triangles
