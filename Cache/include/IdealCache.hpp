#pragma once

#include <unordered_map>
#include <iostream>
#include <unistd.h>
#include <vector>
#include <queue>
#include <list>
#include <map>

namespace cache
{

template<typename KeyT>
using UniquePagesIt = typename std::unordered_map<KeyT, std::deque<size_t>>::iterator;

template<typename KeyT>
struct Comparator
{
    bool operator()(const UniquePagesIt<KeyT>& lhs, const UniquePagesIt<KeyT>& rhs) const
    {
        if(lhs->first == rhs->first)
        {
            return false;
        }
        if (lhs->second.size() == 0)
        {
            return true;
        }
        if(rhs->second.size() == 0)
        {
            return false;
        }
        return lhs->second[0] > rhs->second[0];
    }
};

template<typename KeyT, typename D>
class IdealCache
{
    private:

        size_t size_;

        std::unordered_map<KeyT, std::deque<size_t>> uniquePages_;
        std::map<UniquePagesIt<KeyT>, D, Comparator<KeyT>> cache_;

    public:

        IdealCache(size_t cacheSize):
            size_(cacheSize)
            {}

        size_t countCacheHits(const std::vector<KeyT>& keys, D getPage(KeyT))
        {
            firstPass(keys);
            return secondPass(keys, getPage);
        }

        static int getData(int key)
        {
            usleep(5000);
            return key;
        }

    private:

        void firstPass(const std::vector<KeyT>& keys)
        {
            for(size_t i = 0; i < keys.size(); i++)
            {
                auto hit = uniquePages_.find(keys[i]);
                if(hit == uniquePages_.cend())
                {
                    uniquePages_.emplace(keys[i], std::deque<size_t>{i});
                }
                else
                {
                    hit->second.push_back(i);
                }
            }
        }

        size_t secondPass(const std::vector<KeyT>& keys, D getPage(KeyT))
        {
            size_t cacheHits = 0;
            for(size_t i = 0; i < keys.size(); i++)
            {
                auto hit = uniquePages_.find(pages[i]);
                if(hit == uniquePages_.cend())
                {
                    std::cout << "Can't find " << keys[i] << std::endl;
                    return 0;
                }

                if(cache_.find(hit) == cache_.cend())
                {
                    if(cache_.size() == size_)
                    {
                        cache_.erase(cache_.begin());
                    }
                    if(hit->second.size() != 0)
                    {
                        hit->second.pop_front();
                    }
                    cache_.emplace(hit, getPage(hit->first));
                }
                else
                {
                    cache_.erase(hit);
                    if(hit->second.size())
                    {
                        hit->second.pop_front();
                    }
                    cache_.emplace(hit, getPage(hit->first));
                    cacheHits++;
                }
            }
            return cacheHits;
        }

        void dumpFirstPass() const
        {
            std::cout << "First Pass Dump\n";
            for(const auto& page: uniquePages_)
            {
                std::cout << "|key: " << page.first << "|";
                for(size_t i = 0; i < page.second.size(); i++)
                {
                    std::cout << page.second[i] << "; ";
                }
                std::cout << "|" << std::endl;
            }
        }

        void dumpSecondPass() const
        {
            std::cout << "Second Pass Dump\n";
            std::cout << "Cache size " << size_ << " and " << cache_.size() << "\n";
            for(const auto& cachedPage: cache_)
            {
                std::cout << "|key: " << cachedPage.first->first << "|";
                for(size_t i = 0; i < cachedPage.first->second.size(); i++)
                {
                    std::cout << cachedPage.first->second[i] << "; ";
                }
                std::cout << "|" << std::endl;
            }
        }
};

}
