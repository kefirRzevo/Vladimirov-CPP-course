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

    const addr_t stackEndPtr_;
    const addr_t instrEndPtr_;
    const addr_t constEndPtr_;

    addr_t instrCurPtr_;
    addr_t constCurPtr_;

    void clear() {
        instrs_.clear();
        consts_.clear();
        constCurPtr_ = instrEndPtr_;
        instrCurPtr_ = stackEndPtr_;
    }

    friend class VirtualMachine;

// |             |                       |                     |
// |0    stackEnd|instrsBeg     instrsEnd|constsBeg   constsEnd|size
// |_____________|_______________________|_____________________|
// | stack       | program               | constants           |
// |_____________|_______________________|_____________________|
// |             |                       |                     |

public:
    Image(size_t stackSize = 49152U, size_t instrsSize = 15360U, size_t constsSize = 1024U)
    : stackEndPtr_(stackSize),
      instrEndPtr_(stackSize + instrsSize),
      constEndPtr_(stackSize + instrsSize + constsSize) {
        instrCurPtr_ = stackEndPtr_;
        constCurPtr_ = instrEndPtr_;
    }

    void disassembble(std::ostream& os) const {
        os << "Literals" << std::endl;
        for (const auto& constant: consts_) {
            os << std::left << std::setw(8) << constant.first;
            constant.second->disassemble(os);
            os << std::endl;
        }
        os << "Instructions" << std::endl;
        for (const auto& instr: instrs_) {
            os <<std::left << std::setw(8) << instr.first;
            instr.second->disassemble(os);
            os << std::endl;
        }
    }

    template<typename ConstType, typename... ConstArgs>
    addr_t addConst(ConstArgs&&... args) {
        auto uptr = std::make_unique<ConstType>(std::forward<ConstArgs>(args)...);
        auto constPtr = constCurPtr_;
        if (constPtr + uptr->getSize() > constEndPtr_) {
            throw std::logic_error("Too many constants");
        }
        consts_.push_back(std::move(std::make_pair(constPtr, std::move(uptr))));
        constCurPtr_ = constPtr + consts_.back().second->getSize();
        return constPtr;
    }

    template<typename InstrType, typename... InstrArgs>
    size_t addInstr(InstrArgs&&... args) {
        auto uptr = std::make_unique<InstrType>(std::forward<InstrArgs>(args)...);
        auto instrPtr = instrCurPtr_;
        if (instrPtr + uptr->getSize() > instrEndPtr_) {
            throw std::logic_error("Too many instructions");
        }
        instrs_.push_back(std::move(std::make_pair(instrPtr, std::move(uptr))));
        instrCurPtr_ = instrPtr + instrs_.back().second->getSize();
        return instrs_.size() - 1U;
    }

    addr_t getInstrCurAddr() const {
        return instrCurPtr_;
    }

    addr_t getConstCurAddr() const {
        return constCurPtr_;
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

    std::vector<byte_type> memory_;
    std::array<value_type, 2U> registers_;
    size_t memSize_;
    size_t stackSize_;

    bool halted = false;

public:
    VirtualMachine() = default;

    void loadImage(Image&& image) {
        auto instrs = std::move(image.instrs_);
        auto consts = std::move(image.consts_);
        memSize_ = image.constEndPtr_;
        stackSize_ = image.stackEndPtr_;
        memory_.reserve(memSize_);
        sp() = 0U;
        ip() = image.stackEndPtr_;
        image.clear();
        for (auto it = instrs.begin(); it != instrs.end(); ++it) {
            (*it).second->write(std::addressof(memory_[(*it).first]));
        }
        for (auto it = consts.begin(); it != consts.end(); ++it) {
            (*it).second->write(std::addressof(memory_[(*it).first]));
        }
    }

    void execute() {
        Opcode opcode;
        while (!halted) {
            utils::read(std::addressof(memory_[ip()]), opcode);
            auto instr = Instruction::create(opcode);
            instr->read(std::addressof(memory_[ip() + sizeof(Opcode)]));
            ip() += instr->getSize();
            instr->execute(*this);
        }
    }

    void halt() {
        halted = true;
    }

    template<typename T>
    void toStack(size_t indx, const T& val) {
        if (indx + sizeof(T) > stackSize_) {
            throw std::logic_error("Can't look to stack");
        }
        std::memcpy(std::addressof(memory_[indx]), std::addressof(val), sizeof(T));
    }

    template<typename T>
    T atMemory(size_t indx) {
        if (indx + sizeof(T) > memSize_) {
            throw std::logic_error("Can't look at memory");
        }
        T val{0};
        std::memcpy(std::addressof(val), std::addressof(memory_[indx]), sizeof(T));
        return val;
    }

    template<typename T>
    void push(const T& val) {
        toStack<T>(sp(), val);
        sp() += sizeof(T);
    }

    template<typename T>
    T pop() {
        sp() -= sizeof(T);
        return atMemory<T>(sp());
    }

    value_type& sp() {
        return registers_[0];
    }

    value_type& ip() {
        return registers_[1];
    }
};

} // namespace paracl
