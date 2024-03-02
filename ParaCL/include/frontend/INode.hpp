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

template<typename Node>
class VisitableExpression : public Expression
{
protected:
    using Expression::loc_;

public:
    VisitableExpression(const location& loc) :
        Expression(loc) {}

    void accept(NodeVisitor& visitor) override {
        visitor.visit(static_cast<Node*>(this));
    }
};

class UnaryExpression : public VisitableExpression<UnaryExpression>
{
protected:
    using VisExpr = VisitableExpression<UnaryExpression>;
    using VisExpr::loc_;

    UnaryOperator op_;
    Expression* expr_ = nullptr;

public:
    UnaryExpression(const location& loc, UnaryOperator op, Expression* expr) :
        VisExpr(loc), op_(op), expr_(expr) {}

    UnaryOperator getOperator() const {
        return op_;
    }

    Expression* getExpr() const {
        return expr_;
    }
};

class BinaryExpression : public VisitableExpression<BinaryExpression>
{
protected:
    using VisExpr = VisitableExpression<BinaryExpression>;
    using VisExpr::loc_;

    BinaryOperator op_;
    Expression* left_ = nullptr;
    Expression* right_ = nullptr;

public:
    BinaryExpression(const location& loc,
                     BinaryOperator op, Expression* left, Expression* right) :
        VisExpr(loc), op_(op), left_(left), right_(right) {}

    BinaryOperator getOperator() const {
        return op_;
    }

    Expression* getLeftExpr() const {
        return left_;
    }

    Expression* getRightExpr() const {
        return right_;
    }
};

class TernaryExpression : public VisitableExpression<TernaryExpression>
{
protected:
    using VisExpr = VisitableExpression<TernaryExpression>;
    using VisExpr::loc_;

    Expression* condition_ = nullptr;
    Expression* onTrue_ = nullptr;
    Expression* onFalse_ = nullptr;

public:
    TernaryExpression(const location& loc, Expression* condition,
                      Expression* onTrue, Expression* onFalse) :
        VisExpr(loc), condition_(condition),
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
};

class ConstantExpression : public VisitableExpression<ConstantExpression>
{
protected:
    using VisExpr = VisitableExpression<ConstantExpression>;
    using VisExpr::loc_;

    int value_;

public:
    ConstantExpression(const location& loc, const int& value) :
        VisExpr(loc), value_(value) {}

    int getValue() const {
        return value_;
    }
};

class VariableExpression : public VisitableExpression<VariableExpression>
{
protected:
    using VisExpr = VisitableExpression<VariableExpression>;
    using VisExpr::loc_;

    std::string name_;

public:
    VariableExpression(const location& loc, std::string_view name) :
        VisExpr(loc), name_(name) {}

    std::string_view getName() const {
        return name_;
    }
};

class InputExpression : public VisitableExpression<InputExpression>
{
protected:
    using VisExpr = VisitableExpression<InputExpression>;
    using VisExpr::loc_;

public:
    InputExpression(const location& loc):
        VisExpr(loc) {}
};

class Statement : public INode
{
protected:
    using INode::loc_;

public:
    Statement(const location &loc) :
        INode(loc) {}
};

template<typename Node>
class VisitableStatement : public Statement
{
protected:
    using Statement::loc_;

public:
    VisitableStatement(const location& loc) :
        Statement(loc) {}

    void accept(NodeVisitor& visitor) override {
        visitor.visit(static_cast<Node*>(this));
    }
};

class BlockStatement : public VisitableStatement<BlockStatement>
{
protected:
    using VisStat = VisitableStatement<BlockStatement>;
    using VisStat::loc_;

    Scope scope_;
    std::vector<Statement* > statements_;

public:
    BlockStatement(const location& loc) :
        VisStat(loc) {}

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
};

class ExpressionStatement : public VisitableStatement<ExpressionStatement>
{
protected:
    using VisStat = VisitableStatement<ExpressionStatement>;
    using VisStat::loc_;

    Expression* expr_;

public:
    ExpressionStatement(const location& loc, Expression* expr) :
        VisStat(loc), expr_(expr) {}

    Expression* getExpr() const {
        return expr_;
    }
};

class IfStatement : public VisitableStatement<IfStatement>
{
protected:
    using VisStat = VisitableStatement<IfStatement>;
    using VisStat::loc_;

    Scope scope_;
    Expression* condition_ = nullptr;
    Statement* trueBlock_ = nullptr;

public:
    IfStatement(const location& loc,
                Expression* condition, Statement* trueBlock) :
        VisStat(loc), condition_(condition), trueBlock_(trueBlock) {}

    Scope& getScope() {
        return scope_;
    }

    Expression* getCondExpr() const {
        return condition_;
    }

    Statement* getTrueBlock() const {
        return trueBlock_;
    }
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
};

class WhileStatement : public VisitableStatement<WhileStatement>
{
protected:
    using VisStat = VisitableStatement<WhileStatement>;
    using VisStat::loc_;

    Scope scope_;
    Expression* condition_ = nullptr;
    Statement* block_ = nullptr;

public:
    WhileStatement(const location& loc,
                   Expression* condition, Statement* block) :
        VisStat(loc), condition_(condition), block_(block) {}

    Scope& getScope() {
        return scope_;
    }

    Expression* getCondExpr() const {
        return condition_;
    }

    Statement* getBlock() const {
        return block_;
    }
};

class OutputStatement : public VisitableStatement<OutputStatement>
{
protected:
    using VisStat = VisitableStatement<OutputStatement>;
    using VisStat::loc_;

    Expression* expr_;

public:
    OutputStatement(const location& loc, Expression* expr) :
        VisStat(loc), expr_(expr) {}

    Expression* getExpr() const {
        return expr_;
    }
};

class BreakStatement : public VisitableStatement<BreakStatement>
{
protected:
    using VisStat = VisitableStatement<BreakStatement>;
    using VisStat::loc_;

    WhileStatement* loop_ = nullptr;

public:
    BreakStatement(const location& loc) :
        VisStat(loc) {}

    WhileStatement* getLoopStat() const {
        return loop_;
    }

    void setLoopStat(WhileStatement* stat) {
        loop_ = stat;
    }
};

class ContinueStatement : public VisitableStatement<ContinueStatement>
{
protected:
    using VisStat = VisitableStatement<ContinueStatement>;
    using VisStat::loc_;

    WhileStatement* loop_ = nullptr;

public:
    ContinueStatement(const location& loc) :
        VisStat(loc) {}

    WhileStatement* getLoopStat() const {
        return loop_;
    }

    void setLoopStat(WhileStatement* stat) {
        loop_ = stat;
    }
};

} // namespace paracl
