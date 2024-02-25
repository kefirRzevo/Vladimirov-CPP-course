#pragma once

#include <vector>
#include <stdexcept>

#include "frontend/INode.hpp"

namespace paracl
{

class NodeCopier final : public NodeVisitor
{
private:
    AST& tree_;
    std::vector<INode*> copied_;

public:
    NodeCopier(AST& tree)
    : tree_(tree) {}

    INode* copy(INode* root) {
        if (!root) {
            return nullptr;
        }
        root->accept(*this);
        INode* copiedRoot = copied_.back();
        copied_.pop_back();
        if (!copied_.empty()) {
            throw std::runtime_error("Copied stack isn't empty");
        }
        return copiedRoot;
    }

    void visit(UnaryExpression* node) override {
        using Type = UnaryExpression;
        auto nodeLoc = node->getLocation();
        auto nodeOp = node->getOperator();
        auto nodeExpr = node->getExpr();
        nodeExpr->accept(*this);
        auto expr = static_cast<Expression*>(copied_.back());
        copied_.pop_back();
        auto n = tree_.createNode<Type>(nodeLoc, nodeOp, expr);
        copied_.push_back(n);
    }

    void visit(BinaryExpression* node) override {
        using Type = BinaryExpression;
        auto nodeLoc = node->getLocation();
        auto nodeOp = node->getOperator();
        auto nodeLeft = node->getLeftExpr();
        auto nodeRight = node->getRightExpr();
        nodeRight->accept(*this);
        auto right = static_cast<Expression*>(copied_.back());
        copied_.pop_back();
        nodeLeft->accept(*this);
        auto left = static_cast<Expression*>(copied_.back());
        copied_.pop_back();
        auto n = tree_.createNode<Type>(nodeLoc, nodeOp, left, right);
        copied_.push_back(n);
    }

    void visit(TernaryExpression* node) override {
        using Type = TernaryExpression;
        auto nodeLoc = node->getLocation();
        auto nodeCond = node->getCondExpr();
        auto nodeTrue = node->getTrueExpr();
        auto nodeFalse = node->getFalseExpr();
        nodeFalse->accept(*this);
        auto onFalse = static_cast<Expression*>(copied_.back());
        copied_.pop_back();
        nodeTrue->accept(*this);
        auto onTrue = static_cast<Expression*>(copied_.back());
        copied_.pop_back();
        nodeCond->accept(*this);
        auto cond = static_cast<Expression*>(copied_.back());
        copied_.pop_back();
        auto n = tree_.createNode<Type>(nodeLoc, cond, onTrue, onFalse);
        copied_.push_back(n);
    }

    void visit(ConstantExpression* node) override {
        using Type = ConstantExpression;
        auto nodeLoc = node->getLocation();
        auto nodeVal = node->getValue();
        auto n = tree_.createNode<Type>(nodeLoc, nodeVal);
        copied_.push_back(n);
    }

    void visit(VariableExpression* node) override {
        using Type = VariableExpression;
        auto nodeLoc = node->getLocation();
        auto nodeName = node->getName();
        auto n = tree_.createNode<Type>(nodeLoc, nodeName);
        copied_.push_back(n);
    }

    void visit(InputExpression* node) override {
        using Type = InputExpression;
        auto nodeLoc = node->getLocation();
        auto n = tree_.createNode<Type>(nodeLoc);
        copied_.push_back(n);
    }

    void visit(BlockStatement* node) override {
        using Type = BlockStatement;
        auto nodeLoc = node->getLocation();
        auto n = tree_.createNode<Type>(nodeLoc);
        for (auto it = node->rbegin(); it != node->rend(); ++it) {
            (*it)->accept(*this);
            auto stat = static_cast<Statement*>(copied_.back());
            copied_.pop_back();
            n->addStatement(stat);
        }
        copied_.push_back(n);
    }

    void visit(ExpressionStatement* node) override {
        using Type = ExpressionStatement;
        auto nodeLoc = node->getLocation();
        auto nodeExpr = node->getExpr();
        nodeExpr->accept(*this);
        auto expr = static_cast<Expression*>(copied_.back());
        copied_.pop_back();
        auto n = tree_.createNode<Type>(nodeLoc, expr);
        copied_.push_back(n);
    }

    void visit(IfStatement* node) override {
        using Type = IfStatement;
        auto nodeLoc = node->getLocation();
        auto nodeCond = node->getCondExpr();
        auto nodeTrue = node->getTrueBlock();
        nodeTrue->accept(*this);
        auto trueBlock = static_cast<Statement*>(copied_.back());
        copied_.pop_back();
        nodeCond->accept(*this);
        auto cond = static_cast<Expression*>(copied_.back());
        copied_.pop_back();
        auto n = tree_.createNode<Type>(nodeLoc, cond, trueBlock);
        copied_.push_back(n);
    }

    void visit(IfElseStatement* node) override {
        using Type = IfElseStatement;
        auto nodeLoc = node->getLocation();
        auto nodeCond = node->getCondExpr();
        auto nodeTrue = node->getTrueBlock();
        auto nodeFalse = node->getFalseBlock();
        nodeFalse->accept(*this);
        auto falseBlock = static_cast<Statement*>(copied_.back());
        copied_.pop_back();
        nodeTrue->accept(*this);
        auto trueBlock = static_cast<Statement*>(copied_.back());
        copied_.pop_back();
        nodeCond->accept(*this);
        auto cond = static_cast<Expression*>(copied_.back());
        copied_.pop_back();
        auto n = tree_.createNode<Type>(nodeLoc, cond, trueBlock, falseBlock);
        copied_.push_back(n);
    }

    void visit(WhileStatement* node) override {
        using Type = WhileStatement;
        auto nodeLoc = node->getLocation();
        auto nodeCond = node->getCondExpr();
        auto nodeBlock = node->getBlock();
        nodeBlock->accept(*this);
        auto block = static_cast<Statement*>(copied_.back());
        copied_.pop_back();
        nodeCond->accept(*this);
        auto cond = static_cast<Expression*>(copied_.back());
        copied_.pop_back();
        auto n = tree_.createNode<Type>(nodeLoc, cond, block);
        copied_.push_back(n);
    }

    void visit(OutputStatement* node) override {
        using Type = OutputStatement;
        auto nodeLoc = node->getLocation();
        auto nodeExpr = node->getExpr();
        nodeExpr->accept(*this);
        auto expr = static_cast<Expression*>(copied_.back());
        copied_.pop_back();
        auto n = tree_.createNode<Type>(nodeLoc, expr);
        copied_.push_back(n);
    }

    void visit(BreakStatement* node) override {
        using Type = BreakStatement;
        auto nodeLoc = node->getLocation();
        auto n = tree_.createNode<Type>(nodeLoc);
        copied_.push_back(n);
    }

    void visit(ContinueStatement* node) override {
        using Type = ContinueStatement;
        auto nodeLoc = node->getLocation();
        auto n = tree_.createNode<Type>(nodeLoc);
        copied_.push_back(n);
    }
};

} // namespace paracl {
