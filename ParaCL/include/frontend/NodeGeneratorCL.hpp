#pragma once

#include <iostream>

#include "frontend/INode.hpp"

namespace paracl
{

class NodeGeneratorCl final : public NodeVisitor
{
private:
    std::string filepath_;
    std::ofstream os_;

public:
    NodeGeneratorCl(const std::string& filepath)
    : filepath_(filepath), os_{filepath} {}

    void generate(INode* root) {
        if (root) {
            root->accept(*this);
        }
        os_.flush();
        std::string cmd = "clang-format -i " + filepath_;
        std::system(cmd.c_str());
    }

    void visit(UnaryExpression* node) override {
        auto nodeOp = node->getOperator();
        if (isPrefix(nodeOp)) {
            os_ << toString(nodeOp);
        }
        node->getExpr()->accept(*this);
        if (isPostfix(nodeOp)) {
            os_ << toString(nodeOp);
        }
    }

    void visit(BinaryExpression* node) override {
        node->getLeftExpr()->accept(*this);
        os_ << toString(node->getOperator());
        node->getRightExpr()->accept(*this);
    }

    void visit(TernaryExpression* node) override {
        node->getCondExpr()->accept(*this);
        os_ << "?";
        node->getTrueExpr()->accept(*this);
        os_ << ":";
        node->getFalseExpr()->accept(*this);
    }

    void visit(ConstantExpression* node) override {
        os_ << node->getValue();
    }

    void visit(VariableExpression* node) override {
        os_ << node->getName();
    }

    void visit(InputExpression* ) override {
        os_ << "?";
    }

    void visit(BlockStatement* node) override {
        for (auto&& statement : *node) {
            statement->accept(*this);
        }
    }

    void visit(ExpressionStatement* node) override {
        node->getExpr()->accept(*this);
        os_ << ";";
    }

    void visit(IfStatement* node) override {
        os_ << "if(";
        node->getCondExpr()->accept(*this);
        os_ << "){";
        node->getTrueBlock()->accept(*this);
        os_ << "}";
    }

    void visit(IfElseStatement* node) override {
        os_ << "if(";
        node->getCondExpr()->accept(*this);
        os_ << "){";
        node->getTrueBlock()->accept(*this);
        os_ << "}else{";
        node->getFalseBlock()->accept(*this);
        os_ << "}";
    }

    void visit(WhileStatement* node) override {
        os_ << "while(";
        node->getCondExpr()->accept(*this);
        os_ << "){";
        node->getBlock()->accept(*this);
        os_ << "}";
    }

    void visit(OutputStatement* node) override {
        os_ << "print(";
        node->getExpr()->accept(*this);
        os_ << ");";
    }

    void visit(BreakStatement* ) override {
        os_ << "break;";
    }

    void visit(ContinueStatement* ) override {
        os_ << "continue;";
    }
};

} // namespace paracl
