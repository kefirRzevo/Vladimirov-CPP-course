#pragma once

#include <iostream>
#include <optional>
#include <string_view>
#include <unordered_map>

namespace paracl
{

struct VariableExpression;

struct StringHash
{
    std::size_t operator()(const std::string_view& key) const {
        return std::hash<std::string_view>()(key);
    }
};;

struct StringEqual
{
    bool operator()(const std::string_view& lhs, const std::string_view& rhs) const {
        return lhs == rhs;
    }
};

class Scope final
{
private:
    using VarsMap = std::unordered_map<std::string_view, VariableExpression*, StringHash, StringEqual>;

    VarsMap map_;

public:
    Scope() = default;

    void declare(std::string_view name, VariableExpression* node) {
        map_.emplace(name, node);
    }

    bool declared(std::string_view name) const {
        return map_.count(name);
    }

    std::optional<VariableExpression*> lookupVar(std::string_view name) const {
        auto found = map_.find(name);
        if (found == map_.end()) {
            return std::nullopt;
        }
        return found->second;
    }

    auto begin() const {
        return map_.begin();
    }

    auto end() const {
        return map_.end();
    }

    size_t size() const {
        return map_.size();
    }

    void clear() {
        map_.clear();
    }
};

class ScopeStack final
{
private:
    std::vector<Scope> scopes_;

public:
    ScopeStack() = default;

    void beginScope() {
        scopes_.emplace_back();
    }

    void beginScope(const Scope& table) {
        scopes_.push_back(table);
    }

    void endScope() {
        scopes_.pop_back();
    }

    void declare(std::string_view name, VariableExpression* node) {
        if (!scopes_.empty()) {
            scopes_.back().declare(name, node);
        }
    }

    bool declared(std::string_view name) const {
        return lookupScope(name) != std::nullopt;
    }

    std::optional<Scope> lookupScope(std::string_view name) const {
        for (auto it = scopes_.rbegin(); it != scopes_.rend(); ++it) {
            if ((*it).declared(name)) {
                return *it;
            }
        }
        return std::nullopt;
    }

    std::optional<VariableExpression*> lookupVar(std::string_view name) const {
        for (auto it = scopes_.rbegin(); it != scopes_.rend(); ++it) {
            auto found = (*it).lookupVar(name);
            if (found) {
                return found;
            }
        }
        return std::nullopt;
    }

    auto begin() const {
        return scopes_.rbegin();
    }

    auto end() const {
        return scopes_.rend();
    }

    size_t size() const {
        return scopes_.size();
    }

    void clear() {
        scopes_.clear();
    }
};

class AddrBlock
{
private:
    using addr_t = size_t;
    using AddrMap = std::unordered_map<std::string_view, addr_t, StringHash, StringEqual>;

    addr_t addr_;
    AddrMap map_;

public:
    AddrBlock(addr_t addrBeg) :
        addr_(addrBeg) {}

    void pushVar(std::string_view name) {
        map_.emplace(name, addr_);
        addr_ += sizeof(int);
    }

    std::optional<addr_t> lookupAddr(std::string_view name) const {
        auto found = map_.find(name);
        if (found == map_.end()) {
            return std::nullopt;
        }
        return found->second;
    }

    addr_t getAddr() const {
        return addr_;
    }
};

class AddrBlockStack final
{
private:
    using addr_t = size_t;

    std::vector<AddrBlock> blocks_;

public:
    AddrBlockStack() = default;

    void beginScope() {
        blocks_.emplace_back(getAddr());
    }

    void beginScope(const Scope& scope) {
        blocks_.emplace_back(getAddr());
        auto& back = blocks_.back();
        for (const auto& var: scope) {
            back.pushVar(var.first);
        }
    }

    void endScope() {
        blocks_.pop_back();
    }

    void pushVar(std::string_view name) {
        if (!blocks_.empty()) {
            auto& back = blocks_.back();
            back.pushVar(name);
        }
    }

    std::optional<addr_t> lookupVar(std::string_view name) const {
        for (auto it = blocks_.rbegin(); it != blocks_.rend(); ++it) {
            auto found = (*it).lookupAddr(name);
            if (found) {
                return found;
            }
        }
        return std::nullopt;
    }

    addr_t getAddr() const {
        return blocks_.empty() ? 0U : blocks_.back().getAddr();
    }
};

} // namespace paracl
