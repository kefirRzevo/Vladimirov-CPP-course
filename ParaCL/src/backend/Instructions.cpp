#include <string>

#include "backend/VirtualMachine.hpp"

namespace paracl
{

void iPushAddr::execute(VirtualMachine& machine) {
    auto val = machine.atMemory<int>(val_);
    machine.push<int>(val);
}

void iPushVal::execute(VirtualMachine& machine) {
    machine.push<int>(val_);
}

void iPopAddr::execute(VirtualMachine& machine) {
    auto val = machine.pop<int>();
    machine.toStack<int>(val_, val);
}

void iPopVal::execute(VirtualMachine& machine) {
    machine.pop<int>();
}

void iMul::execute(VirtualMachine& machine) {
    auto sec = machine.pop<int>();
    auto fir = machine.pop<int>();
    machine.push<int>(fir * sec);
}

void iDiv::execute(VirtualMachine& machine) {
    auto sec = machine.pop<int>();
    auto fir = machine.pop<int>();
    machine.push<int>(fir / sec);
}

void iMod::execute(VirtualMachine& machine) {
    auto sec = machine.pop<int>();
    auto fir = machine.pop<int>();
    machine.push<int>(fir % sec);
}

void iAdd::execute(VirtualMachine& machine) {
    auto sec = machine.pop<int>();
    auto fir = machine.pop<int>();
    machine.push<int>(fir + sec);
}

void iSub::execute(VirtualMachine& machine) {
    auto sec = machine.pop<int>();
    auto fir = machine.pop<int>();
    machine.push<int>(fir - sec);
}

void iCmpL::execute(VirtualMachine& machine) {
    auto sec = machine.pop<int>();
    auto fir = machine.pop<int>();
    machine.push<int>(fir < sec);
}

void iCmpG::execute(VirtualMachine& machine) {
    auto sec = machine.pop<int>();
    auto fir = machine.pop<int>();
    machine.push<int>(fir > sec);
}

void iCmpLE::execute(VirtualMachine& machine) {
    auto sec = machine.pop<int>();
    auto fir = machine.pop<int>();
    machine.push<int>(fir <= sec);
}

void iCmpGE::execute(VirtualMachine& machine) {
    auto sec = machine.pop<int>();
    auto fir = machine.pop<int>();
    machine.push<int>(fir >= sec);
}

void iCmpEQ::execute(VirtualMachine& machine) {
    auto sec = machine.pop<int>();
    auto fir = machine.pop<int>();
    machine.push<int>(fir == sec);
}

void iCmpNE::execute(VirtualMachine& machine) {
    auto sec = machine.pop<int>();
    auto fir = machine.pop<int>();
    machine.push<int>(fir != sec);
}

void iAnd::execute(VirtualMachine& machine) {
    auto sec = machine.pop<int>();
    auto fir = machine.pop<int>();
    machine.push<int>(fir && sec);
}

void iOr::execute(VirtualMachine& machine) {
    auto sec = machine.pop<int>();
    auto fir = machine.pop<int>();
    machine.push<int>(fir || sec);
}

void iNot::execute(VirtualMachine& machine) {
    auto val = machine.pop<int>();
    machine.push<int>(!val);
}

void iOut::execute(VirtualMachine& machine) {
    auto val = machine.pop<int>();
    machine.ofstream() << val << std::endl;
}

void iIn::execute(VirtualMachine& machine) {
    int val = 0;
    machine.ifstream() >> val;
    machine.push<int>(val);
}

void strOut::execute(VirtualMachine& machine) {
    auto addr = machine.pop<size_t>();
    auto size = machine.atMemory<size_t>(addr);
    std::string res;
    for (size_t i = 0U; i < size; ++i) {
        char sym = machine.atMemory<char>(addr + sizeof(size_t) + i);
        res.push_back(sym);
    }
    machine.ofstream() << res << std::endl;
}

void Alloca::execute(VirtualMachine& machine) {
    machine.sp() += val_;
}

void Jmp::execute(VirtualMachine& machine) {
    machine.ip() = val_;
}

void JmpTrue::execute(VirtualMachine& machine) {
    auto val = machine.pop<int>();
    if (val) {
        machine.ip() = val_;
    }
}

void JmpFalse::execute(VirtualMachine& machine) {
    auto val = machine.pop<int>();
    if (!val) {
        machine.ip() = val_;
    }
}

void Hlt::execute(VirtualMachine& machine) {
    machine.halt();
}

} // namespace paracl
