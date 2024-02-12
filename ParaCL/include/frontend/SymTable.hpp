#pragma once

#include <iostream>
#include <optional>
#include <string_view>
#include <unordered_map>

#include "../Utils.hpp"

namespace paracl
{

struct VariableExpression;
struct StringHash;
struct StringEqual;

class Scope final
{
private:
    using VarsMap = std::unordered_map<std::string_view, VariableExpression*, StringHash, StringEqual>;

    VarsMap map_;

public:
    Scope() = default;

    void declare(std::string_view name, VariableExpression* node) {
        [[maybe_unused]] auto [it, flag] = map_.emplace(name, node);
        if (!flag) {
            throw std::runtime_error("Can't emplace to hashmap");
        }
    }

    bool declared(std::string_view name) const {
        return map_.count(name);
    }

    std::optional<VariableExpression*> lookupVar(std::string_view name) const {
        auto found = map_.find(name);
        if (found != map_.end()) {
            return found->second;
        }
        return std::nullopt;
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

    void endScope() {
        scopes_.pop_back();
    }

    void declare(std::string_view name, VariableExpression* node) {
        if (!scopes_.empty()) {
            auto& back = scopes_.back();
            back.declare(name, node);
        }
    }

    bool declared(std::string_view name) const {
        for (auto it = scopes_.rbegin(); it != scopes_.rend(); ++it) {
            if (it->declared(name)) {
                return true;
            }
        }
        return false;
    }

    std::optional<VariableExpression*> lookupVar(std::string_view name) const {
        for (auto it = scopes_.rbegin(); it != scopes_.rend(); ++it) {
            auto found = it->lookupVar(name);
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
};

} // namespace paracl
