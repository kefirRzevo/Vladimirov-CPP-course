#pragma once

#include <list>
#include <vector>

#include "INode.hpp"

namespace paracl
{

class NodeVisitor
{
private:
    using value_type = std::pair<INode*, bool>;
    using iterator = std::list<value_type>::iterator;
    using list = std::list<value_type>;

public:
    virtual void visit(INode* node) {
        if (typeid(*node) == typeid(UnaryExpression)) {
            visit(static_cast<UnaryExpression*>(node));
        } else if (typeid(*node) == typeid(BinaryExpression)) {
            visit(static_cast<BinaryExpression*>(node));
        } else if (typeid(*node) == typeid(TernaryExpression)) {
            visit(static_cast<TernaryExpression*>(node));
        } else if (typeid(*node) == typeid(ConstantExpression)) {
            visit(static_cast<ConstantExpression*>(node));
        } else if (typeid(*node) == typeid(VariableExpression)) {
            visit(static_cast<VariableExpression*>(node));
        } else if (typeid(*node) == typeid(InputExpression)) {
            visit(static_cast<InputExpression*>(node));
        } else if (typeid(*node) == typeid(BlockStatement)) {
            visit(static_cast<BlockStatement*>(node));
        } else if (typeid(*node) == typeid(ExpressionStatement)) {
            visit(static_cast<ExpressionStatement*>(node));
        } else if (typeid(*node) == typeid(IfStatement)) {
            visit(static_cast<IfStatement*>(node));
        } else if (typeid(*node) == typeid(IfElseStatement)) {
            visit(static_cast<IfElseStatement*>(node));
        } else if (typeid(*node) == typeid(WhileStatement)) {
            visit(static_cast<WhileStatement*>(node));
        } else if (typeid(*node) == typeid(OutputStatement)) {
            visit(static_cast<OutputStatement*>(node));
        } else if (typeid(*node) == typeid(BreakStatement)) {
            visit(static_cast<BreakStatement*>(node));
        } else if (typeid(*node) == typeid(ContinueStatement)) {
            visit(static_cast<ContinueStatement*>(node));
        } else {
            throw std::runtime_error("Can not cast base class");
        }
    }

    std::vector<INode*> postOrderTraversal(INode* node) {
        std::list<value_type> nodes;
        nodes.emplace_front(node, false);
        iterator cur = nodes.begin();
        while(cur != nodes.end()) {
            if (cur->second == false) {
                cur = push(cur, nodes);
            } else {
                cur = std::next(cur);
            }
        }
        std::vector<INode*> nodes_;
        for (auto& node: nodes) {
            nodes_.push_back(node.first);
        }
        return nodes_;
    }

private:
    iterator push(iterator& it, list& nodes) {
        INode* node = it->first;
        if (typeid(*node) == typeid(UnaryExpression)) {
            UnaryExpression* un = static_cast<UnaryExpression*>(node);
            it->second = true;
            return nodes.emplace(it, un->expr_, false);
        } else if (typeid(*node) == typeid(BinaryExpression)) {
            BinaryExpression* bin = static_cast<BinaryExpression*>(node);
            it->second = true;
            it = nodes.emplace(it, bin->right_, false);
            return nodes.emplace(it, bin->left_, false);
        } else if (typeid(*node) == typeid(TernaryExpression)) {
            TernaryExpression* ter = static_cast<TernaryExpression*>(node);
            it->second = true;
            it = nodes.emplace(it, ter->onFalse_, false);
            it = nodes.emplace(it, ter->onTrue_, false);
            return nodes.emplace(it, ter->condition_, false);
        } else if (typeid(*node) == typeid(ConstantExpression)) {
            it->second = true;
            return it;
        } else if (typeid(*node) == typeid(VariableExpression)) {
            it->second = true;
            return it;
        } else if (typeid(*node) == typeid(InputExpression)) {
            it->second = true;
            return it;
        } else if (typeid(*node) == typeid(BlockStatement)) {
            BlockStatement* block = static_cast<BlockStatement*>(node);
            it->second = true;
            auto stats = block->statements_;
            for (auto nodeIt = stats.rbegin(); nodeIt != stats.rend(); ++nodeIt) {
                it = nodes.emplace(it, *nodeIt, false);
            }
            return it;
        } else if (typeid(*node) == typeid(ExpressionStatement)) {
            ExpressionStatement* exprStat = static_cast<ExpressionStatement*>(node);
            it->second = true;
            return nodes.emplace(it, exprStat->expr_, false);
        } else if (typeid(*node) == typeid(IfElseStatement)) {
            IfElseStatement* ifStat = static_cast<IfElseStatement*>(node);
            it->second = true;
            it = nodes.emplace(it, ifStat->falseBlock_, false);
            it = nodes.emplace(it, ifStat->trueBlock_, false);
            return nodes.emplace(it, (INode*)ifStat->condition_, false);
        } else if (typeid(*node) == typeid(IfStatement)) {
            IfStatement* ifStat = static_cast<IfStatement*>(node);
            it->second = true;
            it = nodes.emplace(it, ifStat->trueBlock_, false);
            return nodes.emplace(it, ifStat->condition_, false);
        } else if (typeid(*node) == typeid(WhileStatement)) {
            WhileStatement* whileStat = static_cast<WhileStatement*>(node);
            it->second = true;
            it = nodes.emplace(it, whileStat->block_, false);
            return nodes.emplace(it, whileStat->condition_, false);
        } else if (typeid(*node) == typeid(OutputStatement)) {
            OutputStatement* out = static_cast<OutputStatement*>(node);
            it->second = true;
            return nodes.emplace(it, out->expr_, false);
        } else if (typeid(*node) == typeid(BreakStatement)) {
            it->second = true;
            return it;
        } else if (typeid(*node) == typeid(ContinueStatement)) {
            it->second = true;
            return it;
        } else {
            throw std::runtime_error("Can not cast base class");
        }
    }

protected:
    virtual void visit(UnaryExpression* node) = 0;

    virtual void visit(BinaryExpression* node) = 0;

    virtual void visit(TernaryExpression* node) = 0;

    virtual void visit(ConstantExpression* node) = 0;

    virtual void visit(VariableExpression* node) = 0;

    virtual void visit(InputExpression* node) = 0;

    virtual void visit(BlockStatement* node) = 0;

    virtual void visit(ExpressionStatement* node) = 0;

    virtual void visit(IfStatement* node) = 0;

    virtual void visit(IfElseStatement* node) = 0;

    virtual void visit(WhileStatement* node) = 0;

    virtual void visit(OutputStatement* node) = 0;

    virtual void visit(BreakStatement* node) = 0;

    virtual void visit(ContinueStatement* node) = 0;
};

} // namespace paracl
