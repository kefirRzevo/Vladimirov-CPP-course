#pragma once

#include <vector>

#include "frontend/SymTable.hpp"
#include "frontend/INode.hpp"
#include "frontend/Driver.hpp"

namespace paracl
{

class Driver;

class NodeSemanticAnalyzer final : public NodeVisitor
{
private:
    using NodeVisitor::visit;

    Driver& driver_;
    ScopeStack scopes_;
    std::vector<INode*> loops_;
    bool declareMode_ = false;

public:
    NodeSemanticAnalyzer(Driver& driver)
    : driver_(driver) {}

    void analyze(INode* root) {
        if (root) {
            root->accept(*this);
        }
    }

    void visit(UnaryExpression* node) override {
        if (isPostfix(node->op_) || isPrefix(node->op_)) {
            if (typeid(*node->expr_) != typeid(VariableExpression)) {
                driver_.reportError<UnassignableExpression>(node->loc_);
            }
        }
        node->expr_->accept(*this);
    }

    void visit(BinaryExpression* node) override {
        switch (node->op_) {
        case BinaryOperator::BIN_ASSIGN: {
            node->right_->accept(*this);
            declareMode_ = true;
            if (typeid(*node->left_) != typeid(VariableExpression)) {
                driver_.reportError<UnassignableExpression>(node->loc_);
            }
            node->left_->accept(*this);
            declareMode_ = false;
            break;
        }
        default: {
            node->left_->accept(*this);
            node->right_->accept(*this);
            break;
        }
        }
    }

    void visit(TernaryExpression* node) override {
        node->condition_->accept(*this);
        node->onTrue_->accept(*this);
        node->onFalse_->accept(*this);
    }

    void visit(ConstantExpression* ) override {}

    void visit(VariableExpression* node) override {
        if (declareMode_ && !scopes_.declared(node->name_)) {
            scopes_.declare(node->name_, node);
        } else if (!scopes_.declared(node->name_)) {
            driver_.reportError<UndeclaredIdentifier>(node->loc_, node->name_);
        }
    }

    void visit(InputExpression* ) override {}

    void visit(BlockStatement* node) override {
        scopes_.beginScope(node->scope_);
        for (auto statement: node->statements_) {
            statement->accept(*this);
        }
        scopes_.endScope();
    }

    void visit(ExpressionStatement* node) override {
        node->expr_->accept(*this);
    }

    void visit(IfStatement* node) override {
        scopes_.beginScope(node->scope_);
        node->condition_->accept(*this);
        node->trueBlock_->accept(*this);
        scopes_.endScope();
    }

    void visit(IfElseStatement* node) override {
        scopes_.beginScope(node->scope_);
        node->condition_->accept(*this);
        node->trueBlock_->accept(*this);
        node->falseBlock_->accept(*this);
        scopes_.endScope();
    }

    void visit(WhileStatement* node) override {
        scopes_.beginScope(node->scope_);
        loops_.push_back(node);
        node->condition_->accept(*this);
        node->block_->accept(*this);
        loops_.pop_back();
        scopes_.endScope();
    }

    void visit(OutputStatement* node) override {
        node->expr_->accept(*this);
    }

    void visit(BreakStatement* node) override {
        if (loops_.empty()) {
            driver_.reportError<OutOfLoopStatement>(node->loc_, "break");
        } else {
            node->loop_ = static_cast<WhileStatement*>(loops_.back());
        }
    }

    void visit(ContinueStatement* node) override {
        if (loops_.empty()) {
            driver_.reportError<OutOfLoopStatement>(node->loc_, "continue");
        } else {
            node->loop_ = static_cast<WhileStatement*>(loops_.back());
        }
    }
};

} //namespace paracl

#include "Driver.hpp"
