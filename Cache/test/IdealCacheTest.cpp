#include <gtest/gtest.h>

#include "../include/IdealCache.hpp"

TEST(IdealCacheTest, test0) 
{
	std::vector<int> test0{1, 3, 2, 4, 1, 2, 3, 2, 4};
	cache::IdealCache<int, int> cache{3};

	EXPECT_EQ(cache.countCacheHits(test0, cache::IdealCache<int, int>::getData), 4);
}

TEST(IdealCacheTest, test1) 
{
	std::vector<int> test1{1, 1, 1, 1, 1, 1, 1, 1, 1};
	cache::IdealCache<int, int> cache{2};

	EXPECT_EQ(cache.countCacheHits(test1, cache::IdealCache<int, int>::getData), 8);
}

TEST(IdealCacheTest, test2) 
{
	std::vector<int> test2{1, 2, 3, 4, 5, 6, 1, 2, 7, 8};
	cache::IdealCache<int, int> cache{5};

	EXPECT_EQ(cache.countCacheHits(test2, cache::IdealCache<int, int>::getData), 2);
}

TEST(IdealCacheTest, test3) 
{
	std::vector<int> test3{2, 4, 2, 4, 6, 4, 6, 7, 6, 6, 7, 9, 6, 4, 3, 5, 7, 8, 6, 5, 4, 3, 4, 5, 7, 8, 8, 7, 6};
	cache::IdealCache<int, int> cache{5};

	EXPECT_EQ(cache.countCacheHits(test3, cache::IdealCache<int, int>::getData), 20);
}

TEST(IdealCacheTest, test4) 
{
	std::vector<int> test4{1, 2, 3, 4, 5, 1, 2, 3, 4, 5};
	cache::IdealCache<int, int> cache{5};

	EXPECT_EQ(cache.countCacheHits(test4, cache::IdealCache<int, int>::getData), 5);
}

TEST(IdealCacheTest, test5) 
{
	std::vector<int> test5{2, 2, 1, 3, 3, 4, 1, 5, 6, 2};
	cache::IdealCache<int, int> cache{3};

	EXPECT_EQ(cache.countCacheHits(test5, cache::IdealCache<int, int>::getData), 4);
}

int main()
{
	::testing::InitGoogleTest();
	return RUN_ALL_TESTS();
}
