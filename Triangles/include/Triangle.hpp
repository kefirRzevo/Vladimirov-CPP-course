#pragma once

/*

Class Triangle. Main geometic object in this project. It has method intersect(Triangle),
that checks whether 2 triangles intersect.

Class Triangle2. Same to class Triangle, but in the 2d space.

*/

#include <array>
#include <cassert>
#include "Geometry.hpp"
#include "Segment.hpp"
#include "Vector.hpp"

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
        static void renumber(std::pair<T, Vector<T>>(& distVert)[3]) {
            std::array<char, 3U> signs{};
            for (size_t i = 0; i < signs.size(); ++i) {
                signs[i] = sign(distVert[i].first);
            }

            if (signs[0] + signs[1] + signs[2] == 0) {
                if (signs[1] != 0) {
                    std::swap(distVert[0], distVert[1]);
                }
                return;
            }

            size_t i = 0;
            while (signs[0] != signs[1] && i < 3U) {
                std::swap(signs[i % 3], signs[(i + 1) % 3]);
                std::swap(distVert[i % 3], distVert[(i + 1) % 3]);
                i++;
            }
        }

        bool intersects2d(const Triangle<T>& t, const Plane<T>& p1) const {
            auto axis = p1.norm_.greatestComponent();
            Triangle2<T> flatT1 = projectOnAxisPlane(axis);
            Triangle2<T> flatT2 = t.projectOnAxisPlane(axis);
            return flatT1.intersects(flatT2);
        }

        static Segment1<T> toSegment1(const std::pair<T, Vector<T>>(& distVert)[3], const Line<T>& line) {
            Vector<T> p = line.point_;
            Vector<T> d = line.dir_;

            T v1_ = dot(d, distVert[0].second - p);
            T v2_ = dot(d, distVert[1].second - p);
            T v3_ = dot(d, distVert[2].second - p);
            T t1 = v1_ + (v3_ - v1_) * distVert[0].first / (distVert[0].first - distVert[2].first);
            T t2 = v2_ + (v3_ - v2_) * distVert[1].first / (distVert[1].first - distVert[2].first);
            return Segment1<T>{t1, t2};
        }

        bool intersects3d(const Triangle<T>& t, const Line<T>& line, const Plane<T>& p1, const Plane<T>& p2) const {
            std::pair<T, Vector<T>> distVert1[3] = {
                std::make_pair(p2.signedDistance(v1_), v1_),
                std::make_pair(p2.signedDistance(v3_), v3_),
                std::make_pair(p2.signedDistance(v2_), v2_),
            };
            if (sameSign(distVert1[0].first, distVert1[1].first, distVert1[2].first)) {
                return false;
            }
            renumber(distVert1);

            std::pair<T, Vector<T>> distVert2[3] = {
                std::make_pair(p1.signedDistance(t.v1_), t.v1_),
                std::make_pair(p1.signedDistance(t.v3_), t.v3_),
                std::make_pair(p1.signedDistance(t.v2_), t.v2_),
            };
            if (sameSign(distVert2[0].first, distVert2[1].first, distVert2[2].first)) {
                return false;
            }
            renumber(distVert2);

            Segment1<T> seg1 = Triangle<T>::toSegment1(distVert1, line);
            Segment1<T> seg2 = Triangle<T>::toSegment1(distVert2, line);
            return seg1.intersects(seg2);
        }

        bool trianglesIntersects(const Triangle<T>& t) const {
            Plane<T> p1{v1_, v2_, v3_};
            Plane<T> p2{t.v1_, t.v2_, t.v3_};
            auto [type, line] = p1.intersects(p2);
            if (type == IntersectionType::Belongs) {
                return intersects2d(t, p1);
            } else if (type == IntersectionType::Intersects) {
                return intersects3d(t, line, p1, p2);
            } else {
                return false;
            }
        }

        std::array<Vector<T>, 3U> sortedVertices() const {
            std::array<Vector<T>, 3U> vert{v1_, v2_, v3_};
            auto sortX = [](const Vector<T>& a, const Vector<T>& b){ return greaterEq(a.x_, b.x_); };
            auto sortY = [](const Vector<T>& a, const Vector<T>& b){ return greaterEq(a.y_, b.y_); };
            auto sortZ = [](const Vector<T>& a, const Vector<T>& b){ return greaterEq(a.z_, b.z_); };
            std::sort(vert.begin(), vert.end(), sortX);
            std::sort(vert.begin(), vert.end(), sortY);
            std::sort(vert.begin(), vert.end(), sortZ);
            return vert;
        }

    public:
        Vector<T> v1_;
        Vector<T> v2_;
        Vector<T> v3_;

        Triangle():
            v1_(), v2_(), v3_() {}

        Triangle(const Vector<T>& v1, const Vector<T>& v2, const Vector<T>& v3):
            v1_(v1), v2_(v2), v3_(v3) {}

        bool valid() const {
            Vector s1 = v1_ - v3_;
            Vector s2 = v2_ - v3_;
            return !s1.collinear(s2); 
        }

        bool operator==(const Triangle<T>& t) const {
            auto vert1 = sortedVertices();
            auto vert2 = t.sortedVertices();
            return vert1[0] == vert2[0] && vert1[1] == vert2[1] && vert1[2] == vert2[2];
        }

        bool operator!=(const Triangle<T>& t) const {
            return !operator==(t);
        }

        std::pair<bool, Vector<T>> isPoint() const {
            if (v1_ == v2_ && v2_ == v3_) {
                return std::make_pair(true, v1_);
            } else {
                return std::make_pair(false, Vector<T>{});
            }
        }

        std::pair<bool, Segment<T>> isSegment() const {
            Line<T> line{v3_, v1_ - v3_};
            auto [flag, param] = line.contains(v2_);
            if(!flag) {
                return std::make_pair(false, Segment<T>{});
            } else if (lessEq(param, T{0})) {
                return std::make_pair(true, Segment{v2_, v1_});
            } else if (greaterEq(param, T{1})) {
                return std::make_pair(true, Segment{v3_, v2_});
            } else {
                return std::make_pair(true, Segment{v3_, v1_});
            }
        }

        Triangle2<T> projectOnAxisPlane(size_t axis) const {
            switch (axis)
            {
            case 0U:
                return Triangle2<T>{{v1_.y_, v1_.z_}, {v2_.y_, v2_.z_}, {v3_.y_, v3_.z_}};
            case 1U:
                return Triangle2<T>{{v1_.z_, v1_.x_}, {v2_.z_, v2_.x_}, {v3_.z_, v3_.x_}};
            case 2U:
                return Triangle2<T>{{v1_.x_, v1_.y_}, {v2_.x_, v2_.y_}, {v3_.x_, v3_.y_}};
            default:
                throw std::out_of_range("Incorrect axis index");
            }
        }

        bool contains(const Vector<T>& p) const {
            auto [pointFlag, point] = isPoint();
            if (pointFlag) {
                return point == p;
            }
            auto[segFlag, seg] = isSegment();
            if (segFlag) {
                return seg.contains(p);
            }

            Vector a = v1_ - p;
            Vector b = v2_ - p;
            Vector c = v3_ - p;
            Vector u = cross(b, c);
            Vector v = cross(c, a);
            Vector w = cross(a, b);
            return (less(dot(u, v), T{0}) || less(dot(u, w), T{0})) ? false : true;
        }

        bool intersects(const Segment<T>& seg) const {
            auto [pointFlag, point] = isPoint();
            if (pointFlag) {
                return seg.contains(point);
            }
            auto[segmentFlag, segment] = isSegment();
            if (segmentFlag) {
                return seg.intersects(segment);
            }

            Plane<T> plane{v1_, v2_, v3_};
            auto [intersectType, intersectPoint] = plane.intersects(seg);
            size_t axis = plane.norm_.greatestComponent();
            Triangle2<T> flatT = projectOnAxisPlane(axis);
            if (intersectType == IntersectionType::Intersects) {
                return flatT.contains(intersectPoint.projectOnAxis(axis));
            } else if (intersectType == IntersectionType::Belongs) {
                return flatT.intersects(seg.projectOnAxis(axis));
            } else {
                return false;
            }
        }

        bool intersects(const Triangle<T>& t) const {
            auto [pointFlag, point] = isPoint();
            if (pointFlag) {
                return t.contains(point);
            }
            auto [segFlag, seg] = isSegment();
            if (segFlag) {
                return t.intersects(seg);
            }
            return trianglesIntersects(t);
        }

        Vector2<T>& operator[](size_t index) {
            switch(index)
            {
            case 0U:
                return v1_;
            case 1U:
                return v2_;
            case 2U:
                return v3_;
            default:
                throw std::out_of_range("Incorrect index");
            }
        }

        const Vector2<T>& operator[](size_t index) const {
            switch(index)
            {
            case 0U:
                return v1_;
            case 1U:
                return v2_;
            case 2U:
                return v3_;
            default:
                throw std::out_of_range("Incorrect index");
            }
        }

        void dump() const {
            std::cout << "Vert1 {" << v1_.x_ << ", " << v1_.y_ << ", " << v1_.z_ << "}\n";
            std::cout << "Vert2 {" << v2_.x_ << ", " << v2_.y_ << ", " << v2_.z_ << "}\n";
            std::cout << "Vert3 {" << v3_.x_ << ", " << v3_.y_ << ", " << v3_.z_ << "}";
            std::cout << std::endl;
        }
};

