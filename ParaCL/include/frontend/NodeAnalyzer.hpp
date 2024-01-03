#pragma once

#include <vector>

#include "NodeVisitor.hpp"
#include "SymTable.hpp"
#include "INode.hpp"
#include "Driver.hpp"

namespace paracl
{

class Driver;

class NodeSemanticAnalyzer final : public NodeVisitor
{
private:
    using NodeVisitor::visit;

    Driver& driver_;
    ScopeChecker scopes_;
    std::vector<INode*> whiles_;
    bool declareMode = false;

public:
    NodeSemanticAnalyzer(Driver& driver)
    : driver_(driver) {}

    void analyze(INode* root) {
        root->accept(*this);
    }

    void visit(UnaryExpression* node) override {
        switch(node->op_) {
            case UnaryOperator::UN_PREFIX_INC:
            case UnaryOperator::UN_PREFIX_DEC:
            case UnaryOperator::UN_POSTFIX_INC:
            case UnaryOperator::UN_POSTFIX_DEC:
                if (typeid(*node->expr_) != typeid(VariableExpression)) {
                    driver_.reportError<UnassignableExpression>(node->loc_);
                }
                break;
            default:
                break;
        }
        node->expr_->accept(*this);
    }

    void visit(BinaryExpression* node) override {
        node->right_->accept(*this);
        if (node->op_ == BinaryOperator::BIN_ASSIGN) {
            declareMode = true;
            if (typeid(*node->left_) != typeid(VariableExpression)) {
                driver_.reportError<UnassignableExpression>(node->loc_);
            }
            node->left_->accept(*this);
            declareMode = false;
        } else {
            node->left_->accept(*this);
        }
    }

    void visit(TernaryExpression* node) override {
        node->condition_->accept(*this);
        node->onTrue_->accept(*this);
        node->onFalse_->accept(*this);
    }

    void visit(ConstantExpression* ) override {}

    void visit(VariableExpression* node) override {
        if (declareMode) {
            scopes_.declare(node->name_, node);
        } else if (!scopes_.declared(node->name_)) {
            driver_.reportError<UndeclaredIdentifier>(node->loc_, node->name_);
        }
    }

    void visit(InputExpression* ) override {}

    void visit(BlockStatement* node) override {
        scopes_.beginScope(std::addressof(node->table_));
        for (auto statement: node->statements_) {
            statement->accept(*this);
        }
        scopes_.endScope();
    }

    void visit(ExpressionStatement* node) override {
        node->expr_->accept(*this);
    }

    void visit(IfStatement* node) override {
        node->condition_->accept(*this);
        node->trueBlock_->accept(*this);
    }

    void visit(IfElseStatement* node) override {
        node->condition_->accept(*this);
        node->trueBlock_->accept(*this);
        node->falseBlock_->accept(*this);
    }

    void visit(WhileStatement* node) override {
        whiles_.push_back(node);
        node->condition_->accept(*this);
        node->block_->accept(*this);
        whiles_.pop_back();
    }

    void visit(OutputStatement* node) override {
        node->expr_->accept(*this);
    }

    void visit(BreakStatement* node) override {
        if (whiles_.empty()) {
            driver_.reportError<OutOfLoopStatement>(node->loc_, "break");
        } else {
            node->whileStat = static_cast<WhileStatement*>(whiles_.back());
        }
    }

    void visit(ContinueStatement* node) override {
        if (whiles_.empty()) {
            driver_.reportError<OutOfLoopStatement>(node->loc_, "continue");
        } else {
            node->whileStat = static_cast<WhileStatement*>(whiles_.back());
        }
    }
};

} //namespace paracl

#include "Driver.hpp"
