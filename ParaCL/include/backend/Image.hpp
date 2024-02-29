#pragma once

#include <vector>
#include <memory>
#include <fstream>
#include <utility>
#include <iomanip>
#include <iostream>
#include <stdexcept>
#include <string_view>

#include "backend/Instructions.hpp"
#include "backend/Constants.hpp"

namespace paracl
{

class VirtualMachine;

class Image final
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

// |             |                       |                     |
// |0    stackEnd|instrsBeg     instrsEnd|constsBeg   constsEnd|size
// |_____________|_______________________|_____________________|
// | stack       | program               | constants           |
// |_____________|_______________________|_____________________|
// |             |                       |                     |

public:
    Image(
        size_t stackSize = 49152U,
        size_t instrsSize = 15360U,
        size_t constsSize = 1024U
    )
    : stackEndPtr_(stackSize),
      instrEndPtr_(stackSize + instrsSize),
      constEndPtr_(stackSize + instrsSize + constsSize) {
        instrCurPtr_ = stackEndPtr_;
        constCurPtr_ = instrEndPtr_;
    }

    void disassemble(const std::string& filepath) const {
        std::ofstream os(filepath);
        os << "Literals" << std::endl;
        for (auto&& constant : consts_) {
            os << std::left << std::setw(8) << constant.first;
            constant.second->disassemble(os);
            os << std::endl;
        }
        os << "Instructions" << std::endl;
        for (auto&& instr : instrs_) {
            os << std::left << std::setw(8) << instr.first;
            instr.second->disassemble(os);
            os << std::endl;
        }
    }

    template<typename ConstType, typename... ConstArgs>
    addr_t addConst(ConstArgs&&... args) {
        auto uptr = std::make_unique<ConstType>(
            std::forward<ConstArgs>(args)...
        );
        auto constPtr = constCurPtr_;
        if (constPtr + uptr->getSize() > constEndPtr_) {
            throw std::logic_error("Too many constants");
        }
        consts_.push_back(
            std::move(std::make_pair(constPtr, std::move(uptr)))
        );
        constCurPtr_ = constPtr + consts_.back().second->getSize();
        return constPtr;
    }

    template<typename InstrType, typename... InstrArgs>
    size_t addInstr(InstrArgs&&... args) {
        auto uptr = std::make_unique<InstrType>(
            std::forward<InstrArgs>(args)...
        );
        auto instrPtr = instrCurPtr_;
        if (instrPtr + uptr->getSize() > instrEndPtr_) {
            throw std::logic_error("Too many instructions");
        }
        instrs_.push_back(
            std::move(std::make_pair(instrPtr, std::move(uptr)))
        );
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

    std::vector<instr_value> getInstrs() {
        return std::move(instrs_);
    }

    std::vector<const_value> getConsts() {
        return std::move(consts_);
    }

    addr_t getStackEndPtr() const {
        return stackEndPtr_;
    }

    addr_t getInstrEndPtr() const {
        return instrEndPtr_;
    }

    addr_t getConstEndPtr() const {
        return constEndPtr_;
    }
};

} // namespace paracl
