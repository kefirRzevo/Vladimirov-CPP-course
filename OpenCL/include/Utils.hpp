#pragma once

#include <vector>
#include <limits>
#include <iostream>

namespace opencl
{

template<typename T>
inline void readDataWithSize(std::vector<T>& data, std::istream& is) {
    is.seekg(0, std::ios::beg);
    size_t size = 0U;
    is >> size;
    T temp(0);
    for (auto i = 0UL; i < size; ++i) {
        is >> temp;
        data.emplace_back(temp);
    }
}

template<typename T>
inline void readData(std::vector<T>& data, std::istream& is) {
    is.seekg(0, std::ios::beg);
    T temp(0);
    while (is >> temp) {
        data.emplace_back(temp);
    }
}

template<typename T>
inline void write(const std::vector<T>& data, std::ostream& os) {
    os << data.size() << std::endl;
    for (auto&& elem: data) {
        os << elem << " ";
    }
    os << std::endl;
}

template<typename T>
inline void expand(std::vector<T>& data) {
    auto oldSize = data.size();
    auto newSize = std::bit_ceil(data.size());
    for (auto i = oldSize; i < newSize; ++i) {
        data.push_back(std::numeric_limits<T>::max());
    }
}

//all times in microseconds

struct CpuSortPassedTime
{
    long long cpu = 0;
};

struct GpuSortPassedTime
{
    long long cpu = 0;
    long long gpu = 0;
};

} // namespace opencl
