#include "../include/LFUCache.hpp"
#include <gtest/gtest.h>

TEST(LFUCacheTest, test0) 
{
	std::vector<int> test0{1, 3, 2, 4, 1, 2, 3, 2, 4};
	cache::LFUCache<int> cache{3};

	EXPECT_EQ(cache.countCacheHits(test0), 2);
}

TEST(LFUCacheTest, test1) 
{
	std::vector<int> test1{1, 1, 1, 1, 1, 1, 1, 1, 1};
	cache::LFUCache<int> cache{2};

	EXPECT_EQ(cache.countCacheHits(test1), 8);
}

int main()
{
	::testing::InitGoogleTest();
	return RUN_ALL_TESTS();
}
