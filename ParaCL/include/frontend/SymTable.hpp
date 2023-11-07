#pragma once

#include <iostream>
#include <optional>
#include <string_view>
#include <unordered_map>
#include "AST.hpp"

namespace paracl::frontend
{

class SymTable;
class Scopes;

class SymTable final
{
private:
    struct keyHash
    {
        std::size_t operator()(const string_view& key) const {
            return std::hash<K>()(key);
        }
    };

    struct keyEqual
    {
        bool operator() (const string_view& lhs, const string_view& rhs) const {
            return lhs == rhs;
        }
    };

    using Map = std::unordered_map<string_view, VariableExpression*, keyHash, keyEqual>;

    Map table_;

public:
    SymTable() = default;

    void declare(std::string_view name, VariableExpression* node) {
        table_.emplace(name, node);
    }

    bool declared(std::string_view name) const {
        return table_.count(name);
    }

    std::optional<VariableExpression* > lookupVariable(std::string_view name) const {
        auto found = table_.find(name);
        if (found == table_.end()) {
            return std::nullopt;
        }
        return found.second;
    }

    auto begin() const {
        return table_.begin();
    }

    auto end() const {
        return table_.end();
    }

    size_t size() const {
        return table_.size();
    }
};

class ScopeChecker final
{
private:
    std::vector<SymTable* > scopes_;

public:
    ScopeChecker() = default;

    void beginScope(SymTable* table) {
        scopes_.push_back(table);
    }

    void endScope() {
        scopes_.pop_back();
    }

    std::optional<SymTable* > lookupScope(std::string_view name) const {
        for (auto it = scopes_.rbegin(); it != scopes_.rend(); ++it) {
            if (it->declared(name)) {
                return *it;
            }
        }
        return std::nullopt;
    }

    void declare(std::string_view name, VariableExpression* node) {
        if (!scopes_.empty()) {
            scopes_.back().declare(name, node);
        }
    }

    bool declared(std::string_view name) const {
        return lookupScope(name) != std::nullopt;
    }

    std::optional<VariableExpression* > lookupVariable(std::string_view name) const {
        for (auto it = scopes_.rbegin(); it != scopes_.rend(); ++it) {
            auto found = it->lookupVariable(name);
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

} // namespace paracl::frontend {
