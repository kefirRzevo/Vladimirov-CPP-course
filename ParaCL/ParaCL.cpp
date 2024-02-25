#include "Manager.hpp"

int main(int argc, const char* argv[]) {
    try {
        paracl::Manager manager;
        manager.run(argc, argv);
    } catch (const std::exception& e) {
        std::cerr << e.what() << std::endl;
        return 0;
    }
}