#include <gtest/gtest.h>
#include <unordered_set>
#include <vector>
#include <limits>

#include "../include/Geometry.hpp"
#include "../include/OctroTree.hpp"
#include "../include/BoundingBox.hpp"

using namespace triangles;

template<typename T>
void getIntersectionsSlow(const std::vector<Triangle<T>>& vec, std::unordered_set<size_t>& result) {
    for (size_t i = 0; i < vec.size(); ++i) {
        for (size_t j = 0; j < vec.size(); ++j) {
            if (i != j && vec[i].intersects(vec[j])) {
                result.insert(i);
                result.insert(j);
            }
        }
    }
}

TEST(TriangleIntersectionTest, test0)
{
    size_t size = 500;
    std::vector<Triangle<float>> triangles{};

    for (size_t i = 0; i < size; ++i) {
        float minVal = static_cast<float>(std::rand()) / RAND_MAX * 100.f;
        float maxVal = minVal + static_cast<float>(std::rand()) / RAND_MAX * 10.f;
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

    OctoTree<float> octo{};
    std::fstream logfile1{"../mylog.dot", std::ios::out};
    assert(!logfile1.bad());
    for (size_t i = 0; i < size; ++i) {
        octo.insert(NodeValue{triangles[i], BoundingBox{triangles[i]}, i});
    }
    octo.dump(logfile1);

    //for visualization in https://www.matheretter.de/geoservant/ru
    //for (size_t i = 0; i < size; ++i)
    //{
        //std::cout << "треугольник(" << triangles[i].v1_.x_ << "|" << triangles[i].v1_.y_ << "|" << triangles[i].v1_.z_ << " ";
        //std::cout << triangles[i].v2_.x_ << "|" << triangles[i].v2_.y_ << "|" << triangles[i].v2_.z_ << " ";
        //std::cout << triangles[i].v3_.x_ << "|" << triangles[i].v3_.y_ << "|" << triangles[i].v3_.z_ << ")\n";
    //}

    std::unordered_set<size_t> res1{};
    getIntersectionsSlow(triangles, res1);
    std::unordered_set<size_t> res2{};
    octo.getIntersections(res2);
    bool sameResults = true;
    for (const auto& val1: res1) {
        if (res2.find(val1) == res2.end()) {
            sameResults = false;
        }
    }
    EXPECT_TRUE(res1.size() == res2.size());
    EXPECT_TRUE(sameResults);
}

int main()
{
	::testing::InitGoogleTest();
	return RUN_ALL_TESTS();
}
