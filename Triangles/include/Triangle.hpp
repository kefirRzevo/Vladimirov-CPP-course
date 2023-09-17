#pragma once

#include <cassert>

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
class Triangle {

    private:
        static void renumber(std::pair<T, Vector<T>>(& distVert)[3]) {
            char signs[3]{};
            for (size_t i = 0; i < 3U; i++) {
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

        bool validTrianglesIntersects(const Triangle<T>& t) const {
            Plane<T> p1{v1_, v2_, v3_};
            Plane<T> p2{t.v1_, t.v2_, t.v3_};
            auto [type, line] = p1.intersects(p2);
            if (type == IntersectionType::Belongs) {
                auto axis = p1.norm_.greatestComponent();
                Triangle2<T> flatT1 = projectOnAxisPlane(axis);
                Triangle2<T> flatT2 = t.projectOnAxisPlane(axis);
                return flatT1.intersects(flatT2);
            } else if (type == IntersectionType::Intersects) {
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
            } else {
                return false;
            }
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
            return v1_ == t.v1_ && v2_ == t.v2_ && v3_ == t.v3_;
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

            return validTrianglesIntersects(t);
        }

        void dump() const {
            std::cout << "Vert1 {" << v1_.x_ << ", " << v1_.y_ << ", " << v1_.z_ << "}\n";
            std::cout << "Vert2 {" << v2_.x_ << ", " << v2_.y_ << ", " << v2_.z_ << "}\n";
            std::cout << "Vert3 {" << v3_.x_ << ", " << v3_.y_ << ", " << v3_.z_ << "}\n";
            std::cout.flush();
        }
};

template<typename T>
class Triangle2 {
    
    public:
        Vector2<T> v1_;
        Vector2<T> v2_;
        Vector2<T> v3_;

        Triangle2():
            v1_(), v2_(), v3_() {}

        Triangle2(const Vector2<T>& v1, const Vector2<T>& v2, const Vector2<T>& v3):
            v1_(v1), v2_(v2), v3_(v3) {}

        bool equals(const Triangle2<T>& t) const {
            return  (v1_ == t.v1_ && v2_ == t.v2_ && v3_ == t.v3_) ||
                    (v1_ == t.v1_ && v2_ == t.v3_ && v3_ == t.v2_) ||
                    (v1_ == t.v2_ && v2_ == t.v3_ && v3_ == t.v1_) ||
                    (v1_ == t.v2_ && v2_ == t.v1_ && v3_ == t.v3_) ||
                    (v1_ == t.v3_ && v2_ == t.v1_ && v3_ == t.v2_) ||
                    (v1_ == t.v3_ && v2_ == t.v2_ && v3_ == t.v1_);
        }

        bool valid() const {
            Vector2 s1 = v1_ - v3_;
            Vector2 s2 = v2_ - v3_;
            return !s1.collinear(s2); 
        }

        bool intersects(const Triangle2<T>& t) const {
            Segment2<T> segs1[3]{{v2_, v3_}, {v3_, v1_}, {v1_, v2_}};
            Segment2<T> segs2[3]{{t.v2_, t.v3_}, {t.v3_, t.v1_}, {t.v1_, t.v2_}};
            for (size_t i = 0; i < 3U; ++i) {
                for (size_t j = 0; j < 3U; ++j) {
                    if (segs1[i].intersects(segs2[j])) {
                        return true;
                    }
                }
            }
            return false;
        }

        bool intersects(const Segment2<T>& seg) const {
            Segment2<T> segs[3]{{v2_, v3_}, {v3_, v1_}, {v1_, v2_}};
            for (size_t i = 0; i < 3U; ++i) {
                if (segs[i].intersects(seg)) {
                    return true;
                }
            }
            return false;
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
            std::cout << "Vert3 {" << v3_.x_ << ", " << v3_.y_ << "}\n";
            std::cout.flush();
        }
};

} //namespace triangles
