#pragma once

#include <vector>
#include <iostream>

#include "NodeVisitor.hpp"

namespace paracl
{

class NodeDumper final : public NodeVisitor
{
private:
    std::ostream& os_;

    void printLink(INode* parent, INode* child) {
        os_ << "\tnode_" << parent << " -> node_" << child << ";\n";
    }

    void printNode(INode* node, const std::string& color, const std::string& label) {
        os_ << "\tnode_" << node << "[fillcolor=" << color << ", label = \"" << node->loc_ << " | " << label << "\"];\n";
    }

public:
    NodeDumper(std::ostream& os)
    : os_(os) {}

    void dump(INode* root) {
        os_ << "digraph {\n";
        os_ << "\tnode[shape=record, style=filled, fontcolor=black];\n";
        if (root) {
            root->accept(*this);
        }
        os_ << "}" << std::endl;
    }

    void visit(UnaryExpression* node) override {
        std::string res;
        if (isPrefix(node->op_)) {
            res = "^";
        }
        res += toString(node->op_);
        if (isPostfix(node->op_)) {
            res += "^";
        }
        printNode(node, "orange", res);
        printLink(node, node->expr_);
        node->expr_->accept(*this);
    }

    void visit(BinaryExpression* node) override {
        printNode(node, "orange", toString(node->op_));
        printLink(node, node->left_);
        printLink(node, node->right_);
        node->left_->accept(*this);
        node->right_->accept(*this);
    }

    void visit(TernaryExpression* node) override {
        printNode(node, "orange", "? :");
        printLink(node, node->condition_);
        printLink(node, node->onTrue_);
        printLink(node, node->onFalse_);
        node->condition_->accept(*this);
        node->onTrue_->accept(*this);
        node->onFalse_->accept(*this);
    }

    void visit(ConstantExpression* node) override {
        printNode(node, "yellow", std::to_string(node->value_));
    }

    void visit(VariableExpression* node) override {
        printNode(node, "pink", node->name_);
    }

    void visit(InputExpression* node) override {
        printNode(node, "orange", "input");
    }

    void visit(BlockStatement* node) override {
        printNode(node, "green", "Statement Block");
        for (auto statement: node->statements_) {
            printLink(node, statement);
            statement->accept(*this);
        }
    }

    void visit(ExpressionStatement* node) override {
        printNode(node, "green", "Expression Statement");
        printLink(node, node->expr_);
        node->expr_->accept(*this);
    }

    void visit(IfStatement* node) override {
        printNode(node, "green", "if");
        printLink(node, node->condition_);
        printLink(node, node->trueBlock_);
        node->condition_->accept(*this);
        node->trueBlock_->accept(*this);
    }

    void visit(IfElseStatement* node) override {
        printNode(node, "green", "if else");
        printLink(node, node->condition_);
        printLink(node, node->trueBlock_);
        printLink(node, node->falseBlock_);
        node->condition_->accept(*this);
        node->trueBlock_->accept(*this);
        node->falseBlock_->accept(*this);
    }

    void visit(WhileStatement* node) override {
        printNode(node, "green", "while");
        printLink(node, node->condition_);
        printLink(node, node->block_);
        node->condition_->accept(*this);
        node->block_->accept(*this);
    }

    void visit(OutputStatement* node) override {
        printNode(node, "green", "output");
        printLink(node, node->expr_);
        node->expr_->accept(*this);
    }

    void visit(BreakStatement* node) override {
        printNode(node, "green", "break");
    }

    void visit(ContinueStatement* node) override {
        printNode(node, "green", "continue");
    }
};

} // namespace paracl {
