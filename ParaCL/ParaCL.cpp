#include "Manager.hpp"

int main(int argc, char* argv[]) {
    try {
        Manager manager;
        manager.run(argc, argv);
    } catch (const std::exception& e) {
        std::cerr << e.what() << std::endl;
    }
}
