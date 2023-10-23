#include <chrono>
#include "../include/Utils.hpp"
#include "../include/Tree.hpp"

int main()
{
    try {
        auto start = std::chrono::high_resolution_clock::now();
		auto res = tree::myProcess(std::cin);
        auto end = std::chrono::high_resolution_clock::now();
        auto elapsed = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
		for (size_t i = 0; i < res.size(); ++i) {
			std::cout << res[i] << " ";
		}
        std::cout << "\nTime processed: " << static_cast<double>(elapsed.count()) * 0.001 << " msec" << std::endl;
	} catch (const std::exception& e) {
		std::cout << "Error: " << e.what() << std::endl;
	}
    return 0;
}
