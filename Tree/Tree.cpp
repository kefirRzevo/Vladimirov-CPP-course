#include "include/Utils.hpp"
#include "include/Tree.hpp"

using namespace tree;

int main()
{
    try {
        auto res = myProcess(std::cin);
        for (size_t i = 0; i < res.size(); ++i) {
            std::cout << res[i] << " ";
        }
        std::cout << std::endl;
    } catch (const std::exception& e) {
        std::cout << "Error: " << e.what() << std::endl;
    }
    return 0;
}
