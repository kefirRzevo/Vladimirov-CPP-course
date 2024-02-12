#pragma once

#include "frontend/NodeVisitor.hpp"
#include "backend/StackFrame.hpp"

namespace paracl
{

class Image;

class ConstantPool final
{
private:
    using addr_t = size_t;
    using Pool = std::unordered_map<int, addr_t>;

    Pool pool_;
    Image& im_;

public:
    ConstantPool(Image& image) :
        im_(image) {}

    std::optional<addr_t> lookupConst(int value) {
        auto found = pool_.find(value);
        if (found != pool_.cend()) {
            return found->second;
        }
        return std::nullopt;
    }

    std::optional<addr_t> pushConst(int value) {
        auto addr = im_.addConst<ConstInt>(value);
        pool_.emplace(value, addr);
        return addr;
    }
};

class NodeCodegen final : public NodeVisitor
{
private:
    using NodeVisitor::visit;
    using addr_t = size_t;

    Image im_;

    std::vector<UnaryExpression*> postfixes_;

    StackFrame frame_;
    ConstantPool consts_;

    void codegenPostfixes() {
        if (postfixes_.empty()) {
            return;
        }
        for (auto& node: postfixes_) {
            auto var = static_cast<VariableExpression*>(node->expr_);
            node->expr_->accept(*this);
            switch (node->op_) {
            case UnaryOperator::UN_POSTFIX_INC:
                im_.addInstr<iPushVal>(1);
                im_.addInstr<iAdd>();
                break;
            case UnaryOperator::UN_POSTFIX_DEC:
                im_.addInstr<iPushVal>(1);
                im_.addInstr<iSub>();
                break;
            default:
                throw std::logic_error("Operator is not postfix");
            }
            auto addr = frame_.lookupVar(var->name_);
            if (!addr) {
                throw std::logic_error("Variable not found");
            }
            im_.addInstr<iPopAddr>(addr.value());
        }
        postfixes_.clear();
    }

    void codegenPrefix(UnaryExpression* node) {
        auto var = static_cast<VariableExpression*>(node->expr_);
        node->expr_->accept(*this);
        switch (node->op_) {
        case UnaryOperator::UN_PREFIX_INC:
            im_.addInstr<iPushVal>(1);
            im_.addInstr<iAdd>();
            break;
        case UnaryOperator::UN_PREFIX_DEC:
            im_.addInstr<iPushVal>(1);
            im_.addInstr<iSub>();
            break;
        default:
            throw std::logic_error("Operator is not prefix");
        }
        auto addr = frame_.lookupVar(var->name_);
        if (!addr) {
            throw std::logic_error("Variable not found");
        }
        im_.addInstr<iPopAddr>(addr.value());
    }

    void codegenArithmetic(BinaryExpression* node) {
        node->left_->accept(*this);
        node->right_->accept(*this);
        switch (node->op_) {
        case BinaryOperator::BIN_MUL:
            im_.addInstr<iMul>();
            break;
        case BinaryOperator::BIN_DIV:
            im_.addInstr<iDiv>();
            break;
        case BinaryOperator::BIN_MOD:
            im_.addInstr<iMod>();
            break;
        case BinaryOperator::BIN_ADD:
            im_.addInstr<iAdd>();
            break;
        case BinaryOperator::BIN_SUB:
            im_.addInstr<iSub>();
            break;
        case BinaryOperator::BIN_L:
            im_.addInstr<iCmpL>();
            break;
        case BinaryOperator::BIN_G:
            im_.addInstr<iCmpG>();
            break;
        case BinaryOperator::BIN_LE:
            im_.addInstr<iCmpLE>();
            break;
        case BinaryOperator::BIN_GE:
            im_.addInstr<iCmpGE>();
            break;
        case BinaryOperator::BIN_EQ:
            im_.addInstr<iCmpEQ>();
            break;
        case BinaryOperator::BIN_NE:
            im_.addInstr<iCmpNE>();
            break;
        case BinaryOperator::BIN_AND:
            im_.addInstr<iAnd>();
            break;
        case BinaryOperator::BIN_OR:
            im_.addInstr<iOr>();
            break;
        default:
            throw std::logic_error("Operator is not arithmetic");
        }
    }

public:
    NodeCodegen() :
        im_(), frame_(im_), consts_(im_) {}

