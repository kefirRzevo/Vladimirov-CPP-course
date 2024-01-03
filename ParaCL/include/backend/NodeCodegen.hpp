#pragma once

#include "frontend/NodeVisitor.hpp"

namespace paracl
{

class NodeCodegen final : public NodeVisitor
{
private:

public:
    void visit(UnaryExpression* node) {

    }

    void visit(BinaryExpression* node) {
    }

    void visit(TernaryExpression* node) {
    }

    void visit(ConstantExpression* node) {
    }

    void visit(VariableExpression* node) {
    }

    void visit(InputExpression* node) {
    }

    void visit(BlockStatement* node) {
    }

    void visit(ExpressionStatement* node) {
    }

    void visit(IfStatement* node) {
    }

    void visit(IfElseStatement* node) {
    }

    void visit(WhileStatement* node) {
    }

    void visit(OutputStatement* node) {
    }

    void visit(BreakStatement* node) {
    }

    void visit(ContinueStatement* node) {
    }
};

} // namespace paracl::backend {
