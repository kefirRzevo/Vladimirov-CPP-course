#pragma once

#include "NodeVisitor.hpp"

namespace paracl
{

class NodeCopier final : public NodeVisitor
{
private:
    using NodeVisitor::postOrderTraversal;
    using NodeVisitor::visit;

    AST& tree_;
    std::vector<INode*> copied_;

public:
    NodeCopier(AST& tree)
    : tree_(tree) {}

    INode* copy(INode* root) {
        auto nodes = postOrderTraversal(root);
        for (auto& node: nodes) {
            visit(node);
        }
        return copied_.back();
    }

protected:
    void visit(UnaryExpression* node) override {
        Expression* expr = static_cast<Expression*>(copied_.back());
        copied_.pop_back();
        UnaryExpression* un = tree_.createNode<UnaryExpression>(node->loc_, node->op_, expr);
        copied_.push_back(un);
    }

    void visit(BinaryExpression* node) override {
        Expression* right = static_cast<Expression*>(copied_.back());
        copied_.pop_back();
        Expression* left = static_cast<Expression*>(copied_.back());
        copied_.pop_back();
        BinaryExpression* bin = tree_.createNode<BinaryExpression>(node->loc_, node->op_, left, right);
        copied_.push_back(bin);
    }

    void visit(TernaryExpression* node) override {
        Expression* onFalse = static_cast<Expression*>(copied_.back());
        copied_.pop_back();
        Expression* onTrue = static_cast<Expression*>(copied_.back());
        copied_.pop_back();
        Expression* condition = static_cast<Expression*>(copied_.back());
        copied_.pop_back();
        TernaryExpression* ter = tree_.createNode<TernaryExpression>(node->loc_, condition, onTrue, onFalse);
        copied_.push_back(ter);
    }

    void visit(ConstantExpression* node) override {
        ConstantExpression* expr = tree_.createNode<ConstantExpression>(node->loc_, node->value_);
        copied_.push_back(expr);
    }

    void visit(VariableExpression* node) override {
        VariableExpression* var = tree_.createNode<VariableExpression>(node->loc_, node->name_);
        copied_.push_back(var);
    }

    void visit(InputExpression* node) override {
        InputExpression* in = tree_.createNode<InputExpression>(node->loc_);
        copied_.push_back(in);
    }

    void visit(BlockStatement* node) override {
        BlockStatement* block = tree_.createNode<BlockStatement>(node->loc_);
        for (size_t i = 0; i < node->statements_.size(); ++i) {
            Statement* stat = static_cast<Statement*>(copied_.back());
            copied_.pop_back();
            block->statements_.push_front(stat);
        }
        copied_.push_back(block);
    }

    void visit(ExpressionStatement* node) override {
        Expression* expr = static_cast<Expression*>(copied_.back());
        copied_.pop_back();
        ExpressionStatement* exprStat = tree_.createNode<ExpressionStatement>(node->loc_, expr);
        copied_.push_back(exprStat);
    }

    void visit(IfStatement* node) override {
        Statement* trueBlock = static_cast<Statement*>(copied_.back());
        copied_.pop_back();
        Expression* condition = static_cast<Expression*>(copied_.back());
        copied_.pop_back();
        IfStatement* ifStat = tree_.createNode<IfStatement>(node->loc_, condition, trueBlock);
        copied_.push_back(ifStat);
    }

    void visit(IfElseStatement* node) override {
        Statement* falseBlock = static_cast<Statement*>(copied_.back());
        copied_.pop_back();
        Statement* trueBlock = static_cast<Statement*>(copied_.back());
        copied_.pop_back();
        Expression* condition = static_cast<Expression*>(copied_.back());
        copied_.pop_back();
        IfElseStatement* ifElseStat = tree_.createNode<IfElseStatement>(node->loc_, condition, trueBlock, falseBlock);
        copied_.push_back(ifElseStat);
    }

    void visit(WhileStatement* node) override {
        Statement* block = static_cast<Statement*>(copied_.back());
        copied_.pop_back();
        Expression* condition = static_cast<Expression*>(copied_.back());
        copied_.pop_back();
        WhileStatement* whileStat = tree_.createNode<WhileStatement>(node->loc_, condition, block);
        copied_.push_back(whileStat);
    }

    void visit(OutputStatement* node) override {
        Expression* expr = static_cast<Expression*>(copied_.back());
        copied_.pop_back();
        OutputStatement* out = tree_.createNode<OutputStatement>(node->loc_, expr);
        copied_.push_back(out);
    }

    void visit(BreakStatement* node) override {
        BreakStatement* breakStat = tree_.createNode<BreakStatement>(node->loc_);
        copied_.push_back(breakStat);
    }

    void visit(ContinueStatement* node) override {
        ContinueStatement* continueStat = tree_.createNode<ContinueStatement>(node->loc_);
        copied_.push_back(continueStat);
    }
};

} // namespace paracl {
