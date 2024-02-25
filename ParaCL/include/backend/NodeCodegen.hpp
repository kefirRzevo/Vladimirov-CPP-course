#pragma once

#include <tuple>
#include <memory>
#include <vector>
#include <stdexcept>

#include "frontend/NodeVisitor.hpp"
#include "backend/StackFrame.hpp"

namespace paracl
{

class Image;

class NodeCodegen final : public NodeVisitor
{
private:
    using NodeVisitor::visit;
    using addr_t = size_t;

    enum class LoopResetType
    {
        CONTINUE,
        BREAK,
    };
    using LoopReset = std::tuple<WhileStatement*, LoopResetType, addr_t>;

    std::vector<UnaryExpression*> postfixes_;
    std::vector<LoopReset> loopResets_;

    std::unique_ptr<Image> im_;
    std::unique_ptr<ConstantPool> consts_;
    StackFrame frame_;

    void beginScope(const Scope& scope) {
        auto size = frame_.beginScope(scope);
        if (!size) {
            throw std::runtime_error("Can't begin scope");
        }
        if (size.value() != 0U) {
            im_->addInstr<Alloca>(size.value());
        }
    }

    void endScope() {
        auto size = frame_.endScope();
        if (!size) {
            throw std::runtime_error("Can't end scope");
        }
        if (size.value() != 0U) {
            im_->addInstr<Alloca>(-static_cast<int>(size.value()));
        }
    }

    void codegenPostfixes() {
        if (postfixes_.empty()) {
            return;
        }
        for (auto&& node : postfixes_) {
            auto var = static_cast<VariableExpression*>(node->getExpr());
            auto addr = frame_.lookupVar(var->getName());
            if (!addr) {
                throw std::logic_error("Variable not found");
            }
            im_->addInstr<iPushAddr>(addr.value());
            switch (node->getOperator()) {
            case UnaryOperator::UN_POSTFIX_INC:
                im_->addInstr<iPushVal>(1);
                im_->addInstr<iAdd>();
                break;
            case UnaryOperator::UN_POSTFIX_DEC:
                im_->addInstr<iPushVal>(1);
                im_->addInstr<iSub>();
                break;
            default:
                throw std::logic_error("Operator is not postfix");
            }
            im_->addInstr<iPopAddr>(addr.value());
        }
        postfixes_.clear();
    }

    void codegenPrefix(UnaryExpression* node) {
        auto var = static_cast<VariableExpression*>(node->getExpr());
        auto addr = frame_.lookupVar(var->getName());
        if (!addr) {
            throw std::logic_error("Variable not found");
        }
        im_->addInstr<iPushAddr>(addr.value());
        switch (node->getOperator()) {
        case UnaryOperator::UN_PREFIX_INC:
            im_->addInstr<iPushVal>(1);
            im_->addInstr<iAdd>();
            break;
        case UnaryOperator::UN_PREFIX_DEC:
            im_->addInstr<iPushVal>(1);
            im_->addInstr<iSub>();
            break;
        default:
            throw std::logic_error("Operator is not prefix");
        }
        im_->addInstr<iPopAddr>(addr.value());
    }

    void codegenArithmetic(BinaryExpression* node) {
        node->getLeftExpr()->accept(*this);
        node->getRightExpr()->accept(*this);
        switch (node->getOperator()) {
        case BinaryOperator::BIN_MUL:
            im_->addInstr<iMul>();
            break;
        case BinaryOperator::BIN_DIV:
            im_->addInstr<iDiv>();
            break;
        case BinaryOperator::BIN_MOD:
            im_->addInstr<iMod>();
            break;
        case BinaryOperator::BIN_ADD:
            im_->addInstr<iAdd>();
            break;
        case BinaryOperator::BIN_SUB:
            im_->addInstr<iSub>();
            break;
        case BinaryOperator::BIN_L:
            im_->addInstr<iCmpL>();
            break;
        case BinaryOperator::BIN_G:
            im_->addInstr<iCmpG>();
            break;
        case BinaryOperator::BIN_LE:
            im_->addInstr<iCmpLE>();
            break;
        case BinaryOperator::BIN_GE:
            im_->addInstr<iCmpGE>();
            break;
        case BinaryOperator::BIN_EQ:
            im_->addInstr<iCmpEQ>();
            break;
        case BinaryOperator::BIN_NE:
            im_->addInstr<iCmpNE>();
            break;
        case BinaryOperator::BIN_AND:
            im_->addInstr<iAnd>();
            break;
        case BinaryOperator::BIN_OR:
            im_->addInstr<iOr>();
            break;
        default:
            throw std::logic_error("Operator is not arithmetic");
        }
    }

