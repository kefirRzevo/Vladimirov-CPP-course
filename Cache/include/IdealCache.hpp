#pragma once

#include <unordered_map>
#include <iostream>
#include <vector>
#include <queue>
#include <list>
#include <set>

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

template<typename KeyT>
class IdealCache
{
    private:

        size_t size_;

        std::unordered_map<KeyT, std::deque<size_t>> uniquePages_;
        std::set<UniquePagesIt<KeyT>, Comparator<KeyT>> cache_;

    public:

        IdealCache(size_t cacheSize):
            size_(cacheSize)
            {}

        size_t countCacheHits(const std::vector<KeyT>& pages)
        {
            firstPass(pages);
            return secondPass(pages);
        }

    private:

        void firstPass(const std::vector<KeyT>& pages)
        {
            for(size_t i = 0; i < pages.size(); i++)
            {
                auto hit = uniquePages_.find(pages[i]);
                if(hit == uniquePages_.cend())
                {
                    uniquePages_.emplace(pages[i], std::deque<size_t>{i});
                }
                else
                {
                    hit->second.push_back(i);
                }
            }
        }

        size_t secondPass(const std::vector<KeyT>& pages)
        {
            size_t cacheHits = 0;
            for(size_t i = 0; i < pages.size(); i++)
            {
                auto hit = uniquePages_.find(pages[i]);
                if(hit == uniquePages_.cend())
                {
                    std::cout << "Can't find " << pages[i] << std::endl;
                    return 0;
                }

                if(cache_.find(hit) == cache_.cend())
                {
                    if(cache_.size() == size_)
                    {
                        cache_.erase(cache_.begin());
                    }
                    if(hit->second.size())
                    {
                        hit->second.pop_front();
                    }
                    cache_.insert(hit);
                }
                else
                {
                    cache_.erase(hit);
                    if(hit->second.size())
                    {
                        hit->second.pop_front();
                    }
                    cache_.insert(hit);
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
                std::cout << "|key: " << cachedPage->first << "|";
                for(size_t i = 0; i < cachedPage->second.size(); i++)
                {
                    std::cout << cachedPage->second[i] << "; ";
                }
                std::cout << "|" << std::endl;
            }
        }
};

}
