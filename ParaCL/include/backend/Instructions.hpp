#pragma once

#include <string>
#include <memory>
#include <fstream>
#include <iostream>
#include <stdexcept>

#include "Utils.hpp"

namespace paracl
{

class VirtualMachine;

enum class Opcode : char
{
    //ALLOC,
    iPUSH_ADDR,
    iPUSH_VAL,
    iPOP_ADDR,
    iADD,
    iSUB,
    iMUL,
    iDIV,
    iOUT,
    strOUT,
    iIN,
    HLT,
};

inline std::istream& operator>>(std::istream& is, Opcode& val) {
    return is >> (char&)val;
}

inline std::ostream& operator<<(std::ostream& os, const Opcode& val) {
    return os << (char&)val;
}

class Instruction
{
protected:
    Opcode opcode_;

public:
    Instruction(Opcode opcode) :
        opcode_(opcode) {}

    Opcode getOpcode() const {
        return opcode_;
    }

    //static std::unique_ptr<Instruction> create(Opcode code);

    virtual void execute(VirtualMachine& machine) = 0;

    //virtual void read(std::istream& is) = 0;

    virtual void write(std::ostream& os) = 0;

    virtual ~Instruction() = default;
};

class Nonargument : public Instruction
{
protected:
    using Instruction::opcode_;

public:
    Nonargument(Opcode opcode) :
        Instruction(opcode) {}

    //void read(std::istream& is) override {
    //    is.seekg(Instruction::size - sizeof(Opcode), std::ios::cur);
    //}

    void write(std::ostream& os) override {
        utils::write(os, opcode_);
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
        Instruction(opcode), val_(val) {}

    //void read(std::istream& is) override {
    //    is >> val_;
    //}

    void write(std::ostream& os) override {
        utils::write(os, opcode_);
        utils::write(os, val_);
    }
};
/*
class Alloc final : public Nonargument
{
public:
    Alloc() :
        Nonargument(Opcode::ALLOC) {}

    void execute(VirtualMachine& machine) override;
};
*/
class iPushAddr final : public Argument<size_t>
{
private:
    using Argument::val_;

public:
    iPushAddr(size_t addr) :
        Argument(Opcode::iPUSH_ADDR, addr) {} 

    void execute(VirtualMachine& machine) override;
};

class iPushVal final : public Argument<int>
{
private:
    using Argument::val_;

public:
    iPushVal(int val) :
        Argument(Opcode::iPUSH_VAL, val) {} 

    void execute(VirtualMachine& machine) override;
};

class iPopAddr final : public Argument<size_t>
{
private:
    using Argument::val_;

public:
    iPopAddr(size_t addr) :
        Argument(Opcode::iPOP_ADDR, addr) {} 

    void execute(VirtualMachine& machine) override;
};

class iAdd final : public Nonargument
{
public:
    iAdd() :
        Nonargument(Opcode::iADD) {}

    void execute(VirtualMachine& machine) override;
};

class iSub final : public Nonargument
{
public:
    iSub() :
        Nonargument(Opcode::iSUB) {}

    void execute(VirtualMachine& machine) override;
};

class iMul final : public Nonargument
{
public:
    iMul() :
        Nonargument(Opcode::iMUL) {}

    void execute(VirtualMachine& machine) override;
};

class iDiv final : public Nonargument
{
public:
    iDiv() :
        Nonargument(Opcode::iDIV) {}

    void execute(VirtualMachine& machine) override;
};

class iOut final : public Nonargument
{
public:
    iOut() :
        Nonargument(Opcode::iOUT) {}

    void execute(VirtualMachine& machine) override;
};

class strOut final : public Argument<size_t>
{
public:
    strOut(size_t addr) :
        Argument(Opcode::strOUT, addr) {}

    void execute(VirtualMachine& machine) override;
};

class iIn final : public Nonargument
{
public:
    iIn() :
        Nonargument(Opcode::iIN) {}

    void execute(VirtualMachine& machine) override;
};

class Hlt final : public Nonargument
{
public:
    Hlt() :
        Nonargument(Opcode::HLT) {}

    void execute(VirtualMachine& machine) override;
};
/*
inline std::unique_ptr<Instruction> Instruction::create(Opcode code) {
    switch(code) {
    case Opcode::ALLOC:
        return std::make_unique<Alloc>();
    case Opcode::iPUSH_ADDR:
        return std::make_unique<iPushAddr>(0U);
    case Opcode::iPUSH_VAL:
        return std::make_unique<iPushVal>(0);
    case Opcode::iPOP_ADDR:
        return std::make_unique<iPopAddr>(0U);
    case Opcode::iADD:
        return std::make_unique<iAdd>();
    case Opcode::iSUB:
        return std::make_unique<iSub>();
    case Opcode::iMUL:
        return std::make_unique<iMul>();
    case Opcode::iDIV:
        return std::make_unique<iDiv>();
    case Opcode::iOUT:
        return std::make_unique<iOut>();
    case Opcode::iIN:
        return std::make_unique<iIn>();
    case Opcode::HLT:
        return std::make_unique<Hlt>();
    default:
        throw std::logic_error("Unknown opcode");
    }
}
*/
} // namespace paracl
