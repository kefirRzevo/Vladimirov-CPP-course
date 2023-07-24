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

TEST(LFUCacheTest, test2) 
{
	std::vector<int> test2{1, 2, 3, 4, 5, 6, 1, 2, 7, 8};
	cache::LFUCache<int> cache{5};

	EXPECT_EQ(cache.countCacheHits(test2), 0);
}

TEST(LFUCacheTest, test3) 
{
	std::vector<int> test3{2, 4, 2, 4, 6, 4, 6, 7, 6, 6, 7, 9, 6, 4, 3, 5, 7, 8, 6, 5, 4, 3, 4, 5, 7, 8, 8, 7, 6};
	cache::LFUCache<int> cache{5};

	EXPECT_EQ(cache.countCacheHits(test3), 17);
}

TEST(LFUCacheTest, test4) 
{
	std::vector<int> test4{1, 2, 3, 4, 5, 1, 2, 3, 4, 5};
	cache::LFUCache<int> cache{5};

	EXPECT_EQ(cache.countCacheHits(test4), 5);
}

TEST(LFUCacheTest, test5) 
{
	std::vector<int> test5{2, 2, 1, 3, 3, 4, 1, 5, 6, 2};
	cache::LFUCache<int> cache{3};

	EXPECT_EQ(cache.countCacheHits(test5), 3);
}

int main()
{
	::testing::InitGoogleTest();
	return RUN_ALL_TESTS();
}
