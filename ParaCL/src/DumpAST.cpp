#include "../include/frontend/AST.hpp"

namespace paracl::frontend
{

void ErrorNode::dump(std::ostream& os) const {
    os << "\tnode[shape=triangle, style=filled, fontcolor=black, fillcolor=red];\n";
    os << "\tnode_" << this << "[label = \"Error\"];\n";
}

void Expression::dump(std::ostream& os) const {
    os << "\tnode[shape=triangle, style=filled, fontcolor=black, fillcolor=orange];\n";
}

void BinaryExpression::dump(std::ostream& os) const {
    Expression::dump(os);
    std::string string;
    switch (op_) {
    case BIN_ADD:
        string = "+";
        break;
    case BIN_SUB:
        string = "-";
        break;
    default:
        string = "Unknown";
        break;
    }
    os << "\tnode_" << this << "[label = \"" << string << "\"];\n";
    os << "\tnode_" << this << " -> node_" << left_ << ";\n";
    assert(left_);
    left_->dump(os);
    assert(right_);
    os << "\tnode_" << this << " -> node_" << right_ << ";\n";
    right_->dump(os);
}

void UnaryExpression::dump(std::ostream& os) const {
    Expression::dump(os);
    std::string string;
    switch (op_)
    {
    case UN_NEG:
        string = "-";
        break;
    case UN_POS:
        string = "+";
        break;
    case UN_NOT:
        string = "!";
        break;
    default:
        string = "Unknown";
        break;
    }
    os << "\tnode_" << this << "[label = \"" << string << "\"];\n";
    os << "\tnode_" << this << " -> node_" << expr_ << ";\n";
    assert(expr_);
    expr_->dump(os);
}

void ConstantExpression::dump(std::ostream& os) const {
    os << "\tnode_" << this << "[label = \"" << value_ << "\", fillcolor=yellow];\n";
}

void VariableExpression::dump(std::ostream& os) const {
    os << "\tnode_" << this << "[label = \"" << name_ << "\", fillcolor=pink];\n";
}

void InputExpression::dump(std::ostream& os) const {
    os << "\tnode_" << this << "[label = \"?\", fillcolor=lightbrown];\n";
}

void Statement::dump(std::ostream& os) const {
    os << "\tnode[shape=oval, style=filled, fontcolor=black, fillcolor=lightgreen];\n";
}

void BlockStatement::dump(std::ostream& os) const {
    Statement::dump(os);
    os << "\tnode_" << this << "[label = \"Block\"];\n";
    for (const auto& statement: statements_) {
        assert(statement);
        os << "\tnode_" << this << " -> node_" << statement << ";\n";
        statements->dump(os);
    }
}

void AssignmentStatement::dump(std::ostream& os) const {
    Statement::dump(os);
    os << "\tnode_" << this << "[label = \"=\"];\n";
    os << "\tnode_" << this << " -> node_" << left_ << ";\n";
    assert(left_);
    left_->dump(os);
    os << "\tnode_" << this << " -> node_" << right_ << ";\n";
    assert(right_);
    right_->dump(os);
}

void IfStatement::dump(std::ostream& os) const {
    Statement::dump(os);
    os << "\tnode_" << this << "[label = \"if\"];\n";
    os << "\tnode_" << this << " -> node_" << condition_ << ";\n";
    assert(condition_);
    condition_->dump(os);
    os << "\tnode_" << this << " -> node_" << trueBlock_ << "[label = \"true\"];\n";
    assert(trueBlock_);
    trueBlock_->dump(os);
    if (falseBlock_) {
        os << "\tnode_" << this << " -> node_" << falseBlock_ << "[label = \"false\"];\n";
        falseBlock_->dump(os);
    }
}

void WhileStatement::dump(std::ostream& os) const {
    Statement::dump(os);
    os << "\tnode_" << this << "[label = \"while\"];\n";
    os << "\tnode_" << this << " -> node_" << condition_ << ";\n";
    assert(condition_);
    condition_->dump(os);
    os << "\tnode_" << this << " -> node_" << block_ << ";\n";
    assert(block_);
    block_->dump(os);
}

void OutputStatement::dump(std::ostream& os) const {
    Statement::dump(os);
    os << "\tnode_" << this << "[label = \"print\", fillcolor=lightbrown];\n";
}

} // namespace paracl::frontend {
