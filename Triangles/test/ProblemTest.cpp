#include <gtest/gtest.h>
#include <unordered_set>
#include <vector>
#include <limits>

#include "../include/Geometry.hpp"
#include "../include/OctroTree.hpp"
#include "../include/BoundingBox.hpp"

using namespace triangles;
using namespace compare;

TEST(BoundingBoxTest, test0) 
{
    Triangle<float> t1{{0, 0, 0}, {0, 1, 1}, {1, 0, 1}};
    BoundingBox<float> b1{t1};
    Triangle<float> t2{{0, 1, 0}, {1, 0, 0}, {0, 0, 0}};
    BoundingBox<float> b2{t2};
    EXPECT_TRUE(b1.intersects(b2));
    EXPECT_TRUE(t1.intersects(t2));

    Vector<float> v1{0, 0, 0};
    BoundingBox<float> b3{{-1, -1, -1}, {1, 1, 1}};
    auto [flag1, res1] = b3.intersects(v1);
    EXPECT_TRUE(flag1 == true && res1 == Quadrant::count);

    BoundingBox<float> b4{{0, 0, 1}, {1, 1, 1}};
    auto [flag2, res2] = b4.intersects(v1);
    EXPECT_TRUE(flag2 == true && res2 == Quadrant::count);

    BoundingBox<float> b5{{-2, -1, 1}, {-1, -0.5, 2}};
    auto [flag3, res3] = b5.intersects(v1);
    EXPECT_TRUE(flag3 == false && res3 == Quadrant::_x_yz);

    BoundingBox<float> b6{{3, 1, 1}, {3, 2, 4}};
    auto [flag4, res4] = b6.intersects(v1);
    EXPECT_TRUE(flag4 == false && res4 == Quadrant::xyz);

    BoundingBox<float> b7{{0, 0, 0}, {1, 1, 1}};
    Vector<float> v2{0, 1, 0};
    Vector<float> v3{0.5, 0.5, 0.2};
    Vector<float> v4{1.1, 1.1, 1.2};
    EXPECT_TRUE(b7.contains(v2));
    EXPECT_TRUE(b7.contains(v3));
    EXPECT_FALSE(b7.contains(v4));
}

template<typename T>
bool slowIntersects(const Triangle<T>& t1, const Triangle<T>& t2)
{
    Segment<T> seg11{t1.v2_, t1.v3_};
    Segment<T> seg12{t1.v1_, t1.v3_};
    Segment<T> seg13{t1.v1_, t1.v2_};

    Segment<T> seg21{t2.v2_, t2.v3_};
    Segment<T> seg22{t2.v1_, t2.v3_};
    Segment<T> seg23{t2.v1_, t2.v2_};

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

template<typename T>
void getSlowIntersections(const std::vector<Triangle<T>>& vec, std::unordered_set<size_t>& result)
{
    for(size_t i = 0; i < vec.size(); ++i)
    {
        for(size_t j = 0; j < vec.size(); ++j)
        {
            if(i != j && vec[i].intersects(vec[j]))
            {
                result.insert(i);
                result.insert(j);
            }
        }
    }
}

TEST(TriangleIntersectionTest, test2)
{
    size_t size = 500;
    std::vector<Triangle<float>> triangles{};

    for(size_t i = 0; i < size; ++i)
    {
        float minVal = static_cast<float>(std::rand()) / RAND_MAX * 100.f;
        float maxVal = minVal + static_cast<float>(std::rand()) / RAND_MAX * 50.f;
        Vector<float> v1{
            static_cast<float>(std::rand()) / RAND_MAX * (maxVal - minVal) + minVal,
            static_cast<float>(std::rand()) / RAND_MAX * (maxVal - minVal) + minVal,
            static_cast<float>(std::rand()) / RAND_MAX * (maxVal - minVal) + minVal
        };
        Vector<float> v2{
            static_cast<float>(std::rand()) / RAND_MAX * (maxVal - minVal) + minVal,
            static_cast<float>(std::rand()) / RAND_MAX * (maxVal - minVal) + minVal,
            static_cast<float>(std::rand()) / RAND_MAX * (maxVal - minVal) + minVal
        };
        Vector<float> v3{
            static_cast<float>(std::rand()) / RAND_MAX * (maxVal - minVal) + minVal,
            static_cast<float>(std::rand()) / RAND_MAX * (maxVal - minVal) + minVal,
            static_cast<float>(std::rand()) / RAND_MAX * (maxVal - minVal) + minVal
        };
        triangles.push_back(Triangle{v1, v2, v3});
    }

    //for(size_t i = 0; i < size; ++i)
    //{
        //std::cout << "треугольник(" << triangles[i].v1_.x_ << "|" << triangles[i].v1_.y_ << "|" << triangles[i].v1_.z_ << " ";
        //std::cout << triangles[i].v2_.x_ << "|" << triangles[i].v2_.y_ << "|" << triangles[i].v2_.z_ << " ";
        //std::cout << triangles[i].v3_.x_ << "|" << triangles[i].v3_.y_ << "|" << triangles[i].v3_.z_ << ")\n";
    //}

    std::unordered_set<size_t> res1{};
    getSlowIntersections(triangles, res1);

    OctoTree<float> octo{};
    std::fstream logfile1{"../build/mylog.dot", std::ios::out};
    std::unordered_set<size_t> res2{};
    for(size_t i = 0; i < size; ++i)
    {
        octo.insert(NodeValue{triangles[i], BoundingBox{triangles[i]}, i});
    }
    octo.dump(logfile1);
    octo.getIntersections(res2);

    for(const auto& val1: res1)
    {
        EXPECT_TRUE(res2.find(val1) != res2.end());
    }
}

int main()
{
	::testing::InitGoogleTest();
	return RUN_ALL_TESTS();
}
