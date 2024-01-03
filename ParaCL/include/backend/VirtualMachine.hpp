#pragma once

#include <iostream>
#include <unordered_map>

#include "Constants.hpp"
#include "Instructions.hpp"

namespace paracl
{

class Image
{
private:
    std::vector<std::unique_ptr<Instruction>> instrs_;
    std::vector<std::unique_ptr<Const>> consts_;

    size_t curAddr_ = 0U;

public:
    Image() = default;

    void readImage(std::istream& is) {

    }

    void writeImage(std::ostream& os) {

    }

    template<typename ConstType, typename... ConstArgs>
    size_t addConst(ConstArgs&&... args) {
        auto uptr = std::make_unique<ConstType>(std::forward<ConstArgs>(args)...);
        auto ptr = uptr.get();
        ptr->setAddress(curAddr_);
        curAddr_ += ptr->getSize();
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

    std::array<byte, stack_size> stack_;
    std::array<value_type, num_registers> registers_;

public:
    VirtualMachine() = default;

    void loadImage(Image& image) {

    }

    void execute() {
        
    }

    value_type& sp() { return registers_[0]; }
    value_type& ip() { return registers_[1]; }
};

} // namespace paracl
