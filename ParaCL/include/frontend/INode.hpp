#pragma once

#include <string>
#include <vector>
#include <string_view>

#include "location.hpp"
#include "frontend/SymTable.hpp"
#include "frontend/Operators.hpp"
#include "frontend/NodeVisitor.hpp"

namespace paracl
{

#define VISITABLE                               \
void accept(NodeVisitor& visitor) override {    \
    visitor.visit(this);                        \
}

class INode
{
protected:
    location loc_;

public:
    INode(const location &loc) :
        loc_(loc) {}

    const location& getLocation() const {
        return loc_;
    }

    virtual void accept(NodeVisitor& visitor) = 0;

    virtual ~INode() {}
};

class Expression : public INode
{
protected:
    using INode::loc_;

public:
    Expression(const location& loc) :
        INode(loc) {}
};

class UnaryExpression : public Expression
{
protected:
    using Expression::loc_;

    UnaryOperator op_;
    Expression* expr_ = nullptr;

public:
    UnaryExpression(const location& loc, UnaryOperator op, Expression* expr) :
        Expression(loc), op_(op), expr_(expr) {}

    UnaryOperator getOperator() const {
        return op_;
    }

    Expression* getExpr() const {
        return expr_;
    }

    VISITABLE;
};

class BinaryExpression : public Expression
{
protected:
    using Expression::loc_;

    BinaryOperator op_;
    Expression* left_ = nullptr;
    Expression* right_ = nullptr;

public:
    BinaryExpression(const location& loc,
                     BinaryOperator op, Expression* left, Expression* right) :
        Expression(loc), op_(op), left_(left), right_(right) {}

    BinaryOperator getOperator() const {
        return op_;
    }

    Expression* getLeftExpr() const {
        return left_;
    }

    Expression* getRightExpr() const {
        return right_;
    }

    VISITABLE;
};

class TernaryExpression : public Expression
{
protected:
    using Expression::loc_;

    Expression* condition_ = nullptr;
    Expression* onTrue_ = nullptr;
    Expression* onFalse_ = nullptr;

public:
    TernaryExpression(const location& loc, Expression* condition,
                      Expression* onTrue, Expression* onFalse) :
        Expression(loc), condition_(condition),
        onTrue_(onTrue), onFalse_(onFalse) {}

    Expression* getCondExpr() const {
        return condition_;
    }

    Expression* getTrueExpr() const {
        return onTrue_;
    }

    Expression* getFalseExpr() const {
        return onFalse_;
    }

    VISITABLE;
};

class ConstantExpression : public Expression
{
protected:
    using Expression::loc_;

    int value_;

public:
    ConstantExpression(const location& loc, const int& value) :
        Expression(loc), value_(value) {}

    int getValue() const {
        return value_;
    }

    VISITABLE;
};

class VariableExpression : public Expression
{
protected:
    using Expression::loc_;

    std::string name_;

public:
    VariableExpression(const location& loc, std::string_view name) :
        Expression(loc), name_(name) {}

    std::string_view getName() const {
        return name_;
    }

    VISITABLE;
};

class InputExpression : public Expression
{
protected:
    using Expression::loc_;

public:
    InputExpression(const location& loc):
        Expression(loc) {}

    VISITABLE;
};

class Statement : public INode
{
protected:
    using INode::loc_;

public:
    Statement(const location &loc) :
        INode(loc) {}
};

class BlockStatement : public Statement
{
protected:
    using Statement::loc_;

    Scope scope_;
    std::vector<Statement* > statements_;

public:
    BlockStatement(const location& loc) :
        Statement(loc) {}

    Scope& getScope() {
        return scope_;
    }

    void addStatement(Statement* statement) {
        statements_.push_back(statement);
    }

    auto begin() const {
        return statements_.begin();
    }

    auto end() const {
        return statements_.end();
    }

    auto rbegin() const {
        return statements_.rbegin();
    }

    auto rend() const {
        return statements_.rend();
    }

    VISITABLE;
};

class ExpressionStatement : public Statement
{
protected:
    using Statement::loc_;

    Expression* expr_;

public:
    ExpressionStatement(const location& loc, Expression* expr) :
        Statement(loc), expr_(expr) {}

    Expression* getExpr() const {
        return expr_;
    }

    VISITABLE;
};

class IfStatement : public Statement
{
protected:
    using Statement::loc_;

    Scope scope_;
    Expression* condition_ = nullptr;
    Statement* trueBlock_ = nullptr;

public:
    IfStatement(const location& loc,
                Expression* condition, Statement* trueBlock) :
        Statement(loc), condition_(condition), trueBlock_(trueBlock) {}

    Scope& getScope() {
        return scope_;
    }

    Expression* getCondExpr() const {
        return condition_;
    }

    Statement* getTrueBlock() const {
        return trueBlock_;
    }

    VISITABLE;
};

class IfElseStatement : public IfStatement
{
protected:
    using IfStatement::loc_;
    using IfStatement::scope_;
    using IfStatement::condition_;
    using IfStatement::trueBlock_;

    Statement* falseBlock_ = nullptr;

public:
    IfElseStatement(const location& loc, Expression* condition,
                    Statement* trueBlock, Statement* falseBlock) :
        IfStatement(loc, condition, trueBlock), falseBlock_(falseBlock) {}

    Statement* getFalseBlock() const {
        return falseBlock_;
    }

    VISITABLE;
};

class WhileStatement : public Statement
{
protected:
    using Statement::loc_;

    Scope scope_;
    Expression* condition_ = nullptr;
    Statement* block_ = nullptr;

public:
    WhileStatement(const location& loc,
                   Expression* condition, Statement* block) :
        Statement(loc), condition_(condition), block_(block) {}

    Scope& getScope() {
        return scope_;
    }

    Expression* getCondExpr() const {
        return condition_;
    }

    Statement* getBlock() const {
        return block_;
    }

    VISITABLE;
};

class OutputStatement : public Statement
{
protected:
    using Statement::loc_;

    Expression* expr_;

public:
    OutputStatement(const location& loc, Expression* expr) :
        Statement(loc), expr_(expr) {}

    Expression* getExpr() const {
        return expr_;
    }

    VISITABLE;
};

class BreakStatement : public Statement
{
protected:
    using Statement::loc_;

    WhileStatement* loop_ = nullptr;

public:
    BreakStatement(const location& loc) :
        Statement(loc) {}

    WhileStatement* getLoopStat() const {
        return loop_;
    }

    void setLoopStat(WhileStatement* stat) {
        loop_ = stat;
    }

    VISITABLE;
};

class ContinueStatement : public Statement
{
protected:
    using Statement::loc_;

    WhileStatement* loop_ = nullptr;

public:
    ContinueStatement(const location& loc) :
        Statement(loc) {}

    WhileStatement* getLoopStat() const {
        return loop_;
    }

    void setLoopStat(WhileStatement* stat) {
        loop_ = stat;
    }

    VISITABLE;
};

} // namespace paracl
