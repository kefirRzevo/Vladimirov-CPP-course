#pragma once

#include <array>
#include <utility>
#include <stdexcept>
#include <algorithm>
#include <unordered_map>

#include "Image.hpp"

namespace paracl
{

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
