#include <fstream>
#include "include/frontend/Driver.hpp"

using namespace paracl;

int main() {
    try {
        Driver drv{"../tests/ex1.cl"};
        drv.getParser()->parse();
        if (drv.getAST()->getRoot() != nullptr) {
            drv.getAST()->semanticAnalyze(drv);
            std::fstream dumpFile{"../res/dump_ex1.dot", std::ios::out};
            drv.getAST()->dump(dumpFile);

            std::fstream dumpFile2{"../res/dump_ex2.dot", std::ios::out};
            AST ast{*drv.getAST()};
            ast.dump(dumpFile2);
        }
        if (drv.getReporter()->hasErrors()) {
            drv.getReporter()->reportAllErrors(std::cout);
        } else {
            assert(drv.getAST()->getRoot());
            std::cout << "Nice" << std::endl;
        }
    } catch(const std::exception& e) {
        std::cerr << e.what() << '\n';
    }
}