template<typename T>
class Triangle2
{
    private:
        Vector2<T> v1_;
        Vector2<T> v2_;
        Vector2<T> v3_;

        std::array<Vector2<T>, 3U> sortedVertices() const {
            std::array<Vector2<T>, 3U> vert{v1_, v2_, v3_};
            auto sortX = [](const Vector2<T>& a, const Vector2<T>& b){ return greaterEq(a.x_, b.x_); };
            auto sortY = [](const Vector2<T>& a, const Vector2<T>& b){ return greaterEq(a.y_, b.y_); };
            std::sort(vert.begin(), vert.end(), sortX);
            std::sort(vert.begin(), vert.end(), sortY);
            return vert;
        }

    public:
        Triangle2():
            v1_(), v2_(), v3_() {}

        Triangle2(const Vector2<T>& v1, const Vector2<T>& v2, const Vector2<T>& v3):
            v1_(v1), v2_(v2), v3_(v3) {}

        bool operator==(const Triangle2<T>& t) const {
            auto vert1 = sortedVertices();
            auto vert2 = t.sortedVertices();
            return vert1[0] == vert2[0] && vert1[1] == vert2[1] && vert1[2] == vert2[2];
        }

        bool operator!=(const Triangle2<T>& t) const {
            return !operator==(t);
        }

