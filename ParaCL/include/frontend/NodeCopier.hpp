#pragma once

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
        assert(copied_.empty());
        return copiedRoot;
    }

    void visit(UnaryExpression* node) override {
        using Type = UnaryExpression;
        node->expr_->accept(*this);
        Expression* expr = static_cast<Expression*>(copied_.back());
        copied_.pop_back();
        Type* n = tree_.createNode<Type>(node->loc_, node->op_, expr);
        copied_.push_back(n);
    }

    void visit(BinaryExpression* node) override {
        using Type = BinaryExpression;
        node->right_->accept(*this);
        Expression* right = static_cast<Expression*>(copied_.back());
        copied_.pop_back();
        node->left_->accept(*this);
        Expression* left = static_cast<Expression*>(copied_.back());
        copied_.pop_back();
        Type* n = tree_.createNode<Type>(node->loc_, node->op_, left, right);
        copied_.push_back(n);
    }

    void visit(TernaryExpression* node) override {
        using Type = TernaryExpression;
        node->onFalse_->accept(*this);
        Expression* onFalse = static_cast<Expression*>(copied_.back());
        copied_.pop_back();
        node->onTrue_->accept(*this);
        Expression* onTrue = static_cast<Expression*>(copied_.back());
        copied_.pop_back();
        node->condition_->accept(*this);
        Expression* condition = static_cast<Expression*>(copied_.back());
        copied_.pop_back();
        Type* n = tree_.createNode<Type>(node->loc_, condition,
                                         onTrue, onFalse);
        copied_.push_back(n);
    }

    void visit(ConstantExpression* node) override {
        using Type = ConstantExpression;
        Type* n = tree_.createNode<Type>(node->loc_,node->value_);
        copied_.push_back(n);
    }

    void visit(VariableExpression* node) override {
        using Type = VariableExpression;
        Type* n = tree_.createNode<Type>(node->loc_, node->name_);
        copied_.push_back(n);
    }

    void visit(InputExpression* node) override {
        using Type = InputExpression;
        InputExpression* n = tree_.createNode<Type>(node->loc_);
        copied_.push_back(n);
    }

    void visit(BlockStatement* node) override {
        using Type = BlockStatement;
        Type* n = tree_.createNode<Type>(node->loc_);
        auto&& stats = node->statements_;
        for (auto it = stats.rbegin(); it != stats.rend(); ++it) {
            (*it)->accept(*this);
            Statement* stat = static_cast<Statement*>(copied_.back());
            copied_.pop_back();
            n->addStatement(stat);
        }
        copied_.push_back(n);
    }

    void visit(ExpressionStatement* node) override {
        using Type = ExpressionStatement;
        node->expr_->accept(*this);
        Expression* expr = static_cast<Expression*>(copied_.back());
        copied_.pop_back();
        Type* n = tree_.createNode<Type>(node->loc_, expr);
        copied_.push_back(n);
    }

    void visit(IfStatement* node) override {
        using Type = IfStatement;
        node->trueBlock_->accept(*this);
        Statement* trueBlock = static_cast<Statement*>(copied_.back());
        copied_.pop_back();
        node->condition_->accept(*this);
        Expression* condition = static_cast<Expression*>(copied_.back());
        copied_.pop_back();
        Type* n = tree_.createNode<Type>(node->loc_, condition, trueBlock);
        copied_.push_back(n);
    }

    void visit(IfElseStatement* node) override {
        using Type = IfElseStatement;
        node->falseBlock_->accept(*this);
        Statement* falseBlock = static_cast<Statement*>(copied_.back());
        copied_.pop_back();
        node->trueBlock_->accept(*this);
        Statement* trueBlock = static_cast<Statement*>(copied_.back());
        copied_.pop_back();
        node->condition_->accept(*this);
        Expression* condition = static_cast<Expression*>(copied_.back());
        copied_.pop_back();
        Type* n = tree_.createNode<Type>(node->loc_, condition,
                                         trueBlock, falseBlock);
        copied_.push_back(n);
    }

    void visit(WhileStatement* node) override {
        using Type = WhileStatement;
        node->block_->accept(*this);
        Statement* block = static_cast<Statement*>(copied_.back());
        copied_.pop_back();
        node->condition_->accept(*this);
        Expression* condition = static_cast<Expression*>(copied_.back());
        copied_.pop_back();
        Type* n = tree_.createNode<Type>(node->loc_, condition, block);
        copied_.push_back(n);
    }

    void visit(OutputStatement* node) override {
        using Type = OutputStatement;
        node->expr_->accept(*this);
        Expression* expr = static_cast<Expression*>(copied_.back());
        copied_.pop_back();
        Type* n = tree_.createNode<Type>(node->loc_, expr);
        copied_.push_back(n);
    }

    void visit(BreakStatement* node) override {
        using Type = BreakStatement;
        Type* n = tree_.createNode<Type>(node->loc_);
        copied_.push_back(n);
    }

    void visit(ContinueStatement* node) override {
        using Type = ContinueStatement;
        Type* n = tree_.createNode<Type>(node->loc_);
        copied_.push_back(n);
    }
};

} // namespace paracl {
