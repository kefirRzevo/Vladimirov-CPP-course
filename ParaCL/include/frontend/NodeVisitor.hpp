#pragma once

#include <cassert>

namespace paracl
{

struct UnaryExpression;
struct BinaryExpression;
struct TernaryExpression;
struct ConstantExpression;
struct VariableExpression;
struct InputExpression;
struct Statement;
struct BlockStatement;
struct ExpressionStatement;
struct IfStatement;
struct IfElseStatement;
struct WhileStatement;
struct OutputStatement;
struct BreakStatement;
struct ContinueStatement;

class NodeVisitor
{
public:
    virtual void visit(UnaryExpression* node) = 0;

    virtual void visit(BinaryExpression* node) = 0;

    virtual void visit(TernaryExpression* node) = 0;

    virtual void visit(ConstantExpression* node) = 0;

    virtual void visit(VariableExpression* node) = 0;

    virtual void visit(InputExpression* node) = 0;

    virtual void visit(BlockStatement* node) = 0;

    virtual void visit(ExpressionStatement* node) = 0;

    virtual void visit(IfStatement* node) = 0;

    virtual void visit(IfElseStatement* node) = 0;

    virtual void visit(WhileStatement* node) = 0;

    virtual void visit(OutputStatement* node) = 0;

    virtual void visit(BreakStatement* node) = 0;

    virtual void visit(ContinueStatement* node) = 0;
};

} // namespace paracl
