#include <gtest/gtest.h>

#include "../include/Geometry.hpp"

using namespace triangles;

TEST(VectorTest, test0) 
{
	Vector<int> v1{1, 2, 3};
	Vector<int> v2{2, 3, 4};
	Vector<int> v3{-1, 2, -1};
	
	EXPECT_EQ(dot(v1, v2), 20);
	EXPECT_EQ(cross(v1, v2), v3);
}

int main()
{
	::testing::InitGoogleTest();
	return RUN_ALL_TESTS();
}