        bool valid() const {
            Vector2 s1 = v1_ - v3_;
            Vector2 s2 = v2_ - v3_;
            return !s1.collinear(s2); 
        }

        bool intersects(const Triangle2<T>& t) const {
            Segment2<T> segs1[3]{{v2_, v3_}, {v3_, v1_}, {v1_, v2_}};
            Segment2<T> segs2[3]{{t.v2_, t.v3_}, {t.v3_, t.v1_}, {t.v1_, t.v2_}};
            std::array<size_t, 3U> axis{0U, 1U, 2U};
            return std::any_of(axis.cbegin(), axis.cend(), [&](size_t i) {
                return std::any_of(axis.cbegin(), axis.cend(), [&](size_t j) { return segs1[i].intersects(segs2[j]); });
            });
        }

        bool intersects(const Segment2<T>& seg) const {
            Segment2<T> segs[3]{{v2_, v3_}, {v3_, v1_}, {v1_, v2_}};
            std::array<size_t, 3U> axis{0U, 1U, 2U};
            auto intersects = [&](size_t i) { return segs[i].intersects(seg); };
            return std::any_of(axis.cbegin(), axis.cend(), intersects);
        }

        bool contains(const Vector2<T>& p) const {
            Triangle2<T> triag{v1_, v2_, v3_};
            Orientation orient = orientation(v1_, v2_, v3_);
            if (orient == Orientation::Collinear) {
                return Segment2<T>{v1_, v2_}.contains(p) || Segment2<T>{v2_, v3_}.contains(p);
            } else if (orient == Orientation::Clockwise) {
                std::swap(triag[0], triag[1]);
            }

            T dist1 = Segment2<T>{v2_, v3_}.signedDistance(p);
            if(less(dist1, T{0})) {
                return false;
            }
            T dist2 = Segment2<T>{v1_, v3_}.signedDistance(p);
            if(less(dist2, T{0})) {
                return false;
            }
            T dist3 = Segment2<T>{v1_, v2_}.signedDistance(p);
            if(less(dist3, T{0})) {
                return false;
            }
            return true;
        }

        Vector2<T>& operator[](size_t index) {
            switch(index)
            {
            case 0U:
                return v1_;
            case 1U:
                return v2_;
            case 2U:
                return v3_;
            default:
                throw std::out_of_range("Incorrect index");
            }
        }

        const Vector2<T>& operator[](size_t index) const {
            switch(index)
            {
            case 0U:
                return v1_;
            case 1U:
                return v2_;
            case 2U:
                return v3_;
            default:
                throw std::out_of_range("Incorrect index");
            }
        }

        void dump() const {
            std::cout << "Vert1 {" << v1_.x_ << ", " << v1_.y_ << "}\n";
            std::cout << "Vert2 {" << v2_.x_ << ", " << v2_.y_ << "}\n";
            std::cout << "Vert3 {" << v3_.x_ << ", " << v3_.y_ << "}";
            std::cout << std::endl;
        }
};

} //namespace triangles
