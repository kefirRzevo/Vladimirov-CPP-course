#pragma once

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

    size_t curAddr_;

private:
    std::vector<std::unique_ptr<Instruction>> getInstructions() && {
        return std::move(instrs_);
    }

    std::vector<std::unique_ptr<Const>> getConstants() && {
        return std::move(consts_);
    }

    size_t getMaxAddress() && {
        return curAddr_;
    }

    friend class VirtualMachine;

public:
    Image(size_t maxAddr) :
        curAddr_(maxAddr) {}

    void readImage(std::istream& is) {
        size_t numConstants = 0U, numInstrs = 0U;
        Constant type;
        is >> numConstants;
        for (size_t i = 0; i < numConstants; ++i) {
            is >> type;
            auto const_ = Const::create(type);
            const_->read(is);
            consts_.push_back(const_);
        }
        Opcode opcode;
        is >> numInstrs; 
        for (size_t i = 0; i < numInstrs; ++i) {
            is >> opcode;
            auto instr = Instruction::create(opcode);
            instr->read(is);
            instrs_.push_back(instr);
        }
    }

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
    size_t addConst(ConstArgs&&... args) {
        auto uptr = std::make_unique<ConstType>(std::forward<ConstArgs>(args)...);
        auto ptr = uptr.get();
        ptr->setAddress(curAddr_);
        if (ptr->getSize() > curAddr_) {
            throw std::logic_error("Too many constants");
        }
        curAddr_ -= ptr->getSize();
        consts_.push_back(std::move(uptr));
        return consts_.size() - 1U;
    }

    template<typename InstrType, typename... InstrArgs>
    size_t addInstruction(InstrArgs&&... args) {
        auto uptr = std::make_unique<InstrType>(std::forward<InstrArgs>(args)...);
        auto ptr = uptr.get();
        instrs_.push_back(std::move(uptr));
        return instrs_.size() - 1U;
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
};

class VirtualMachine
{
private:
    using value_type = int;
    using byte = char;

    static constexpr size_t stack_size = 65536;
    static constexpr size_t num_registers = 2;

    std::vector<std::unique_ptr<Instruction>> instrs_;
    std::array<byte, stack_size> stack_;
    std::array<value_type, num_registers> registers_;

    size_t maxAddr_ = stack_size;
    bool halted = false;

public:
    VirtualMachine() = default;

    void loadImage(Image&& image) {
        instrs_ = std::move(image).getInstructions();
        auto consts_ = std::move(image).getConstants();
        std::ostream os;
        for (auto it = consts_.begin(); it != consts_.end(); ++ it) {
            (*it)->write(os);
        }
        std::filebuf* obuf = os.rdbuf();
        obuf->sgetn(std::addressof(stack_[maxAddr_]), stack_size - maxAddr_);
    }

    void execute() {
        while (!halted) {
            instrs_[ip()]->execute(*this);
        }
    }

    static size_t size() {
        return stack_size;
    }

    template<typename T>
    void toStack(size_t indx, const T& val) {
        if (indx + sizeof(T) > maxAddr_) {
            throw std::logic_error("Can't look to stack");
        }
        std::copy_n(std::addressof(val), sizeof(T), std::addressof(stack_[sp()]));
    }

    template<typename T>
    T atStack(size_t indx) {
        if (indx + sizeof(T) > maxAddr_) {
            throw std::logic_error("Can't look at stack");
        }
        T val{0};
        std::copy_n(std::addressof(stack_[indx]), sizeof(T), std::addressof(val));
        return val;
    }

    template<typename T>
    void push(const T& val) {
        toStack<T>(sp());
        sp() += sizeof(T);
    }

    template<typename T>
    T pop() {
        sp() -= sizeof(T);
        return atStack<T>(sp());
    }

    value_type& sp() {
        return registers_[0];
    }

    value_type& ip() {
        return registers_[1];
    }
};

} // namespace paracl
`