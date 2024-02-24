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
        auto nodeLoc = node->getLocation();
        auto nodeOp = node->getOperator();
        auto nodeExpr = node->getExpr();
        if (isPostfix(nodeOp) || isPrefix(nodeOp)) {
            if (typeid(*nodeExpr) != typeid(VariableExpression)) {
                driver_.reportError<UnassignableExpression>(nodeLoc);
            }
        }
        nodeExpr->accept(*this);
    }

    void visit(BinaryExpression* node) override {
        auto nodeLoc = node->getLocation();
        auto nodeOp = node->getOperator();
        auto nodeLeft = node->getLeftExpr();
        auto nodeRight = node->getRightExpr();
        switch (nodeOp) {
        case BinaryOperator::BIN_ASSIGN: {
            nodeRight->accept(*this);
            declareMode_ = true;
            if (typeid(*nodeLeft) != typeid(VariableExpression)) {
                driver_.reportError<UnassignableExpression>(nodeLoc);
            }
            nodeLeft->accept(*this);
            declareMode_ = false;
            break;
        }
        default: {
            nodeLeft->accept(*this);
            nodeRight->accept(*this);
            break;
        }
        }
    }

    void visit(TernaryExpression* node) override {
        node->getCondExpr()->accept(*this);
        node->getTrueExpr()->accept(*this);
        node->getFalseExpr()->accept(*this);
    }

    void visit(ConstantExpression* ) override {}

    void visit(VariableExpression* node) override {
        auto nodeLoc = node->getLocation();
        auto nodeName = node->getName();
        if (declareMode_ && !scopes_.declared(nodeName)) {
            scopes_.declare(nodeName, node);
        } else if (!scopes_.declared(nodeName)) {
            driver_.reportError<UndeclaredIdentifier>(nodeLoc, nodeName);
        }
    }

    void visit(InputExpression* ) override {}

    void visit(BlockStatement* node) override {
        scopes_.beginScope(node->getScope());
        for (auto&& statement : *node) {
            statement->accept(*this);
        }
        scopes_.endScope();
    }

    void visit(ExpressionStatement* node) override {
        node->getExpr()->accept(*this);
    }

    void visit(IfStatement* node) override {
        scopes_.beginScope(node->getScope());
        node->getCondExpr()->accept(*this);
        node->getTrueBlock()->accept(*this);
        scopes_.endScope();
    }

    void visit(IfElseStatement* node) override {
        scopes_.beginScope(node->getScope());
        node->getCondExpr()->accept(*this);
        node->getTrueBlock()->accept(*this);
        node->getFalseBlock()->accept(*this);
        scopes_.endScope();
    }

    void visit(WhileStatement* node) override {
        scopes_.beginScope(node->getScope());
        loops_.push_back(node);
        node->getCondExpr()->accept(*this);
        node->getBlock()->accept(*this);
        loops_.pop_back();
        scopes_.endScope();
    }

    void visit(OutputStatement* node) override {
        node->getExpr()->accept(*this);
    }

    void visit(BreakStatement* node) override {
        auto nodeLoc = node->getLocation();
        if (loops_.empty()) {
            driver_.reportError<OutOfLoopStatement>(nodeLoc, "break");
        } else {
            node->setLoopStat(static_cast<WhileStatement*>(loops_.back()));
        }
    }

    void visit(ContinueStatement* node) override {
        auto nodeLoc = node->getLocation();
        if (loops_.empty()) {
            driver_.reportError<OutOfLoopStatement>(nodeLoc, "continue");
        } else {
            node->setLoopStat(static_cast<WhileStatement*>(loops_.back()));
        }
    }
};

} //namespace paracl

#include "Driver.hpp"
