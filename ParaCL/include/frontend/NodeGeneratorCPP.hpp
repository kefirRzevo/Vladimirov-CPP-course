#pragma once

#include <string>
#include <random>
#include <ostream>
#include <iostream>
#include <algorithm>

#include "frontend/INode.hpp"

namespace paracl
{

class NodeGeneratorCpp final : public NodeVisitor
{
private:
    std::string filepath_;
    std::string inputVar_;
    std::ofstream os_;

    void beginScope(const Scope& scope) {
        auto empty = scope.empty();
        if (!empty) {
            os_ << "{\n";
            for (auto& var: scope) {
                os_ << "int " << var.first << ";\n";
            }
        }
    }

    void endScope(const Scope& scope) {
        if (!scope.empty()) {
            os_ << "}\n";
        }
    }

    void generateRandomInputVar() {
        inputVar_ = "temp";
        inputVar_ += std::to_string(std::rand() % 1000 + 500);
    }

public:
    NodeGeneratorCpp(const std::string& filepath)
    : filepath_(filepath), os_{filepath} {}

    void generate(INode* root) {
        generateRandomInputVar();
        os_ << "#include <iostream>\n\n";
        os_ << "int main()\n{\n";
        os_ << "int " << inputVar_ << ";\n";
        if (root) {
            root->accept(*this);
        }
        os_ << "return 0;\n}\n"; 
        os_.flush();
        std::string cmd = "clang-format -i " + filepath_;
        std::system(cmd.c_str());
    }

    void visit(UnaryExpression* node) override {
        os_ << "(";
        if (isPostfix(node->op_)) {
            node->expr_->accept(*this);
            os_ << toString(node->op_);
        } else {
            os_ << toString(node->op_);
            node->expr_->accept(*this);
        }
        os_ << ")";
    }

    void visit(BinaryExpression* node) override {
        os_ << "(";
        node->left_->accept(*this);
        os_ << toString(node->op_);
        node->right_->accept(*this);
        os_ << ")";
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
        os_ << "(std::cin >> " << inputVar_ << " , " << inputVar_ << ")";
    }

    void visit(BlockStatement* node) override {
        beginScope(node->scope_);
        for (auto statement: node->statements_) {
            statement->accept(*this);
        }
        endScope(node->scope_);
    }

    void visit(ExpressionStatement* node) override {
        node->expr_->accept(*this);
        os_ << ";";
    }

    void visit(IfStatement* node) override {
        beginScope(node->scope_);
        os_ << "if(";
        node->condition_->accept(*this);
        os_ << "){";
        node->trueBlock_->accept(*this);
        os_ << "}";
        endScope(node->scope_);
    }

    void visit(IfElseStatement* node) override {
        beginScope(node->scope_);
        os_ << "if(";
        node->condition_->accept(*this);
        os_ << "){";
        node->trueBlock_->accept(*this);
        os_ << "}else{";
        node->falseBlock_->accept(*this);
        os_ << "}";
        endScope(node->scope_);
    }

    void visit(WhileStatement* node) override {
        beginScope(node->scope_);
        os_ << "while(";
        node->condition_->accept(*this);
        os_ << "){";
        node->block_->accept(*this);
        os_ << "}";
        endScope(node->scope_);
    }

    void visit(OutputStatement* node) override {
        os_ << "std::cout << ";
        node->expr_->accept(*this);
        os_ << "<< std::endl;\n";
    }

    void visit(BreakStatement* ) override {
        os_ << "break;";
    }

    void visit(ContinueStatement* ) override {
        os_ << "continue;";
    }
};

} // namespace paracl
