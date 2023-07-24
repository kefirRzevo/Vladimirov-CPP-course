#include <string>

#include "include/LFUCache.hpp"
#include "include/IdealCache.hpp"

int main(int argc, char* argv[])
{
    std::vector<int> data;
    size_t cacheSize = 0;

    if(argc > 2)
    {
        size_t capacity = static_cast<size_t>(argc - 2);
        for(size_t i = 0; i < capacity; i++)
        {
            data.push_back(std::stoi(argv[2 + i]));
        }
        cacheSize = std::stoi(argv[1]);
    }
    else
    {
        data = {1, 2, 3, 4, 5, 6, 1, 2, 7, 8};
        cacheSize = 5;
    }

    cache::LFUCache<int> LFU{cacheSize};
    cache::IdealCache<int> ideal{cacheSize};

    std::cout << "LFU   cache hits " << LFU.countCacheHits(data) << std::endl;
    std::cout << "Ideal cache hits " << ideal.countCacheHits(data) << std::endl;

    return 0;
}
