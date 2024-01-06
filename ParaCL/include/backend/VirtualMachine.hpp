#pragma once

#include <sstream>
#include <fstream>
#include <utility>
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
    using addr_t = size_t;
    using instr_value = std::pair<addr_t, std::unique_ptr<Instruction>>;
    using const_value = std::pair<addr_t, std::unique_ptr<Const>>;

    std::vector<instr_value> instrs_;
    std::vector<const_value> consts_;

    addr_t begInstr_;
    addr_t endInstr_;
    addr_t begConst_;
    addr_t endConst_;

private:
    auto getInstructions() && {
        return std::move(instrs_);
    }

    auto getConstants() && {
        return std::move(consts_);
    }

    addr_t getInstrBegAddr() && {
        return begInstr_;
    }
    
    addr_t getInstrEndAddr() && {
        return endInstr_;
    }

    addr_t getConstBegAddr() && {
        return begConst_;
    }

    addr_t getConstEndAddr() && {
        return endConst_;
    }

    void clear() {
        instrs_.clear();
        consts_.clear();
        begInstr_ = 0U;
        endInstr_ = 0U;
        begConst_ = 0U;
        endConst_ = 0U;
    }

    friend class VirtualMachine;

public:
    Image(addr_t size) :
        begInstr_(0U), endInstr_(0U),  begConst_(size), endConst_(size) {}
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
*/
    template<typename ConstType, typename... ConstArgs>
    void addConst(ConstArgs&&... args) {
        auto uptr = std::make_unique<ConstType>(std::forward<ConstArgs>(args)...);
        if (uptr->getSize() > endConst_) {
            throw std::logic_error("Too many constants");
        }
        endConst_ -= uptr->getSize();
        if (endConst_ < endInstr_) {
            throw std::logic_error("Too many constants");
        }
        consts_.push_back(std::move(std::make_pair(endConst_, std::move(uptr))));
    }

    template<typename InstrType, typename... InstrArgs>
    void addInstruction(InstrArgs&&... args) {
        auto uptr = std::make_unique<InstrType>(std::forward<InstrArgs>(args)...);
        endInstr_ += uptr->getSize();
        if (endInstr_ > endConst_) {
            throw std::logic_error("Too many instructions");
        }
        instrs_.push_back(std::move(std::make_pair(endInstr_, std::move(uptr))));
    }

    addr_t getInstrCurAddr() const {
        return endInstr_;
    }

    addr_t getConstCurAddr() const {
        return endConst_;
    }

    Instruction* getInstr(size_t id) {
        return instrs_[id].second.get();
    }

    addr_t getInstrAddr(size_t id) {
        return instrs_[id].first;
    }

    Const* getConst(size_t id) {
        return consts_[id].second.get();
    }

    addr_t getConstAddr(size_t id) {
        return consts_[id].first;
    }
};

class VirtualMachine
{
private:
    using value_type = size_t;
    using byte_type = char;

    std::vector<byte_type> stack_;
    std::vector<value_type> registers_{2U};

    size_t instrBeg_ = 0U;
    size_t instrEnd_ = 0U;
    size_t constEnd_ = 0U;
    size_t constBeg_ = 0U;
    bool halted = false;

public:
    VirtualMachine() = default;

    void loadImage(Image&& image) {
        auto instrs = std::move(image).getInstructions();
        auto consts = std::move(image).getConstants();
        instrBeg_ = std::move(image).getInstrBegAddr();
        instrEnd_ = std::move(image).getInstrEndAddr();
        constBeg_ = std::move(image).getConstBegAddr();
        constEnd_ = std::move(image).getConstEndAddr();
        image.clear();
        stack_.reserve(constBeg_);
        std::ostringstream oss{};
        for (auto it = instrs.begin(); it != instrs.end(); ++it) {
            (*it).second->write(oss);
        }
        byte_type clear = 0;
        for (size_t i = 0U; i < constEnd_ - instrEnd_; ++i) {
            utils::write(oss, clear);
        }
        for (auto it = consts.rbegin(); it != consts.rend(); ++it) {
            (*it).second->write(oss);
        }
        std::string str = oss.str();
        std::memcpy(std::addressof(stack_[instrBeg_]), str.c_str(), constBeg_ - instrBeg_);
    }

    void execute() {
        ip() = instrBeg_;
        sp() = instrEnd_;
        Opcode opcode;
        while (!halted) {
            utils::read(absoluteAddr(ip()), opcode);
            auto instr = Instruction::create(opcode);
            instr->read(absoluteAddr(ip() + sizeof(Opcode)));
            instr->execute(*this);
            ip() += instr->getSize();
            if (ip() > instrEnd_) {
                throw std::logic_error("Unavailable instruction");
            }
        }
    }

    void halt() {
        halted = true;
    }

    template<typename T>
    void toStack(size_t indx, const T& val) {
        if (indx + sizeof(T) > constBeg_) {
            throw std::logic_error("Can't look to stack");
        }
        std::memcpy(std::addressof(stack_[indx]), std::addressof(val), sizeof(T));
    }

    template<typename T>
    T fromStack(size_t indx) {
        if (indx + sizeof(T) > constBeg_) {
            throw std::logic_error("Can't look from stack");
        }
        T val{0};
        std::memcpy(std::addressof(val), std::addressof(stack_[indx]), sizeof(T));
        return val;
    }

    byte_type* absoluteAddr(size_t addr) {
        return reinterpret_cast<byte_type*>(std::addressof(stack_[addr]));
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
