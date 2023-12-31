#include <unistd.h>
#include <string>

#include "include/LFUCache.hpp"
#include "include/IdealCache.hpp"

int main(int argc, char* argv[])
{
    std::vector<int> data{};
    size_t cacheSize = 0;
    size_t capacity = 0;

    if(argc > 2)
    {
        cacheSize = std::stoi(argv[1]);
        capacity = std::stoi(argv[2]);
        for(size_t i = 0; i < capacity; i++)
        {
            data.push_back(std::stoi(argv[3 + i]));
        }
    }
    else
    {
        std::cin >> cacheSize;
        std::cin >> capacity;
        int temp = 0;
        for(size_t i = 0; i < capacity; i++)
        {
            std::cin >> temp;
            data.push_back(temp);
        }
    }

    cache::LFUCache<int, int> lfu{cacheSize};
    cache::IdealCache<int, int> ideal{cacheSize};

    std::cout << "LFU   cache hits " << 
        lfu.countCacheHits(data, cache::LFUCache<int, int>::getData) << std::endl;
    std::cout << "Ideal cache hits " <<
        ideal.countCacheHits(data, cache::IdealCache<int, int>::getData) << std::endl;

    return 0;
}