    void codegenLoopResets(WhileStatement* node, addr_t cond, addr_t exit) {
        if (loopResets_.empty()) {
            return;
        }
        auto back = loopResets_.back();
        while (std::get<WhileStatement*>(back) == node) {
            auto type = std::get<LoopResetType>(back);
            auto jmpIndx = std::get<addr_t>(back);
            auto jmp = static_cast<Jmp*>(im_->getInstr(jmpIndx));
            switch (type) {
            case LoopResetType::BREAK:
                jmp->setAddr(exit);
                break;
            case LoopResetType::CONTINUE:
                jmp->setAddr(cond);
                break;
            default:
                throw std::logic_error("Unknown loop reset type");
            }
            loopResets_.pop_back();
            if (loopResets_.empty()) {
                return;
            }
            back = loopResets_.back();
        }
    }

public:
    NodeCodegen() = default;

    std::unique_ptr<Image> codegen(INode* root) {
        im_ = std::make_unique<Image>();
        consts_ = std::make_unique<ConstantPool>(*im_);
        if (root) {
            root->accept(*this);
        }
        im_->addInstr<Hlt>();
        consts_.reset();
        std::unique_ptr<Image> im = std::move(im_);
        im_.release();
        return im;
    }

    void visit(UnaryExpression* node) {
        auto nodeOp = node->getOperator();
        auto nodeExpr = node->getExpr();
        if (isPrefix(nodeOp)) {
            codegenPrefix(node);
            nodeExpr->accept(*this);
        } else if (isPostfix(nodeOp)) {
            nodeExpr->accept(*this);
            postfixes_.push_back(node);
        } else {
            switch (nodeOp) {
            case UnaryOperator::UN_SUB:
                im_->addInstr<iPushVal>(0);
                nodeExpr->accept(*this);
                im_->addInstr<iSub>();
                break;
            case UnaryOperator::UN_ADD:
                nodeExpr->accept(*this);
                break;
            case UnaryOperator::UN_NOT:
                nodeExpr->accept(*this);
                im_->addInstr<iNot>();
                break;
            default:
                throw std::logic_error("Unknown unary operator");
            }
        }
    }

    void visit(BinaryExpression* node) {
        auto nodeOp = node->getOperator();
        if (isArithmetic(nodeOp)) {
            codegenArithmetic(node);
        } else {
            auto nodeLeft = node->getLeftExpr();
            auto nodeRight = node->getRightExpr();
            switch (nodeOp) {
            case BinaryOperator::BIN_COMMA: {
                nodeRight->accept(*this);
                im_->addInstr<iPopVal>();
                nodeLeft->accept(*this);
                break;
            }
            case BinaryOperator::BIN_ASSIGN: {
                auto lhs = static_cast<VariableExpression*>(nodeLeft);
                auto addr = frame_.lookupVar(lhs->getName());
                if (!addr) {
                    throw std::runtime_error("Can't push variable");
                }
                nodeRight->accept(*this);
                im_->addInstr<iPopAddr>(addr.value());
                im_->addInstr<iPushAddr>(addr.value());
                break;
            }
            default:
                throw std::logic_error("Unknown binary operator");
            }
        }
    }

    void visit(TernaryExpression* node) {
        node->getCondExpr()->accept(*this);
        codegenPostfixes();
        auto jmpTrueIndx = im_->addInstr<JmpTrue>();
        node->getFalseExpr()->accept(*this);
        codegenPostfixes();
        auto jmpIndx = im_->addInstr<Jmp>();
        auto trueAddr = im_->getInstrCurAddr();

        node->getTrueExpr()->accept(*this);
        codegenPostfixes();
        auto exitAddr = im_->getInstrCurAddr();

        auto jmpTrue = static_cast<JmpTrue*>(im_->getInstr(jmpTrueIndx));
        jmpTrue->setAddr(trueAddr);
        auto jmp = static_cast<Jmp*>(im_->getInstr(jmpIndx));
        jmp->setAddr(exitAddr);
    }

