#pragma once

#include <vector>
#include <iostream>

#include "frontend/INode.hpp"

namespace paracl
{

class NodeGeneratorDot final : public NodeVisitor
{
private:
    std::string filepath_;
    std::ofstream os_;

    void printLink(INode* parent, INode* child) {
        os_ << "\tnode_" << parent << " -> node_" << child << ";\n";
    }

    void printNode(INode* node, std::string_view col, std::string_view label) {
        auto nodeLoc = node->getLocation();
        os_ << "\tnode_" << node;
        os_ <<"[fillcolor=" << col << ", label = \"";
        os_ << nodeLoc << " | " << label << "\"];\n";
    }

public:
    NodeGeneratorDot(const std::string& filepath)
    : os_(filepath) {}

    void generate(INode* root) {
        os_ << "digraph {\n";
        os_ << "\tnode[shape=record, style=filled, fontcolor=black];\n";
        if (root) {
            root->accept(*this);
        }
        os_ << "}" << std::endl;
        os_.flush();
    }

    void visit(UnaryExpression* node) override {
        std::string res;
        auto nodeOp = node->getOperator();
        auto nodeExpr = node->getExpr();
        if (isPrefix(nodeOp)) {
            res = "\\^";
        }
        res += "\\" + toString(nodeOp);
        if (isPostfix(nodeOp)) {
            res += "\\^";
        }
        printNode(node, "orange", res);
        printLink(node, nodeExpr);
        nodeExpr->accept(*this);
    }

    void visit(BinaryExpression* node) override {
        auto nodeOp = node->getOperator();
        auto nodeLeft = node->getLeftExpr();
        auto nodeRight = node->getRightExpr();
        printNode(node, "orange", "\\" + toString(nodeOp));
        printLink(node, nodeLeft);
        printLink(node, nodeRight);
        nodeLeft->accept(*this);
        nodeRight->accept(*this);
    }

    void visit(TernaryExpression* node) override {
        auto nodeCond = node->getCondExpr();
        auto nodeTrue = node->getTrueExpr();
        auto nodeFalse = node->getFalseExpr();
        printNode(node, "orange", "\\? \\:");
        printLink(node, nodeCond);
        printLink(node, nodeTrue);
        printLink(node, nodeFalse);
        nodeCond->accept(*this);
        nodeTrue->accept(*this);
        nodeFalse->accept(*this);
    }

    void visit(ConstantExpression* node) override {
        auto nodeVal = node->getValue();
        printNode(node, "yellow", std::to_string(nodeVal));
    }

    void visit(VariableExpression* node) override {
        auto nodeName = node->getName();
        printNode(node, "pink", nodeName);
    }

    void visit(InputExpression* node) override {
        printNode(node, "orange", "input");
    }

    void visit(BlockStatement* node) override {
        printNode(node, "green", "Statement Block");
        for (auto&& statement : *node) {
            printLink(node, statement);
            statement->accept(*this);
        }
    }

    void visit(ExpressionStatement* node) override {
        auto nodeExpr = node->getExpr();
        printNode(node, "green", "Expression Statement");
        printLink(node, nodeExpr);
        nodeExpr->accept(*this);
    }

    void visit(IfStatement* node) override {
        auto nodeCond = node->getCondExpr();
        auto nodeTrue = node->getTrueBlock();
        printNode(node, "green", "if");
        printLink(node, nodeCond);
        printLink(node, nodeTrue);
        nodeCond->accept(*this);
        nodeTrue->accept(*this);
    }

    void visit(IfElseStatement* node) override {
        auto nodeCond = node->getCondExpr();
        auto nodeTrue = node->getTrueBlock();
        auto nodeFalse = node->getFalseBlock();
        printNode(node, "green", "if else");
        printLink(node, nodeCond);
        printLink(node, nodeTrue);
        printLink(node, nodeFalse);
        nodeCond->accept(*this);
        nodeTrue->accept(*this);
        nodeFalse->accept(*this);
    }

    void visit(WhileStatement* node) override {
        auto nodeCond = node->getCondExpr();
        auto nodeBlock = node->getBlock();
        printNode(node, "green", "while");
        printLink(node, nodeCond);
        printLink(node, nodeBlock);
        nodeCond->accept(*this);
        nodeBlock->accept(*this);
    }

    void visit(OutputStatement* node) override {
        auto nodeExpr = node->getExpr();
        printNode(node, "green", "output");
        printLink(node, nodeExpr);
        nodeExpr->accept(*this);
    }

    void visit(BreakStatement* node) override {
        printNode(node, "green", "break");
    }

    void visit(ContinueStatement* node) override {
        printNode(node, "green", "continue");
    }
};

} // namespace paracl {
