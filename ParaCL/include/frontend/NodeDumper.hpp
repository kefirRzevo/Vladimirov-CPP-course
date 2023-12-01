#pragma once

#include <vector>
#include <iostream>

#include "NodeVisitor.hpp"

namespace paracl
{

class NodeDumper final : public NodeVisitor
{
private:
    using NodeVisitor::postOrderTraversal;
    using NodeVisitor::visit;

    std::ostream& os_;

    void printLink(INode* parent, INode* child) {
        os_ << "\tnode_" << parent << " -> node_" << child << ";\n";
    }

    void printNode(INode* node, const std::string& color, const std::string& label) {
        os_ << "\tnode_" << node << "[fillcolor=" << color << ", label = \"" << node->loc_ << " | " << label << "\"];\n";
    }

    std::string unOper(UnaryOperation op) {
        switch (op) {
        case UnaryOperation::UN_SUB:
            return "-";
        case UnaryOperation::UN_ADD:
            return "+";
        case UnaryOperation::UN_NOT:
            return "!";
        case UnaryOperation::UN_PREFIX_INC:
            return "++^";
        case UnaryOperation::UN_PREFIX_DEC:
            return "--^";
        case UnaryOperation::UN_POSTFIX_INC:
            return "^++";
        case UnaryOperation::UN_POSTFIX_DEC:
            return "^--";
        default:
            return "Unknown";
        }
    }

    std::string binOper(BinaryOperation op) {
        switch (op) {
        case BinaryOperation::BIN_MUL:
            return "*";
        case BinaryOperation::BIN_DIV:
            return "/";
        case BinaryOperation::BIN_MOD:
            return "%";
        case BinaryOperation::BIN_ADD:
            return "+";
        case BinaryOperation::BIN_SUB:
            return "-";
        case BinaryOperation::BIN_L:
            return "\\<";
        case BinaryOperation::BIN_G:
            return "\\>";
        case BinaryOperation::BIN_LE:
            return "\\<=";
        case BinaryOperation::BIN_GE:
            return "\\>=";
        case BinaryOperation::BIN_EQ:
            return "==";
        case BinaryOperation::BIN_NE:
            return "!=";
        case BinaryOperation::BIN_AND:
            return "&&";
        case BinaryOperation::BIN_OR:
            return "||";
        case BinaryOperation::BIN_ASSIGN:
            return "=";
        case BinaryOperation::BIN_COMMA:
            return ",";
        default:
            return "Unknown";
        }
    }

public:
    NodeDumper(std::ostream& os)
    : os_(os) {}

    void dump(INode* root) {
        os_ << "digraph {\n";
        os_ << "\tnode[shape=record, style=filled, fontcolor=black];\n";
        auto nodes = postOrderTraversal(root);
        for (auto node: nodes) {
            visit(node);
        }
        os_ << "}" << std::endl;
    }

protected:
    virtual void visit(UnaryExpression* node) {
        printNode(node, "orange", unOper(node->op_));
        printLink(node, node->expr_);
    }

    virtual void visit(BinaryExpression* node) {
        printNode(node, "orange", binOper(node->op_));
        printLink(node, node->left_);
        printLink(node, node->right_);
    }

    virtual void visit(TernaryExpression* node) {
        printNode(node, "orange", "? :");
        printLink(node, node->condition_);
        printLink(node, node->onTrue_);
        printLink(node, node->onFalse_);
    }

    virtual void visit(ConstantExpression* node) {
        printNode(node, "yellow", std::to_string(node->value_));
    }

    virtual void visit(VariableExpression* node) {
        printNode(node, "pink", node->name_);
    }

    virtual void visit(InputExpression* node) {
        printNode(node, "orange", "Input");
    }

    virtual void visit(BlockStatement* node) {
        printNode(node, "green", "Statement Block");
        for (auto& statement: node->statements_) {
            printLink(node, statement);
        }
    }

    virtual void visit(ExpressionStatement* node) {
        printNode(node, "green", "Expression Statement");
        printLink(node, node->expr_);
    }

    virtual void visit(IfStatement* node) {
        printNode(node, "green", "if");
        printLink(node, node->condition_);
        printLink(node, node->trueBlock_);
    }

    virtual void visit(IfElseStatement* node) {
        printNode(node, "green", "if else");
        printLink(node, node->condition_);
        printLink(node, node->trueBlock_);
        printLink(node, node->falseBlock_);
    }

    virtual void visit(WhileStatement* node) {
        printNode(node, "green", "while");
        printLink(node, node->condition_);
        printLink(node, node->block_);
    }

    virtual void visit(OutputStatement* node) {
        printNode(node, "greenn", "Output");
        printLink(node, node->expr_);
    }

    virtual void visit(BreakStatement* node) {
        printNode(node, "green", "Break");
    }

    virtual void visit(ContinueStatement* node) {
        printNode(node, "green", "Break");
    }
};

} // namespace paracl {
