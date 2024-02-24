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
        auto nodeOp = node->getOperator();
        os_ << "(";
        if (isPostfix(nodeOp)) {
            node->getExpr()->accept(*this);
            os_ << toString(nodeOp);
        } else {
            os_ << toString(nodeOp);
            node->getExpr()->accept(*this);
        }
        os_ << ")";
    }

    void visit(BinaryExpression* node) override {
        os_ << "(";
        node->getLeftExpr()->accept(*this);
        os_ << toString(node->getOperator());
        node->getRightExpr()->accept(*this);
        os_ << ")";
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
        os_ << "(std::cin >> " << inputVar_ << " , " << inputVar_ << ")";
    }

    void visit(BlockStatement* node) override {
        auto nodeScope = node->getScope();
        beginScope(nodeScope);
        for (auto&& statement : *node) {
            statement->accept(*this);
        }
        endScope(nodeScope);
    }

    void visit(ExpressionStatement* node) override {
        node->getExpr()->accept(*this);
        os_ << ";";
    }

    void visit(IfStatement* node) override {
        auto nodeScope = node->getScope();
        beginScope(nodeScope);
        os_ << "if(";
        node->getCondExpr()->accept(*this);
        os_ << "){";
        node->getTrueBlock()->accept(*this);
        os_ << "}";
        endScope(nodeScope);
    }

    void visit(IfElseStatement* node) override {
        auto nodeScope = node->getScope();
        beginScope(nodeScope);
        os_ << "if(";
        node->getCondExpr()->accept(*this);
        os_ << "){";
        node->getTrueBlock()->accept(*this);
        os_ << "}else{";
        node->getFalseBlock()->accept(*this);
        os_ << "}";
        endScope(nodeScope);
    }

    void visit(WhileStatement* node) override {
        auto nodeScope = node->getScope();
        beginScope(nodeScope);
        os_ << "while(";
        node->getCondExpr()->accept(*this);
        os_ << "){";
        node->getBlock()->accept(*this);
        os_ << "}";
        endScope(nodeScope);
    }

    void visit(OutputStatement* node) override {
        os_ << "std::cout << ";
        node->getExpr()->accept(*this);
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
