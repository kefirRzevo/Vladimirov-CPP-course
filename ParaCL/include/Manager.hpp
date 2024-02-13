#pragma once

#include <string>

#include "frontend/Driver.hpp"
#include "backend/NodeCodegen.hpp"
#include "backend/VirtualMachine.hpp"

class Manager
{
private:
    const std::string dumpOption = "--dot";
    const std::string decodeOption = "--decode";
    const std::string disasmOption = "--asm";

public:
    Manager() = default;

    void run(int argc, char* argv[]) {
        using namespace paracl;

        Driver drv;
        drv.setFilepath(argv[1]);
        drv.parse();
        drv.semanticAnalyze();
        if (drv.hasErrors()) {
            drv.reportAllErrors(std::cout);
        } else {
            std::cout << "Nice" << std::endl;
            drv.dumpAST("../res/dump_ex1.dot");
            drv.decodeAST("../res/decode_ex1.cl");
            NodeCodegen codegener;
            auto im = codegener.codegen(drv.getRoot());
            im.disassembble("../res/disasm_ex1.txt");
            VirtualMachine m;
            m.loadImage(std::move(im));
            m.execute();
        }
    }
};
