#include <gtest/gtest.h>

#include "../include/Geometry.hpp"

TEST(TrianglesTest, test0) 
{
	EXPECT_EQ(4, 4);
}

int main()
{
	::testing::InitGoogleTest();
	return RUN_ALL_TESTS();
}
