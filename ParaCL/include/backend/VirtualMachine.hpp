#pragma once

#include <sstream>
#include <fstream>
#include <iostream>
#include <stdexcept>
#include <algorithm>
#include <unordered_map>

#include "Constants.hpp"
#include "Instructions.hpp"

namespace paracl
{

class VirtualMachine;

class Image
{
private:
    std::vector<std::unique_ptr<Instruction>> instrs_;
    std::vector<std::unique_ptr<Const>> consts_;

    size_t maxAddr_;
    size_t curAddr_;

private:
    std::vector<std::unique_ptr<Instruction>> getInstructions() && {
        return std::move(instrs_);
    }

    std::vector<std::unique_ptr<Const>> getConstants() && {
        return std::move(consts_);
    }

    size_t getConstBeginAddr() && {
        return curAddr_;
    }

    size_t getConstEndAddr() && {
        return maxAddr_;
    }

    friend class VirtualMachine;

public:
    Image(size_t size) :
        maxAddr_(size), curAddr_(size) {}
/*
    void readImage(std::istream& is) {
        size_t numConstants = 0U, numInstrs = 0U;
        Constant type;
        is >> numConstants;
        for (size_t i = 0; i < numConstants; ++i) {
            is >> type;
            auto const_ = Const::create(type);
            const_->read(is);
            consts_.push_back(std::move(const_));
        }
        Opcode opcode;
        is >> numInstrs;
        for (size_t i = 0; i < numInstrs; ++i) {
            is >> opcode;
            auto instr = Instruction::create(opcode);
            instr->read(is);
            instrs_.push_back(std::move(instr));
        }
    }
*/
    void writeImage(std::ostream& os) {
        os << consts_.size();
        for (auto& const_: consts_) {
            const_->write(os);
        }
        os << instrs_.size();
        for (auto& instr : instrs_) {
            instr->write(os);
        }
    }

    template<typename ConstType, typename... ConstArgs>
    void addConst(ConstArgs&&... args) {
        auto uptr = std::make_unique<ConstType>(std::forward<ConstArgs>(args)...);
        auto ptr = uptr.get();
        if (ptr->getSize() > curAddr_) {
            throw std::logic_error("Too many constants");
        }
        curAddr_ -= ptr->getSize();
        ptr->setAddress(curAddr_);
        consts_.push_back(std::move(uptr));
    }

    template<typename InstrType, typename... InstrArgs>
    void addInstruction(InstrArgs&&... args) {
        auto uptr = std::make_unique<InstrType>(std::forward<InstrArgs>(args)...);
        instrs_.push_back(std::move(uptr));
    }

    size_t curInstr() const {
        return instrs_.size() - 1U;
    }

    size_t curConst() const {
        return consts_.size() - 1U;
    }

    Const* getConst(size_t id) {
        return consts_[id].get();
    }

    Instruction* getInstruction(size_t id) {
        return instrs_[id].get();
    }
};

class VirtualMachine
{
private:
    using value_type = size_t;
    using byte = char;

    std::vector<std::unique_ptr<Instruction>> instrs_;
    std::vector<byte> stack_;
    std::vector<value_type> registers_{2U};

    size_t constEnd_ = 0U;
    size_t constBegin_ = 0U;
    bool halted = false;

public:
    VirtualMachine() = default;

    void loadImage(Image&& image) {
        instrs_ = std::move(image).getInstructions();
        auto consts_ = std::move(image).getConstants();
        constEnd_ = std::move(image).getConstEndAddr();
        constBegin_ = std::move(image).getConstBeginAddr();
        stack_.reserve(constEnd_);
        std::ostringstream ss{};
        for (auto it = consts_.rbegin(); it != consts_.rend(); ++it) {
            (*it)->write(ss);
        }
        std::string str = ss.str();
        std::copy_n(str.c_str(), constEnd_ - constBegin_, std::addressof(stack_[constBegin_]));
    }

    void execute() {
        ip() = 0U;
        sp() = 0U;
        while (!halted) {
            instrs_[ip()++]->execute(*this);
            if (ip() > instrs_.size()) {
                throw std::logic_error("Unavailable instruction");
            }
        }
    }

    void halt() {
        halted = true;
    }

    template<typename T>
    void toStack(size_t indx, const T& val) {
        if (indx + sizeof(T) > constEnd_) {
            throw std::logic_error("Can't look to stack");
        }
        std::memcpy(std::addressof(stack_[indx]), std::addressof(val), sizeof(T));
    }

    template<typename T>
    T fromStack(size_t indx) {
        if (indx + sizeof(T) > constEnd_) {
            throw std::logic_error("Can't look from stack");
        }
        T val{0};
        std::memcpy(std::addressof(val), std::addressof(stack_[indx]), sizeof(T));
        return val;
    }

    byte* absoluteAddr(size_t addr) {
        return reinterpret_cast<byte*>(std::addressof(stack_[addr]));
    }

    template<typename T>
    void push(const T& val) {
        toStack<T>(sp(), val);
        sp() += sizeof(T);
    }

    template<typename T>
    T pop() {
        sp() -= sizeof(T);
        return fromStack<T>(sp());
    }

    value_type& sp() {
        return registers_[0];
    }

    value_type& ip() {
        return registers_[1];
    }
};

} // namespace paracl
