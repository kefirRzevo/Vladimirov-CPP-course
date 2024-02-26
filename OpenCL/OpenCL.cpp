#include <numeric>
#include <fstream>
#include <iostream>
#include "GpuBitonicSort.hpp"

using namespace opencl;

int main(int , char* argv[])
{
    try {
        std::vector<int> data;
        std::ifstream is{"../tests/001.dat"};
        readDataWithSize(data, is);
        auto oldSize = data.size();
        expand(data);
        auto newSize = data.size();

        OpenClApp app(argv[0]);
        app.buildProgram<int>("../src/BitonicInt.cl");
        app.gpuBitonicSort(data);

        for (auto i = newSize; i > oldSize; --i) {
            data.pop_back();
        }
        for (auto&& elem : data) {
            std::cout << elem << " ";
        }
        std::cout << std::endl;
    } catch (const std::exception& e) {
        std::cout << "Error: " << e.what() << std::endl;
    }
    return 0;
}
