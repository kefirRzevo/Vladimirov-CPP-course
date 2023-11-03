#include "include/Matrix.hpp"

using namespace matrix;

int main()
{
    try {
		std::cin.exceptions(std::cin.failbit);
		size_t size = 0;
		in >> size;
		Matrix<double> m{size};
		in >> m;
		std::cout << m.det();
	} catch (const std::exception& e) {
		std::cout << "Error: " << e.what() << std::endl;
	}
    return 0;
}
