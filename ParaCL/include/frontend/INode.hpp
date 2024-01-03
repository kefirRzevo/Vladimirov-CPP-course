#pragma once

#include <memory>
#include <string>
#include <vector>
#include <fstream>
#include <utility>
#include <iostream>

#include "location.hpp"
#include "SymTable.hpp"
#include "Operators.hpp"

namespace paracl
{

class NodeVisitor;
class SymTable;

#define VISITABLE                               \
void accept(NodeVisitor& visitor) override {    \
    visitor.visit(this);                        \
}

struct INode
{
    location loc_;

    INode(const location &loc) :
        loc_(loc) {}

    virtual void accept(NodeVisitor& visitor) = 0;

    virtual ~INode() {}
};

struct Expression;
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

} // namespace paracl

#include "NodeVisitor.hpp"

namespace paracl
{

struct Expression : public INode
{
    using INode::loc_;

    Expression(const location& loc) :
        INode(loc) {}
};

struct UnaryExpression : public Expression
{
    using Expression::loc_;

    UnaryOperator op_;
    Expression* expr_ = nullptr;

    UnaryExpression(const location& loc, UnaryOperator op, Expression* expr) :
        Expression(loc), op_(op), expr_(expr) {}

    VISITABLE;
};

struct BinaryExpression : public Expression
{
    using Expression::loc_;

    BinaryOperator op_;
    Expression* left_ = nullptr;
    Expression* right_ = nullptr;

    BinaryExpression(const location& loc, BinaryOperator op, Expression* left, Expression* right) :
        Expression(loc), op_(op), left_(left), right_(right) {}

    VISITABLE;
};

struct TernaryExpression : public Expression
{
    using Expression::loc_;

    Expression* condition_ = nullptr;
    Expression* onTrue_ = nullptr;
    Expression* onFalse_ = nullptr;

    TernaryExpression(const location& loc, Expression* condition, Expression* onTrue, Expression* onFalse) :
        Expression(loc), condition_(condition), onTrue_(onTrue), onFalse_(onFalse) {}

    VISITABLE;
};

struct ConstantExpression : public Expression
{
    using Expression::loc_;

    int value_;

    ConstantExpression(const location& loc, const int& value) :
        Expression(loc), value_(value) {}

    VISITABLE;
};

struct VariableExpression : public Expression
{
    using Expression::loc_;

    std::string name_;

    VariableExpression(const location& loc, const std::string& name) :
        Expression(loc), name_(name) {}

    VISITABLE;
};

struct InputExpression : public Expression
{
    using Expression::loc_;

    InputExpression(const location& loc):
        Expression(loc) {}

    VISITABLE;
};

struct Statement : public INode
{
    using INode::loc_;

    Statement(const location &loc) :
        INode(loc) {}
};

struct BlockStatement : public Statement
{
    using Statement::loc_;

    SymTable table_;
    std::vector<Statement* > statements_;

    BlockStatement(const location& loc) :
        Statement(loc) {}

    VISITABLE;
};

struct ExpressionStatement : public Statement
{
    using Statement::loc_;

    Expression* expr_;

    ExpressionStatement(const location& loc, Expression* expr) :
        Statement(loc), expr_(expr) {}

    VISITABLE;
};

struct IfStatement : public Statement
{
    using Statement::loc_;

    Expression* condition_ = nullptr;
    Statement* trueBlock_ = nullptr;

    IfStatement(const location& loc, Expression* condition, Statement* trueBlock) :
        Statement(loc), condition_(condition), trueBlock_(trueBlock) {}

    VISITABLE;
};

struct IfElseStatement : public IfStatement
{
    using IfStatement::loc_;
    using IfStatement::condition_;
    using IfStatement::trueBlock_;

    Statement* falseBlock_ = nullptr;

    IfElseStatement(const location& loc, Expression* condition, Statement* trueBlock, Statement* falseBlock) :
        IfStatement(loc, condition, trueBlock), falseBlock_(falseBlock) {}

    VISITABLE;
};

struct WhileStatement : public Statement
{
    using Statement::loc_;

    Expression* condition_ = nullptr;
    Statement* block_ = nullptr;

    WhileStatement(const location& loc, Expression* condition, Statement* block) :
        Statement(loc), condition_(condition), block_(block) {}

    VISITABLE;
};

struct OutputStatement : public Statement
{
    using Statement::loc_;

    Expression* expr_;

    OutputStatement(const location& loc, Expression* expr) :
        Statement(loc), expr_(expr) {}

    VISITABLE;
};

struct BreakStatement : public Statement
{
    using Statement::loc_;

    WhileStatement* whileStat = nullptr;

    BreakStatement(const location& loc) :
        Statement(loc) {}

    VISITABLE;
};

struct ContinueStatement : public Statement
{
    using Statement::loc_;

    WhileStatement* whileStat = nullptr;

    ContinueStatement(const location& loc) :
        Statement(loc) {}

    VISITABLE;
};

} // namespace paracl
