#pragma once

#include <unordered_map>
#include <iostream>
#include <list>

namespace cache
{

template<typename T, typename KeyT = int >
class LFUCache
{
    private:

        struct FreqNode;
        struct PageNode;

        using FreqNodeIt = typename std::list<FreqNode>::iterator;
        using PageNodeIt = typename std::list<PageNode>::iterator;

        typedef struct FreqNode
        {
            std::list<PageNode> list_;
            size_t frequency_;

            FreqNode(size_t frequency):
                frequency_(frequency) {};
        } FreqNode;

        typedef struct PageNode
        {
            FreqNodeIt iterator_;
            T pageData_;
            KeyT key_;

            PageNode(const FreqNodeIt& iterator, T pageData, KeyT key):
                iterator_(iterator), pageData_(pageData), key_(key) {};
        } PageNode;

        size_t size_;
        std::list<FreqNode> list_;
        std::unordered_map<KeyT, PageNodeIt> hashTab_;

    public:

        LFUCache(size_t size):
            size_(size) {}

        bool cache(const KeyT& key, T getPage(KeyT))
        {
            auto hit = hashTab_.find(key);
            if(hit == hashTab_.cend())
            {
                if(hashTab_.size() == size_)
                {
                    FreqNode& smallestFreqNode = list_.front();
                    hashTab_.erase(smallestFreqNode.list_.begin()->key_);
                    smallestFreqNode.list_.pop_front();
                    if(smallestFreqNode.list_.size() == 0)
                    {
                        list_.pop_front();
                    }
                }

                if(list_.size() == 0 || list_.front().frequency_ != 1)
                {
                    list_.emplace_front(1);
                }

                list_.front().list_.emplace_back(list_.begin(), getPage(key), key);
                hashTab_.emplace(key, std::prev(list_.front().list_.end()));
                std::cout << key << " miss" << std::endl;
                return false;
            }
            else
            {
                PageNodeIt pageNodeIt = hit->second;
                FreqNodeIt freqNodeIt = pageNodeIt->iterator_;

                if(freqNodeIt == list_.rbegin().base() ||
                std::next(freqNodeIt)->frequency_ != freqNodeIt->frequency_ + 1)
                {
                    list_.emplace(std::next(freqNodeIt), freqNodeIt->frequency_ + 1);
                }

                std::next(freqNodeIt)->list_.emplace_back(std::next(freqNodeIt), pageNodeIt->pageData_, pageNodeIt->key_);
                freqNodeIt->list_.erase(pageNodeIt);

                hashTab_.erase(key);
                hashTab_.emplace(key, std::prev(std::next(freqNodeIt)->list_.end()));

                if(freqNodeIt->list_.size() == 0)
                {
                    list_.erase(freqNodeIt);
                }
                std::cout << key << " found" << std::endl;
                return true;
            }
        }

        void dump() const
        {
            std::cout << "Dump:\nList       ";
            for(const auto& freqNode: list_)
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
