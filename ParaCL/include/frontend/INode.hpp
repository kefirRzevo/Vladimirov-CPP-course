#pragma once

#include <deque>
#include <memory>
#include <string>
#include <vector>
#include <fstream>
#include <utility>
#include <iostream>

#include "location.hpp"
#include "SymTable.hpp"

namespace paracl
{

class NodeVisitor;
class SymTable;

struct INode
{
    location loc_;

    INode(const location &loc) :
        loc_(loc) {}

    virtual void accept(NodeVisitor& visitor);

    virtual ~INode() {}
};

struct Expression : public INode
{
    using INode::loc_;

    Expression(const location& loc) :
        INode(loc) {}
};

enum class UnaryOperation
{
    UN_SUB,
    UN_ADD,
    UN_NOT,

    UN_PREFIX_INC,
    UN_PREFIX_DEC,
    UN_POSTFIX_INC,
    UN_POSTFIX_DEC,
};

struct UnaryExpression : public Expression
{
    using Expression::loc_;

    UnaryOperation op_;
    Expression* expr_ = nullptr;

    UnaryExpression(const location& loc, UnaryOperation op, Expression* expr) :
        Expression(loc), op_(op), expr_(expr) {}
};

enum class BinaryOperation
{
    BIN_MUL,
    BIN_DIV,
    BIN_MOD,

    BIN_ADD,
    BIN_SUB,

    BIN_L,
    BIN_G,
    BIN_LE,
    BIN_GE,

    BIN_EQ,
    BIN_NE,
    BIN_AND,
    BIN_OR,

    BIN_ASSIGN,
    BIN_COMMA,
};

struct BinaryExpression : public Expression
{
    using Expression::loc_;

    BinaryOperation op_;
    Expression* left_ = nullptr;
    Expression* right_ = nullptr;

    BinaryExpression(const location& loc, BinaryOperation op, Expression* left, Expression* right) :
        Expression(loc), op_(op), left_(left), right_(right) {}
};

struct TernaryExpression : public Expression
{
    using Expression::loc_;

    Expression* condition_ = nullptr;
    Expression* onTrue_ = nullptr;
    Expression* onFalse_ = nullptr;

    TernaryExpression(const location& loc, Expression* condition, Expression* onTrue, Expression* onFalse) :
        Expression(loc), condition_(condition), onTrue_(onTrue), onFalse_(onFalse) {}
};

struct ConstantExpression : public Expression
{
    using Expression::loc_;

    int value_;

    ConstantExpression(const location& loc, const int& value) :
        Expression(loc), value_(value) {}
};

struct VariableExpression : public Expression
{
    using Expression::loc_;

    std::string name_;

    VariableExpression(const location& loc, const std::string& name) :
        Expression(loc), name_(name) {}
};

struct InputExpression : public Expression
{
    using Expression::loc_;

    InputExpression(const location& loc):
        Expression(loc) {}
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
    std::deque<Statement* > statements_;

    BlockStatement(const location& loc) :
        Statement(loc) {}
};

struct ExpressionStatement : public Statement
{
    using Statement::loc_;

    Expression* expr_;

    ExpressionStatement(const location& loc, Expression* expr) :
        Statement(loc), expr_(expr) {}
};

struct IfStatement : public Statement
{
    using Statement::loc_;

    Expression* condition_ = nullptr;
    Statement* trueBlock_ = nullptr;

    IfStatement(const location& loc, Expression* condition, Statement* trueBlock) :
        Statement(loc), condition_(condition), trueBlock_(trueBlock) {}
};

struct IfElseStatement : public IfStatement
{
    using IfStatement::loc_;
    using IfStatement::condition_;
    using IfStatement::trueBlock_;

    Statement* falseBlock_ = nullptr;

    IfElseStatement(const location& loc, Expression* condition, Statement* trueBlock, Statement* falseBlock) :
        IfStatement(loc, condition, trueBlock), falseBlock_(falseBlock) {}
};

struct WhileStatement : public Statement
{
    using Statement::loc_;

    Expression* condition_ = nullptr;
    Statement* block_ = nullptr;

    WhileStatement(const location& loc, Expression* condition, Statement* block) :
        Statement(loc), condition_(condition), block_(block) {}
};

struct OutputStatement : public Statement
{
    using Statement::loc_;

    Expression* expr_;

    OutputStatement(const location& loc, Expression* expr) :
        Statement(loc), expr_(expr) {}
};

struct BreakStatement : public Statement
{
    using Statement::loc_;

    WhileStatement* whileStat = nullptr;

    BreakStatement(const location& loc) :
        Statement(loc) {}
};

struct ContinueStatement : public Statement
{
    using Statement::loc_;

    WhileStatement* whileStat = nullptr;

    ContinueStatement(const location& loc) :
        Statement(loc) {}
};

} // namespace paracl
