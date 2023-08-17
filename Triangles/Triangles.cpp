#include <iostream>

#include "include/Geometry.hpp"

using namespace triangles;

int main()
{
	Plane<float> p3{{1., -2., 1.}, {4., -2., -2.}, {4., 1., 4.}};
	p3.dumpABCD();
    return 0;
}
