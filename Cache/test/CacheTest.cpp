#include <cstdlib>
#include <iostream>
#include <gtest/gtest.h>

class CRectTest : public ::testing::Test{
};

TEST_F(CRectTest, CheckPerimeter) 
{
	std::pair<int, int> rect;
	rect.first = 5;
	rect.second = 6;

	ASSERT_TRUE(rect.first+rect.second == 22/2);
}

int main(int argc, char **argv)
{
	::testing::InitGoogleTest(&argc, argv);
    std::cout << "Tests started" << std::endl;
	return RUN_ALL_TESTS();
}
