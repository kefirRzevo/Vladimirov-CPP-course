#pragma once

#include "frontend/SymTable.hpp"
#include "frontend/NodeVisitor.hpp"
#include "backend/VirtualMachine.hpp"

namespace paracl
{

class NodeCodegen final : public NodeVisitor
{
private:
    using NodeVisitor::visit;


    Image im;

    std::vector<UnaryExpression*> postfixes_;

    AddrBlock globals_;
    AddrBlockStack scopes_;

    void codegenPostfixes() {
        if (postfixes_.empty()) {
            return;
        }
        for (auto& node: postfixes_) {
            auto var = static_cast<VariableExpression*>(node->expr_);
            node->expr_->accept(*this);
            switch (node->op_) {
            case UnaryOperator::UN_POSTFIX_INC:
                im.addInstr<iPushVal>(1);
                im.addInstr<iAdd>();
                break;
            case UnaryOperator::UN_POSTFIX_DEC:
                im.addInstr<iPushVal>(1);
                im.addInstr<iSub>();
                break;
            default:
                throw std::logic_error("Operator is not postfix");
            }
            auto addr = scopes_.lookupVar(var->name_);
            if (!addr) {
                throw std::logic_error("Variable not found");
            }
            im.addInstr<iPopAddr>(addr.value());
        }
        postfixes_.clear();
    }

    void codegenPrefix(UnaryExpression* node) {
        auto var = static_cast<VariableExpression*>(node->expr_);
        node->expr_->accept(*this);
        switch (node->op_) {
        case UnaryOperator::UN_PREFIX_INC:
            im.addInstr<iPushVal>(1);
            im.addInstr<iAdd>();
            break;
        case UnaryOperator::UN_PREFIX_DEC:
            im.addInstr<iPushVal>(1);
            im.addInstr<iSub>();
            break;
        default:
            throw std::logic_error("Operator is not prefix");
        }
        auto addr = scopes_.lookupVar(var->name_);
        if (!addr) {
            throw std::logic_error("Variable not found");
        }
        im.addInstr<iPopAddr>(addr.value());
    }

    void codegenArithmetic(BinaryExpression* node) {
        node->left_->accept(*this);
        node->right_->accept(*this);
        switch (node->op_) {
        case BinaryOperator::BIN_MUL:
            im.addInstr<iMul>();
            break;
        case BinaryOperator::BIN_DIV:
            im.addInstr<iDiv>();
            break;
        case BinaryOperator::BIN_MOD:
            im.addInstr<iMod>();
            break;
        case BinaryOperator::BIN_ADD:
            im.addInstr<iAdd>();
            break;
        case BinaryOperator::BIN_SUB:
            im.addInstr<iSub>();
            break;
        case BinaryOperator::BIN_L:
            im.addInstr<iCmpL>();
            break;
        case BinaryOperator::BIN_G:
            im.addInstr<iCmpG>();
            break;
        case BinaryOperator::BIN_LE:
            im.addInstr<iCmpLE>();
            break;
        case BinaryOperator::BIN_GE:
            im.addInstr<iCmpGE>();
            break;
        case BinaryOperator::BIN_EQ:
            im.addInstr<iCmpEQ>();
            break;
        case BinaryOperator::BIN_NE:
            im.addInstr<iCmpNE>();
            break;
        case BinaryOperator::BIN_AND:
            im.addInstr<iAnd>();
            break;
        case BinaryOperator::BIN_OR:
            im.addInstr<iOr>();
            break;
        default:
            throw std::logic_error("Operator is not arithmetic");
        }
    }

public:
    NodeCodegen() = default;

    Image codegen(INode* root) {
        if (root) {
            root->accept(*this);
        }
        return im;
    }

    void visit(UnaryExpression* node) {
        if (isPrefix(node->op_)) {
            codegenPrefix(node);
        } else if (isPostfix(node->op_)) {
            node->expr_->accept(*this);
            postfixes_.push_back(node);
        } else {
            switch (node->op_) {
            case UnaryOperator::UN_SUB:
                im.addInstr<iPushVal>(0);
                node->expr_->accept(*this);
                im.addInstr<iSub>();
                break;
            case UnaryOperator::UN_ADD:
                node->expr_->accept(*this);
                break;
            case UnaryOperator::UN_NOT:
                node->expr_->accept(*this);
                im.addInstr<iNot>();
                break;
            default:
                throw std::logic_error("Unknown unary operator");
            }
        }
    }

    void visit(BinaryExpression* node) {
        if (isArithmetic(node->op_)) {
            codegenArithmetic(node);
        } else {
            switch (node->op_) {
            case BinaryOperator::BIN_COMMA: {
                node->right_->accept(*this);
                im.addInstr<iPopVal>();
                node->left_->accept(*this);
                break;
            }
            case BinaryOperator::BIN_ASSIGN: {
                node->right_->accept(*this);
                auto lhs = static_cast<VariableExpression*>(node->left_);
                auto addr = scopes_.lookupVar(lhs->name_);
                if (!addr) {
                    throw std::logic_error("Variable not found");
                }
                im.addInstr<iMov>(addr.value());
                break;
            }
            }
        }
    }

    void visit(TernaryExpression* node) {
        node->condition_->accept(*this);
        codegenPostfixes();
        auto jmpTrueIndx = im.addInstr<JmpTrue>();
        node->onFalse_->accept(*this);
        codegenPostfixes();
        auto jmpIndx = im.addInstr<Jmp>();

        auto trueAddr = im.getInstrCurAddr();
        node->onTrue_->accept(*this);
        codegenPostfixes();
        auto exitAddr = im.getInstrCurAddr();

        auto jmpTrue = static_cast<JmpTrue*>(im.getInstr(jmpTrueIndx));
        jmpTrue->setAddr(exitAddr);
        auto jmp = static_cast<Jmp*>(im.getInstr(jmpIndx));
        jmp->setAddr(trueAddr);
    }

    void visit(ConstantExpression* node) {

    }

    void visit(VariableExpression* node) {
    }

    void visit(InputExpression* ) {
        im.addInstr<iIn>();
    }

    void visit(BlockStatement* node) {

    }

    void visit(ExpressionStatement* node) {
        node->expr_->accept(*this);
        codegenPostfixes();
        im.addInstr<iPopVal>();
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
