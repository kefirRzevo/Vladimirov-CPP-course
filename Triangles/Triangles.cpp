#include <iostream>

#include "include/Geometry.hpp"

using namespace triangles;

int main()
{
	Vector<int> v1{1, 2, 3};
	Vector<int> v2{1, 2, 3};
	Vector<double> v3{-1.00000001, 2, -1};
	Vector<double> v4{-0.9999991, 2, -1.0000001};

    std::cout << (v1 == v2) << " " << (v3 == v4) << std::endl;
    return 0;
}
