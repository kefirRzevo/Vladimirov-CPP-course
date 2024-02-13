#pragma once

#include <iostream>

#include "frontend/INode.hpp"

namespace paracl
{

class NodeDecoder final : public NodeVisitor
{
private:
    std::string filepath_;
    std::ofstream os_;

public:
    NodeDecoder(const std::string& filepath)
    : filepath_(filepath), os_{filepath} {}

    void decode(INode* root) {
        if (root) {
            root->accept(*this);
        }
        os_.flush();
        std::string cmd = "clang-format -i " + filepath_;
        std::system(cmd.c_str());
    }

    void visit(UnaryExpression* node) override {
        if (isPrefix(node->op_)) {
            os_ << toString(node->op_);
        }
        node->expr_->accept(*this);
        if (isPostfix(node->op_)) {
            os_ << toString(node->op_);
        }
    }

    void visit(BinaryExpression* node) override {
        node->left_->accept(*this);
        os_ << toString(node->op_);
        node->right_->accept(*this);
    }

    void visit(TernaryExpression* node) override {
        node->condition_->accept(*this);
        os_ << "?";
        node->onTrue_->accept(*this);
        os_ << ":";
        node->onFalse_->accept(*this);
    }

    void visit(ConstantExpression* node) override {
        os_ << node->value_;
    }

    void visit(VariableExpression* node) override {
        os_ << node->name_;
    }

    void visit(InputExpression* ) override {
        os_ << "?";
    }

    void visit(BlockStatement* node) override {
        for (auto statement: node->statements_) {
            statement->accept(*this);
        }
    }

    void visit(ExpressionStatement* node) override {
        node->expr_->accept(*this);
        os_ << ";";
    }

    void visit(IfStatement* node) override {
        os_ << "if(";
        node->condition_->accept(*this);
        os_ << "){";
        node->trueBlock_->accept(*this);
        os_ << "}";
    }

    void visit(IfElseStatement* node) override {
        os_ << "if(";
        node->condition_->accept(*this);
        os_ << "){";
        node->trueBlock_->accept(*this);
        os_ << "}else{";
        node->falseBlock_->accept(*this);
        os_ << "}";
    }

    void visit(WhileStatement* node) override {
        os_ << "while(";
        node->condition_->accept(*this);
        os_ << "){";
        node->block_->accept(*this);
        os_ << "}";
    }

    void visit(OutputStatement* node) override {
        os_ << "print(";
        node->expr_->accept(*this);
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