    void visit(ConstantExpression* node) {
        auto nodeVal = node->getValue();
        auto addr = consts_->lookupConst(nodeVal);
        if (!addr) {
            addr = consts_->pushConst(nodeVal);
            if (!addr) {
                throw std::runtime_error("Can't push const");
            }
        }
        im_->addInstr<iPushAddr>(addr.value());
    }

    void visit(VariableExpression* node) {
        auto addr = frame_.lookupVar(node->getName());
        if (!addr) {
            throw std::runtime_error("Can't push variable");
        }
        im_->addInstr<iPushAddr>(addr.value());
    }

    void visit(InputExpression* ) {
        im_->addInstr<iIn>();
    }

    void visit(BlockStatement* node) {
        beginScope(node->getScope());
        for (auto&& statement : *node) {
            statement->accept(*this);
        }
        endScope();
    }

    void visit(ExpressionStatement* node) {
        node->getExpr()->accept(*this);
        codegenPostfixes();
        im_->addInstr<iPopVal>();
    }

    void visit(IfStatement* node) {
        beginScope(node->getScope());
        node->getCondExpr()->accept(*this);
        codegenPostfixes();
        auto jmpFalseIndx = im_->addInstr<JmpFalse>();
        node->getTrueBlock()->accept(*this);
        codegenPostfixes();
        auto exitAddr = im_->getInstrCurAddr();

        auto jmpFalse = static_cast<JmpFalse*>(im_->getInstr(jmpFalseIndx));
        jmpFalse->setAddr(exitAddr);
        endScope();
    }

    void visit(IfElseStatement* node) {
        beginScope(node->getScope());
        node->getCondExpr()->accept(*this);
        codegenPostfixes();
        auto jmpTrueIndx = im_->addInstr<JmpTrue>();
        node->getFalseBlock()->accept(*this);
        codegenPostfixes();
        auto jmpIndx = im_->addInstr<Jmp>();
        auto trueAddr = im_->getInstrCurAddr();

        node->getTrueBlock()->accept(*this);
        codegenPostfixes();
        auto exitAddr = im_->getInstrCurAddr();

        auto jmpTrue = static_cast<JmpTrue*>(im_->getInstr(jmpTrueIndx));
        jmpTrue->setAddr(trueAddr);
        auto jmp = static_cast<Jmp*>(im_->getInstr(jmpIndx));
        jmp->setAddr(exitAddr);
        endScope();
    }

    void visit(WhileStatement* node) {
        beginScope(node->getScope());
        auto condAddr = im_->getInstrCurAddr();

        node->getCondExpr()->accept(*this);
        codegenPostfixes();
        auto jmpFalseIndx = im_->addInstr<JmpFalse>();
        node->getBlock()->accept(*this);
        auto jmpIndx = im_->addInstr<Jmp>();
        auto exitAddr = im_->getInstrCurAddr();

        auto jmpFalse = static_cast<JmpFalse*>(im_->getInstr(jmpFalseIndx));
        jmpFalse->setAddr(exitAddr);
        auto jmp = static_cast<Jmp*>(im_->getInstr(jmpIndx));
        jmp->setAddr(condAddr);
        codegenLoopResets(node, condAddr, exitAddr);
        endScope();
    }

    void visit(OutputStatement* node) {
        node->getExpr()->accept(*this);
        im_->addInstr<iOut>();
        codegenPostfixes();
    }

    void visit(BreakStatement* node) {
        auto nodeLoop = node->getLoopStat();
        auto jmpIndx = im_->addInstr<Jmp>();
        loopResets_.emplace_back(nodeLoop, LoopResetType::BREAK, jmpIndx);
    }

    void visit(ContinueStatement* node) {
        auto nodeLoop = node->getLoopStat();
        auto jmpIndx = im_->addInstr<Jmp>();
        loopResets_.emplace_back(nodeLoop, LoopResetType::CONTINUE, jmpIndx);
    }
};

} // namespace paracl::backend {