    Image codegen(INode* root) {
        if (root) {
            root->accept(*this);
        }
        im_.addInstr<Hlt>();
        return std::move(im_);
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
                im_.addInstr<iPushVal>(0);
                node->expr_->accept(*this);
                im_.addInstr<iSub>();
                break;
            case UnaryOperator::UN_ADD:
                node->expr_->accept(*this);
                break;
            case UnaryOperator::UN_NOT:
                node->expr_->accept(*this);
                im_.addInstr<iNot>();
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
                im_.addInstr<iPopVal>();
                node->left_->accept(*this);
                break;
            }
            case BinaryOperator::BIN_ASSIGN: {
                auto lhs = static_cast<VariableExpression*>(node->left_);
                auto addr = frame_.lookupVar(lhs->name_);
                if (!addr) {
                    addr = frame_.pushVar(lhs->name_);
                    if (!addr) {
                        throw std::runtime_error("Can't push variable");
                    }
                }
                node->right_->accept(*this);
                im_.addInstr<iPopAddr>(addr.value());
                im_.addInstr<iPushAddr>(addr.value());
                break;
            }
            default:
                throw std::logic_error("Unknown binary operator");
            }
        }
    }

    void visit(TernaryExpression* node) {
        node->condition_->accept(*this);
        codegenPostfixes();
        auto jmpTrueIndx = im_.addInstr<JmpTrue>();
        node->onFalse_->accept(*this);
        codegenPostfixes();
        auto jmpIndx = im_.addInstr<Jmp>();
        auto trueAddr = im_.getInstrCurAddr();

        node->onTrue_->accept(*this);
        codegenPostfixes();
        auto exitAddr = im_.getInstrCurAddr();

        auto jmpTrue = static_cast<JmpTrue*>(im_.getInstr(jmpTrueIndx));
        jmpTrue->setAddr(exitAddr);
        auto jmp = static_cast<Jmp*>(im_.getInstr(jmpIndx));
        jmp->setAddr(trueAddr);
    }

    void visit(ConstantExpression* node) {
        auto addr = consts_.lookupConst(node->value_);
        if (!addr) {
            addr = consts_.pushConst(node->value_);
            if (!addr) {
                throw std::runtime_error("Can't push const");
            }
        }
        im_.addInstr<iPushAddr>(addr.value());
    }

    void visit(VariableExpression* node) {
        auto addr = frame_.lookupVar(node->name_);
        if (!addr) {
            addr = frame_.pushVar(node->name_);
            if (!addr) {
                throw std::runtime_error("Can't push variable");
            }
        }
        im_.addInstr<iPushAddr>(addr.value());
    }

    void visit(InputExpression* ) {
        im_.addInstr<iIn>();
    }

    void visit(BlockStatement* node) {
        frame_.beginScope();
        for (auto statement: node->statements_) {
            statement->accept(*this);
        }
        frame_.endScope();
    }

    void visit(ExpressionStatement* node) {
        node->expr_->accept(*this);
        codegenPostfixes();
        im_.addInstr<iPopVal>();
    }

    void visit(IfStatement* node) {
        frame_.beginScope();
        node->condition_->accept(*this);
        codegenPostfixes();
        auto jmpFalseIndx = im_.addInstr<JmpFalse>();
        node->trueBlock_->accept(*this);
        codegenPostfixes();
        auto exitAddr = im_.getInstrCurAddr();

        auto jmpFalse = static_cast<JmpFalse*>(im_.getInstr(jmpFalseIndx));
        jmpFalse->setAddr(exitAddr);
        frame_.endScope();
    }

    void visit(IfElseStatement* node) {
        frame_.beginScope();
        node->condition_->accept(*this);
        codegenPostfixes();
        auto jmpTrueIndx = im_.addInstr<JmpTrue>();
        node->falseBlock_->accept(*this);
        codegenPostfixes();
        auto jmpIndx = im_.addInstr<Jmp>();
        auto trueAddr = im_.getInstrCurAddr();

        node->trueBlock_->accept(*this);
        codegenPostfixes();
        auto exitAddr = im_.getInstrCurAddr();

        auto jmpTrue = static_cast<JmpTrue*>(im_.getInstr(jmpTrueIndx));
        jmpTrue->setAddr(trueAddr);
        auto jmp = static_cast<Jmp*>(im_.getInstr(jmpIndx));
        jmp->setAddr(exitAddr);
        frame_.endScope();
    }

    void visit(WhileStatement* node) {
        frame_.beginScope();
        auto condAddr = im_.getInstrCurAddr();

        node->condition_->accept(*this);
        codegenPostfixes();
        auto jmpFalseIndx = im_.addInstr<JmpFalse>();
        node->block_->accept(*this);
        auto jmpIndx = im_.addInstr<Jmp>();
        auto exitAddr = im_.getInstrCurAddr();

        auto jmpFalse = static_cast<JmpFalse*>(im_.getInstr(jmpFalseIndx));
        jmpFalse->setAddr(exitAddr);
        auto jmp = static_cast<Jmp*>(im_.getInstr(jmpIndx));
        jmp->setAddr(condAddr);
        frame_.endScope();
    }

    void visit(OutputStatement* node) {
        frame_.beginScope();
        node->expr_->accept(*this);
        im_.addInstr<iOut>();
        frame_.endScope();
    }

    void visit(BreakStatement* ) {

    }

    void visit(ContinueStatement* ) {

    }
};

} // namespace paracl::backend {
