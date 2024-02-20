#pragma once

#include <vector>
#include <memory>
#include <string>
#include <fstream>
#include <utility>
#include <iostream>

#include "frontend/INode.hpp"

namespace paracl
{

class Driver;

class AST final
{
private:
    std::vector<std::unique_ptr<INode>> nodes_;
    INode* root_ = nullptr;

public:
    AST() = default;

    AST(const AST& rhs);

    AST& operator=(const AST& rhs) {
        if (this == std::addressof(rhs)) {
            return *this;
        }
        AST temp{rhs};
        *this = std::move(temp);
        return *this;
    }

    AST(AST&&) = default;

    AST& operator=(AST&&) = default;

    ~AST() = default;

    template<typename NodeType, typename... NodeArgs>
    NodeType* createNode(NodeArgs&&... args) {
        auto uptr = std::make_unique<NodeType>(std::forward<NodeArgs>(args)...);
        auto ptr = uptr.get();
        nodes_.push_back(std::move(uptr));
        return ptr;
    }

    void setRoot(INode* root) noexcept {
        root_ = root;
    }

    INode* getRoot() noexcept {
        return root_;
    }

    void clear() noexcept {
        nodes_.clear();
        root_ = nullptr;
    }

    void generateDot(const std::string& filepath) const;

    void generateCl(const std::string& filepath) const;

    void generateCpp(const std::string& filepath) const;

    void semanticAnalyze(Driver& driver);
};

} // namespace paracl {
