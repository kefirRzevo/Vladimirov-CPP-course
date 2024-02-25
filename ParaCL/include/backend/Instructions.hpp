#pragma once

#include <memory>
#include <iostream>
#include <stdexcept>

#include "Utils.hpp"

namespace paracl
{

class VirtualMachine;

enum class Opcode : char
{
    iPUSH_ADDR,
    iPUSH_VAL,
    iPOP_ADDR,
    iPOP_VAL,
    iMUL,
    iDIV,
    iMOD,
    iADD,
    iSUB,
    iCMP_L,
    iCMP_G,
    iCMP_LE,
    iCMP_GE,
    iCMP_EQ,
    iCMP_NE,
    iAND,
    iOR,
    iNOT,
    iOUT,
    iIN,
    strOUT,
    ALLOCA,
    JMP,
    JMP_TRUE,
    JMP_FALSE,
    HLT,
};

inline std::istream& operator>>(std::istream& is, Opcode& val) {
    return is >> (std::underlying_type_t<Opcode>&)val;
}

inline std::ostream& operator<<(std::ostream& os, const Opcode& val) {
    return os << (std::underlying_type_t<Opcode>&)val;
}

class Instruction
{
protected:
    size_t size_;
    Opcode opcode_;

public:
    Instruction(size_t size, Opcode opcode) :
        size_(size), opcode_(opcode) {}

    size_t getSize() const {
        return size_;
    }

    Opcode getOpcode() const {
        return opcode_;
    }

    static std::unique_ptr<Instruction> create(Opcode code);

    virtual void execute(VirtualMachine& machine) = 0;

    virtual void read(const char* buf) = 0;

    virtual void write(char* buf) const = 0;

    virtual void disassemble(std::ostream& os) const = 0;

    virtual ~Instruction() = default;
};

class Nonargument : public Instruction
{
protected:
    using Instruction::opcode_;

public:
    Nonargument(Opcode opcode) :
        Instruction(sizeof(Opcode), opcode) {}

    void read(const char* ) override {}

    void write(char* buf) const override {
        utils::write(buf, opcode_);
    }
};

template<typename T>
class Argument : public Instruction
{
protected:
    using Instruction::opcode_;
    T val_;

public:
    Argument(Opcode opcode, const T& val) :
        Instruction(sizeof(Opcode) + sizeof(T), opcode), val_(val) {}

    void read(const char* buf) override {
        utils::read(buf, val_);
    }

    void write(char* buf) const override {
        utils::write(buf, opcode_);
        utils::write(buf + sizeof(Opcode), val_);
    }
};

class iPushAddr final : public Argument<size_t>
{
private:
    using Argument::val_;

public:
    iPushAddr(size_t addr = 0U) :
        Argument(Opcode::iPUSH_ADDR, addr) {} 

    void execute(VirtualMachine& machine) override;

    void disassemble(std::ostream& os) const override {
        os << "int_push [" << val_ << "]";
    }
};

class iPushVal final : public Argument<int>
{
private:
    using Argument::val_;

public:
    iPushVal(int val = 0) :
        Argument(Opcode::iPUSH_VAL, val) {}

    void execute(VirtualMachine& machine) override;

    void disassemble(std::ostream& os) const override {
        os << "int_push " << val_;
    }
};

class iPopAddr final : public Argument<size_t>
{
private:
    using Argument::val_;

public:
    iPopAddr(size_t addr = 0U) :
        Argument(Opcode::iPOP_ADDR, addr) {} 

    void execute(VirtualMachine& machine) override;

    void disassemble(std::ostream& os) const override {
        os << "int_pop [" << val_ << "]";
    }
};

class iPopVal final : public Nonargument
{
public:
    iPopVal() :
        Nonargument(Opcode::iPOP_VAL) {} 

    void execute(VirtualMachine& machine) override;

    void disassemble(std::ostream& os) const override {
        os << "int_pop";
    }
};

class iMul final : public Nonargument
{
public:
    iMul() :
        Nonargument(Opcode::iMUL) {}

    void execute(VirtualMachine& machine) override;

