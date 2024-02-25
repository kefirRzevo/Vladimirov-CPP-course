#pragma once

#include <vector>
#include <optional>
#include <string_view>
#include <unordered_map>

#include "Utils.hpp"
#include "backend/Image.hpp"
#include "frontend/SymTable.hpp"

namespace paracl
{

struct StringHash;
struct StringEqual;

class ConstantPool final
{
private:
    using addr_t = size_t;
    using Pool = std::unordered_map<int, addr_t>;

    Pool pool_;
    Image& im_;

public:
    ConstantPool(Image& image) :
        im_(image) {}

    std::optional<addr_t> lookupConst(int value) {
        auto found = pool_.find(value);
        if (found != pool_.cend()) {
            return found->second;
        }
        return std::nullopt;
    }

    std::optional<addr_t> pushConst(int value) {
        auto addr = im_.addConst<ConstInt>(value);
        pool_.emplace(value, addr);
        return addr;
    }
};

class MemBlock final
{
private:
    using addr_t = size_t;
    using MemMap = std::unordered_map<
        std::string_view, addr_t, StringHash, StringEqual>;

    addr_t begAddr_;
    addr_t curAddr_;
    MemMap map_;

public:
    MemBlock(addr_t addrBeg) :
        begAddr_(addrBeg), curAddr_(addrBeg) {}

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

    addr_t getCurAddr() const {
        return blocks_.empty() ? 0U : blocks_.back().getCurAddr();
    }

public:
    StackFrame() = default;

    std::optional<addr_t> beginScope(const Scope& scope) {
        blocks_.emplace_back(getCurAddr());
        auto& back = blocks_.back();
        for (auto&& var: scope) {
            auto& back = blocks_.back();
            auto addr = back.pushVar(var.first);
            if (!addr) {
                return std::nullopt;
            }
        }
        return back.size();
    }

    std::optional<addr_t> endScope() {
        if (!blocks_.empty()) {
            auto size = blocks_.back().size();
            blocks_.pop_back();
            return size;
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
            auto found = it->lookupVar(name);
            if (found) {
                return found;
            }
        }
        return std::nullopt;
    }
};

} // namespace paracl
