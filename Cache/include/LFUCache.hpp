#pragma once

#include <unordered_map>
#include <iostream>
#include <unistd.h>
#include <vector>
#include <list>

namespace cache
{

template<typename KeyT = int, typename D = int>
class LFUCache
{
    private:

        struct FreqNode;
        struct PageNode;

        using FreqNodeIt = typename std::list<FreqNode>::iterator;
        using PageNodeIt = typename std::list<PageNode>::iterator;
        using HashTabIt  = typename std::unordered_map<KeyT, PageNodeIt>::iterator;

        struct FreqNode
        {
            std::list<PageNode> list_;
            size_t frequency_;

            FreqNode(size_t frequency):
                frequency_(frequency) {};
        };

        struct PageNode
        {
            FreqNodeIt iterator_;
            D pageData_;
            KeyT key_;

            PageNode(const FreqNodeIt& iterator, D pageData, KeyT key):
                iterator_(iterator), pageData_(pageData), key_(key) {};
        };

        size_t size_;
        std::list<FreqNode> cache_;
        std::unordered_map<KeyT, PageNodeIt> hashTab_;

    public:

        LFUCache(size_t size):
            size_(size) {}

       size_t countCacheHits(const std::vector<KeyT>& keys, D getPage(KeyT))
        {
            size_t cacheHits = 0;
            for(size_t i = 0; i < keys.size(); i++)
            {
                if(isCached(keys[i], getPage))
                {
                    cacheHits++;
                }
            }
            return cacheHits;
        }

        static int getData(int key)
        {
            return key;
        }

    private:

        void insert(const KeyT& key, D getPage(KeyT))
        {
            if(hashTab_.size() == size_)
            {
                FreqNode& smallestFreqNode = cache_.front();
                hashTab_.erase(smallestFreqNode.list_.begin()->key_);
                smallestFreqNode.list_.pop_front();
                if(smallestFreqNode.list_.size() == 0)
                {
                    cache_.pop_front();
                }
            }

            if(cache_.size() == 0 || cache_.front().frequency_ != 1)
            {
                cache_.emplace_front(1);
            }

            cache_.front().list_.emplace_back(cache_.begin(), getPage(key), key);
            hashTab_.emplace(key, std::prev(cache_.front().list_.end()));
        }

        void cacheUpdate(const KeyT& key, const HashTabIt& hit)
        {
                PageNodeIt pageNodeIt = hit->second;
                FreqNodeIt freqNodeIt = pageNodeIt->iterator_;
                FreqNodeIt nextFreqNodeIt = std::next(freqNodeIt);

                if(freqNodeIt == cache_.rbegin().base() ||
                nextFreqNodeIt->frequency_ != freqNodeIt->frequency_ + 1)
                {
                    cache_.emplace(nextFreqNodeIt, freqNodeIt->frequency_ + 1);
                }

                nextFreqNodeIt = std::next(freqNodeIt);
                nextFreqNodeIt->list_.emplace_back(nextFreqNodeIt, pageNodeIt->pageData_, pageNodeIt->key_);
                freqNodeIt->list_.erase(pageNodeIt);

                hashTab_.erase(key);
                hashTab_.emplace(key, std::prev(nextFreqNodeIt->list_.end()));

                if(freqNodeIt->list_.size() == 0)
                {
                    cache_.erase(freqNodeIt);
                }
        }

        bool isCached(const KeyT& key, D getPage(KeyT))
        {
            auto hit = hashTab_.find(key);
            if(hit == hashTab_.cend())
            {
                insert(key, getPage);
                return false;
            }
            else
            {
                cacheUpdate(key, hit);
                return true;
            }
        }

        void dump() const
        {
            std::cout << "Dump:\nList       ";
            for(const auto& freqNode: cache_)
            {
                std::cout << "|freq " << freqNode.frequency_ << ": nodes";
                for(const auto& pageNode: freqNode.list_)
                {
                    std::cout << " key "  << pageNode.pageData_ << ";";
                };
            }
            std::cout << "|\nHash size " << hashTab_.size();
            for(const auto& node: hashTab_)
            {
                std::cout << "|key: " << node.first << "; freq " << node.second->iterator_->frequency_;
            }
            std::cout << "|" << std::endl;
        }
};

}
