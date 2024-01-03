#include <fstream>
#include <cassert>
#include "include/frontend/Driver.hpp"

using namespace paracl;

int main() {
    try {
        Driver drv{"../tests/ex1.cl"};
        drv.parse();
        if (drv.hasErrors()) {
            drv.reportAllErrors(std::cout);
        } else {
            std::cout << "Nice" << std::endl;
            drv.semanticAnalyze();
            std::fstream dumpFile{"../res/dump_ex1.dot", std::ios::out};
            drv.dumpAST(dumpFile);
            drv.decodeAST("../res/decode_ex1.cl");
        }
    } catch(const std::exception& e) {
        std::cerr << e.what() << '\n';
    }
}
