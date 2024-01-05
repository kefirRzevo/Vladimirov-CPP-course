#pragma once

#include <string>
#include <memory>
#include <fstream>
#include <iostream>
#include <stdexcept>

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

std::istream& operator<<(std::istream& is, Opcode& val) {
    return is >> (char&)val;
}

class Instruction
{
protected:
    Opcode opcode_;

public:
    static constexpr size_t size = 5U;

    Instruction(Opcode opcode) :
        opcode_(opcode) {}

    Opcode getOpcode() const {
        return opcode_;
    }

    static std::unique_ptr<Instruction> create(Opcode code);

    virtual void execute(VirtualMachine& machine) = 0;

    virtual void read(std::istream& is) = 0;

    virtual void write(std::ostream& os) = 0;

    virtual ~Instruction() = default;
};

class Arithmetic : public Instruction
{
protected:
    using Instruction::opcode_;

public:
    Arithmetic(Opcode opcode) :
        Instruction(opcode) {}

    void read(std::istream& is) override {
        auto pos = is.tellp();
        is.seekp(pos + Instruction::size - sizeof(Opcode));
    }

    void write(std::ostream& os) override {
        os >> opcode_;
        auto pos = os.tellp();
        os.seekp(pos + Instruction::size - sizeof(Opcode));
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

class Addressed : public Instruction
{
private:
    int addr_;

public:
    Addressed(Opcode opcode) :
        Instruction(opcode) {}

    void read(std::istream& is) override {
        is >> addr_;
    }

    void write(std::ostream& os) override {
        os >> opcode_ >> addr_;
    }
};

class Push : public Addressed
{
private:
    size_t addr_;

public:
    Push(size_t addr) :
        Addressed(Opcode::PUSH), addr_(addr) {} 

    void execute(VirtualMachine& machine) override;
};

/*
std::unique_ptr<Instruction> Istruction::create(Opcode code) {
    switch(code) {
    case Opcode::ADD:
        return std::make_unique<Add>();
    case Opcode::ADD:
        return std::make_unique<Sub>();
    default:
        throw std::logic_error("Unknown opcode");
    }
}
*/

} // namespace paracl
