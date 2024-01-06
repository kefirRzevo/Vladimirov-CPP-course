#include "VirtualMachine.hpp"
#include "Instructions.hpp"
#include "Constants.hpp"

using namespace paracl;

int main()
{
  try {
    Image i{65536U};
    i.addConst<ConstInt>(700);
    i.addConst<ConstInt>(11);
    i.addConst<ConstStr>("alallal");
    i.addInstruction<iPushAddr>(i.getConstAddr(0U));
    i.addInstruction<iPushVal>(302);
    i.addInstruction<iAdd>();
    i.addInstruction<iOut>();
    i.addInstruction<strOut>(i.getConstAddr(2U));
    i.addInstruction<Hlt>();
    VirtualMachine m;
    m.loadImage(std::move(i));
    m.execute();
  } catch (std::exception& e) {
    std::cout << e.what() << std::endl;
  }
  return 0;
}
