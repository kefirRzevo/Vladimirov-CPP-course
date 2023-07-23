#include "include/LFUCache.hpp"
#include "include/IdealCache.hpp"

using namespace cache;

int getPageData(int key)
{
    return key;
}

int main(int argc, char* argv[])
{
    /*LFUCache<int, int> cache{4};
    cache.cache(5, getPageData);
    cache.cache(5, getPageData);
    cache.cache(5, getPageData);
    cache.cache(5, getPageData);
    cache.cache(3, getPageData);
    cache.cache(3, getPageData);
    cache.cache(2, getPageData);
    cache.cache(1, getPageData);
    cache.cache(4, getPageData);
    cache.cache(6, getPageData);
    cache.cache(5, getPageData);
    cache.cache(5, getPageData);
    cache.cache(5, getPageData);
    cache.cache(5, getPageData);*/
    std::vector<int> data{1, 3, 2, 4, 1, 2, 3, 2, 4};
    IdealCache<int> cache{3, data};

    return 0;
}
