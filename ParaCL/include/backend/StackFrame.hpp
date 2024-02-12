#pragma once

#include <unordered_map>

#include "../Utils.hpp"
#include "Image.hpp"

namespace paracl
{

struct StringHash;
struct StringEqual;

class MemBlock
{
private:
    using addr_t = size_t;
    using MemMap = std::unordered_map<std::string_view, addr_t, StringHash, StringEqual>;

    addr_t begAddr_;
    addr_t curAddr_;
    MemMap map_;
    Image& im_;

public:
    MemBlock(addr_t addrBeg, Image& im) :
        begAddr_(addrBeg), curAddr_(addrBeg), im_(im) {}

    std::optional<addr_t> pushVar(std::string_view name) {
        [[maybe_unused]] auto [it, emplaced] = map_.emplace(name, curAddr_);
        if (!emplaced) {
            return std::nullopt;
        }
        return push(sizeof(int));
    }

    std::optional<addr_t> push(size_t size) {
        auto oldAddr = curAddr_;
        curAddr_ += size;
        im_.addInstr<Alloca>(size);
        return oldAddr;
    }

    std::optional<addr_t> lookupVar(std::string_view name) const {
        auto found = map_.find(name);
        if (found != map_.end()) {
            return found->second;
        }
        return std::nullopt;
    }

    addr_t getCurAddr() const {
        return curAddr_;
    }

    size_t size() const {
        return curAddr_ - begAddr_;
    }
};

class StackFrame final
{
private:
    using addr_t = size_t;

    std::vector<MemBlock> blocks_;
    Image& im_;

public:
    StackFrame(Image& im) :
        im_(im) {}

    void beginScope() {
        blocks_.emplace_back(getCurAddr(), im_);
    }

    void endScope() {
        blocks_.pop_back();
    }

    std::optional<addr_t> pushVar(std::string_view name) {
        if (!blocks_.empty()) {
            auto& back = blocks_.back();
            return back.pushVar(name);
        }
        return std::nullopt;
    }

    std::optional<addr_t> push(size_t size) {
        if (!blocks_.empty()) {
            auto& back = blocks_.back();
            return back.push(size);
        }
        return std::nullopt;
    }

    std::optional<addr_t> lookupVar(std::string_view name) const {
        for (auto it = blocks_.rbegin(); it != blocks_.rend(); ++it) {
            auto found = (*it).lookupVar(name);
            if (found) {
                return found;
            }
        }
        return std::nullopt;
    }

    addr_t getCurAddr() const {
        return blocks_.empty() ? 0U : blocks_.back().getCurAddr();
    }
};

} // namespace paracl
