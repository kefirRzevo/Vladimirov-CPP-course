#include <gtest/gtest.h>

#include "../include/Geometry.hpp"

using namespace triangles;

TEST(VectorTest, test0) 
{
	Vector<int> v1{1, 2, 3};
	Vector<int> v2{1, 2, 3};
	Vector<double> v3{-1.00000001, 2, -1};
	Vector<double> v4{-0.999999, 2, -1.0000001};
	Vector<double> v5{0.00001, 0.000009, 0};
	
	EXPECT_EQ(v1, v2);
	EXPECT_EQ(v3, v4);
	EXPECT_EQ(dot(v1, v2), 14);
	EXPECT_TRUE(v5.equalsZero());
}

int main()
{
	::testing::InitGoogleTest();
	return RUN_ALL_TESTS();
}
