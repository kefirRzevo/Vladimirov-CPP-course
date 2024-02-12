#include "include/frontend/Driver.hpp"
#include "include/backend/NodeCodegen.hpp"
#include "include/backend/VirtualMachine.hpp"

using namespace paracl;

int main() {
    try {
        Driver drv{"../test/ex1.cl"};
        drv.parse();
        drv.semanticAnalyze();
        if (drv.hasErrors()) {
            drv.reportAllErrors(std::cout);
        } else {
            std::cout << "Nice" << std::endl;
            std::fstream dumpFile{"../res/dump_ex1.dot", std::ios::out};
            std::fstream disasmFile{"../res/disasm_ex1.txt", std::ios::out};
            drv.dumpAST(dumpFile);
            drv.decodeAST("../res/decode_ex1.cl");
            NodeCodegen codegener;
            auto im = codegener.codegen(drv.getRoot());
            im.disassembble(disasmFile);
            VirtualMachine m;
            m.loadImage(std::move(im));
            m.execute();
        }
    } catch(const std::exception& e) {
        std::cerr << e.what() << '\n';
    }
}
