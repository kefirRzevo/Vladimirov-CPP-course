#pragma once

#include <memory>
#include <string>
#include <vector>
#include <fstream>
#include <utility>
#include <iostream>
#include "../../src/location.hpp"
#include "SymTable.hpp"

namespace paracl::frontend
{

struct Node
{
    Location loc_;

    Node(const Location &loc) :
        loc_(loc) {}

    virtual void dump(std::ostream& os) const = 0;

    virtual ~Node() {}
};

struct ErrorNode : public Node
{
    std::string msg_;

    ErrorNode(const std::string& msg, const Location& loc) :
        Node(loc), msg_(msg) {}
};

struct Expression : public Node
{
    using Node::loc_;

    Expression(const Location& loc) :
        Node(loc) {}

    void dump(std::ostream& os) const override;
};

enum class BinaryOperation
{
    BIN_ADD,
    BIN_SUB,
    //    BIN_MUL,
    //    BIN_DIV,
    //    BIN_EQ,
    //    BIN_NE,
};

struct BinaryExpression : public Expression
{
    BinaryOperation op_;
    Expression* left_ = nullptr;
    Expression* right_ = nullptr;

    BinaryExpression(BinaryOperation op, Expression* left, Expression* right, const Location& loc) :
        Expression(loc), op_(op), left_(left), right_(right) {}

    void dump(std::ostream& os) const override;
};

enum class UnaryOperation
{
    UN_NEG,
    UN_POS,
    UN_NOT,
};

struct UnaryExpression : public Expression
{
    UnaryExpression op_;
    Expression* expr_ = nullptr;

    UnaryExpression(UnaryOperation op, Expression* expr, const Location& loc) :
        Expression(loc), op_(op), expr_(expr) {}

    void dump(std::ostream& os) const override;
};

struct ConstantExpression : public Expression
{
    int value_;

    ConstantExpression(const int& value, const Location& loc) :
        Expression(loc), value_(value) {}

    void dump(std::ostream& os) const override;
};

struct VariableExpression : public Expression
{
    std::string name_;

    VariableExpression(const std::string& name, const Location& loc) :
        Expression(loc), name_(name) {}

    void dump(std::ostream& os) const override;
};

struct InputExpression : public Expression
{
    InputExpression(const Location& loc):
        Expression(loc) {}

    void dump(std::ostream& os) const override;
};

struct Statement : public Node
{
    using Node::loc_;

    Statement(const Location &loc) :
        Node(loc) {}

    void dump(std::ostream& os) const override;  
};

struct BlockStatement : public Statement
{
    SymTable table_;
    std::vector<Statement* > statements_;

    BlockStatement(const Location& loc) :
        Statement(loc) {}

    void dump(std::ostream& os) const override;
};

struct AssignmentStatement : public Statement
{
    VariableExpression* left_;
    Expression* right_;

    AssignmentStatement(VariableExpression* left, Expression* right, const Location& loc) :
        Statement(loc), left_(left), right_(right) {}

    void dump(std::ostream& os) const override;
};

struct IfStatement : public Statement
{
    Expression* condition_ = nullptr;
    BlockStatement* trueBlock_ = nullptr;
    BlockStatement* falseBlock_ = nullptr;

    IfStatement(Expression* condition, BlockStatement* trueBlock, const Location& loc) :
        Statement(loc), condition_(condition), trueBlock_(trueBlock) {}

    IfStatement(Expression* condition, BlockStatement* trueBlock, BlockStatement* falseBlock, const Location& loc) :
        Statement(loc), condition_(condition), trueBlock_(trueBlock), falseBlock_(falseBlock) {}

    void dump(std::ostream& os) const override;
};

struct WhileStatement : public Statement
{
    Expression* condition_ = nullptr;
    BlockStatement* block_ = nullptr;

    WhileStatement(Expression* condition, BlockStatement* block, const Location& loc) :
        Statement(loc), condition_(condition), block_(block) {}

    void dump(std::ostream& os) const override;
};

struct OutputStatement : public Statement
{
    Expression* expr_;

    OutputStatement(Expression* expr, const Location& loc) :
        Statement(loc), expr_(expr) {}

    void dump(std::ostream& os) const override;
};

class Tree final
{
private:
    std::vector<std::unique_ptr<Node>> nodes_;
    Node* root_ = nullptr;

public:
    Tree() = default;
    Tree(const Tree&) = delete;
    Tree& operator=(const Tree&) = delete;
    Tree(Tree&&) = default;
    Tree& operator=(Tree&&) = default;
    ~Tree() = default;

    template<typename NodeType, typename... NodeArgs>
    NodeType* createNode(NodeArgs&&... args) {
        auto uptr = std::make_unique<NodeType>(std::forward<NodeArgs>(args));
        auto ptr = uptr.get();
        nodes_.push_back(std::move(uptr));
        return ptr;
    }

    void setRoot(Node* root) {
        root_ = root;
    }

    const Node* getRoot() const {
        return root_;
    }

    void dump(std::ostream& os) const {
        os << "digraph {";
        root_->dump();
        os << "}" << std::endl;
    }
};

} // namespace paracl::frontend {
