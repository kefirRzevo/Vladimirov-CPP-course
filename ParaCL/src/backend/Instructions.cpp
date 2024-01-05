//#include "backend/VirtualMachine.hpp"
#include "../../include/backend/VirtualMachine.hpp"

namespace paracl
{

void Add::execute(VirtualMachine& machine) {
    auto sec = machine.pop<int>();
    auto fir = machine.pop<int>();
    machine.push<int>(fir + sec);
}

void Sub::execute(VirtualMachine& machine) {
    auto sec = machine.pop<int>();
    auto fir = machine.pop<int>();
    machine.push<int>(fir - sec);
}

void Push::execute(VirtualMachine& machine) {
    auto sec = machine.atStack<int>(addr_);
    machine.push<int>(sec);
}

} // namespace paracl
