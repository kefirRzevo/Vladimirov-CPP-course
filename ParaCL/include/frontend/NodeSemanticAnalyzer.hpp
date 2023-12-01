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

    void clear() {
        scopes_.clear();
    }

    void analyze(INode* root) {
        visit(root);
    }

protected:
    void visit(UnaryExpression* node) override {
        switch(node->op_) {
            case UnaryOperation::UN_PREFIX_INC:
            case UnaryOperation::UN_PREFIX_DEC:
            case UnaryOperation::UN_POSTFIX_INC:
            case UnaryOperation::UN_POSTFIX_DEC:
                if (typeid(*node->expr_) != typeid(VariableExpression)) {
                    driver_.getReporter()->reportError<UnassignableExpression>(node->loc_);
                }
                break;
            default:
                break;
        }
        visit(node->expr_);
    }

    void visit(BinaryExpression* node) override {
        visit(node->right_);
        if (node->op_ == BinaryOperation::BIN_ASSIGN) {
            declareMode = true;
            if (typeid(*node->left_) != typeid(VariableExpression)) {
                driver_.getReporter()->reportError<UnassignableExpression>(node->loc_);
            }
            visit(node->left_);
            declareMode = false;
        } else {
            visit(node->left_);
        }
    }

    void visit(TernaryExpression* node) override {
        visit(node->condition_);
        visit(node->onTrue_);
        visit(node->onFalse_);
    }

    void visit(ConstantExpression* ) override {}

    void visit(VariableExpression* node) override {
        if (declareMode) {
            scopes_.declare(node->name_, node);
        } else if (!scopes_.declared(node->name_)) {
            driver_.getReporter()->reportError<UndeclaredIdentifier>(node->loc_, node->name_);
        }
    }

    void visit(InputExpression* ) override {}

    void visit(BlockStatement* node) override {
        scopes_.beginScope(std::addressof(node->table_));
        for (auto statement: node->statements_) {
            visit(statement);
        }
        scopes_.endScope();
    }

    void visit(ExpressionStatement* node) override {
        visit(node->expr_);
    }

    void visit(IfStatement* node) override {
        visit(node->condition_);
        visit(node->trueBlock_);
    }

    void visit(IfElseStatement* node) override {
        visit(node->condition_);
        visit(node->trueBlock_);
        visit(node->falseBlock_);
    }

    void visit(WhileStatement* node) override {
        whiles_.push_back(node);
        visit(node->condition_);
        visit(node->block_);
        whiles_.pop_back();
    }

    void visit(OutputStatement* node) override {
        visit(node->expr_);
    }

    void visit(BreakStatement* node) override {
        if (whiles_.empty()) {
            driver_.getReporter()->reportError<OutOfLoopStatement>(node->loc_, "break");
        } else {
            node->whileStat = static_cast<WhileStatement*>(whiles_.back());
        }
    }

    void visit(ContinueStatement* node) override {
        if (whiles_.empty()) {
            driver_.getReporter()->reportError<OutOfLoopStatement>(node->loc_, "continue");
        } else {
            node->whileStat = static_cast<WhileStatement*>(whiles_.back());
        }
    }
};

} //namespace paracl

#include "Driver.hpp"
