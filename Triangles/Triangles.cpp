#include <iostream>

#include "include/Geometry.hpp"

using namespace triangles;

int main()
{
    Vector<int> a{2, 2, 3};
    Vector<int> b{1, 1, 2};
    Vector<int> c{};
    c = +a = b;
    a.dump();
    b.dump();
    c.dump();
    std::cout<<dot(a, b);
    return 0;
}