    void disassemble(std::ostream& os) const override {
        os << "int_mul";
    }
};

class iDiv final : public Nonargument
{
public:
    iDiv() :
        Nonargument(Opcode::iDIV) {}

    void execute(VirtualMachine& machine) override;

    void disassemble(std::ostream& os) const override {
        os << "int_div";
    }
};

class iMod final : public Nonargument
{
public:
    iMod() :
        Nonargument(Opcode::iMOD) {}

    void execute(VirtualMachine& machine) override;

    void disassemble(std::ostream& os) const override {
        os << "int_mod";
    }
};

class iAdd final : public Nonargument
{
public:
    iAdd() :
        Nonargument(Opcode::iADD) {}

    void execute(VirtualMachine& machine) override;

    void disassemble(std::ostream& os) const override {
        os << "int_add";
    }
};

class iSub final : public Nonargument
{
public:
    iSub() :
        Nonargument(Opcode::iSUB) {}

    void execute(VirtualMachine& machine) override;

    void disassemble(std::ostream& os) const override {
        os << "int_sub";
    }
};

class iCmpL final : public Nonargument
{
public:
    iCmpL() :
        Nonargument(Opcode::iCMP_L) {}

    void execute(VirtualMachine& machine) override;

    void disassemble(std::ostream& os) const override {
        os << "int_cmp_l";
    }
};

class iCmpG final : public Nonargument
{
public:
    iCmpG() :
        Nonargument(Opcode::iCMP_G) {}

    void execute(VirtualMachine& machine) override;

    void disassemble(std::ostream& os) const override {
        os << "int_cmp_g";
    }
};

class iCmpLE final : public Nonargument
{
public:
    iCmpLE() :
        Nonargument(Opcode::iCMP_LE) {}

    void execute(VirtualMachine& machine) override;

    void disassemble(std::ostream& os) const override {
        os << "int_cmp_le";
    }
};

class iCmpGE final : public Nonargument
{
public:
    iCmpGE() :
        Nonargument(Opcode::iCMP_GE) {}

    void execute(VirtualMachine& machine) override;

    void disassemble(std::ostream& os) const override {
        os << "int_cmp_ge";
    }
};

class iCmpEQ final : public Nonargument
{
public:
    iCmpEQ() :
        Nonargument(Opcode::iCMP_EQ) {}

    void execute(VirtualMachine& machine) override;

    void disassemble(std::ostream& os) const override {
        os << "int_cmp_eq";
    }
};

class iCmpNE final : public Nonargument
{
public:
    iCmpNE() :
        Nonargument(Opcode::iCMP_NE) {}

    void execute(VirtualMachine& machine) override;

    void disassemble(std::ostream& os) const override {
        os << "int_cmp_ne";
    }
};

class iAnd final : public Nonargument
{
public:
    iAnd() :
        Nonargument(Opcode::iAND) {}

    void execute(VirtualMachine& machine) override;

    void disassemble(std::ostream& os) const override {
        os << "int_and";
    }
};

class iOr final : public Nonargument
{
public:
    iOr() :
        Nonargument(Opcode::iOR) {}

    void execute(VirtualMachine& machine) override;

    void disassemble(std::ostream& os) const override {
        os << "int_or";
    }
};

class iNot final : public Nonargument
{
public:
    iNot() :
        Nonargument(Opcode::iNOT) {}

    void execute(VirtualMachine& machine) override;

    void disassemble(std::ostream& os) const override {
        os << "int_not";
    }
};

class iOut final : public Nonargument
{
public:
    iOut() :
        Nonargument(Opcode::iOUT) {}

    void execute(VirtualMachine& machine) override;

    void disassemble(std::ostream& os) const override {
        os << "int_print";
    }
};

class iIn final : public Nonargument
{
public:
    iIn() :
        Nonargument(Opcode::iIN) {}

    void execute(VirtualMachine& machine) override;

    void disassemble(std::ostream& os) const override {
        os << "int_read";
    }
};

class strOut final : public Nonargument
{
public:
    strOut() :
        Nonargument(Opcode::strOUT) {}

