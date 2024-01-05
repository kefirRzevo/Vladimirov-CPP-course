//#include "backend/VirtualMachine.hpp"
#include "../../include/backend/VirtualMachine.hpp"

namespace paracl
{

//void Alloc::execute(VirtualMachine& ) {
    //auto fir = machine.pop<size_t>();
    //machine.push<size_t>(fir);
//}

void iPushAddr::execute(VirtualMachine& machine) {
    auto val = machine.fromStack<int>(val_);
    machine.push<int>(val);
}

void iPushVal::execute(VirtualMachine& machine) {
    machine.push<int>(val_);
}

void iPopAddr::execute(VirtualMachine& machine) {
    auto val = machine.pop<int>();
    machine.toStack<int>(val_, val);
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

void iOut::execute(VirtualMachine& machine) {
    auto val = machine.pop<int>();
    std::cout << val;
}

void strOut::execute(VirtualMachine& machine) {
    auto size = machine.fromStack<size_t>(val_);
    std::string val{machine.absoluteAddr(val_), size};
    std::cout << val;
}

void iIn::execute(VirtualMachine& machine) {
    int val = 0;
    std::cin >> val;
    machine.push<int>(val);
}

void Hlt::execute(VirtualMachine& machine) {
    machine.halt();
}

} // namespace paracl
