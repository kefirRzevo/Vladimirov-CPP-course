#pragma once

namespace paracl
{

class UnaryExpression;
class BinaryExpression;
class TernaryExpression;
class ConstantExpression;
class VariableExpression;
class InputExpression;
class Statement;
class BlockStatement;
class ExpressionStatement;
class IfStatement;
class IfElseStatement;
class WhileStatement;
class OutputStatement;
class BreakStatement;
class ContinueStatement;

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

    virtual ~NodeVisitor() = default;
};

} // namespace paracl