    void execute(VirtualMachine& machine) override;

    void disassemble(std::ostream& os) const override {
        os << "string_print";
    }
};

class Alloca final : public Argument<int>
{
public:
    Alloca(int offset = 0) :
        Argument(Opcode::ALLOCA, offset) {}

    void execute(VirtualMachine& machine) override;

    void disassemble(std::ostream& os) const override {
        os << "alloca " << val_;
    }
};

class Jmp final : public Argument<size_t>
{
public:
    Jmp(size_t addr = 0U) :
        Argument(Opcode::JMP, addr) {}

    void setAddr(size_t addr) {
        val_ = addr;
    }

    void execute(VirtualMachine& machine) override;

    void disassemble(std::ostream& os) const override {
        os << "jmp " << val_;
    }
};

class JmpTrue final : public Argument<size_t>
{
public:
    JmpTrue(size_t addr = 0U) :
        Argument(Opcode::JMP_TRUE, addr) {}

    void setAddr(size_t addr) {
        val_ = addr;
    }

    void execute(VirtualMachine& machine) override;

    void disassemble(std::ostream& os) const override {
        os << "jmp_true " << val_;
    }
};

class JmpFalse final : public Argument<size_t>
{
public:
    JmpFalse(size_t addr = 0U) :
        Argument(Opcode::JMP_FALSE, addr) {}

    void setAddr(size_t addr) {
        val_ = addr;
    }

    void execute(VirtualMachine& machine) override;

    void disassemble(std::ostream& os) const override {
        os << "jmp_false " << val_;
    }
};

class Hlt final : public Nonargument
{
public:
    Hlt() :
        Nonargument(Opcode::HLT) {}

    void execute(VirtualMachine& machine) override;

    void disassemble(std::ostream& os) const override {
        os << "hlt";
    }
};

inline std::unique_ptr<Instruction> Instruction::create(Opcode code) {
    switch(code) {
    case Opcode::iPUSH_ADDR:
        return std::make_unique<iPushAddr>();
    case Opcode::iPUSH_VAL:
        return std::make_unique<iPushVal>();
    case Opcode::iPOP_ADDR:
        return std::make_unique<iPopAddr>();
    case Opcode::iPOP_VAL:
        return std::make_unique<iPopVal>();
    case Opcode::iMUL:
        return std::make_unique<iMul>();
    case Opcode::iDIV:
        return std::make_unique<iDiv>();
    case Opcode::iMOD:
        return std::make_unique<iMod>();
    case Opcode::iADD:
        return std::make_unique<iAdd>();
    case Opcode::iSUB:
        return std::make_unique<iSub>();
    case Opcode::iCMP_L:
        return std::make_unique<iCmpL>();
    case Opcode::iCMP_G:
        return std::make_unique<iCmpG>();
    case Opcode::iCMP_LE:
        return std::make_unique<iCmpLE>();
    case Opcode::iCMP_GE:
        return std::make_unique<iCmpGE>();
    case Opcode::iCMP_EQ:
        return std::make_unique<iCmpEQ>();
    case Opcode::iCMP_NE:
        return std::make_unique<iCmpNE>();
    case Opcode::iAND:
        return std::make_unique<iAnd>();
    case Opcode::iOR:
        return std::make_unique<iOr>();
    case Opcode::iNOT:
        return std::make_unique<iNot>();
    case Opcode::iOUT:
        return std::make_unique<iOut>();
    case Opcode::iIN:
        return std::make_unique<iIn>();
    case Opcode::strOUT:
        return std::make_unique<strOut>();
    case Opcode::ALLOCA:
        return std::make_unique<Alloca>();
    case Opcode::JMP:
        return std::make_unique<Jmp>();
    case Opcode::JMP_TRUE:
        return std::make_unique<JmpTrue>();
    case Opcode::JMP_FALSE:
        return std::make_unique<JmpFalse>();
    case Opcode::HLT:
        return std::make_unique<Hlt>();
    default:
        throw std::logic_error("Unknown opcode");
    }
}

} // namespace paracl
