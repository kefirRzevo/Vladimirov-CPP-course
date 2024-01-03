#pragma once

#include <string>

namespace paracl
{

class VirtualMachine;

enum class Opcode : char
{
    ALLOCA,
    STORE,
    LOAD,
    PUSH,
    POP,
    ADD,
    SUB,
    MUL,
    DIV,
};

class Instruction
{
protected:
    Opcode opcode_;

public:
    static constexpr size_t size = 4U;

    Instruction(Opcode opcode) :
        opcode_(opcode) {}

    Opcode getOpcode() const {
        return opcode_;
    }

    virtual void execute(VirtualMachine& machine) = 0;

    virtual size_t read(void* addr) = 0;

    virtual size_t write(void* addr) const = 0;
};

class Arithmetic : public Instruction
{
protected:
    using Instruction::opcode_;

public:
    Arithmetic(Opcode opcode) :
        Instruction(opcode) {}

    size_t read(void* ) override {
        return Instruction::size - sizeof(Opcode);
    }

    size_t write(void* addr) const override {
        *reinterpret_cast<Opcode*>(addr) = opcode_;
        return Instruction::size;
    }
};

class Add : public Arithmetic
{
public:
    Add() :
        Arithmetic(Opcode::ADD) {}

    void execute(VirtualMachine& machine) override;
};

class Sub : public Arithmetic
{
public:
    Sub() :
        Arithmetic(Opcode::SUB) {}

    void execute(VirtualMachine& machine) override;
};

Opcode readOpcode(void* addr) {
    return *reinterpret_cast<Opcode*>(addr);
}

} // namespace paracl
